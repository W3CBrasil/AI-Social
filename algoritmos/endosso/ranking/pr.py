# -*- coding: utf-8 -*-
import datetime
from optparse import OptionParser
import os
import warnings

from scipy.sparse.base import SparseEfficiencyWarning

from ranking import TweetInfo
from ranking.arlei_ranking import compute_relevance
import logging

FORMAT = '[%(levelname)s] [%(asctime)s] [%(name)s] %(message)s'
logger = logging.getLogger(__file__)
logging.basicConfig(level=logging.DEBUG, format=FORMAT)

# from ranking.arlei_ranking import compute_relevance
def read_data(input_dir, start, reference):
	current_date = start
	while current_date <= reference:
		path = os.path.join(input_dir, current_date.strftime('%Y%m%d'))
		with open(path) as f:
			for line in f:
				info = TweetInfo(*line.strip().split('\t'))
				yield info
		logger.debug('Read input file %s', path)
		current_date += datetime.timedelta(days=1)
def main():
	parser = OptionParser()
	# parser.add_option("-i", "--input", dest="input", help=u"Database input file (sqlite3)", metavar="INPUT")
	parser.add_option("-i", "--input-dir", dest="input_dir", 
					help=u"Input dir containing CSV files."
						u"Files must contain 1 line per tweet and fields username, tweet.id, retweet.id, "
						u"tweet.create_date (yyyy-mm-ddThh:MM:ss), entities (comma-delimited, optional) and "
						u"user.foto. File names must be in the format yyyymmdd. ", 
					metavar="INPUT")
	parser.add_option("-n", "--iteractions", dest="iteractions", help=u"Number of iteractions (default = 20)", 
					metavar="ITERACTIONS", default=20)
	parser.add_option("-d", "--damping-factor", dest="damping_factor", help=u"Damping factor (default: 0.85)", 
					metavar="DAMPING", default='0.85')
	parser.add_option("-o", "--output", dest="output", 
					help=u"Output dir. Files will be named after type (CONTENT or USER) and date", 
					metavar="OUPUT")
	parser.add_option("-t", "--type", dest="type", help=u"Type (USER or CONTENT)", metavar="TYPE")
	parser.add_option("-u", "--user", dest="user", help=u"User (screen name in Twitter). Not used anymore.", 
					metavar="USER", default='')
	parser.add_option("", "--date", dest="date", help=u"Reference date. Defaults to today, format yyyymmdd", 
					metavar="Date")
	parser.add_option("-r", "--days", dest="days", 
					help=u"Number of days to consider. The range goes from [date - days -1, date]", 
					metavar="DAYS", default='60')


	warnings.simplefilter("ignore", SparseEfficiencyWarning)	

	(options, _) = parser.parse_args()
	
	if not (options.input_dir and options.iteractions and options.type):
		parser.print_usage()
		exit(1)
	if options.date:
		reference = datetime.datetime.strptime(options.date, '%Y%m%d')
	else:
		reference = datetime.datetime.today().replace(hour=0, minute=0, second=0, microsecond=0)
	start = reference - datetime.timedelta(days=int(options.days))
	
	data_iterator = read_data(options.input_dir, start, reference)
	(user_relevance, content_relevance, content_to_user, 
		user_photo, content_to_date, content_to_entities) = compute_relevance(data_iterator,
					int(options.iteractions), float(options.damping_factor), options.user, reference, start)
	
	user_relevance_table = {}
	outfile = '{0}/{1}_{2}'.format(options.output, options.type[0], reference.strftime('%Y%m%d'))
	
	with open(outfile, 'w') as output_file:
		for user in xrange(0, len(user_relevance)):
			user_relevance_table[user_relevance[user][0]] = user_relevance[user][1]
				
			if options.type == "USER":
				if options.user != user_relevance[user][0]:
					u = user_relevance[user][0]
					try:
						print >> output_file, (u'{0}\t{1}\t{2}'.format(
																		user_relevance[user][0], 
																		user_relevance[user][1] * 1000000, 
																		user_photo[u])).encode('utf8')
					except:
						print "Erro de encoding"
		if options.type == "CONTENT":
			for content in xrange(0, len(content_relevance)):
				user = content_to_user[content_relevance[content][0]]
				data = content_to_date[content_relevance[content][0]]
				entities = content_to_entities[content_relevance[content][0]]
				print >> output_file, '{0}\t{1}\t{2}\t{3}\t{4}\thttp://twitter.com/{3}/status/{0}\t{5}'.format(
					content_relevance[content][0], content_relevance[content][1] * 1000000,
							user_photo[user], user, data, entities)
		
	logger.info('All done. Thanks for flying with us. Check output file %s', outfile)
if __name__ == '__main__':
	main()

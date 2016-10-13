# -*- coding: utf-8 -*-
from optparse import OptionParser
from ranking.arlei_ranking import compute_relevance

if __name__ == '__main__':
	parser = OptionParser()
	parser.add_option("-i", "--input", dest="input", help=u"Input file", metavar="INPUT")
	parser.add_option("-n", "--iteractions", dest="iteractions", help=u"Number of iteractions", metavar="ITERACTIONS")
	parser.add_option("-d", "--damping-factor", dest="damping_factor", help=u"Damping factor", metavar="DAMPING", default='0.85')
	parser.add_option("-o", "--output", dest="output", help=u"Output file", metavar="OUPUT")
	parser.add_option("-t", "--type", dest="type", help=u"Type (USER or CONTENT)", metavar="TYPE")
	parser.add_option("-u", "--user", dest="user", help=u"User (screen name in Twitter)", metavar="USER", default='')
	
	(options, args) = parser.parse_args()
	
	if not (options.input and options.iteractions and options.output and options.type):
		parser.print_usage()
		exit(1)
	
	(user_relevance, content_relevance, content_to_user, user_photo) = compute_relevance(options.input, 
							int(options.iteractions), float(options.damping_factor), options.user)
	
	#(user_num_retweets,user_average_num_retweets,user_num_retweeters,user_sum_num_retweets_retweeter) = compute_user_statistics(input_file_name)
	
	user_relevance_table = {}
	
	with open(options.output, 'w') as output_file:
	
		#output_file.write("user,influence,retweets,avg retweets,retweeters,sum retweeters\n")
		
		for user in xrange(0, len(user_relevance)):
			user_relevance_table[user_relevance[user][0]] = user_relevance[user][1]
				
			if options.type == "USER":
				if options.user != user_relevance[user][0]:
					print >> output_file, '{0},{1}'.format(user_relevance[user][0], user_relevance[user][1])
		#			 output_file.write(str(user_relevance[user][0])+","+ str(user_relevance[user][1])+","+str(user_num_retweets[user_relevance[user][0]])+","+str(user_average_num_retweets[user_relevance[user][0]])+","+str(user_num_retweeters[user_relevance[user][0]])+","+str(user_sum_num_retweets_retweeter[user_relevance[user][0]])+"\n")
		
		if options.type == "CONTENT":
			###(content_num_retweets, content_sum_num_retweets_retweeter, content_user) = compute_content_statistics(options.input)
			#output_file.write("content,relevance,user,influence user,retweets,sum retweeters\n")
			for content in xrange(0, len(content_relevance)):
				print >> output_file, '{0},{1},http://twitter.com/{2}/status/{0}'.format(content_relevance[content][0], content_relevance[content][1],
							content_to_user[content_relevance[content][0]])
		#		output_file.write(str(content_relevance[content][0])+","+ str(content_relevance[content][1])+","+content_user[content_relevance[content][0]]+","+str(user_relevance_table[content_user[content_relevance[content][0]]])+","+str(content_num_retweets[content_relevance[content][0]])+","+str(content_sum_num_retweets_retweeter[content_relevance[content][0]])+"\n")
		
	

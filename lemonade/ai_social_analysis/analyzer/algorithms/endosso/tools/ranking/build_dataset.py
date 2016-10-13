# -*- coding: utf-8 -*-
"""
" Obtém os dados do mongodb para um contexto e exporta-os para o sqlite3.
"
"""
import datetime
from optparse import OptionParser
import os

import pymongo

from pipeline.util.load_workflow import WorkflowLoader


# import email.utils as eut
def _fix_url_foto(url):
	return url.replace('http://a0.twimg.com/profile_images/', '')
def main():
	
	parser = OptionParser()

	parser.add_option("-c", "--contexto", dest="contexto", help=u"Id do contexto no observatório da web" ,
			metavar="CONTEXTO")
	parser.add_option("-w", "--workflow", dest="workflow", help=u"Caminho para arquivo de workflow" ,
			metavar="WORKFLOW")
	parser.add_option("-d", "--date", dest="date", help=u"Data de referência, formato aaaa-mm-dd" ,
			metavar="DATA")
	parser.add_option("--output-dir", dest="output_dir", help=u"Caminho para diretório de saída" ,
			metavar="DIR")
	(options, _) = parser.parse_args()

	if all([options.workflow, options.contexto, options.date, options.output_dir]):
		workflow = WorkflowLoader().load(options.workflow)
		conn = pymongo.Connection(workflow['global']['mongodb'], read_preference=pymongo.ReadPreference.SECONDARY)
		tweets_coll = conn['coleta_%s' % options.contexto]['tweets_%s' % options.contexto]
		
		inicio = datetime.datetime.strptime(options.date, '%Y-%m-%d') 
		fim = inicio + datetime.timedelta(days=1) - datetime.timedelta(seconds=1)
	
		print u"Gerando base de dados para período", inicio, fim

		filters = {
			'created_at': {'$gte': inicio, '$lte': fim},
			'retweeted_status': {'$exists': True}
		};
		
		fields = {
				'user.screen_name': 1, 'retweeted_status.id': 1, 'retweeted_status.user.screen_name': 1,
				'created_at': 1, 'control.entities.entities': 1, 'retweeted_status.created_at': 1,
				'retweeted_status.retweet_count': 1, 'user.profile_image_url': 1,
				'retweeted_status.user.profile_image_url': 1
		};
		all_tweets = {}
		with open(os.path.join(options.output_dir, options.date.replace("-", "")), "w") as output:
			for i, tweet in enumerate(tweets_coll.find(filters, fields, timeout=False)):
				retweet = tweet['retweeted_status']
				rt_id = retweet['id']
				if all_tweets.has_key(rt_id):
					if all_tweets[rt_id]['retweet_count'] < retweet['retweet_count']:
						all_tweets[rt_id]['retweet_count'] = retweet['retweet_count']
				else:
					all_tweets[rt_id] = retweet
		
				if tweet.has_key('control') and tweet['control'].has_key('entities'):
					entities = [str(item) for item in tweet['control']['entities']['entities']]
				else:
					entities = []
				
				# Quem retuitou
				print >> output, "{0}\t{1}\t{2}\t{3}\t{4}\t{5}".format(
													tweet['user']['screen_name'],
													tweet['_id'],
													retweet['id'],
													tweet['created_at'].isoformat(),
													','.join(entities),
													_fix_url_foto(tweet['user']['profile_image_url'])) 
				
				# if isinstance(retweet['created_at'], datetime.datetime):
				# 	date_original_tweet = retweet['created_at']
				# else:
				# 	date_original_tweet = datetime.datetime(*eut.parsedate(retweet['created_at'])[:6])
					
				# Quem foi retuitado
				# print >> output, "{0}\t{1}\t{2}\t{3}\t{4}".format(
				# cursor.execute(INSERT_SQL, [retweet['user']['screen_name'], retweet['id'], date_original_tweet, 
				# 	','.join(entities),_fix_url_foto( retweet['user']['profile_image_url'])])  
				if i and i % 2000 == 0:
					print "Processados %d tweets" % i
		conn.close()
	else:
		parser.print_help()	
if __name__ == '__main__':
	main()

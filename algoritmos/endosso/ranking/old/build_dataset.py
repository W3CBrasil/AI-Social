# -*- coding: utf-8 -*-
import datetime
import email.utils as eut
import pymongo
from optparse import OptionParser


if __name__ == '__main__':
	conn = pymongo.Connection(map(lambda x : '200.131.6.{0}:27017'.format(x), [252, 253, 254]), 
								read_preference=pymongo.ReadPreference.SECONDARY)
	
	parser = OptionParser()
	parser.add_option("-c", "--contexto", dest="contexto", help=u"Id do contexto no observatório da web" , 
			metavar="CONTEXTO")
	
	(options, args) = parser.parse_args()
	
	tweets_coll = conn['coleta_%d' % options.contexto]['tweets_%d' % options.contexto]
	inicio = datetime.datetime.strptime('2011-05-10', '%Y-%m-%d') 
	#fim = inicio + datetime.timedelta(days=1) - datetime.timedelta(seconds=1)
	fim = datetime.datetime.now() 
	filters = {'created_at': {'$gte': inicio, '$lte': fim}, 'retweeted_status': {'$exists': True}}
	filters = {'retweeted_status': {'$exists': True}}
	
	fields = {'user.screen_name': 1, 'retweeted_status.id': 1, 'retweeted_status.user.screen_name': 1, 'created_at': 1,
			'retweeted_status.created_at': 1, 'retweeted_status.retweet_count': 1}
	
	dados = set()
	for i, tweet in enumerate(tweets_coll.find(filters, fields, timeout=False)):
		
		dados.add((tweet['user']['screen_name'], tweet['retweeted_status']['id'], tweet['created_at'])) # Quem retuitou
		date_original_tweet = datetime.datetime(*eut.parsedate(tweet['retweeted_status']['created_at'])[:6])
		dados.add((tweet['retweeted_status']['user']['screen_name'], tweet['retweeted_status']['id'], 
				date_original_tweet))  # Quem foi retuitado
		if i and i % 1000 == 0:
			print "Processados %d tweets" % i

	files = {}
	for user, tweet_id, data in sorted(dados, key=lambda x: x[2]):
		key = data.strftime('%Y%m%d')
		if not key in files:
			files[key] = open('{0}.csv'.format(key), 'w')
		
		print >> files[key], '{0},{1},{2}'.format(user, tweet_id, data.strftime('%Y%m%d%H%M%S'))
							

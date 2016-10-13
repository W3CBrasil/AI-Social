# -*- coding: utf-8 -*-
import datetime
import MySQLdb
import csv
import sys
import os
import pymongo
from optparse import OptionParser
from warnings import filterwarnings

def processa(db, filename, contexto, geral):
	conn = MySQLdb.connect(host = 'mysql1.ctweb.inweb.org.br', port=3306, user = 'proc', passwd = 'pr0ce55@', 
		db = db, use_unicode=True, charset = 'utf8') 
	cursor = conn.cursor()
		
	insert_item = """INSERT INTO painel_tweetpopular(contexto_id, usuario, tweet_id, descricao, foto_usuario, hora, data_referencia, periodo, score, habilitado, geral) 
					VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s) ON DUPLICATE KEY UPDATE score= %s """

	data_ref = datetime.datetime.strptime(filename[1:9],'%Y%m%d') - datetime.timedelta(days=1)
	print filename, data_ref

	total_score = 0.0
	with open(filename) as f:
		reader = csv.reader(f, delimiter='\t')
		for i, (tweet_id, score, foto, usuario, data, url, entidades) in enumerate(reader):
			score = float(score)
			data1 = datetime.datetime.strptime(data[0:10],'%Y-%m-%d')
			if data1 != data_ref:
				#print data1, data_ref
				continue	
			else:
				params = [int(contexto), usuario, tweet_id, '', 'http://a0.twimg.com/profile_images/' + foto, data, data1, 'd', score, 1, 1, score]
				cursor.execute(insert_item, params)
			total_score += score
			if i % 1000 == 0:
				conn.commit()
				print "Processados {0} e confirmados".format(i)
	print "processado {0}. Score total: {1}".format(filename, total_score)
	cursor.execute('UPDATE painel_tweetpopular SET score = 1000*score / %s WHERE data_referencia = %s', [total_score, data_ref])
	conn.commit()
	mongo_conn = pymongo.Connection('10.5.5.40')
	tweets_col = mongo_conn['coleta_{0}'.format(contexto)]['tweets_{0}'.format(contexto)]
	unload_col = mongo_conn['coleta_{0}'.format(contexto)]['unload_tweets_{0}'.format(contexto)]

	cursor.execute("SELECT id, tweet_id FROM painel_tweetpopular WHERE descricao IS NULL OR descricao = '' AND data_referencia = %s", [data_ref])
	updater = conn.cursor()
	for _id, tweet_id in cursor:
		tweet = tweets_col.find_one({'_id': int(tweet_id)}, {'text': 1})
		if not tweet:
			tweet = unload_col.find_one({'_id': int(tweet_id)}, {'text': 1})
		if tweet:
			updater.execute('UPDATE painel_tweetpopular SET descricao = %s WHERE id = %s', [tweet['text'], _id])
		#else:
		#	print "Não encontrado {0}".format(tweet_id)
	conn.commit()
	updater.close()
	cursor.close()
	conn.close()

if __name__ == '__main__':
	parser = OptionParser()
	parser.add_option("-d", "--db", dest="db", help=u"Database name in portal", metavar="DATABASE")
	parser.add_option("-i", "--input", dest="filename", help=u"Input file", metavar="INPUT")
	parser.add_option("-g", "--geral", dest="geral", help=u"Geral", metavar="GERAL")
	parser.add_option("-c", "--ctx", dest="ctx", help=u"Context", metavar="CONTEXT")

	filterwarnings('ignore', category = MySQLdb.Warning)
	(options, args) = parser.parse_args()
	
	processa(db=options.db, filename=options.filename, contexto=options.ctx, geral=options.geral)

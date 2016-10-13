import os
import pymongo
import MySQLdb
import sys
from collections import defaultdict

c = pymongo.Connection('10.5.5.40', read_preference=pymongo.ReadPreference.SECONDARY_ONLY)
tcol = c['coleta_131']['tweets_131']
tunloadcol = c['coleta_131']['unload_tweets_131']

INSERT_SQL_1 = '''INSERT INTO conteudo_itemconteudodestaque(conteudo_id, texto, autor, data_publicacao, id_original, id_autor, imagem, score)
				VALUES (%s, %s, %s, %s, %s, %s, %s, %s)'''
INSERT_SQL_2 = '''INSERT INTO conteudo_evolucaoitemconteudodestaque(item_id, data, score) VALUES (%s, %s, %s) ON DUPLICATE KEY UPDATE score = %s'''

conn = MySQLdb.connect (host='mysql1.ctweb.inweb.org.br', db='bbb13',
					   user='proc', passwd='pr0ce55@', compress=True, port=3306, charset='utf8', use_unicode=True)

evolucao_score =  defaultdict(lambda: defaultdict(defaultdict))

id_conteudo = 1
cursor = conn.cursor()
cursor.execute('''UPDATE conteudo_conteudodestaque SET data_inicial = %s, data_final = %s WHERE id = %s''', [first_date, last_date, id_conteudo]) 

tweets_date = defaultdict(list)
name = sys.argv[1]
print "Dados processados para {0}".format(name)
date = name[1:]
with open(date, 'r') as f:
	# Assume-se que os dados no arquivo estao ordenados pelo score.
	# Apenas os top 1000 entram.
	for c, line in enumerate(f):
		#if c > 1000:
		#	break
		(tweet_id, score, img, user, data, link, entities ) = line.strip().split('\t')
		evolucao_score[tweet_id][date[1:]] = score
		tweets_date[date].append((tweet_id, score)) 

total = len(evolucao_score)
for counter, (tweet_id, values) in enumerate(evolucao_score.iteritems()):
	datas = sorted(values.iterkeys())
	result = []
	max_score = 0
	for date in datas:
		score = float(values[date]) * .001
		result.append((date, score))
		if max_score < score:
			max_score = score
			 
	##print ','.join(result)
	obj = tcol.find_one({'_id': int(tweet_id)}, {'text': 1, 'created_at': 1, 'user.name': 1, 'user.screen_name': 1, 'user.profile_image_url': 1})
	if obj is None:
		obj = tunloadcol.find_one({'_id': int(tweet_id)}, {'text': 1, 'created_at': 1, 'user.name': 1, 'user.screen_name': 1, 'user.profile_image_url': 1})

	if obj:
		cursor.execute("SELECT id FROM conteudo_itemconteudodestaque WHERE id_original = %s", tweet_id)
		row = cursor.fetchone()
		if row:
			last_id = row[0]
		else:
			insert_values = [id_conteudo, obj['text'].replace(r'\/', '/'), obj['user']['name'], obj['created_at'], 
					tweet_id, obj['user']['screen_name'], obj['user']['profile_image_url'].replace(r'\/', '/'), max_score ];
			cursor.execute(INSERT_SQL_1, insert_values)
			last_id = cursor.lastrowid

		for date, score in result:
			cursor.execute(INSERT_SQL_2, [last_id, date, score, score])
		#	print (INSERT_SQL_2, [last_id, date, score, score])
	else:
		print "Not found: ", tweet_id
	if counter and (counter % 100 == 0):
		print "Processados %d / %d" % (counter, total)

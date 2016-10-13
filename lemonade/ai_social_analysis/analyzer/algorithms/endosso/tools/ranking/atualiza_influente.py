# -*- coding: utf-8 -*-
import datetime
import MySQLdb
import csv
import sys
import os
from optparse import OptionParser

def processa(db, filename, contexto, geral):

	conn = MySQLdb.connect(host = 'mysql1.ctweb.inweb.org.br', port=3306, user = 'proc', passwd = 'pr0ce55@', 
		db = db, use_unicode=True, charset = 'utf8') 
	cursor = conn.cursor()
	cursor.execute('SELECT usuario, id FROM autores_usuarioinfluente')
	usuarios_cadastrados = dict([row for row in cursor])
	
		
	insert_user = "INSERT IGNORE INTO autores_usuarioinfluente(contexto_id, usuario, foto_usuario, habilitado) VALUES (%s, %s, %s, %s)"
	insert_evolucao = """INSERT INTO autores_evolucaousuarioinfluente(data_referencia, periodo, score,usuario_influente_id, posicao)	
						VALUES (%s, %s, %s, %s, %s) ON DUPLICATE KEY UPDATE score = %s, posicao = %s;"""
	
	insert_associacao_pessoa = """INSERT INTO autores_pessoausuarioinfluente(usuario_influente_id, pessoa_id, data_referencia, periodo, score, posicao)	
						VALUES (%s, %s, %s, %s, %s, %s) ON DUPLICATE KEY UPDATE score = %s, posicao = %s;"""

	print "Processando arquivo {0}".format(filename)
	with open(filename) as f:
		reader = csv.reader(f, delimiter='\t')
		for i, (user, score, photo) in enumerate(reader):
			if not user in usuarios_cadastrados:
				total = cursor.execute(insert_user, [contexto, user, 'http://a0.twimg.com/profile_images/' + photo, 1])
				usuarios_cadastrados[user] = cursor.lastrowid
				if cursor.lastrowid == 0:
					cursor.execute('SELECT usuario, id FROM autores_usuarioinfluente WHERE usuario = %s ', [user])
					user, uid = cursor.fetchone()
					usuarios_cadastrados[user] = uid


			influente = usuarios_cadastrados[user]
			score = float(score)
			if geral:
				cursor.execute(insert_evolucao, [filename[1:9], 'd', score, influente, i + 1, score, i + 1 ])
			else:
				cursor.execute(insert_associacao_pessoa, [influente, entity_id, filename[1:9], 'd', score, i + 1, score, i + 1])
				#print influente, entity_id, filename[1:9], 'd', score, score 
			if i % 1000 == 0 and i:
				conn.commit()
				print "Processados {0}".format(i)
		print "Inseridos ou atualizados: {0} autores".format(i)
	conn.commit()
	cursor.close()
	conn.close()
if __name__ == '__main__':
	parser = OptionParser()
	parser.add_option("-d", "--db", dest="db", help=u"Database name in portal", metavar="DATABASE")
	parser.add_option("-i", "--input", dest="filename", help=u"Input file", metavar="INPUT")
	parser.add_option("-g", "--geral", dest="geral", help=u"Geral", metavar="GERAL")
	parser.add_option("-c", "--ctx", dest="ctx", help=u"Context", metavar="CONTEXT")


	(options, args) = parser.parse_args()
	
	processa(db=options.db, filename=options.filename, contexto=options.ctx, geral=options.geral)

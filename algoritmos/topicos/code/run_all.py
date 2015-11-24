# Gisele L. Pappa, 10/2015
# Extracao de Topicos com LDA
# Documento ignora acentuacao para evitar problemas de encoding
# Script para pre-processar os comentarios de uma consulta, rodar o LDA e processar sua saida
# Gera um conjunto de topicos descrito por um conjunto de palavras
# Para cada topico, mostra os comentarios mais relevantes 
# Arquivos gerados de interesse: docs_per_topic__name, onde name eh o nome do arquivos de entrada
# docs_per_topic contem um topico seguido das palavras que o descrevem e a lista de documentos associados ao topico 
#!/usr/bin/python
from unicodedata import normalize
from collections import defaultdict
import sys
import re
from nltk.corpus import stopwords
import os


#Parametros definidos pelo ususario:
# docs        -  diretorio onde esta o texto a ser processado
# output_dir  -  diretorio onde sera salva a saida pos-processada do LDA
# idx_id      -  indica a coluna do id do comentario no arquivo de entrada
# idx_text    -  indica a coluna do texto no arquivo de entrada
# suffix      -  sufixo adicionado ao nome do arquivo de saida
# num_topics  -  numero de topicos que serao gerados pelo LDA
class Runner(object):
	def __init__(self, docs, output_dir, idx_id, idx_text, suffix, num_topics):
		self.docs         = docs
		self.output_dir   = output_dir
		self.idx_id       = idx_id
		self.idx_text     = idx_text
		self.suffix       = suffix
		self.num_topics   = num_topics
		self.my_stopwords = self.get_my_stopwords()
		self.vocab        = set()
		self.ids          = set()

		os.system('mkdir -p %s' % self.output_dir)

# Define o conjunto de stop words (palavras comuns na lingua portuguesa, como artigos, pronomes, etc)
# Usa o arquivo my_stopwords.txt
	def get_my_stopwords(self):
		return [w.strip() for w in open('./my_stopwords.txt', 'r')]

# Remove do texto caracteres indesejaveis e stopwords
	def get_clean(self, text):
		result = [w.strip().lower() for w in re.split(r'[ -()_?!.<>&:{}[]|\\/;,\"\'\`]\s*', text) \
	if w.strip().isalpha() and w.strip() not in stopwords.words('portuguese')]
		result = [w for w in result if w not in self.my_stopwords and w not in stopwords.words('english')]
		return result

# Remove acentos
	def remover_acentos(self, txt, codif="utf-8"):
		return normalize('NFKD', txt.decode(codif)).encode('ASCII', 'ignore')

# Gera e salva os documentos no formato necessario para rodar o LDA. 
# O arquivo utilizado pelo LDA tem na primeira linha o numero de documentos
# As linhas seguintes representam um documento atraves de uma lista de palavras
# Como o id eh removido da entrada do LDA, salvamos tambem um arquivo com os 
# identificadores dos documentos para que possam ser recuperados posteriormente
	def dump_lda_format(self, docs):
		out_lda = open('%s/lda_input_%s.txt' % (self.output_dir, self.suffix), 'w')
		out_ids = open('%s/lda_ids_%s.txt' % (self.output_dir, self.suffix), 'w')
		out_lda.write('%s\n' % len(docs))
		out_ids.write('%s\n' % len(docs))
		for id in docs:
			out_lda.write('%s\n' % docs[id])
			out_ids.write('%s\n' % id)
		out_lda.close()
		out_ids.close()

#  Gera e salva os documentos pre-processados
	def dump_raw_documents(self, dict_raw_docs):
		out_raw_docs = open('%s/raw_documents_%s.txt' % (self.output_dir, self.suffix), 'w')
		out_raw_docs.write('%s\n' % len(dict_raw_docs))
		for id in dict_raw_docs:
			out_raw_docs.write('%s\n' % dict_raw_docs[id])
		out_raw_docs.close()


# Limpa o texto, gerando as entradas para LDA
	def clean_text(self):
		dict_docs     = defaultdict(lambda: None)
		dict_raw_docs = defaultdict(lambda: None)
		for line_id, line in enumerate(open(self.docs, 'r')):
			if line_id > 0:
				tokens = line.strip().split(',')
				comment_id   = tokens[self.idx_id]
				comment_text = tokens[self.idx_text:len(tokens)]
				tmp_doc      = self.remover_acentos(' '.join(comment_text))
				doc          = self.get_clean(tmp_doc)
	
				if len(doc) > 0:
					dict_docs[comment_id]     = ' '.join(doc)
					dict_raw_docs[comment_id] = ' '.join(comment_text).strip()
		
					self.vocab.update(doc)
					self.ids.add(comment_id)
	
		self.dump_lda_format(dict_docs)
		self.dump_raw_documents(dict_raw_docs)
		
		# numero de palavras presentes nos documentos
		print '# words in vocabulary %s' % len(self.vocab)
		# numero de documentos
		print '# number of documents %s' % len(self.ids)

# Roda o LDA
# Paramteros de entrada do LDA:
# $ lda -est [-alpha <double>] [-beta <double>] [-ntopics <int>] \
#     [-niters <int>] [-savestep <int>] [-twords <int>] -dfile <string>
#   in which (parameters in [] are optional):
#   -est: 
#       ESTimate the LDA model from scratch
#   -alpha <double>: 
#       The value of alpha, hyper-parameter of LDA. The default value
#       of alpha is 50 / K (K is the the number of topics). 
#   -beta <double>:
#       The value of beta, a hyper-parameter of LDA. Its default value is 0.1 
#   -ntopics <int>:
#       The number of topics. Its default value is 100.
#   -niters <int>:
#       The number of Gibbs sampling iterations. The default value is 2000.
#   -savestep <int>:
#       The step (counted by the number of Gibbs sampling iterations) at which
#       the LDA model is saved to hard disk. The default value is 200.
#   -twords <int>:
#       The number of most likely words for each topic to be printed. 
# 		The default value is zero.
#   -dfile <string>:
#       The input training data file. See Section 3.2 for a description of 
#       input data format.
	def run_lda(self):
		lda_cmd = '../GibbsLDA++-0.2/src/lda -est -alpha 0.5 -beta 0.1 -ntopics %s \
              -niters 1000 -savestep 100 -twords 10 \
              -dfile %s/lda_input_%s.txt' % (num_topics, self.output_dir, self.suffix)
		os.system(lda_cmd)


# Le a saida do LDA e, a partir dela, gera uma lista de documentos por topico.
# No momento, cada documento esta associado a apenas um topico.
# Gera 4 arquivos:
# document_topic contem a lista de termos descrevendo os topicos
# topic_rank contem contem, para cada topico, o numero de documentos associados a ele, seguido dos termos que o descrevem
# docs_per_topic contem a lista de documentos por topico
# docs_per_topic_expand contem, alem da lista de documentos pre-processados por topico, o texto utilizado pelo LDA
	def get_docs_per_topic(self):
		cmd = 'python rankDominantTopics.py %s' % (self.output_dir)
		os.system(cmd)
		os.system('mv document_topic.txt %s' % (self.output_dir))
		os.system('mv topic_rank.txt %s' % (self.output_dir))
		raw_docs       = defaultdict(lambda: None)
		desc_topics    = defaultdict(lambda: None)
		docs_per_topic = defaultdict(lambda: set())
		lda_input      = defaultdict(lambda: None)

		for id_doc, line in enumerate(open('%s/raw_documents_%s.txt' % (self.output_dir, self.suffix), 'r')):
			if id_doc > 0: # ignoring header
				raw_docs[id_doc] = line.strip()

		for id_doc, line in enumerate(open('%s/document_topic.txt' % (self.output_dir), 'r')):
			if id_doc > 0: # ignoring header
				topic_id, topic_words = line.strip().split()
				desc_topics[topic_id] = topic_words
				docs_per_topic[id_doc].add(topic_id)

		for id_doc, line in enumerate(open('%s/lda_input_%s.txt' % (self.output_dir, self.suffix), 'r')):
			if id_doc > 0: # ignoring header
				lda_input[id_doc] = line.strip()

		output        = open('%s/docs_per_topic_%s.txt' % (self.output_dir, self.suffix), 'w')
		output_expand = open('%s/docs_per_topic_expand_%s.txt' % (self.output_dir, self.suffix), 'w')
		for topic_id in desc_topics:
			output.write('TOPIC %s : %s\n' % (topic_id, desc_topics[topic_id]))
			output_expand.write('TOPIC %s : %s\n' % (topic_id, desc_topics[topic_id]))
			for doc_id in docs_per_topic:
				output.write('\t -> %s\n' % raw_docs[doc_id])
				output_expand.write('\t -> %s\n' % raw_docs[doc_id])
				output_expand.write('\t -> [%s]\n' % lda_input[doc_id])
		output.close()
		output_expand.close()


# Utilizado para rodar todos os passos implementados acima
# Limpa o texto, roda o LDA e pps-processa a saida do LDA
# Parametros que podem ser modificados pelo usuario:
# docs_comentarios  -  diretorio onde esta o texto a ser processado
# out_comentarios   -  diretorio onde sera salva a saida pos-processada do LDA
# num_topics        -  numero de topicos que serao gerados pelo LDA
if __name__ == '__main__':
	docs_comentarios    = '../../../dados/dadospessoais-comentarios-pdfs-filtrado.csv'
	out_comentarios     = '../data/output/'
	num_topics          = 52
	runner_comentarios  = Runner(docs_comentarios, out_comentarios, 1, 10, 'comentarios', num_topics)
	runner_comentarios.clean_text()
	runner_comentarios.run_lda()
	runner_comentarios.get_docs_per_topic()
 
#   Usado para extrair topicos do texto do projeto de lei 
#	docs_texto_projeto    = '../data/texto_projeto.csv'
#	out_texto_projeto     = '../data/output_texto_projeto/'
#	num_topics            = 52
#	runner_texto_projeto  = Runner(docs_texto_projeto, out_texto_projeto, 0, 7, 'texto_projeto', num_topics)
#	runner_texto_projeto.clean_text()
#	runner_texto_projeto.run_lda()
#	runner_texto_projeto.get_docs_per_topic()


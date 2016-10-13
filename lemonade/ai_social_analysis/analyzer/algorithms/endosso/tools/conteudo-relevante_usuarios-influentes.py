#encoding: utf-8

import json
import re
import sys

dataset_input = sys.argv[1]
output_dir = sys.argv[2]

# ---> Identificação de conteúdo relevante 1: endosso/concordância

entrada = open(dataset_input, 'r')
saida = open('{}/20000101'.format(output_dir), 'w')

for linha in entrada:
  obj = json.loads(linha)
  if obj["comment_parent"] == '0': #comentários de itens: usar commentable_id
    if u'não concordo' not in obj["comment_text"].lower():
      if 'concordo' in obj["comment_text"].lower():
        saida.write("u_{}\tc_{}\ti_{}\t{}\t{}\t{}\n".format(obj["author_id"], obj["comment_id"], obj["commentable_id"], obj["comment_date"], 'arg', 'arg'))
  else: #comentários de comentários: usar comment_parent
    if u'não concordo' not in obj["comment_text"].lower():
      if 'concordo' in obj["comment_text"].lower():
        saida.write("u_{}\tc_{}\tc_{}\t{}\t{}\t{}\n".format(obj["author_id"], obj["comment_id"], obj["comment_parent"], obj["comment_date"], 'arg', 'arg'))

entrada.close()
saida.close()

# ---> Identificação de conteúdo relevante 2: reprovação/discordância
    
entrada = open(dataset_input, 'r')
saida = open('{}/20000202'.format(output_dir), 'w')

for linha in entrada:
  obj = json.loads(linha)
  if obj["comment_parent"] == '0': #comentários de itens: usar commentable_id
    if 'discordo' in obj["comment_text"].lower():
      saida.write("u_{}\tc_{}\ti_{}\t{}\t{}\t{}\n".format(obj["author_id"], obj["comment_id"], obj["commentable_id"], obj["comment_date"], 'arg', 'arg'))
    if u'não concordo' in obj["comment_text"].lower():
      saida.write("u_{}\tc_{}\ti_{}\t{}\t{}\t{}\n".format(obj["author_id"], obj["comment_id"], obj["commentable_id"], obj["comment_date"], 'arg', 'arg'))
  else: #comentários de comentários: usar comment_parent
    if 'discordo' in obj["comment_text"].lower():
      saida.write("u_{}\tc_{}\tc_{}\t{}\t{}\t{}\n".format(obj["author_id"], obj["comment_id"], obj["comment_parent"], obj["comment_date"], 'arg', 'arg'))
    if u'não concordo' in obj["comment_text"].lower():
      saida.write("u_{}\tc_{}\tc_{}\t{}\t{}\t{}\n".format(obj["author_id"], obj["comment_id"], obj["comment_parent"], obj["comment_date"], 'arg', 'arg'))

entrada.close()
saida.close()

# ---> Identificação de usuários influentes

entrada = open(dataset_input, 'r')
saida = open('{}/20000303'.format(output_dir), 'w')

comment_parent_ids = set()

for linha in entrada:
  obj = json.loads(linha)
  if obj["comment_parent"] != '0': #comentários de comentários
    if re.match("(concordo)|(discordo)", obj["comment_text"].lower()):
      comment_parent_ids.add(obj["comment_parent"])

entrada.seek(0)

for linha in entrada:
  obj = json.loads(linha)
  if obj["comment_id"] in comment_parent_ids:
    saida.write("u_{}\t{}\t{}\t{}\t{}\t{}\n".format(obj["author_id"], obj["comment_id"], 'arg', 'arg', 'arg', 'arg'))

entrada.close()
saida.close()


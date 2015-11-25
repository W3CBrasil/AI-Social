import json
import re
import collections
import numpy as np
import pandas as pd
import unicodedata
import math

def strip_accents(s):
    return ''.join(c for c in unicodedata.normalize('NFD', s)
            if unicodedata.category(c) != 'Mn')

def normaliza_texto(texto):
    return strip_accents(texto.strip().lower())


anteprojeto = pd.read_csv("../../dados/dadospessoais-anteprojeto.csv", 
                          dtype={"commentable_id"      : pd.core.common.CategoricalDtype,
                                 "commentable_parent"  : pd.core.common.CategoricalDtype,
                                 "commentable_article" : pd.core.common.CategoricalDtype,
                                 "commentable_chapter" : pd.core.common.CategoricalDtype,
                                 "commentable_axis"    : pd.core.common.CategoricalDtype,
                                 "commentable_type"    : pd.core.common.CategoricalDtype,
                                 "commentable_name"    : pd.core.common.CategoricalDtype,
                                 "commentable_text"    : np.character})


comentarios = pd.read_csv("../../dados/dadospessoais-comentarios-pdfs-filtrado.csv",
                          parse_dates=['comment_date'],
                          dtype={"source"         : pd.core.common.CategoricalDtype, 
                                 "comment_id"     : pd.core.common.CategoricalDtype, 
                                 "author_id"      : pd.core.common.CategoricalDtype, 
                                 "author_name"    : np.character, 
                                 "comment_parent" : pd.core.common.CategoricalDtype, 
                                 "commentable_id" : pd.core.common.CategoricalDtype, 
                                 "topic"          : pd.core.common.CategoricalDtype, 
                                 "comment_text"   : np.character })

df = pd.merge(comentarios, anteprojeto, on="commentable_id")

# Contagem de comentarios por topico
contagem_comentarios = dict(df.groupby(["topic"]).size())

# Contagem de comentarios por artigo por topico
nomes_artigos = dict([(row[1][0], (row[1][1], row[1][2])) for row in anteprojeto[["commentable_id", "commentable_name", "commentable_axis"]].iterrows()])
nomes_artigos["0"] = ("Nenhum", 0)
contagem_artigos = collections.defaultdict(list)
for (topico, artigo), contagem_ in df.groupby(["topic", "commentable_article"]).size().to_frame().iterrows():
    nome_artigo, eixo_artigo = nomes_artigos[artigo]
    eixo_artigo = "TCE{}".format(eixo_artigo)
    contagem = int(contagem_[0])
    contagem_artigos[topico].append([nome_artigo, eixo_artigo, contagem])
for topico in contagem_artigos.keys():
    contagem_artigos[topico] = sorted(contagem_artigos[topico], key=lambda a: a[2], reverse=True)

# Contagem de comentarios por eixo
contagem_eixos = dict(df.groupby(["commentable_axis"]).size())

# Contagem de comentarios por eixo por topico
contagem_eixos_topicos = collections.defaultdict(list)
for (topico, eixo), contagem_ in df.groupby(["topic", "commentable_axis"]).size().to_frame().iterrows():
    nome_eixo = "Eixo {}".format(eixo)
    eixo_artigo = "TCE{}".format(eixo)
    contagem = int(contagem_[0])
    contagem_eixos_topicos[topico].append((eixo, contagem))
contagem_eixos_completo = collections.defaultdict(list)
for topico in contagem_eixos_topicos.keys():
    contagens = collections.Counter(dict(contagem_eixos_topicos[topico]))
    for eixo in map(str, range(1, 14)):
        eixo_nome = "TCE{}".format(eixo)
        eixo_contagem = contagens[eixo] 
        eixo_contagem_norm = 100.0 * (contagens[eixo]/contagem_eixos[eixo] )
        contagem_eixos_completo[topico].append({"group":eixo_nome, "count":eixo_contagem, "count_norm":eixo_contagem_norm})

# Top 10 palavras 
top_palavras = []
with open("../../data/inteligencia/topicos/model-00100.twords", "r") as infile:
    for line in infile:
        if re.search("^Topic", line):
            nome_topico = line.strip()
            top_palavras.append([])
        else:
            busca = re.search("\s+(\w+)\s+([\d\.]+)", line)
            if busca: 
                palavra = busca.group(1)
                score = float(busca.group(2))
                top_palavras[-1].append([palavra, score])
            else:
                print("ERROR: line=\"{}\"".format(line.rstrip()))

top_palavras_dict = [dict(a) for a in top_palavras]

# Comentarios
comentarios_topicos = collections.defaultdict(list)
for row_meta, (topico, autor, texto, eixo) in df[["topic", "author_name", "comment_text", "commentable_axis"]].iterrows():
    if not math.isnan(float(topico)):
        topico = int(topico)
        texto_novo = ""
        for palavra in re.split("(\w+)", texto):
            palavra_norm = normaliza_texto(palavra)
            if palavra_norm in top_palavras_dict[topico]:
                palavra_nova = "<mark>{}</mark>".format(palavra)
            else:
                palavra_nova = palavra
            texto_novo += palavra_nova 
        comentarios_topicos[str(topico)].append({"autor":autor, "eixo":"TCE{}".format(eixo), "texto":texto_novo})

# Topicos
topicos = [{"nome": "Topico {}".format(i),
            "contagem_comentarios": int(contagem_comentarios[i]),
            "top_palavras":top_palavras[int(i)],
            "frequencia_artigos":contagem_artigos[i][:10],
            "frequencia_eixos":contagem_eixos_completo[i],
            "comentarios":comentarios_topicos[i]
            } 
                for i in map(str, range(len(top_palavras)))
          ]

with open("../../data_web/tabela_topicos.json", "w") as outfile:
    outfile.write(json.dumps({"topicos": topicos}))
    #for topico in topicos:
    #    outfile.write("{}\n".format(json.dumps(topico)))
    

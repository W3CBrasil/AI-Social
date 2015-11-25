import json
import csv
import numpy as np
import pandas as pd

# Configura contextos
contextos = [
    {"arquivo_comentarios":"../../dados/dadospessoais-comentarios-pdfs-filtrado.csv",
     "prefixo_saida":"../data/"}
]
#for i in range(1, 7):
#    obj = {"arquivo_comentarios" : "../../data/contextos/dadospessoais-comentarios-pdfs-filtrado-contexto{}.csv".format(i),
#           "prefixo_saida"       : "../../data_web/contexto{}/".format(i)}
#    contextos.append(obj)

anteprojeto = pd.read_csv("../../dados/dadospessoais-anteprojeto.csv", 
                          dtype={"commentable_id"      : pd.core.common.CategoricalDtype,
                                 "commentable_parent"  : pd.core.common.CategoricalDtype,
                                 "commentable_article" : pd.core.common.CategoricalDtype,
                                 "commentable_chapter" : pd.core.common.CategoricalDtype,
                                 "commentable_axis"    : pd.core.common.CategoricalDtype,
                                 "commentable_type"    : pd.core.common.CategoricalDtype,
                                 "commentable_name"    : pd.core.common.CategoricalDtype,
                                 "commentable_text"    : np.character})

for contexto in contextos:
    
    comentarios = pd.read_csv(contexto["arquivo_comentarios"], 
                              parse_dates=['comment_date'],
                              dtype={"source"         : pd.core.common.CategoricalDtype, 
                                     "comment_id"     : pd.core.common.CategoricalDtype, 
                                     "author_id"      : pd.core.common.CategoricalDtype, 
                                     "author_name"    : np.character, 
                                     "comment_parent" : pd.core.common.CategoricalDtype, 
                                     "commentable_id" : pd.core.common.CategoricalDtype, 
                                     "comment_text"   : np.character })
    
    df = pd.merge(comentarios, anteprojeto, on="commentable_id")
    
    df["author_id"] = df["author_id"].apply(lambda value: "TCP{}".format(value))
    df["commentable_axis"] = df["commentable_axis"].apply(lambda value: "TCE{}".format(value))
    df["commentable_id"] = df["commentable_id"].apply(lambda value: "TCI{}".format(value))
    
    df.to_csv("{}texto_comentario_participantes.tsv".format(contexto["prefixo_saida"]), sep="\t", encoding="utf-8", 
              index=False, quoting=csv.QUOTE_NONE, columns=["author_id", "comment_text"], header=["id", "comentario"])
    
    df.to_csv("{}texto_comentario_eixos.tsv".format(contexto["prefixo_saida"]), sep="\t", encoding="utf-8", 
              index=False, quoting=csv.QUOTE_NONE, columns=["commentable_axis", "comment_text"], header=["id", "comentario"])
    
    df.to_csv("{}texto_comentario_itens.tsv".format(contexto["prefixo_saida"]), sep="\t", encoding="utf-8", 
              index=False, quoting=csv.QUOTE_NONE, columns=["commentable_id", "comment_text"], header=["id", "comentario"])
    

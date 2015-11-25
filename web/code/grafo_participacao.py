import json
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
    comentarios.drop(["commentable_name"], axis=1, inplace=True)
    
    df = pd.merge(comentarios, anteprojeto, on="commentable_id")
    
    
    # Grafo
    
    grafo = {"nodes":[], "links":[]}
    
    
    # Usuarios
    
    top_usuarios = df.groupby(["author_id", "author_name"]).size()
    top_usuarios.sort_values(ascending=False)
    
    usuarios = [{"group":0, "type":
                 "user", "id":"TCP{}".format(author_id), 
                 "name":author_name, 
                 "value":int(count)} 
                     for (author_id, author_name), count in top_usuarios.iteritems()]
    
    grafo["nodes"].extend(usuarios)
    
    
    # Itens
    
    top_itens = df.groupby(["commentable_id", "commentable_name", "commentable_axis"]).size()
    itens = [{"id":"TCI{}".format(commentable_id), 
              "name":commentable_name,
              "type":"item",
              "group":int(commentable_axis), 
              "value":int(count)} 
                  for (commentable_id, commentable_name, commentable_axis), count in top_itens.iteritems()]
    
    
    # Contagem de comentarios em itens
    
    frequencia = df.groupby(["author_id", "commentable_id"]).size()
    
    
    # Filtra itens que não foram comentados
    
    itens_selecionados = set()
    for (usuario, item), count in frequencia.iteritems():
        itens_selecionados.add(item)
    
    itens = [item for item in itens if item["id"][3:] in itens_selecionados]
    
    grafo["nodes"].extend(itens)
    
    
    # Nodos do grafo
    
    nodos_ids = {nodo["id"]:i for i, nodo in enumerate(grafo["nodes"])}
    
    
    # Criação dos eixos do grafo
    
    for (usuario, item), count in frequencia.iteritems():
        grafo["links"].append({"source":nodos_ids["TCP{}".format(usuario)], 
                               "target":nodos_ids["TCI{}".format(item)], 
                               "value":int(count)})
    
    # Saida
    
    with open("{}participantes_itens_comentaveis_graph.json".format(contexto["prefixo_saida"]), "w") as outfile:
        json.dump(grafo, outfile, indent=4)
    

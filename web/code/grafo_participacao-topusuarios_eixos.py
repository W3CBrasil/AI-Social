import json
import numpy as np
import pandas as pd

# Configura contextos
contextos = [
    {"arquivo_comentarios":"../../dados/dadospessoais-comentarios-pdfs-filtrado.csv",
     "prefixo_saida":"../site/data/"}
]
#for i in range(1, 7):
#    obj = {"arquivo_comentarios" : "../../data/contextos/dadospessoais-comentarios-pdfs-filtrado-contexto{}.csv".format(i),
#           "prefixo_saida"       : "../../data_web/contexto{}/".format(i)}
#    contextos.append(obj)


anteprojeto = pd.read_csv("../../data/dadospessoais-anteprojeto.csv", 
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
    
    
    # Grafo
    
    grafo = {"nodes":[], "links":[]}
    
    
    # Usuarios
    
    top_usuarios = df.groupby(["author_id", "author_name"]).size()
    top_usuarios.sort(ascending=False)
    usuarios_selecionados = set([a[0][0] for a in top_usuarios[:10].iteritems()])
    usuarios = [{"group":0, "type":
                 "user", "id":"TCP{}".format(author_id), 
                 "name":author_name, 
                 "value":int(count)} 
                     for (author_id, author_name), count in top_usuarios[:10].iteritems()]
    
    grafo["nodes"].extend(usuarios)
    

    # Eixos

    top_eixos = df.groupby("commentable_axis").size()
    eixos = [{"group": i, 
              "type": "axis", 
              "id": "TCE{}".format(i), 
              "name": "Eixo {}".format(i), 
              "value":int(count)} 
                  for (i), count in top_eixos.iteritems()]
    
    grafo["nodes"].extend(eixos)
    
     
    # Nodos do grafo
    
    nodos_ids = {nodo["id"]:i for i, nodo in enumerate(grafo["nodes"])}
    
    
    # Contagem de comentarios em eixos
    
    frequencia = df[df["author_id"].isin(usuarios_selecionados)].groupby(["author_id", "commentable_axis"]).size()
    
    for (usuario, eixo), count in frequencia.iteritems():
        grafo["links"].append({"source":nodos_ids["TCP{}".format(usuario)], 
                               "target":nodos_ids["TCE{}".format(eixo)], 
                               "value":int(count)})
    
    
    # Saida
    
    with open("{}participantes_eixos_graph.json".format(contexto["prefixo_saida"]), "w") as outfile:
        json.dump(grafo, outfile, indent=4)
    

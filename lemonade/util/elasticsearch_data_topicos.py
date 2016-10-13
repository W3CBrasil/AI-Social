import json
import elasticsearch
import collections

INDEX_NAME = "dadospessoais_7"

def dados_vis_topicos():

    es = elasticsearch.Elasticsearch()

    search_body = {
        "aggs" : {
            "contagem_comentarios" : { 
                "terms" : { 
                    "field": "topico_palavras", 
                    "size" : 0
                } 
            },
            "frequencia_eixos" : { 
                "terms" : { 
                    "script": "[doc['topico_id'].value + '/' + doc['commentable_axis'].value]",
                    "size": 0
                } 
            } 
        }
    }

    raw_result = es.search(index=INDEX_NAME, search_type="count", body=search_body)
    
    # Frequencia por eixo
    frequencia_eixos = collections.defaultdict(list)
    for topico_eixo in raw_result["aggregations"]["frequencia_eixos"]["buckets"]:

        topico_id, commentable_axis = topico_eixo["key"].split("/")
        contagem = topico_eixo["doc_count"]

        frequencia_eixos[topico_id].append((commentable_axis, contagem))


    # Informações completas de cada tópico
    result = []
    for topico in raw_result["aggregations"]["contagem_comentarios"]["buckets"]:

        topico_id, top_palavras = json.loads(topico["key"])
        contagem_comentarios = topico["doc_count"]

        topico_info = {
            "topico_id"            : topico_id,
            "nome"                 : "Topico {}".format(topico_id),
            "top_palavras"         : top_palavras,
            "contagem_comentarios" : contagem_comentarios,
            "frequencia_eixos"     : [ [commentable_axis, contagem]
                                           for commentable_axis, contagem in sorted(frequencia_eixos[topico_id], 
                                               key=lambda a:a[1], reverse=True) ][:10]
        }

        result.append(topico_info)

    return result


print(json.dumps(dados_vis_topicos()))


import sys
import json
import elasticsearch

es = elasticsearch.Elasticsearch()

# curl 'http://localhost:9200/dadospessoais/comment/_search?search_type=count' -d '{"aggs" : {"contagem" : { "terms" : { "field" : "commentable_axis", "size":20 } } } }'

search_body = {
    "aggs" : {
        "numero_comentarios": {
            "cardinality" : {
                "field" : "comment_id"
            }
        },
        "numero_participantes": {
            "cardinality" : {
                "field" : "author_id"
            }
        },
        "contagem_itens" : { 
            "terms" : { 
                "script": "[doc['commentable_id'].value + ' / ' + doc['commentable_name'].value]",
                "size": 10 
            } 
        },
        "contagem_artigos" : { 
            "terms" : { 
                "script": "[doc['commentable_article'].value + ' / ' + doc['commentable_article_name'].value]",
                "size": 10 
            } 
        },
        "contagem_participantes" : { 
            "terms" : { 
                "script": "[doc['author_id'].value + ' / ' + doc['author_name'].value]",
                "size": 10 
            } 
        },
        "contagem_eixos" : { 
            "terms" : { 
                "field": "commentable_axis", 
                "size":20 
            } 
        } 
    }
}

#print(json.dumps(search_body))

raw_result = es.search(index="dadospessoais", search_type="count", body=search_body)

def format_list_output(groups):
    groups_output = []
    for group in groups:
        element_id, element_name = group["key"].split(" / ", 1)
        elemento = {"id":element_id, "name":element_name, "count":group["doc_count"]}
        groups_output.append(elemento)
    return groups_output

result = {}
result["numero_comentarios"] = raw_result["aggregations"]["numero_comentarios"]["value"]
result["numero_participantes"] = raw_result["aggregations"]["numero_participantes"]["value"]
result["contagem_itens"] = format_list_output(raw_result["aggregations"]["contagem_itens"]["buckets"])
result["contagem_artigos"] = format_list_output(raw_result["aggregations"]["contagem_artigos"]["buckets"])
result["contagem_participantes"] = format_list_output(raw_result["aggregations"]["contagem_participantes"]["buckets"])
result["contagem_eixos"] = [ {"id":group["key"], "name":"Eixo {}".format(group["key"]), "count":group["doc_count"]} 
                                 for group in sorted(raw_result["aggregations"]["contagem_eixos"]["buckets"], 
                                                     key=lambda a:int(a["key"]))                                   ]

#print(json.dumps(raw_result))
print(json.dumps(result))


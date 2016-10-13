import sys
import json
import elasticsearch
import elasticsearch.helpers

es = elasticsearch.Elasticsearch()

result = {"comentarios":[], 
            "termos_comentarios":[], 
            "termos_itemlei":[]}

raw_result = es.search(index="dadospessoais", size=5000, from_=0)

for obj in raw_result["hits"]["hits"]:
    obj = obj["_source"]
    obj_new = {
        "id"             : int(obj["comment_id"]),
        "commentable_id" : int(obj["commentable_id"]),
        "commentable"    : obj["commentable_name"],
        "author_id"      : int(obj["author_id"]),
        "author"         : obj["author_name"],
        "axis_id"        : int(obj["commentable_axis"]),
        "axis"           : "Eixo {}".format(obj["commentable_axis"]),
        "article_id"     : int(obj["commentable_article"]),
        "article"        : obj["commentable_article_name"],
        "date"           : obj["comment_date"]
    }
    result["comentarios"].append(obj_new)

search_body = {
    "aggs" : {
        "termos_comentarios": {
            "terms" : {
                "field" : "comment_text.brazilian",
                "size": 100
            }
        },
        #"termos_itemlei": {
        #    "terms" : {
        #        "field" : "commentable_text"
        #    }
        #}
    }
}


raw_result = es.search(index="dadospessoais", search_type="count", body=search_body)
result["termos_comentarios"] = raw_result["aggregations"]["termos_comentarios"]["buckets"]

print(json.dumps(result))


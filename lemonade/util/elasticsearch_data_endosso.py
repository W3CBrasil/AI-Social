import json
import elasticsearch
import collections

INDEX_NAME = "dadosparciais_45"

def dados_completos(index_name, fields=None):

    es = elasticsearch.Elasticsearch()
    
    results = []
    raw_result = es.search(index=index_name, scroll="1m")
    
    while len(raw_result["hits"]["hits"]) > 0:
   
        new_results = [ obj["_source"] for obj in raw_result["hits"]["hits"] ]

        if fields is not None:
            new_results = [ { key : obj[key] for key in fields } 
                              for obj in new_results ]

        results.extend(new_results)
    
        raw_result = es.scroll(scroll_id=raw_result["_scroll_id"], scroll="1m")

    return results


def dados_vis_endosso():

    es = elasticsearch.Elasticsearch()

    data = dados_completos(INDEX_NAME, ["comment_id", "author_id", "author_name", 
                                        "endosso_author", "comment_parent"]) 

    comment_author = { obj["comment_id"] : obj["author_id"] for obj in data }
    author_score = { obj["author_id"] : obj["endosso_author"] for obj in data }

    # Links
    links = collections.Counter( (obj["author_id"], comment_author[obj["comment_parent"]]) 
                                     for obj in data if obj["comment_parent"] != "0" and 
                                                        obj["comment_parent"] in comment_author )

    # Node 
    selected_authors = set(author_from for (author_from, author_to), count in links.items() ) | \
                       set(author_to for (author_from, author_to), count in links.items() )

    node_values = sorted(list(set( (obj["author_id"], obj["author_name"])
                                       for obj in data if obj["author_id"] in selected_authors ) ),
                         key = lambda a : a[1])

    node_values_id = { author_id : i
                           for i, (author_id, author_name)
                               in enumerate(node_values) }

    # Graph
    graph = {
        "nodes" : [ { "author_id": author_id,
                      "author_name": author_name,
                      "value" : author_score[author_id] } 
                          for author_id, author_name in node_values ],

        "links": sorted([ { "source" : node_values_id[author_from],
                            "target" : node_values_id[author_to],
                            "value"  : count } 
                                for (author_from, author_to), count in links.items() ],
                        key = lambda a: a["value"], reverse=True)
    }

    return graph


print(json.dumps(dados_vis_endosso()))


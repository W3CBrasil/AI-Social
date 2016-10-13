import json
import elasticsearch
import collections

INDEX_NAME = "dadospessoais_9"

def dados_vis_padroes():

    es = elasticsearch.Elasticsearch()

    raw_result = es.search(index=INDEX_NAME, size=5000, from_=0)

    data = collections.defaultdict(list)

    for obj in raw_result["hits"]["hits"]:
        obj = obj["_source"]
        for pattern_id in obj["patterns"]:
            data[pattern_id].append(( obj["author_id"], 
                                      obj["author_name"], 
                                      obj["commentable_article"], 
                                      obj["commentable_article_name"],
                                      obj["commentable_axis"]
                                    ))

    data_count = { pattern_id : collections.Counter(comments)  for pattern_id, comments in data.items() }    
    
    result = {}
    for pattern_id, counter in data_count.items():

	# Row
        row_values = sorted(list(set( (author_id, author_name) 
                                          for author_id, author_name, _, _, _ in counter) ), 
                            key=lambda a: a[1])

        row_values_id = { author_id : i 
                              for i, (author_id, author_name) 
                                  in enumerate(row_values)}

	# Column
        column_values = sorted(list(set( (commentable_article, commentable_article_name, commentable_axis) 
                                          for _, _, commentable_article, commentable_article_name, commentable_axis in counter) ), 
                            key=lambda a: a[1])

        column_values_id = { commentable_article : i 
                                 for i, (commentable_article, commentable_article_name, commentable_axis) 
                                     in enumerate(column_values)}

	# Links
        result[pattern_id] = {

            "row":    [ { "name": author_name } 
                            for author_id, author_name in row_values ],

            "column": [ { "name" : commentable_article_name, "group" : commentable_axis }
                            for commentable_article, commentable_article_name, commentable_axis in column_values ],

            "links": [ { "source" : row_values_id[author_id] , 
                         "target": column_values_id[commentable_article], 
                         "value": count }
                            for (author_id, _, commentable_article, _, _), count in counter.most_common() ]
        }

    return result


print(json.dumps(dados_vis_padroes()))


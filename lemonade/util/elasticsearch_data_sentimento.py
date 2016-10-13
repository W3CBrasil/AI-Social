import json
import elasticsearch
import collections

INDEX_NAME = "dadosparciais_40"

def dados_vis_sentimento():

    es = elasticsearch.Elasticsearch()

    # Coleta valores de média do sentimento com as devidas agregações

    search_body = {
        "aggs" : {
            "media_matriz" : { 
                "terms" : { 
                    "script": "[doc['author_id'].value, doc['author_name'].value.replaceAll('/', ''), doc['commentable_article'].value, doc['commentable_article_name'].value.replaceAll('/', '')].join('/')",
                    "size": 0
                },
                "aggs": {
                    "avg_sentiment": {
                        "avg": {
                            "field" : "sentiment"
                        }
                    }
      		}
            },
            "media_participante" : { 
                "terms" : { 
                    "script": "[doc['author_id'].value, doc['author_name'].value.replaceAll('/', '')].join('/')",
                    "size": 0,
                    "order": {
                        "avg_sentiment" : "desc" 
                    }
                },
                "aggs": {
                    "avg_sentiment": {
                        "avg": {
                            "field" : "sentiment"
                        }
                    }
      		}
            },
            "media_item" : { 
                "terms" : { 
                    "script": "[doc['commentable_id'].value, doc['commentable_name'].value.replaceAll('/', '')].join('/')",
                    "size": 0,
                    "order": {
                        "avg_sentiment" : "desc" 
                    }
                },
                "aggs": {
                    "avg_sentiment": {
                        "avg": {
                            "field" : "sentiment"
                        }
                    }
      		}
            }
        }
    }

    raw_result = es.search(index=INDEX_NAME, search_type="count", body=search_body)


    ## Matriz de agregação porr autor e artigo

    matrix_avg = [ 
        tuple( obj["key"].split("/") +
               [ obj["doc_count"],
                 obj["avg_sentiment"]["value"] ] )
      	  
            for obj in raw_result["aggregations"]["media_matriz"]["buckets"] 
    ]

    # Row
    row_values = sorted(list(set( (author_id, author_name) 
                                      for author_id, author_name, _, _, _, _ in matrix_avg ) ),
                        key=lambda a: a[1])

    row_values_id = { author_id : i 
                          for i, (author_id, author_name) 
                              in enumerate(row_values)}

    # column
    column_values = sorted(list(set( (commentable_article, commentable_article_name) 
                                          for _, _, commentable_article, commentable_article_name, _, _ in matrix_avg ) ),
                           key=lambda a: a[1])

    column_values_id = { commentable_article : i 
                          for i, (commentable_article, commentable_article_name) 
                              in enumerate(column_values)}

    # Links
    media_matriz = {

        "row":    [ { "name": author_name } 
                        for author_id, author_name in row_values ],

        "column": [ { "name" : commentable_article_name }
                        for commentable_article, commentable_article_name in column_values ],

        "links": [ { "source" : row_values_id[author_id] , 
                     "target": column_values_id[commentable_article], 
                     "value": avg }
                        for author_id, _, commentable_article, _, count, avg in matrix_avg ]
    }


    ## Lista de agregação por participante

    participante_avg = [ 
        tuple( obj["key"].split("/") +
               [ obj["doc_count"],
                 obj["avg_sentiment"]["value"] ] )
      	  
            for obj in raw_result["aggregations"]["media_participante"]["buckets"] 
    ]

    media_participante = [
        { "author_id"   : author_id, 
          "author_name" : author_name, 
          "sentiment_avg": avg }
            for author_id, author_name, count, avg in participante_avg
    ]


    ## Lista de agregação por item

    item_avg = [ 
        tuple( obj["key"].split("/") +
               [ obj["doc_count"],
                 obj["avg_sentiment"]["value"] ] )
      	  
            for obj in raw_result["aggregations"]["media_item"]["buckets"] 
    ]
    media_item = [
        { "commentable_id"   : commentable_id, 
          "commentable_name" : commentable_name, 
          "sentiment_avg": avg }
            for commentable_id, commentable_name, count, avg in item_avg
    ]


    ## Monta objeto com todos os dados

    result = {
        "media_matriz" : media_matriz,
        "media_participante" : media_participante,
        "media_item" : media_item
    }

    return result


print(json.dumps(dados_vis_sentimento()))


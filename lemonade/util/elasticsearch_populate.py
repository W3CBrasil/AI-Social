import datetime
import json
import csv

import elasticsearch

# Elastic Search
target_index = "dadospessoais" 
es = elasticsearch.Elasticsearch()

index_properties = {
 "settings": {
   "analysis": {
     "filter": {
       "brazilian_stop": {
         "type":       "stop",
         "stopwords":  "_brazilian_" 
       },
     },
     "analyzer": {
       "my_brazilian": {
         "tokenizer":  "standard",
         "filter": [
           "lowercase",
           "brazilian_stop"
         ]
       }
     }
   }
 },
 "mappings" : {
   "comment" : {
     "properties" : {
       "source" : {
         "type" : "string",
         "index": "not_analyzed"
       },
       "author_id" : {
         "type" : "string",
         "index": "not_analyzed",
       },
       "author_name" : {
         "type" : "string",
         "index": "not_analyzed",
       },
       "comment_text" : {
         "type" : "string",
         "fields": {
           "brazilian": { 
             "type":     "string",
             "analyzer": "my_brazilian"
           }
         }
       },
       "comment_date" : {
         "type" : "date",
         "format" : "dateOptionalTime"
       },
       "comment_id" : {
         "type" : "string",
         "index": "not_analyzed"
       },
       "comment_parent" : {
         "type" : "string",
         "index": "not_analyzed"
       },
       "commentable_id" : {
         "type" : "string",
         "index": "not_analyzed"
       },
       "commentable_parent" : {
         "type" : "string",
         "index": "not_analyzed"
       },
       "commentable_article" : {
         "type" : "string",
         "index": "not_analyzed"
       },
       "commentable_article_name" : {
         "type" : "string",
         "index": "not_analyzed"
       },
       "commentable_chapter" : {
         "type" : "string",
         "index": "not_analyzed"
       },
       "commentable_axis" : {
         "type" : "string",
         "index": "not_analyzed"
       },
       "commentable_type" : {
         "type" : "string",
         "index": "not_analyzed"
       },
       "commentable_name" : {
         "type" : "string",
         "index": "not_analyzed"
       },
       "commentable_text" : {
         "type" : "string",
         "fields": {
           "brazilian": { 
             "type":     "string",
             "analyzer": "brazilian"
           }
         }
       }
     }
   }
 }
}

try:
    es.indices.delete(index=target_index)
except:
    pass
es.indices.create(index=target_index, body=index_properties)

with open("../data/dadospessoais-comentarios-pdfs-filtrado-mesclado.jsonl", "r", encoding="utf-8") as infile:
    for line in infile:
        obj = json.loads(line)
        obj["comment_date"] = datetime.datetime.strptime(obj["comment_date"], "%Y-%m-%d %H:%M:%S") 
        comment_id = obj["comment_id"]
        #TODO: bulk
        es.index(index=target_index, doc_type="comment", id=comment_id, body=obj)


import sys
import json
import elasticsearch
import elasticsearch.helpers

es = elasticsearch.Elasticsearch()

results = []
raw_result = es.search(index="dadospessoais", scroll="1m")

while len(raw_result["hits"]["hits"]) > 0:

    new_results = [ { key : a["_source"][key] for key in ["author_name", "commentable_name", "comment_text"] } 
                   for a in raw_result["hits"]["hits"] ]

    results.extend(new_results)

    raw_result = es.scroll(scroll_id=raw_result["_scroll_id"], scroll="1m")

#print(len(results))
#print(json.dumps(raw_result))
print(json.dumps(results))


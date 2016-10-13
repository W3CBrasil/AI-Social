import sys
import json
import elasticsearch
import elasticsearch.helpers

es = elasticsearch.Elasticsearch()

page_size = 25
raw_result = es.search(index="dadospessoais", size=page_size, from_=0)

print(json.dumps(raw_result))


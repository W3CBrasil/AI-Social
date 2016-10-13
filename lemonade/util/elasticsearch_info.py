import sys
import json
import elasticsearch
import elasticsearch.helpers

index_input = "dadospessoais"

es = elasticsearch.Elasticsearch()

index_info = es.indices.stats(index=index_input)
ndocs = index_info["indices"][index_input]["primaries"]["docs"]["count"]

index_properties = es.indices.get(index=index_input)
fields = sorted(index_properties[index_input]["mappings"]["comment"]["properties"])

print(ndocs)
print(fields)

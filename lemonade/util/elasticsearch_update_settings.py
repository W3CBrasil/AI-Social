import elasticsearch
import json

es = elasticsearch.Elasticsearch()

body_settings = {
  "analysis": {
    "analyzer": {
      "brazilian_acento": {
        "filter": [
          "lowercase",
          "brazilian_stop",
          "asciifolding"
        ],
        "tokenizer": "standard"
      }
    }
  }
}

body_mapping = {
  "comment": {
    "properties": {
      "comment_text": {
        "type": "string",
        "fields": {
          "acento": {
            "type": "string",
            "analyzer": "brazilian_acento"
          }
        }
      }
    }
  }
}

index_name = "dadosparciais_1"

old_settings = es.indices.get(index=index_name)
print(json.dumps(old_settings, indent=4))
print

es.indices.close(index=index_name)
es.indices.put_settings(index=index_name, body=body_settings)
es.indices.put_mapping(index=index_name, doc_type="comment", body=body_mapping)
es.indices.open(index=index_name)

new_settings = es.indices.get(index=index_name)

print(json.dumps(new_settings, indent=4))
print


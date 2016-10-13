import elasticsearch
import time
from analyzer.algorithms import utils

name = "Filtragem"

def run(dataset_input, dataset_output, query="comment_text:PALAVRA"):

    es = elasticsearch.Elasticsearch()
    
    # Clone settings of the original index
    index_properties = es.indices.get(index=dataset_input.name)[dataset_input.name]
    index_properties["settings"]["index"] = {
            "analysis" : index_properties["settings"]["index"]["analysis"]
    }
    
    # Create new index
    es.indices.create(index=dataset_output.name, body=index_properties)
    
    # Query and store documents
    elasticsearch.helpers.reindex(es, dataset_input.name, dataset_output.name, 
                                  query = {"query":{"query_string":{"query":query}}},
                                  chunk_size = 5000)

params_default = utils.function_defaults(run)


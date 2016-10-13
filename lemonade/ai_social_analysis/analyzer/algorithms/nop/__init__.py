import elasticsearch
import time
from analyzer.algorithms import utils

name = "No Operation"

def run(dataset_input, dataset_output, time_to_sleep=5):

    time_to_sleep = int(time_to_sleep)

    # Sleep..
    time.sleep(time_to_sleep)

    # Create elasticsearch index

    es = elasticsearch.Elasticsearch()

    index_properties = es.indices.get(index=dataset_input.name)[dataset_input.name]
    index_properties["settings"]["index"] = {
    	"analysis" : index_properties["settings"]["index"]["analysis"]
    }

    es.indices.create(index=dataset_output.name, body=index_properties)
        
params_default = utils.function_defaults(run)


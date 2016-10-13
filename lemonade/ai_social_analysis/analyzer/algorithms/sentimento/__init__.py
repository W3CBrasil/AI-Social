import elasticsearch
import json

import tempfile
import subprocess
import os
SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))

from analyzer.algorithms import utils


name = "Análise de Sentimentos"


def run(dataset_input, dataset_output):

    # Retrieve all documents
    objs = dataset_input.dados_completos()

    with tempfile.NamedTemporaryFile() as infile:

        # Create temporary jsonl file with all data
        for obj in objs:
            infile.write("{}\n".format(json.dumps(obj)).encode("utf-8"))
        infile.seek(0)

        # Run the sentiment analysis
        output = subprocess.check_output("{}/run_sentiment.sh {}".format(SCRIPT_DIR, 
                                                                         infile.name), shell=True)

    # Build dictionary with sentiment values
    sentiments = [ line.decode("utf-8").rstrip().split(" ") 
                       for line in output.splitlines() ]
    sentiments = { a[0] : float(a[1]) for a in sentiments}
    
    # Connect to the ElasticSearch server
    es = elasticsearch.Elasticsearch()
    
    # Clone settings of the original index
    index_properties = es.indices.get(index=dataset_input.name)[dataset_input.name]
    index_properties["settings"]["index"] = {
        "analysis" : index_properties["settings"]["index"]["analysis"]
    }

    # Add properties of the new field
    index_properties["mappings"]["comment"]["properties"]["sentiment"] = {
        "type" : "float",
        "index" : "not_analyzed"
    }
    
    # Create new index
    es.indices.create(index=dataset_output.name, body=index_properties)

    # Populate index with sentiment values
    for obj in objs:
        obj["sentiment"] = sentiments[obj["comment_id"]]
        es.index(index=dataset_output.name, doc_type="comment", body=obj)


params_default = utils.function_defaults(run)


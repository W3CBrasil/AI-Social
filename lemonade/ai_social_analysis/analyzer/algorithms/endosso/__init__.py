import elasticsearch
import json

import tempfile
import subprocess
import os
SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))

import collections

from analyzer.algorithms import utils


name = "Endosso"


def run(dataset_input, dataset_output):

    # Retrieve all documents
    objs = dataset_input.dados_completos()

    with tempfile.NamedTemporaryFile() as infile:

        # Create temporary jsonl file with all data
        for obj in objs:
            infile.write("{}\n".format(json.dumps(obj)).encode("utf-8"))
        infile.seek(0)

        # Run the classification
        output = subprocess.check_output("{}/run_endosso.sh {}".format(SCRIPT_DIR, 
                                                                       infile.name), shell=True)

    # Build the dictionaries of scores

    scores_comment = collections.defaultdict(float)
    scores_commentable = collections.defaultdict(float)
    scores_author = collections.defaultdict(float)

    scores_raw = [ line.decode("utf-8").rstrip().split(" ") 
                     for line in output.splitlines() ]

    for score_type, score_id, score in scores_raw:

        score_id_type, score_id_value = score_id.split("_")

        if score_type == "reprov":
            score_value = -float(score)         
        else:
            score_value = float(score)         

        if score_id_type == "c":
            scores_comment[score_id_value] = score_value
        elif score_id_type == "i":
            scores_commentable[score_id_value] = score_value
        elif score_id_type == "u":
            scores_author[score_id_value] = score_value


    # Connect to the ElasticSearch server
    es = elasticsearch.Elasticsearch()
    
    # Clone settings of the original index
    index_properties = es.indices.get(index=dataset_input.name)[dataset_input.name]
    index_properties["settings"]["index"] = {
        "analysis" : index_properties["settings"]["index"]["analysis"]
    }

    # Add properties of the new fields
    index_properties["mappings"]["comment"]["properties"]["endosso_comment"] = {
        "type" : "float"
    }
    index_properties["mappings"]["comment"]["properties"]["endosso_commentable"] = {
        "type" : "float"
    }
    index_properties["mappings"]["comment"]["properties"]["endosso_author"] = {
        "type" : "float"
    }
   
    # Create new index
    es.indices.create(index=dataset_output.name, body=index_properties)

    # Populate index with scores
    for obj in objs:

        obj["endosso_comment"] = scores_comment[obj["comment_id"]]
        obj["endosso_commentable"] = scores_commentable[obj["commentable_id"]]
        obj["endosso_author"] = scores_author[obj["author_id"]]
        
        es.index(index=dataset_output.name, doc_type="comment", body=obj)


params_default = utils.function_defaults(run)


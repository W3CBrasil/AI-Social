import elasticsearch
import json

import tempfile
import subprocess
import os
SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))

from analyzer.algorithms import utils


name = "Padrões de Correlação"


def run(dataset_input, dataset_output):

    # Retrieve all documents
    objs = dataset_input.dados_completos()

    with tempfile.NamedTemporaryFile() as infile:

        # Create temporary jsonl file with all data
        for obj in objs:
            infile.write("{}\n".format(json.dumps(obj)).encode("utf-8"))
        infile.seek(0)

        # Run the pattern correlation
        output = subprocess.check_output("{}/run_correlacao.sh {}".format(SCRIPT_DIR, 
                                                                          infile.name), shell=True)

    # Build the list of patterns
    patterns = [ line.decode("utf-8").rstrip().split(" ") 
                     for line in output.splitlines() ]
    patterns = [ (str(i+1), set(a[0].split(",")), set(a[1].split(","))) 
                     for i, a in enumerate(patterns) ]

    # Create list of patterns where the comment is in
    patterns_comments = {}
    for obj in objs:
        patterns_comments[obj["comment_id"]] = [ 
            pattern_id for pattern_id, authors, articles in patterns
                if obj["author_id"] in authors and 
                   obj["commentable_article"] in articles 
        ]

    # Rename pattern ids, to exclude empty patterns
    pattern_ids = set( int(comment_id)
                           for comment_id, patterns in patterns_comments.items()
                               for comment_id in patterns)

    new_pattern_id = { str(pattern_id) : str(i+1) 
                           for i, pattern_id in enumerate(sorted(list(pattern_ids))) }
 
    for comment_id in patterns_comments:
        patterns_comments[comment_id] = sorted([ new_pattern_id[pattern_id] 
                                                     for pattern_id in 
                                                         patterns_comments[comment_id] ])

    # Connect to the ElasticSearch server
    es = elasticsearch.Elasticsearch()
    
    # Clone settings of the original index
    index_properties = es.indices.get(index=dataset_input.name)[dataset_input.name]
    index_properties["settings"]["index"] = {
        "analysis" : index_properties["settings"]["index"]["analysis"]
    }

    # Add properties of the new field
    index_properties["mappings"]["comment"]["properties"]["patterns"] = {
        "type" : "string"
    }
    
    # Create new index
    es.indices.create(index=dataset_output.name, body=index_properties)

    # Populate index with list of patterns
    for obj in objs:

        # Create list of patterns where the comment is in
        obj["patterns"] = patterns_comments[obj["comment_id"]]
            
        es.index(index=dataset_output.name, doc_type="comment", body=obj)


params_default = utils.function_defaults(run)


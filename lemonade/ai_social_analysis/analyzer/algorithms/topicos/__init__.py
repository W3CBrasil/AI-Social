import elasticsearch
import json

import tempfile
import subprocess
import os
SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))

import re
import unicodedata

from analyzer.algorithms import utils


name = "Tópicos"


def get_stopwords(infile_name):
    with open(infile_name, "r") as infile:
        stopwords = set(line.strip().lower() for line in infile)
    return stopwords


def clean_text(unicode_text, stopwords=set()):
    # Lower case
    unicode_text = unicode_text.lower()
    # Remove accents
    unicode_text = unicodedata.normalize("NFD", unicode_text).encode("ascii", "ignore").decode("ascii")
    # Extract words (remove punctuation)
    words = re.findall("\w+", unicode_text)
    # Remove numbers
    words = [word for word in words if not re.search("\d", word)]
    # Remove stopwords
    words = [word for word in words if word not in stopwords]
    # Join words
    unicode_text = u" ".join(words)
    return unicode_text


def run(dataset_input, dataset_output, n_topics=50, n_iters=1000, n_words=10):

    n_topics = int(n_topics)
    n_iters = int(n_iters)
    n_words = int(n_words)

    my_stopwords = get_stopwords("{}/dados/my_stopwords.txt".format(SCRIPT_DIR))

    # Retrieve all documents
    objs = dataset_input.dados_completos()

    with tempfile.NamedTemporaryFile() as infile:

        # Create temporary input file for lda
        for obj in objs:
            text = clean_text(obj["comment_text"], my_stopwords)
            if len(text) > 0:
                infile.write("{} {}\n".format(obj["comment_id"], text).encode("utf-8"))
        infile.seek(0)

        # Run the classification
        output = subprocess.check_output("{}/run_topicos.sh {} {} {} {}".format(SCRIPT_DIR, 
                                                                                infile.name, 
                                                                                n_topics, 
                                                                                n_iters, 
                                                                                n_words), shell=True)
    
    # Build the dictionaries of topics and words

    output_split = [ line.decode("utf-8").rstrip().split(" ", 2) 
                         for line in output.splitlines() ]

    topics_words = dict( ( str(int(out_id)+1), [ tuple(word_score.split(":")) 
                                                  for word_score in out.split(" ") ] ) 
                               for out_type, out_id, out in output_split if out_type == "topic" )

    documents_topics = dict( ( out_id, max([ (str(i+1), float(score)) 
                                                for i, score in enumerate(out.split(" ")) ], 
                                           key=lambda a: a[1])[0] ) 
                               for out_type, out_id, out in output_split if out_type == "doc" )

    # Connect to the ElasticSearch server
    es = elasticsearch.Elasticsearch()
    
    # Clone settings of the original index
    index_properties = es.indices.get(index=dataset_input.name)[dataset_input.name]
    index_properties["settings"]["index"] = {
        "analysis" : index_properties["settings"]["index"]["analysis"]
    }

    # Add properties of the new fields
    index_properties["mappings"]["comment"]["properties"]["topico_id"] = {
        "type" : "string"
    }
    index_properties["mappings"]["comment"]["properties"]["topico_palavras"] = {
        "type" : "string",
        "index": "not_analyzed"
    }

    # Create new index
    es.indices.create(index=dataset_output.name, body=index_properties)

    # Populate index with scores
    for obj in objs:

        obj["topico_id"] = documents_topics.get(obj["comment_id"], "0")

        topico_palavras = topics_words.get(obj["topico_id"], [])
        obj["topico_palavras"] = json.dumps([ obj["topico_id"], 
                                              [ [a[0], float(a[1])] 
                                                  for a in topico_palavras] ] )
        
        es.index(index=dataset_output.name, doc_type="comment", body=obj)


params_default = utils.function_defaults(run)


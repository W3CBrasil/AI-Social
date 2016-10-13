import sys
import json
import elasticsearch
import elasticsearch.helpers


def query_reindex(index_input, index_output, query):

    es = elasticsearch.Elasticsearch()

    # Clone settings of the original index
    index_properties = es.indices.get(index=index_input)[index_input]
    index_properties["settings"]["index"] = {
            "analysis" : index_properties["settings"]["index"]["analysis"]
    }

    # Create new index
    es.indices.create(index=index_output, body=index_properties)

    # Query and store documents
    elasticsearch.helpers.reindex(es, index_input, index_output, 
                                  query = {"query":{"query_string":{"query":query}}},
                                  chunk_size = 5000)


#query = sys.argv[1]

#query_reindex("dadospessoais", "dadospessoais_2", query)

query_reindex("dadospessoais", "dadosparciais_1", "*")
query_reindex("dadospessoais", "dadosparciais_2", "comment_text:privacidade")
query_reindex("dadospessoais", "dadosparciais_3", "comment_text:brasil")
query_reindex("dadospessoais", "dadosparciais_4", "comment_text:dados")
query_reindex("dadospessoais", "dadosparciais_5", "comment_text:eua")

#query_reindex("dadospessoais", "dadosparciais_6", "comment_text:dilma")
#query_reindex("dadospessoais", "dadosparciais_7", "comment_text:empresa")
query_reindex("dadospessoais", "dadosparciais_8", "comment_text:dados")
#query_reindex("dadospessoais", "dadosparciais_9", "comment_text:dados")
#query_reindex("dadospessoais", "dadosparciais_10", "comment_text:dados")
query_reindex("dadospessoais", "dadosparciais_11", "comment_text:brasil")
query_reindex("dadospessoais", "dadosparciais_12", "brasil")
query_reindex("dadospessoais", "dadosparciais_13", "eua")
#query_reindex("dadospessoais", "dadosparciais_14", "comment_text:dados")


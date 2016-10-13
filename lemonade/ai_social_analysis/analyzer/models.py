from django.db import models
from django.forms import ModelForm, TextInput, Select, Textarea
from django.utils import timezone
from django.contrib.auth.models import User, Group
from django.db.models import Q

import re
import datetime
import elasticsearch
import elasticsearch.helpers
import time
import json
import collections
import traceback

from analyzer import algorithms


class FieldTag(models.Model):

    name = models.CharField(max_length = 100, db_index=True)

    def __str__(self):
        return self.name


class Dataset(models.Model):

    project     = models.ForeignKey(Group, on_delete = models.CASCADE)
    id_project  = models.IntegerField()
    name        = models.CharField(max_length = 60, db_index=True)

    owner       = models.ForeignKey(User, on_delete = models.CASCADE, null=True)

    created_at  = models.DateTimeField(auto_now_add = True)

    es_ndocs    = models.IntegerField(default=0, null=True)
    es_fields   = models.ManyToManyField(FieldTag)


    def name_from_id(project_name, dataset_id):
        return "{}_{}".format(project_name, dataset_id)

    def get_from_id(project_name, dataset_id):
        dataset_name = Dataset.name_from_id(project_name, dataset_id) 
        dataset = Dataset.objects.get(name=dataset_name)
        return dataset

    def datasets_user(user):
        user_groups = list(user.groups.all())
        return Dataset.objects.filter(Q(owner=user) | Q(project__in=user_groups)).order_by('-created_at')

    def create_new(project):

        try:
            latest = Dataset.objects.filter(project=project).latest("created_at")
            new_id = latest.id_project + 1
        except Dataset.DoesNotExist:
            new_id = 1

        new_name = Dataset.name_from_id(project, new_id)

        new_dataset = Dataset(project    = project, 
                              id_project = new_id,
                              name       = new_name, 
                              owner      = None)

        new_dataset.save()

        return new_dataset


    def fill_es_fields(self):

        es = elasticsearch.Elasticsearch()
            
        es.indices.flush(index=self.name)

        index_info = es.indices.stats(index=self.name)
        ndocs = index_info["indices"][self.name]["primaries"]["docs"]["count"]
        self.es_ndocs = ndocs

        index_properties = es.indices.get(index=self.name)
        if "comment" in index_properties[self.name]["mappings"]:
            fields = sorted(index_properties[self.name]["mappings"]["comment"]["properties"])
            for field in fields:
                field_obj, created = FieldTag.objects.get_or_create(name=field)
                self.es_fields.add(field_obj)

        self.save()


    def __str__(self):
        return self.name


    def delete(self, *args, **kwargs):

        # Delete index from ElasticSearch before deleting from database
        es = elasticsearch.Elasticsearch()
        if es.indices.exists(index=self.name):
            es.indices.delete(index=self.name)

        super(Dataset, self).delete(*args, **kwargs)


    def user_can_view(self, user):
        return self.owner is None or \
               self.owner.is_superuser or \
               user.is_superuser or \
               self.owner == user or \
               user.groups.filter(pk=self.project.pk).exists()


    def dados_completos(self, fields=None):
    
        es = elasticsearch.Elasticsearch()
        
        results = []
        raw_result = es.search(index=self.name, scroll="1m")
        
        while len(raw_result["hits"]["hits"]) > 0:
       
            new_results = [ obj["_source"] for obj in raw_result["hits"]["hits"] ]

            if fields is not None:
                new_results = [ { key : obj[key] for key in fields } 
                                  for obj in new_results ]

            results.extend(new_results)
        
            raw_result = es.scroll(scroll_id=raw_result["_scroll_id"], scroll="1m")

        return results


    def dados_conteudo(self, page_number, page_size=25):
    
        es = elasticsearch.Elasticsearch()
    
        from_ = page_size*(page_number - 1)
           
        raw_result = es.search(index=self.name, size=page_size, from_=from_)
    
        result = [ { key : a["_source"][key] for key in ["author_name", "commentable_name", "comment_text"] } 
                       for a in raw_result["hits"]["hits"] ]
    
        return result
      

    def lista_visualizacoes(self):
        fields = set(["{}".format(field) for field in self.es_fields.all()])

        visualizacoes = []

        visualizacoes.append({"nome": "calaca", 
                              "titulo":"Busca", 
                              "descricao":"A Busca permite visualizar os comentários presentes no dataset, assim como fazer buscas e filtragens temporárias."})
        visualizacoes.append({"nome": "dashboard", 
                              "titulo":"Dashboard", 
                              "descricao":"O dashboard fornece uma forma de visualizar e explorar os dados através de gráficos e visualizações de dados."})

        if "topico_id" in fields:
            visualizacoes.append({"nome": "vis_topicos", 
                                  "titulo":"Tópicos", 
                                  "descricao":"Visualização da tabela dos tópicos."})

        if "patterns" in fields:
            visualizacoes.append({"nome": "vis_correlacoes", 
                                  "titulo":"Matriz de Correlações", 
                                  "descricao":"Visualização da matriz dos padrões de correlação."})

        if "sentiment" in fields:
            visualizacoes.append({"nome": "vis_sentimento", 
                                  "titulo":"Análise de Sentimentos", 
                                  "descricao":"Visualização da análise de sentimentos."})

        if "endosso_author" in fields:
            visualizacoes.append({"nome": "vis_endosso", 
                                  "titulo":"Endosso", 
                                  "descricao":"Visualização de endosso."})

        return visualizacoes


    def dados_dashboard_geral(self):
    
        es = elasticsearch.Elasticsearch()
        
        result = {"comentarios":[], 
                  "termos_comentarios":[], 
                  "termos_itemlei":[]}
        
        raw_result = es.search(index=self.name, size=5000, from_=0)
        
        for obj in raw_result["hits"]["hits"]:
            obj = obj["_source"]
            obj_new = {
                "id"             : int(obj["comment_id"]),
                "commentable_id" : int(obj["commentable_id"]),
                "commentable"    : obj["commentable_name"],
                "author_id"      : int(obj["author_id"]),
                "author"         : obj["author_name"],
                "axis_id"        : int(obj["commentable_axis"]),
                "axis"           : "Eixo {}".format(obj["commentable_axis"]),
                "article_id"     : int(obj["commentable_article"]),
                "article"        : obj["commentable_article_name"],
                "date"           : obj["comment_date"]
            }
            result["comentarios"].append(obj_new)
        
        search_body = {
            "aggs" : {
                "termos_comentarios": {
                    "terms" : {
                        "field" : "comment_text.brazilian",
                        "size": 100
                    }
                },
                #"termos_itemlei": {
                #    "terms" : {
                #        "field" : "commentable_text"
                #    }
                #}
            }
        }
       
        raw_result = es.search(index=self.name, search_type="count", body=search_body)
        result["termos_comentarios"] = raw_result["aggregations"]["termos_comentarios"]["buckets"]
    
        return result
 

    def dados_vis_topicos(self):
    
        es = elasticsearch.Elasticsearch()
    
        search_body = {
            "aggs" : {
                "contagem_comentarios" : { 
                    "terms" : { 
                        "field": "topico_palavras", 
                        "size" : 0
                    } 
                },
                "frequencia_eixos" : { 
                    "terms" : { 
                        "script": "[doc['topico_id'].value + '/' + doc['commentable_axis'].value]",
                        "size": 0
                    } 
                } 
            }
        }
    
        raw_result = es.search(index=self.name, search_type="count", body=search_body)
        
        # Frequencia por eixo
        frequencia_eixos = collections.defaultdict(list)
        for topico_eixo in raw_result["aggregations"]["frequencia_eixos"]["buckets"]:
    
            topico_id, commentable_axis = topico_eixo["key"].split("/")
            contagem = topico_eixo["doc_count"]
    
            frequencia_eixos[topico_id].append((commentable_axis, contagem))
    
    
        # Informações completas de cada tópico
        result = []
        for topico in raw_result["aggregations"]["contagem_comentarios"]["buckets"]:
    
            topico_id, top_palavras = json.loads(topico["key"])
            contagem_comentarios = topico["doc_count"]
    
            topico_info = {
                "topico_id"            : topico_id,
                "nome"                 : "Topico {}".format(topico_id),
                "top_palavras"         : top_palavras,
                "contagem_comentarios" : contagem_comentarios,
                "frequencia_eixos"     : [ [commentable_axis, contagem]
                                               for commentable_axis, contagem in sorted(frequencia_eixos[topico_id], 
                                                   key=lambda a:a[1], reverse=True) ][:10]
            }
    
            result.append(topico_info)
    
        return result

 
    def dados_vis_correlacoes(self):
    
        es = elasticsearch.Elasticsearch()

        raw_result = es.search(index=self.name, size=5000, from_=0)

        data = collections.defaultdict(list)

        for obj in raw_result["hits"]["hits"]:
            obj = obj["_source"]
            for pattern_id in obj["patterns"]:
                data[pattern_id].append(( obj["author_id"], 
                                          obj["author_name"], 
                                          obj["commentable_article"], 
                                          obj["commentable_article_name"],
                                          obj["commentable_axis"]
                                        ))

        data_count = { pattern_id : collections.Counter(comments)  for pattern_id, comments in data.items() }    
        
        result = {}
        for pattern_id, counter in data_count.items():

            # Row
            row_values = sorted(list(set( (author_id, author_name) 
                                              for author_id, author_name, _, _, _ in counter) ), 
                                key=lambda a: a[1])

            row_values_id = { author_id : i 
                                  for i, (author_id, author_name) 
                                      in enumerate(row_values)}

            # Column
            column_values = sorted(list(set( (commentable_article, commentable_article_name, commentable_axis) 
                                              for _, _, commentable_article, commentable_article_name, commentable_axis in counter) ), 
                                key=lambda a: a[1])

            column_values_id = { commentable_article : i 
                                     for i, (commentable_article, commentable_article_name, commentable_axis) 
                                         in enumerate(column_values)}

            # Links
            result[pattern_id] = {

                "row":    [ { "name": author_name } 
                                for author_id, author_name in row_values ],

                "column": [ { "name" : commentable_article_name, "group" : commentable_axis }
                                for commentable_article, commentable_article_name, commentable_axis in column_values ],

                "links": [ { "source" : row_values_id[author_id] , 
                             "target": column_values_id[commentable_article], 
                             "value": count }
                                for (author_id, _, commentable_article, _, _), count in counter.most_common() ]
            }

        return result


    def dados_vis_sentimento(self):
    
       es = elasticsearch.Elasticsearch()
    
       # Coleta valores de média do sentimento com as devidas agregações
    
       search_body = {
           "aggs" : {
               "media_matriz" : { 
                   "terms" : { 
                       "script": "[doc['author_id'].value, doc['author_name'].value.replaceAll('/', ''), doc['commentable_article'].value, doc['commentable_article_name'].value.replaceAll('/', '')].join('/')",
                       "size": 0
                   },
                   "aggs": {
                       "avg_sentiment": {
                           "avg": {
                               "field" : "sentiment"
                           }
                       }
         		}
               },
               "media_participante" : { 
                   "terms" : { 
                       "script": "[doc['author_id'].value, doc['author_name'].value.replaceAll('/', '')].join('/')",
                       "size": 0,
                       "order": {
                           "avg_sentiment" : "desc" 
                       }
                   },
                   "aggs": {
                       "avg_sentiment": {
                           "avg": {
                               "field" : "sentiment"
                           }
                       }
         		}
               },
               "media_item" : { 
                   "terms" : { 
                       "script": "[doc['commentable_id'].value, doc['commentable_name'].value.replaceAll('/', '')].join('/')",
                       "size": 0,
                       "order": {
                           "avg_sentiment" : "desc" 
                       }
                   },
                   "aggs": {
                       "avg_sentiment": {
                           "avg": {
                               "field" : "sentiment"
                           }
                       }
         		}
               }
           }
       }
    
       raw_result = es.search(index=self.name, search_type="count", body=search_body)
    
    
       ## Matriz de agregação porr autor e artigo
    
       matrix_avg = [ 
           tuple( obj["key"].split("/") +
                  [ obj["doc_count"],
                    obj["avg_sentiment"]["value"] ] )
         	  
               for obj in raw_result["aggregations"]["media_matriz"]["buckets"] 
       ]
    
       # Row
       row_values = sorted(list(set( (author_id, author_name) 
                                         for author_id, author_name, _, _, _, _ in matrix_avg ) ),
                           key=lambda a: a[1])
    
       row_values_id = { author_id : i 
                             for i, (author_id, author_name) 
                                 in enumerate(row_values)}
    
       # column
       column_values = sorted(list(set( (commentable_article, commentable_article_name) 
                                             for _, _, commentable_article, commentable_article_name, _, _ in matrix_avg ) ),
                              key=lambda a: a[1])
    
       column_values_id = { commentable_article : i 
                             for i, (commentable_article, commentable_article_name) 
                                 in enumerate(column_values)}
    
       # Links
       media_matriz = {
    
           "row":    [ { "name": author_name } 
                           for author_id, author_name in row_values ],
    
           "column": [ { "name" : commentable_article_name }
                           for commentable_article, commentable_article_name in column_values ],
    
           "links": [ { "source" : row_values_id[author_id] , 
                        "target": column_values_id[commentable_article], 
                        "value": avg }
                           for author_id, _, commentable_article, _, count, avg in matrix_avg ]
       }
    
    
       ## Lista de agregação por participante
    
       participante_avg = [ 
           tuple( obj["key"].split("/") +
                  [ obj["doc_count"],
                    obj["avg_sentiment"]["value"] ] )
         	  
               for obj in raw_result["aggregations"]["media_participante"]["buckets"] 
       ]
    
       media_participante = [
           { "author_id"   : author_id, 
             "author_name" : author_name, 
             "sentiment_avg": avg }
               for author_id, author_name, count, avg in participante_avg
       ]
    
    
       ## Lista de agregação por item
    
       item_avg = [ 
           tuple( obj["key"].split("/") +
                  [ obj["doc_count"],
                    obj["avg_sentiment"]["value"] ] )
         	  
               for obj in raw_result["aggregations"]["media_item"]["buckets"] 
       ]
       media_item = [
           { "commentable_id"   : commentable_id, 
             "commentable_name" : commentable_name, 
             "sentiment_avg": avg }
               for commentable_id, commentable_name, count, avg in item_avg
       ]
    
    
       ## Monta objeto com todos os dados
    
       result = {
           "media_matriz" : media_matriz,
           "media_participante" : media_participante,
           "media_item" : media_item
       }
    
       return result


    def dados_vis_endosso(self):
    
        es = elasticsearch.Elasticsearch()
    
        data = self.dados_completos(["comment_id", "author_id", "author_name", 
                                     "endosso_author", "comment_parent"]) 
    
        comment_author = { obj["comment_id"] : obj["author_id"] for obj in data }
        author_score = { obj["author_id"] : obj["endosso_author"] for obj in data }
    
        # Links
        links = collections.Counter( (obj["author_id"], comment_author[obj["comment_parent"]]) 
                                         for obj in data if obj["comment_parent"] != "0" and 
                                                            obj["comment_parent"] in comment_author )
    
        # Node 
        selected_authors = set(author_from for (author_from, author_to), count in links.items() ) | \
                           set(author_to for (author_from, author_to), count in links.items() )
    
        node_values = sorted(list(set( (obj["author_id"], obj["author_name"])
                                           for obj in data if obj["author_id"] in selected_authors ) ),
                             key = lambda a : a[1])
    
        node_values_id = { author_id : i
                               for i, (author_id, author_name)
                                   in enumerate(node_values) }
    
        # Graph
        graph = {
            "nodes" : [ { "author_id": author_id,
                          "author_name": author_name,
                          "value" : author_score[author_id] } 
                              for author_id, author_name in node_values ],
    
            "links": sorted([ { "source" : node_values_id[author_from],
                                "target" : node_values_id[author_to],
                                "value"  : count } 
                                    for (author_from, author_to), count in links.items() ],
                            key = lambda a: a["value"], reverse=True)
        }
    
        return graph

    
class Algorithm(models.Model):

    name           = models.CharField(max_length = 60)
    module_name    = models.CharField(max_length = 40, db_index=True)
    params_default = models.TextField()


    def dict_params():
        params = { str(algorithm.pk) : str(algorithm.params_default) 
                       for algorithm in Algorithm.objects.all() }
        return params


    def __str__(self):
        return self.name


    def run(self, params, dataset_input):

        try:
            dataset_output = Dataset.create_new(dataset_input.project)
            getattr(algorithms, self.module_name).run(dataset_input, dataset_output, **params)
            status = ("ok", None)
        except Exception as error:
            dataset_output.delete()
            error = traceback.format_exc()
            dataset_output = None
            status = ("error", error)

        return (dataset_output, status)




class Task(models.Model):

    CREATED_STATE = 1
    RUNNING_STATE = 2
    FINISHED_STATE = 3
    FAILED_STATE = 4
    STATE_CHOICES = (
        (CREATED_STATE, 'Created'),
        (RUNNING_STATE, 'Running'),
        (FINISHED_STATE, 'Finished'),
        (FAILED_STATE, 'Failed'),
    )
    
    name                = models.CharField(max_length = 60)

    dataset_input       = models.ForeignKey(Dataset, on_delete = models.CASCADE, related_name="task_children")
    dataset_output      = models.OneToOneField(Dataset, on_delete = models.CASCADE, related_name="task_parent", null = True, blank = True)

    owner               = models.ForeignKey(User, on_delete = models.CASCADE)

    algorithm           = models.ForeignKey(Algorithm, on_delete = models.CASCADE)
    algorithm_params    = models.TextField()

    created_at          = models.DateTimeField(auto_now_add = True)
    finished_at         = models.DateTimeField(null = True, blank = True)

    state               = models.IntegerField(choices = STATE_CHOICES, default=CREATED_STATE)
    error               = models.TextField(null = True, blank = True)


    def __str__(self):
        return self.name


    def run(self):

        self.state = self.RUNNING_STATE 
        self.save()

        try:
            params = json.loads(self.algorithm_params)
            dataset_output, (status, error) = self.algorithm.run(params, self.dataset_input)
        except Exception:
            e = traceback.format_exc()
            dataset_output, (status, error) = None, ("error", e)

        if status == "ok":

            if dataset_output:
                dataset_output.owner = self.owner
                self.dataset_output = dataset_output
                dataset_output.save()
                dataset_output.fill_es_fields()

            self.finished_at = timezone.now()
            self.state = self.FINISHED_STATE
            self.save()

        else:
            self.state = self.FAILED_STATE
            self.error = error
            self.save()



class TaskForm(ModelForm):

    class Meta:
        model = Task
        fields = ["name", "dataset_input", "algorithm", "algorithm_params"]
        widgets = { 'name'             : TextInput(attrs={"class":"form-control"}),
                    'dataset_input'    : Select(attrs={"class":"form-control"}),
                    'algorithm'        : Select(attrs={"class":"form-control"}),
                    'algorithm_params' : Textarea(attrs={"class":"form-control", "cols":40, "rows":4}),
        }



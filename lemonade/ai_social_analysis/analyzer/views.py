import json
import csv
import io
import elasticsearch
import urllib.request

from django.shortcuts import render, get_object_or_404
from django.http import HttpResponse, HttpResponseForbidden, HttpResponseRedirect
from django.views import generic
#from django.template import loader
from django.contrib.auth.decorators import login_required
from django.utils.decorators import decorator_from_middleware
from django.db.models import Q
from django.conf import settings

from django.contrib.auth.models import User, Group
from .models import Dataset, Algorithm, Task, TaskForm

from corsheaders.middleware import CorsMiddleware

def dataset_permission(view_func):

    def decorator(request, project_name, dataset_id, *args, **kwargs):
        try:
            dataset_name = Dataset.name_from_id(project_name, dataset_id)
            dataset = Dataset.objects.get(name=dataset_name)
            if dataset.user_can_view(request.user):
                return view_func(request, project_name, dataset_id, *args, **kwargs)
            else:
                return render(request, "error.html", {"message":"Usuário não tem permissão."})
        except Dataset.DoesNotExist:
    	    return render(request, "error.html", {"message":"Dataset não existe."})

    return decorator


@login_required
def index(request):
    url_redirect = "http://{}/?userid={}&username={}&email={}".format(settings.URL_LEMONADE,
                                                                      request.user.pk, 
                                                                      request.user.username, 
                                                                      request.user.email) 
    return HttpResponseRedirect(url_redirect)



# Proxy ElasticSearch

@login_required
@dataset_permission
def proxy_elasticsearch_search(request, project_name, dataset_id):
    dataset = Dataset.get_from_id(project_name, dataset_id)
    remoteurl = 'http://127.0.0.1:9200/{}//_search'.format(dataset.name)
    response = urllib.request.urlopen(remoteurl, data=request.body) 
    proxy_response = HttpResponse(response.read(), status=response.status)
    proxy_response["Content-Type"] = response.headers["Content-Type"]
    return proxy_response


# Task

@login_required
def tarefa_info(request, pk):
    tarefa = get_object_or_404(Task, pk=pk)
    if request.user == tarefa.owner:
        return render(request, "tarefa.html", {"task":tarefa})
    else:
        return render(request, "error.html", {"message":"Usuário não tem permissão."})

@login_required
def tarefas_usuario(request):
    tarefas = Task.objects.filter(owner=request.user).order_by('-created_at')
    return render(request, "tarefas.html", {"object_list":tarefas})

@login_required
def tarefa_criar(request):
    if request.method == 'POST':
        form = TaskForm(request.POST)
        default = None
        if form.is_valid():
            new_task = form.save(commit=False)
            new_task.owner = request.user
            new_task.save()
            new_task.run()
            return HttpResponseRedirect('/tarefas/')

    else:
        form = TaskForm()
        form.fields["dataset_input"].queryset = Dataset.datasets_user(request.user)

    return render(request, 'tarefa-criar.html', {'form': form})


@login_required
@decorator_from_middleware(CorsMiddleware)
def tarefa_criar_fluxo(request):

    if request.method == 'POST':
        form = TaskForm(request.POST)
        default = None
        if form.is_valid():
            new_task = form.save(commit=False)
            new_task.owner = request.user
            new_task.save()
            new_task.run()

            response = {
                "id" : new_task.pk,
                "state" : new_task.state,
                "dataset_output" : {
                    "id" : new_task.dataset_output.pk,
                    "name": new_task.dataset_output.name
                } if new_task.dataset_output else None,
                "error" : new_task.error
            }

    else:
        form = TaskForm()
        form.fields["dataset_input"].queryset = Dataset.datasets_user(request.user)

        default = Algorithm.dict_params()

        options_algorithm = [ { "id" : cid, 
                                "name" : name, 
                                "default" : Algorithm.objects.get(pk=cid).params_default } 
                                    for cid, name in form.fields["algorithm"].choices if cid ] 

        options_dataset = [ { "id" : cid, 
                              "name" : name } 
                                  for cid, name in form.fields["dataset_input"].choices if cid ] 
        
        response = { 
            "algorithm": options_algorithm ,
            "dataset_input": options_dataset 
        }

    return HttpResponse(json.dumps(response)) 


@login_required
def parametros_algoritmos(request):
    default = Algorithm.dict_params()
    return render(request, 'parametros_algoritmos.js', {'default': default})



# Dataset

@login_required
def datasets_usuario(request):
    datasets = Dataset.datasets_user(request.user)
    return render(request, "datasets.html", {"object_list":datasets})

@login_required
@dataset_permission
def dataset_info(request, project_name, dataset_id):
    
    dataset = Dataset.get_from_id(project_name, dataset_id)

    if request.method == "GET":
        visualizacoes = dataset.lista_visualizacoes() 
        for i in range(len(visualizacoes)):
            visualizacoes[i]["img"] = "img/{}.png".format(visualizacoes[i]["nome"]) 
        return render(request, "dataset.html", {"dataset":dataset, "visualizacoes":visualizacoes})

    elif request.method == "DELETE":
        if request.user == dataset.owner:
            dataset.delete()
            return HttpResponse()
        else:
            return HttpResponseForbidden()

    else:
        return HttpResponseForbidden()



@login_required
@dataset_permission
def dataset_dados_jsonl(request, project_name, dataset_id):

    dataset = Dataset.get_from_id(project_name, dataset_id)
    objs = dataset.dados_completos()

    output = "\n".join(json.dumps(obj) for obj in objs)+"\n"

    response = HttpResponse(output)
    response['Content-Disposition'] = 'attachment; filename="{}.jsonl"'.format(dataset.name)

    return response


@login_required
@dataset_permission
def dataset_dados_csv(request, project_name, dataset_id):

    dataset = Dataset.get_from_id(project_name, dataset_id)
    objs = dataset.dados_completos()
    fields = ["{}".format(field) for field in dataset.es_fields.all()]

    with io.StringIO() as outfile:
        writer = csv.DictWriter(outfile, fieldnames=fields)
        writer.writeheader()
        for obj in objs:
            writer.writerow(obj)
        output = outfile.getvalue()

    response = HttpResponse(output)
    response['Content-Disposition'] = 'attachment; filename="{}.csv"'.format(dataset.name)

    return response


@login_required
@dataset_permission
def dataset_conteudo(request, project_name, dataset_id, page_number="1"):
    dataset = Dataset.get_from_id(project_name, dataset_id)
    response = dataset.dados_conteudo(int(page_number))
    return HttpResponse(json.dumps(response)) 
 
@login_required
@dataset_permission
def calaca(request, project_name, dataset_id, query="*"):
    dataset = Dataset.get_from_id(project_name, dataset_id)
    return render(request, "calaca.html", {"dataset": dataset, "query":query})

@login_required
@dataset_permission
def dashboard(request, project_name, dataset_id):
    return render(request, "dashboard.html")
 
@login_required
@dataset_permission
def dashboard_dados(request, project_name, dataset_id):
    dataset = Dataset.get_from_id(project_name, dataset_id)
    response = dataset.dados_dashboard_geral()
    return HttpResponse(json.dumps(response)) 
 
@login_required
@dataset_permission
def vis_topicos(request, project_name, dataset_id):
    return render(request, "vis_topicos.html")
 
@login_required
@dataset_permission
def vis_topicos_dados(request, project_name, dataset_id):
    dataset = Dataset.get_from_id(project_name, dataset_id)
    response = dataset.dados_vis_topicos()
    return HttpResponse(json.dumps(response)) 

@login_required
@dataset_permission
def vis_correlacoes(request, project_name, dataset_id):
    return render(request, "vis_correlacoes.html")
 
@login_required
@dataset_permission
def vis_correlacoes_dados(request, project_name, dataset_id):
    dataset = Dataset.get_from_id(project_name, dataset_id)
    response = dataset.dados_vis_correlacoes()
    return HttpResponse(json.dumps(response)) 

@login_required
@dataset_permission
def vis_sentimento(request, project_name, dataset_id):
    return render(request, "vis_sentimento.html")
 
@login_required
@dataset_permission
def vis_sentimento_dados(request, project_name, dataset_id):
    dataset = Dataset.get_from_id(project_name, dataset_id)
    response = dataset.dados_vis_sentimento()
    return HttpResponse(json.dumps(response))

@login_required
@dataset_permission
def vis_endosso(request, project_name, dataset_id):
    return render(request, "vis_endosso.html")
 
@login_required
@dataset_permission
def vis_endosso_dados(request, project_name, dataset_id):
    dataset = Dataset.get_from_id(project_name, dataset_id)
    response = dataset.dados_vis_endosso()
    return HttpResponse(json.dumps(response))


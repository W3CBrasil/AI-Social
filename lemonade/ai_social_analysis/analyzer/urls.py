from django.conf.urls import url
from django.views.decorators.csrf import csrf_exempt

from . import views

urlpatterns = [

    url(r'^$', views.index, name='index'),
    url(r'^tarefas/$', views.tarefas_usuario, name='tarefas_usuario'),
    url(r'^datasets/$', views.datasets_usuario, name='datasets_usuario'),

    url(r'^tarefa/(?P<pk>\d+)/$', views.tarefa_info, name='tarefa_info'),

    url(r'^tarefa/criar/$', views.tarefa_criar, name='tarefa_criar'),
    url(r'^tarefa/criar/parametros_algoritmos.js$', views.parametros_algoritmos, name='parametros_algoritmos'),

    # API Fluxos
    url(r'^tarefa/criar_fluxo/$', csrf_exempt(views.tarefa_criar_fluxo), name='tarefa_criar_fluxo'),

    url(r'^projeto/(?P<project_name>[^/]+)/dataset/(?P<dataset_id>\d+)/es_search/.*$', csrf_exempt(views.proxy_elasticsearch_search), name='proxy_elasticsearch_search'),

    url(r'^projeto/(?P<project_name>[^/]+)/dataset/(?P<dataset_id>\d+)/$', csrf_exempt(views.dataset_info), name='dataset_info'),

    url(r'^projeto/(?P<project_name>[^/]+)/dataset/(?P<dataset_id>\d+)/download/jsonl$', views.dataset_dados_jsonl, name='dataset_dados_jsonl'),
    url(r'^projeto/(?P<project_name>[^/]+)/dataset/(?P<dataset_id>\d+)/download/csv$', views.dataset_dados_csv, name='dataset_dados_csv'),
    
    url(r'^projeto/(?P<project_name>[^/]+)/dataset/(?P<dataset_id>\d+)/conteudo/$', views.dataset_conteudo, name='dataset_conteudo'),
    url(r'^projeto/(?P<project_name>[^/]+)/dataset/(?P<dataset_id>\d+)/conteudo/(?P<page_number>\d+)/$', views.dataset_conteudo, name='dataset_conteudo'),

    url(r'^projeto/(?P<project_name>[^/]+)/dataset/(?P<dataset_id>\d+)/calaca/$', views.calaca, name='calaca'),
    url(r'^projeto/(?P<project_name>[^/]+)/dataset/(?P<dataset_id>\d+)/calaca/(?P<query>.+)/$', views.calaca, name='calaca'),

    url(r'^projeto/(?P<project_name>[^/]+)/dataset/(?P<dataset_id>\d+)/dashboard/$', views.dashboard, name='dashboard'),
    url(r'^projeto/(?P<project_name>[^/]+)/dataset/(?P<dataset_id>\d+)/dashboard/dados.json$', views.dashboard_dados, name='dashboard_dados'),

    url(r'^projeto/(?P<project_name>[^/]+)/dataset/(?P<dataset_id>\d+)/topicos/$', views.vis_topicos, name='vis_topicos'),
    url(r'^projeto/(?P<project_name>[^/]+)/dataset/(?P<dataset_id>\d+)/topicos/dados.json$', views.vis_topicos_dados, name='vis_topicos_dados'),

    url(r'^projeto/(?P<project_name>[^/]+)/dataset/(?P<dataset_id>\d+)/correlacoes/$', views.vis_correlacoes, name='vis_correlacoes'),
    url(r'^projeto/(?P<project_name>[^/]+)/dataset/(?P<dataset_id>\d+)/correlacoes/dados.json$', views.vis_correlacoes_dados, name='vis_correlacoes_dados'),

    url(r'^projeto/(?P<project_name>[^/]+)/dataset/(?P<dataset_id>\d+)/sentimento/$', views.vis_sentimento, name='vis_sentimento'),
    url(r'^projeto/(?P<project_name>[^/]+)/dataset/(?P<dataset_id>\d+)/sentimento/dados.json$', views.vis_sentimento_dados, name='vis_sentimento_dados'),

    url(r'^projeto/(?P<project_name>[^/]+)/dataset/(?P<dataset_id>\d+)/endosso/$', views.vis_endosso, name='vis_endosso'),
    url(r'^projeto/(?P<project_name>[^/]+)/dataset/(?P<dataset_id>\d+)/endosso/dados.json$', views.vis_endosso_dados, name='vis_endosso_dados'),

]


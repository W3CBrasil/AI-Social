Site do Dashboard
=================

Este diretório contém os scripts de processamento para gerar os arquivos de dados
utilizados pelo dashboard, além dos arquivos do site propriamente dito (html, css).

- `code`: scripts de processamento de dados 
- `site`: arquivos do site do dashboard


Softwares Utilizados
--------------------

### Processamento

- Python 3
- Biblioteca de python [Pandas](http://pandas.pydata.org/)

### Site

- [D3](http://d3js.org/)


Execução
--------

Estando no diretório `code`, basta executar os scripts de processamento python:
```
python3 extrai_texto_comentarios.py
python3 calcula_frequencias.py
python3 grafo_participacao.py
python3 grafo_participacao-topusuarios_eixos.py
python3 gera_tabela_topicos.py
```

Os scripts utilizam os arquivos de dados do diretório `dados` da raíz do repositório com entrada, e geram como saída os arquivos de dados do site dentro do diretório `site/data`


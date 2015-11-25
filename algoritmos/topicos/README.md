Modelos de Extração de Tópicos
==============================

Software necessário
-------------------

1. GibbsLDA++ - http://gibbslda.sourceforge.net/

   Versão livre do LDA com Gibbs sampling implementada em C++.

2. Python 

   Neccessário para rodar o script run_all.py

   Também é necessário ter instalado a biblioteca NLTK ( http://www.nltk.org/ ),
   assim como o submódulo de stopwords "nltk.stopwords".

3. run_all.py

   Script para extração de tópicos. Funciona em 3 etapas:
   1. Pré-processamwento da base de dados
   2. Execução do LDA
   3. Pós-processamento da saída do LDA.


Execução
--------

Se a estrutura de diretórios do github for seguida, de dentro da pasta codes, executar o seguinte comando:
```
python run_all.py
```

Os arquivos de entrada devem estar do diretorio de dados da raíz deste repositório:
```
../../../dados
```

Os arquivos de saída serão gravados em:
```
../data/output/
```

Para o caso de mudança de da base de dados, parâmetros do método ou diretórios, veja abaixo a descrição da entrada.

Entrada
-------

O arquivo run_all utiliza:
1. Um arquivo my_stopwords.txt 

   Disponibilizado no github, pasta codes/

2. Uma base de dados de onde se quer extrair os tópicos

   Disponivilizada no github, pasta /dados/dadospessoais-comentarios-pdfs-filtrado.csv

Dentro do arquivo run_all está definida a estrutura de diretórios de/para onde o script lê/escreve os arquivos,
bem como o número de tópicos que serão extraídos.

Para modificar esses parâmetros, é necessário editar em run_all.py:
- docs_comentarios  -  diretorio onde esta o texto a ser processado
- out_comentarios   -  diretorio onde sera salva a saida pos-processada do LDA
- num_topics        -  numero de topicos que serao gerados pelo LDA

Saída
-----

O scprit gera dois arquivos que podem ser analisados pelo usuário: 

1. topic_rank

   Nesse arquivos temos:
   1. Id do tópico
   2. Número de comentários associados ao tópico
   3. Lista de termos mais discriminativos do tópico

2. docs_per_topic_name.txt, onde name é um parâmetro passado para o script

   Nesse arquivo temos:
   1. Id do tópico
   2. Conjunto dos 10 termos mais descritivos do tópicos
   3. Lista de comentários associados ao tópico.



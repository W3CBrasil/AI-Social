Modelos de Extração de Tópicos

Software necessário:

1 - GibbsLDA++ - http://gibbslda.sourceforge.net/
Versão livre do LDA com Gibbs sampling implementada em C++.

2 - Python 
Neccessário para rodar o script run_all.py
Também é necessário ter instalado a biblioteca NLTK ( http://www.nltk.org/ ),
assim como o submódulo de stopwords "nltk.stopwords".

3- run_all.py
Script para extração de tópicos. Funciona em 3 etapas:
i.   Pré-processamwento da base de dados
ii.  Execução do LDA
iii. Pós-processamento da saída do LDA.


--- Execução --- 

Se a estrutura de diretórios do github for seguida, de dentro da pasta codes, executar o seguinte comando:

python run_all.py

Os arquivos de entrada devem estar em:
../data/

Os arquivos de saída serão gravados em:
../data/output/

Para o caso de mudança de da base de dados, parâmetros do método ou diretórios, veja abaixo a descrição da entrada.

--- Entrada ---

O arquivo run_all utiliza:
i.  Um arquivo my_stopwords.txt 
	Disponibilizado no github, pasta /codes/
ii. Uma base de dados de onde se quer extrair os tópicos
	Disponivilizada no github, pasta /data/comentarios.csv

Dentro do arquivo run_all está definida a estrutura de diretórios de/para onde o script lê/escreve os arquivos,
bem como o número de tópicos que serão extraídos.

Para modificar esses parâmetros, é necessário editar em run_all.py:
docs_comentarios  -  diretorio onde esta o texto a ser processado
out_comentarios   -  diretorio onde sera salva a saida pos-processada do LDA
num_topics        -  numero de topicos que serao gerados pelo LDA

--- Saída ---

O scprit gera dois arquivos que podem ser analisados pelo usuário: 

1. topic_rank
Nesse arquivos temos:
i.   Id do tópico
ii.  Número de comentários associados ao tópico
iii  Lista de termos mais discriminativos do tópico

2. docs_per_topic_name.txt, onde name é um parâmetro passado para o script
Nesse arquivo temos:
i.   Id do tópico
ii.  Conjunto dos 10 termos mais descritivos do tópicos
iii. Lista de comentários associados ao tópico.


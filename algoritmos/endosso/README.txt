Identificação de conteúdo relevante e de usuários influentes

A tarefa de identificação de conteúdo relevante busca estabelecer quais itens do anteprojeto e quais comentários geraram maior reação de endosso/concordância e de reprovação/discordância entre os usuários. Complementarmente, a tarefa de identificação de usuários influentes pretende encontrar os usuários cujos comentários causaram tais reações.

- Softwares necessários:

a) Python 2
b) Biblioteca SciPy para Python

- Dados de entrada:

Cada uma das tarefas é dividida em duas etapas: (i) preparação e (ii) execução do algoritmo ProfileRank.
A etapa (i) requer como entrada apenas a base de dados "dadospessoais-comentarios-pdfs-filtrado.jsonl".
A etapa (ii) requer como entrada as saídas da etapa (i), que serão apresentadas mais abaixo.

- Execução:

A etapa (i) é a execução do script "conteudo-relevante_usuarios-influentes.py", que lê a base de dados completa e a prepara como entrada para o algoritmo ProfileRank. 
Para executar a etapa (i), a base completa em formato .jsonl ("dadospessoais-comentarios-pdfs-filtrado.jsonl") deve estar no mesmo diretório que o script "conteudo-relevante_usuarios-influentes.py". Satisfeita essa condição, basta executar o seguinte comando:
python conteudo-relevante_usuarios-influentes.py
O script "conteudo-relevante_usuarios-influentes.py" não requer parametrização. Os arquivos de saída serão gravados no diretório "dados/".

A etapa (ii) é a execução do algoritmo ProfileRank, com base na saída gerada pelo script executado na etapa (i).
É necessário realizar os seguintes passos preliminares:
a. descompactar o código fonte do algoritmo ProfileRank. Em um shell, deve-se ir até o diretório onde o programa foi descompactado. Lá, haverá uma pasta chamada "ranking";
b. definir a variável de ambiente como sendo o diretório atual utilizando o seguinte comando:
export PYTHONPATH=.

O algoritmo ProfileRank solicita os seguintes parâmetros:
a. diretórios de entrada "-i" e saída "-o";
b. tipo de análise a ser realizada "-t", que pode assumir os valores "CONTENT" (para identificação de conteúdo relevante) ou "USER" (para identificação de usuários influentes);
c. arquivo de entrada "--date", que pode assumir os valores "20000101" (para a identificação de conteúdo endossado), "20000202" (para a identificação de conteúdo reprovado) e "20000303" (para a identificação de usuários influentes);
d. parâmetro fixo "-r", que deve sempre assumir o valor "0".

Assim, para executar o programa, basta utilizar um dos três seguintes comandos, de acordo com a função desejada (respectivamente: identificação de conteúdo endossado; identificação de conteúdo reprovado; identificação de usuários influentes):
python ranking/pr.py  -i dados/ -o dados/ -t CONTENT --date 20000101 -r 0
python ranking/pr.py  -i dados/ -o dados/ -t CONTENT --date 20000202 -r 0
python ranking/pr.py  -i dados/ -o dados/ -t USER --date 20000303 -r 0

- Saída:

A etapa (i) gera como saída três arquivos:
a. 20000101, contendo os nomes dos usuários que endossaram conteúdo, os números de identificação dos comentários onde ocorreram os endossos e os números de identificação dos itens do anteprojeto ou dos comentários endossados;
b. 20000202, contendo os nomes dos usuários que reprovaram conteúdo, os números de identificação dos comentários onde ocorreram as reprovações e os números de identificação dos itens do anteprojeto ou dos comentários reprovados;
c. 20000303, contendo os nomes dos usuários cujos comentários receberam endosso ou reprovação e os números de identificação desses comentários.

A etapa (ii) gera como saída os seguintes escores:
a. escore de endosso recebido, por conteúdo (item do anteprojeto ou comentário);
b. escore de reprovação recebida, por conteúdo (item do anteprojeto ou comentário);
c. escore de influência, por usuário.
Esses escores serão incorporados à base de dados na forma de novos campos na base de itens do anteprojeto, de comentários e de usuários.

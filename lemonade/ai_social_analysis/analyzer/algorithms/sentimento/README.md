Análise de Sentimentos
======================

Este diretório contém os scripts de processamento para gerar os arquivos de dados
utilizados pelo dashboard, além dos arquivos do site propriamente dito (html, css).

- `lac`: código fonte do LAC 
- `dados/entrada`: arquivos de entrada utilizados pelo LAC
- `dados/saida`: arquivos de saída com os resultados 

Execução
--------

Estando no diretório `sentimento`, basta executar o script `run_all.sh`:
```
./run_all.sh
```

Saída
-----

Após a execução, o script gera o arquivo `dados/saida/pos.txt`.

Cada linha do arquivo indica um score de sentimento de um comentário.

As linhas possuem 2 colunsa (separadas por um espaço em branco:
1. IDs: guarda informações do comentário, separados pelo caractere `_`. Os campos, internos são, nesta ordem:
  1. fonte do comentário
  2. ID do comentario
  3. ID do autor
  4. ID do item de lei
2. sentimento: escore de sentimento do comentario


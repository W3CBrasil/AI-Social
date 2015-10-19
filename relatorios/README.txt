Página de gráficos com dados aplicados
http://homepages.dcc.ufmg.br/~diego.barros/research/prodape/index.html

Mini-tutoriais de como acessar as ferramentas e navegar:

*Calaca* http://200.131.6.30:9200/_plugin/calaca/
   1. Digite a busca desejada.
   2. Ele exibe cada resultado em elementos individuais de HTML estilizado.
   Apenas alguns campos são exibidos.


*Browser*
http://200.131.6.30:9200/_plugin/browser/?database=dadospessoais&table=comment

   1. Digita a busca desejada no campo "search" e dê enter.
   2. Ele exibe os resultados no formato de tabela com todos os campos.


*Inquisitor* 200.131.6.30:9200/_plugin/inquisitor/ 

   1. Em "Index" selecione "dadospessoais"
   2. Em "Type" selecione "comment"
   3. No caixa de texto, digite a pesquisa avançada desejada, na sintaxe do
   ElasticSearch
      - Ex:
      {"query":{"bool":{"must":[{"term":{"comment.comment_text":"privacidade"}}]}}}
   4. Aperte o botão "Query"
   5. Os resultados serão exibidos em blocos individuais, contendo todos os
   campos, e com o termo buscado marcado de amarelo.


*Head * http://200.131.6.30:9200/_plugin/head/

   1. Acesse a aba "Consulta Estruturada"
   2. Em "Busca" selecione "dadospessoais"
   3. Monte a busca desejada. Ex:
      - "must"
      - "comment.comment_text"
      - "term"
      - "privacidade"
   4. Aperte o botão "Busca"
   5. O resultado será exibido em formato de tabela, com todos os campos.


*Kibana - Aba "Discover"* http://200.131.6.30:8080

   1. Bem similar ao Calaca, você digita a busca e ele exibe os resultados.
   2. A diferença é que ele retorna todos os campos, e marca o termo
   buscado.


*Kibana - Aba "Visualize"* http://200.131.6.30:8080

   1. Nessa aba é possível criar algumas visualizações simples.
   2. Cada tipo de visualização depende de informações diferentes e devem
   ser configuradas de acordo

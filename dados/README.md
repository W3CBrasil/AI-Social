Dados
=====

Temos basicamente dois arquivos de dados. 

O arquivo "dadospessoais-anteprojeto" contém os textos dos itens de lei presentes no anteprojeto, assim como a respectiva hierarquia e suas meta informações. Os campos (colunas) são:
- commentable_id: ID do item de lei.
- commentable_parent: ID do item de lei acima na hierarquia, a qual o item pertence.
- commentable_article: ID do item de lei do tipo artigo a qual o item pertence.
- commentable_chapter: ID do item de lei do tipo capítulo a qual o item pertence.
- commentable_axis: Número do eixo a qual o item pertenece (1 a 13).
- commentable_type: Tipo do item (artigo, alinea, inciso, paragrafo, seção, capítulo e geral).
- commentable_name: Nome abreviado do item de lei.
- commentable_text: Texto completo do item.

O arquivo "dadospessoais-comentarios-pdfs-filtrado" contém os comentários dos participantes dos debates, já filtrados e normalizados. Cada comentário está associado a algum item de lei da tabela "anteprojeto". Além disso, o comentário pode ter sido feito no website do debate ou através de um arquivo PDF. Os campos são:
- source: indica qual a mídia utilizada para enviar o comentário: "comment" indica um comentário feito no website do debate e "pdf" indica um comentário submetido via arquivo PDF.
- comment_id: ID do comentário.
- author_id: ID do participante que fez o comentário.
- author_name: nome do participante que fez o comentário.
- comment_parent: caso o comentário seja uma resposta direta a outro comentário, contém o ID do comentário respondido.
- commentable_id: ID do item de lei que o comentário referencia.
- comment_date: data e hora em que o comentário foi enviado. 
- comment_text: texto completo do comentário.

Além disso, os comentários têm campos adicionais que foram gerados através dos algoritmos de análise e processamento de dados:
- sentiment: valor calculado do algoritmo de análise de sentimentos
- topic: tópico identificado pelo algoritmo de topicalização 


Dados de PDF
------------

Para processar os textos submetidos através de documentos PDF foi necessário realizar um processamento manual. O processo envolveu a segmentação dos textos contidos nos PDFs em porções de textos menores, de modo que cada porção de texto pudesse ser associada a um item de lei do anteprojeto. Em alguns casos, as porções de texto referenciam múltiplos itens de lei, portanto essas porções foram replicadas. Dessa forma, cada replicação referencia um único item de lei. A data e hora exatas dos comentários de PDF são desconhecidas e foram fixadas em "2015-08-01 10:00:00".


Normalização e Filtragem dos Dados
----------------------------------

O banco de dados original dos comentários continha alguns comentários problemáticos (vazios, duplicados, etc). Esses comentários são prejudiciais para as análises, além de poderem gerar ruído nos resultados apresentados. Dessa forma, foi realizada uma tarefa de filtragem e normalização dos dados. As tarefas realizadas foram:

+ Limpeza do texto. Tarefas:
  - Conversão elementos de HTML (ex: "&lt;" --> "<")
  - Remoção tags HTML (ex: "<br>" --> "")
  - Remoção de quebras de linhas e espaços em branco (ex: "\n" --> " ")

+ Filtro de comentários. Foram removidos comentários:
  - vazios (em branco ou sem nenhuma palavra).
  - que continham apenas a palavra "teste".
  - duplicados (alguns usuarios fizeram, por erro dele ou do sistema, exatamente o mesmo comentário no mesmo item). Nesses casos apenas uma instância do comentário é mantida.
  - comentários de uma instituição (ABDTIC) foram replicados tanto no site quanto por PDF. No caso, mantivemos apenas os comentários do site.

Originalmente existiam 2015 comentários. Após a filtragem e normalização obtivemos 1856 comentários. 


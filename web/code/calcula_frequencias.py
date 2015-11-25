import datetime
import json
import csv
import numpy as np
import pandas as pd

# Configura contextos
contextos = [
    {"arquivo_comentarios":"../../dados/dadospessoais-comentarios-pdfs-filtrado.csv",
     "prefixo_saida":"../site/data/"}
]
#for i in range(1, 7):
#    obj = {"arquivo_comentarios" : "../../data/contextos/dadospessoais-comentarios-pdfs-filtrado-contexto{}.csv".format(i),
#           "prefixo_saida"       : "../../data_web/contexto{}/".format(i)}
#    contextos.append(obj)

anteprojeto = pd.read_csv("../../dados/dadospessoais-anteprojeto.csv", 
                          dtype={"commentable_id"      : pd.core.common.CategoricalDtype,
                                 "commentable_parent"  : pd.core.common.CategoricalDtype,
                                 "commentable_article" : pd.core.common.CategoricalDtype,
                                 "commentable_chapter" : pd.core.common.CategoricalDtype,
                                 "commentable_axis"    : pd.core.common.CategoricalDtype,
                                 "commentable_type"    : pd.core.common.CategoricalDtype,
                                 "commentable_name"    : pd.core.common.CategoricalDtype,
                                 "commentable_text"    : np.character})
for contexto in contextos:
    
    comentarios = pd.read_csv(contexto["arquivo_comentarios"], 
                              parse_dates=['comment_date'],
                              dtype={"source"         : pd.core.common.CategoricalDtype, 
                                     "comment_id"     : pd.core.common.CategoricalDtype, 
                                     "author_id"      : pd.core.common.CategoricalDtype, 
                                     "author_name"    : np.character, 
                                     "comment_parent" : pd.core.common.CategoricalDtype, 
                                     "commentable_id" : pd.core.common.CategoricalDtype, 
                                     "comment_text"   : np.character })
    
    comentarios.drop(["commentable_name"], axis=1, inplace=True)
    comentarios["comment_datetime"] = comentarios["comment_date"]
    comentarios["comment_date"] = comentarios["comment_datetime"].apply(lambda a: a.date())
    comentarios["comment_month"] = comentarios["comment_datetime"].apply(
            lambda a: datetime.datetime.strftime(a, "%b"))
    
    df = pd.merge(comentarios, anteprojeto, on="commentable_id")
    
    # Contagem temporal
    count_date = df[df["source"] == "comment"].groupby(["comment_date"]).size().to_frame("freq")
    count_date["freq_acum"] = count_date["freq"].cumsum()
    count_date.reset_index(level=0, inplace=True)
    count_date.columns = ["date", "freq", "freq_acum"]
    count_date.to_csv("{}comentarios_temporal_line_chart.tsv".format(contexto["prefixo_saida"]), sep="\t", quoting=csv.QUOTE_NONE, index=False, na_rep="nan")
    
    # Contagem de Itens
    count_item = df.groupby(["commentable_id", "commentable_name", "commentable_axis"]).size().to_frame("freq")
    count_item.reset_index(inplace=True)
    count_item.columns = ["id", "name", "group", "value"]
    count_item["id"] = count_item["id"].astype(int)
    count_item.sort_values("id", inplace=True)
    count_item["id"] = count_item["id"].apply(lambda x: "TCI{}".format(x))
    count_item.to_csv("{}comentarios_itens_bar_chart.csv".format(contexto["prefixo_saida"]), sep=",", quoting=csv.QUOTE_MINIMAL, index=False, na_rep="0")
    
    # Contagem de Eixos
    df_nomes_eixos = pd.DataFrame({"name":["Escopo e aplicação",
                                           "Dados Pessoais, Dados Anônimos e Dados Sensíveis",
                                           "Princípios",
                                           "Consentimento",
                                           "Término do Tratamento",
                                           "Direitos do Titular",
                                           "Comunicação, Interconexão e Uso Compartilhado de Dados",
                                           "Transferência Internacional de Dados",
                                           "Responsabilidade dos Agentes",
                                           "Segurança e Sigilo de Dados Pessoais",
                                           "Boas Práticas",
                                           "Como assegurar estes direitos, garantias e deveres?",
                                           "Disposições Transitórias"],
                                    "group":list(map(str, range(1, 14)))})
    count_eixo = df.groupby(["commentable_axis"]).size().to_frame("freq")
    count_eixo.reset_index(inplace=True)
    count_eixo.columns = ["group", "value"]
    count_eixo = pd.merge(count_eixo, df_nomes_eixos, on="group")
    count_eixo["id"] = count_eixo["group"].apply(lambda x: "TCE{}".format(x))
    count_eixo["group"] = count_eixo["group"].astype(int)
    count_eixo.sort_values("group", inplace=True)
    count_eixo = count_eixo[["id", "name", "group", "value"]]
    count_eixo.to_csv("{}comentarios_eixos_bar_chart_dashboard.csv".format(contexto["prefixo_saida"]), sep=",", quoting=csv.QUOTE_MINIMAL, index=False, na_rep="0")
    
    # Contagem de participantes
    count_participante = df.groupby(["author_id", "author_name"]).size().to_frame("freq")
    count_participante.reset_index(inplace=True)
    count_participante.columns = ["id", "name", "value"]
    count_participante["id"] = count_participante["id"].apply(lambda x: "TCP{}".format(x))
    count_participante.sort_values("value", ascending=False, inplace=True)
    count_participante.to_csv("{}comentario_participantes_bar_chart.csv".format(contexto["prefixo_saida"]), sep=",", quoting=csv.QUOTE_MINIMAL, index=False)
    
    # Numeros Gerais
    numeros_gerais = pd.DataFrame({"numero_usuarios":[df["author_id"].nunique()], 
                                   "numero_comentarios":[df["comment_id"].nunique()]})
    numeros_gerais = numeros_gerais[["numero_usuarios", "numero_comentarios"]]
    numeros_gerais.to_csv("{}numeros_gerais.tsv".format(contexto["prefixo_saida"]), sep="\t", quoting=csv.QUOTE_NONE, index=False)


#!/bin/bash

# Compila o LAC

cd lac
make
cd ..


# Preprocessamento

cat ../../dados/dadospessoais-comentarios-pdfs-filtrado.csv | gawk 'BEGIN{FS=","}{print($1"_"$2"_"$3"_"$6)}' > dados/entrada/data1.txt

cat ../../dados/dadospessoais-comentarios-pdfs-filtrado.csv | gawk 'BEGIN{FS=","}{print(tolower($8))}' | sed 's/\"/ /g' | sed 's/?/ ? /g' | sed 's/!/ ! /g' | sed 's/\./ \. /g' | sed 's/</ /g' | sed 's/>/ /g' | gawk '{printf("%s CLASS=0", $1); for(i=2;i<=NF;i++) printf(" w=%s", $i); printf("\n");}' > dados/entrada/data2.txt

paste dados/entrada/data1.txt dados/entrada/data2.txt | grep -v source_comment_id_author_id_commentable_id > dados/entrada/data.txt


# Realiza a classificação 

./lac/lazy -i dados/entrada/treino.txt -t dados/entrada/data.txt -s 1 -c 0.00001 -m 4 -e 10000000 > dados/saida/res.txt

grep "id= " dados/saida/res.txt | gawk '{print($4, $18)}' > dados/saida/pos.txt


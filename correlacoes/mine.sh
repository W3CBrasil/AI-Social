#!/bin/sh

# Called with dadospessoais-anteprojeto.csv dadospessoais-comentarios-pdfs-filtrado.csv

# awk -F , '$3 == "geral" || $3 == "capitulo" || $3 == "artigo" { print $1 "," $4 }' "$1" > articles
# articles edited by hand

# Human-readable output (content of a LaTeX tabular)

IFS=,
while read id parentId rest
do
    root=`grep -m 1 ^$id, articles | cut -d , -f 2`
    while [ -z "$root" -a -n "$parentId" ]
    do
	root=`grep -m 1 ^$parentId, articles | cut -d , -f 2`
	parentId=`grep -m 1 ^$parentId, "$1" | cut -d , -f 2`
    done
    if [ -n "$root" ]
    then
	echo $id,$root
    else
	echo $id >> uncategorized
    fi
done < "$1" | awk -F , 'ARGIND == 1 && NR > 1 { article[$1] = $2 }
ARGIND == 2 && $6 in article { print $3 "," article[$6] "," 1 }' - "$2" | multidupehack --ids , --ods \; --ha 1 /dev/stdin -o /dev/stdout | d-peeler -m --ids \; --ods " & " --ois ", " --all "Todas as seções" /dev/stdin -o /dev/stdout | ./sort-n-sets | sed 's/$/\\\\\n\\hline/'

# Computer-readable output

# IFS=,
# while read id parentId rest
# do
#     root=`grep -m 1 ^$id, articles | cut -d , -f 1`
#     while [ -z "$root" -a -n "$parentId" ]
#     do
# 	root=`grep -m 1 ^$parentId, articles | cut -d , -f 1`
# 	parentId=`grep -m 1 ^$parentId, "$1" | cut -d , -f 2`
#     done
#     if [ -n "$root" ]
#     then
# 	echo $id,$root
#     else
# 	echo $id >> uncategorized
#     fi
# done < "$1" | awk -F , 'ARGIND == 1 && NR > 1 { article[$1] = $2 }
# ARGIND == 2 && $6 in article { print $3, article[$6], 1 }' - "$2" | multidupehack --ha 1 /dev/stdin -o /dev/stdout | d-peeler -m --all "`cut -d , -f 1 articles | tr '\n' ','`" /dev/stdin -o /dev/stdout | ./sort-n-sets

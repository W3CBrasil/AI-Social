#!/bin/bash

#DATASET_INPUT="../../../../data/dadospessoais-comentarios-pdfs-filtrado-mesclado.jsonl" 
DATASET_INPUT=$1

SCRIPT_DIR=$(dirname $(readlink -f $0))
OUTPUT_DIR=$(mktemp -d "/tmp/endosso-XXXXXXXX")


# Preprocessamento

python3 $SCRIPT_DIR/tools/conteudo-relevante_usuarios-influentes.py $DATASET_INPUT $OUTPUT_DIR


# Realiza a classificação de endosso

cd $SCRIPT_DIR/tools
export PYTHONPATH=.

python2 ranking/pr.py  -i $OUTPUT_DIR/ -o $OUTPUT_DIR/ -t CONTENT --date 20000101 -r 0 2> /dev/null
python2 ranking/pr.py  -i $OUTPUT_DIR/ -o $OUTPUT_DIR/ -t CONTENT --date 20000202 -r 0 2> /dev/null
python2 ranking/pr.py  -i $OUTPUT_DIR/ -o $OUTPUT_DIR/ -t USER --date 20000303 -r 0 2> /dev/null

awk '{print "endosso", $1, $2}' $OUTPUT_DIR/C_20000101 > $OUTPUT_DIR/saida.txt  
awk '{print "reprov", $1, $2}' $OUTPUT_DIR/C_20000202 >> $OUTPUT_DIR/saida.txt 
awk '{print "influ", $1, $2}' $OUTPUT_DIR/U_20000303 >> $OUTPUT_DIR/saida.txt 

cat $OUTPUT_DIR/saida.txt


# Remove diretório com dados temporários

rm -rf "$OUTPUT_DIR"


#!/bin/bash

#DATASET_INPUT="../../../../data/dadospessoais-comentarios-pdfs-filtrado-mesclado.jsonl" 
DATASET_INPUT=$1

SCRIPT_DIR=$(dirname $(readlink -f $0))

# Compila o multidupehack

BIN_MULTIDUPEHACK="$SCRIPT_DIR/tools/multidupehack/multidupehack"

if [ ! -x $BIN_MULTIDUPEHACK ]; then
    cd "$SCRIPT_DIR/tools/multidupehack"
    make clean > /dev/null
    make > /dev/null
    cd ..
fi


# Compila o d-peeler

BIN_DPEELER="$SCRIPT_DIR/tools/d-peeler/d-peeler"

if [ ! -x $BIN_DPEELER ]; then
    cd "$SCRIPT_DIR/tools/d-peeler"
    make clean > /dev/null
    make > /dev/null
    cd ..
fi


# Seleciona campos que serão utilizados para a correlação
# Encontra padrões de correlação

jq -r '.author_id + " " + .commentable_article + " 1"' $DATASET_INPUT | \
    $BIN_MULTIDUPEHACK --ha 1 /dev/stdin -o /dev/stdout | \
    $BIN_DPEELER -m /dev/stdin -o /dev/stdout


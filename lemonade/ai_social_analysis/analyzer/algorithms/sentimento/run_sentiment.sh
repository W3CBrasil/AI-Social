#!/bin/bash

#DATASET_INPUT="../../../../data/dadospessoais-comentarios-pdfs-filtrado-mesclado.jsonl" 
DATASET_INPUT=$1

SCRIPT_DIR=$(dirname $(readlink -f $0))
OUTPUT_DIR=$(mktemp -d "/tmp/sentiment_analysis-XXXXXXXX")


# Compila o LAC

BIN_LAC="$SCRIPT_DIR/tools/lac/lazy"

if [ ! -x $BIN_LAC ]; then
    cd "$SCRIPT_DIR/tools/lac"
    make clean > /dev/null
    make > /dev/null
    cd ..
fi


# Preprocessamento

cat $DATASET_INPUT | 
    jq -r '.comment_id' \
    > "$OUTPUT_DIR/data1.txt"

cat $DATASET_INPUT | \
    jq -r '.comment_text' | \
    awk '{print(tolower($0))}' | \
    sed 's/\"/ /g' | \
    sed 's/?/ ? /g' | \
    sed 's/!/ ! /g' | \
    sed 's/\./ \. /g' | \
    sed 's/</ /g' | \
    sed 's/>/ /g' | \
    gawk '{printf("%s CLASS=0", $1); for(i=2;i<=NF;i++) printf(" w=%s", $i); printf("\n");}' \
    > "$OUTPUT_DIR/data2.txt"

paste "$OUTPUT_DIR/data1.txt" "$OUTPUT_DIR/data2.txt" \
    > "$OUTPUT_DIR/data.txt"


# Realiza a classificação 

$BIN_LAC \
    -i "$SCRIPT_DIR/dados/treino.txt" \
    -t "$OUTPUT_DIR/data.txt" \
    -s 1 -c 0.00001 -m 4 -e 10000000 \
    > "$OUTPUT_DIR/res.txt"

grep "id= " "$OUTPUT_DIR/res.txt" | \
    gawk '{print($4, $18)}' \
    > "$OUTPUT_DIR/pos.txt"

cat "$OUTPUT_DIR/pos.txt"


# Remove diretório com dados temporários

rm -rf "$OUTPUT_DIR"


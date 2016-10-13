#!/bin/bash

DATASET_INPUT=$1
N_TOPICS=$2
N_ITERS=$3
N_WORDS=$4

SCRIPT_DIR=$(dirname $(readlink -f $0))
OUTPUT_DIR=$(mktemp -d "/tmp/topicos-XXXXXXXX")


# Compila o lda

BIN_LDA="$SCRIPT_DIR/tools/GibbsLDA++-0.2/src/lda"

if [ ! -x $BIN_LDA ]; then
    cd "$SCRIPT_DIR/tools/GibbsLDA++-0.2"
    make clean > /dev/null
    make > /dev/null
    cd ..
fi


# Formata entrada para o LDA

wc -l $DATASET_INPUT > $OUTPUT_DIR/input.txt
cut -d " " -f 2- $DATASET_INPUT >> $OUTPUT_DIR/input.txt 


# Executa o LDA 

$BIN_LDA -est \
    -alpha 0.5 -beta 0.1 \
    -ntopics "$N_TOPICS" \
    -niters "$N_ITERS" \
    -twords $N_WORDS \
    -savestep "$N_TOPICS" \
    -dfile "$OUTPUT_DIR/input.txt" \
    > /dev/null


# Formata arquivos de saida

cat "$OUTPUT_DIR/model-final.twords" | \
    sed 's/Topic //g;s/   /,/g' | tr '\n' ' ' | sed 's/ \t/\;/g;s/ /\n/g;s/th://g;s/,/:/g;s/;/ /g' | \
    awk '{print "topic", $0}' \
    > "$OUTPUT_DIR/output.txt"

awk '{print $1}' $DATASET_INPUT | \
    paste -d " " - "$OUTPUT_DIR/model-final.theta" | \
    awk '{print "doc", $0}' \
    >> "$OUTPUT_DIR/output.txt"

cat "$OUTPUT_DIR/output.txt"


# Remove diretório com dados temporários

rm -rf "$OUTPUT_DIR"


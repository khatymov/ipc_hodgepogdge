#!/bin/bash
app="./reader"
args="../data/source.txt ../data/dest11.txt"
#args="../data/source.txt ../data/dest10.txt"
cd ./cmake-build-debug/
$app $args & $app $args
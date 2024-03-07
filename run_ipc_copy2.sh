#!/bin/bash
app="./reader"
args="../data/source.txt ../data/dest1.txt"
cd ./cmake-build-debug/
$app $args & $app $args
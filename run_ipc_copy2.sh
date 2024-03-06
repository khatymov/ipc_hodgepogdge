#!/bin/bash
app="./reader"
args="../data/source.txt ../data/target67.txt"
cd ./cmake-build-debug/
$app $args & $app $args
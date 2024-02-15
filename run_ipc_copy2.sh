#!/bin/bash
app="./server"
args="../data/source.txt ../data/target97.txt"
cd ./cmake-build-debug/
$app $args > first.txt & $app $args > second.txt
#!/bin/bash
app="./server"
args="../data/source.txt ../data/target78.txt"
cd ./cmake-build-debug/
$app $args & $app $args
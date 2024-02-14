#!/bin/bash
app="./server"
args="../data/source.txt ../data/target70.txt"
cd ./cmake-build-debug/
$app $args & $app $args & $app $args
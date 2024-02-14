#!/bin/bash
app="./server"
args="../data/source.txt ../data/target67.txt"
cd ./cmake-build-debug/
$app $args & $app $args
#!/bin/bash
app="./reader"
args="../data/rene_run.mp4 ../data/dest17.txt"
#args="../data/source.txt ../data/dest10.txt"
cd ./cmake-build-debug/
$app $args > ../out_1.txt & $app $args > ../out_2.txt
ls -tlh ../data/dest17.txt
#!/bin/bash

app="./reader"
source_file="../data/rene_run.mp4 "
target_file="../data/dest22.txt"
#args="../data/source.txt ../data/dest10.txt"
cd ./cmake-build-debug/
set -x
$app $source_file $target_file> "$target_file".out1 & $app $source_file $target_file> "$target_file".out2
ls -tlh ../data/$target_file
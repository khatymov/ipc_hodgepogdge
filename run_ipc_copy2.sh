#!/bin/bash
app="./reader"
args="../data/video.MP4 ../data/res5.mp4"
#args="../data/source.txt ../data/dest10.txt"
cd ./cmake-build-debug/
$app $args & $app $args
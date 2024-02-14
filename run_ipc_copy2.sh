#!/bin/bash

args="../data/source.txt ../data/target28.txt"
cd ./cmake-build-debug/
./my_project $args & ./my_project $args
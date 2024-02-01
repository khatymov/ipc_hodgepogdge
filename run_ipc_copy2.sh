#!/bin/bash

args="../data/source.txt ../data/target15.txt /shared_memory5.tmp"
cd ./cmake-build-debug/
./my_project $args & ./my_project $args
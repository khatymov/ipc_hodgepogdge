#!/bin/bash

args="../data/source.txt ../data/target29.txt"
cd ./cmake-build-debug/
./my_project $args & ./my_project $args & ./my_project $args
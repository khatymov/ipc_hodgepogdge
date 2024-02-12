#!/bin/bash

args="data/source.txt data/target.txt /shared_memory5.tmp"
./cmake-build-debug/my_project $args & ./cmake-build-debug/my_project $args
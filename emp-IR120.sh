#!/bin/bash

if [[ $# != '2' && $# != '3' ]]; then
  echo "$0 input.rel output-ir0.rel [--save-temps]"
  exit 1
fi

if [[ $3 == '--save-temps' ]]; then
	emp-compile $1 printer > a.cpp
   c++ a.cpp -std=c++11
else
	emp-compile $1 printer | c++ -x c++ -std=c++11 -
fi

./a.out > $2
if [[ $3 != '--save-temps' ]]; then
  rm ./a.out
fi

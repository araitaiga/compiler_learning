#!/bin/bash

# ./test.sh ./../build/9cc
# 9ccのpathを受け取る
path_9cc=$1

assert() {
  expected="$1"
  input="$2"

  # ./9cc "$input" >tmp.s
  $path_9cc "$input" >tmp.s
  # cc -o tmp tmp.s
  gcc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

input1="1+2"
expected1="3"
assert $expected1 $input1

input2="1+2-4+20"
expected2="19"
assert $expected2 $input2

echo OK

#!/bin/bash

test_dir="./test"
rm -rf $test_dir
mkdir $test_dir

echo "Building the calculator compiler..."
build_script_path="./build.sh"
bash $build_script_path

exe_path="./build/9cc"
echo "Exe File Path: $exe_path"

assert() {
  expected="$1"
  input="$2"

  # ./9cc "$input" >./test/tmp.s
  $exe_path "$input" >"$test_dir/tmp.s"
  # cc -o tmp tmp.s
  gcc -o "$test_dir/tmp" "$test_dir/tmp.s"
  $test_dir/tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo -e "\e[32m$input --> $actual\e[m"
  else
    echo -e "\e[31m$input --> $expected expected, but got $actual\e[m"
    # exit 1
  fi
}

input1="1+2"
expected1="3"
assert $expected1 $input1

input2="1+2-4+20"
expected2="19"
assert $expected2 $input2

input3="-2*+10+21"
expected3="1"
assert $expected3 $input3

input4="2*(-3+4)"
expected4="2"
assert $expected4 $input4

input5="2==2"
expected5="1"
assert $expected5 $input5

input6="2==3"
expected6="0"
assert $expected6 $input6

input7="2!=2"
expected7="0"
assert $expected7 $input7

input8="2!=3"
expected8="1"
assert $expected8 $input8

input9="0<(3<=3)==1"
expected9="1"
assert $expected9 $input9

# 先に右側から評価される
input10="0<(3==3)==1"
expected10="1"
assert $expected10 $input10

# echo OK

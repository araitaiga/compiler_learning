#!/bin/bash

## global variable
test_dir="./test"

## functions
function cleanup() {
  echo "Cleaning up..."
  rm -rf $test_dir
  mkdir $test_dir
}

function build() {
  echo "Building the calculator compiler..."
  local -r build_script_path="./build.sh"
  bash $build_script_path
}

function exeute() {
  local -r input=$1
  local -r exe_path="./build/9cc"
  # echo "Exe File Path: $exe_path"

  # ./9cc "$input" >./test/tmp.s
  $exe_path "$input" >"$test_dir/tmp.s"
  # cc -o tmp tmp.s
  gcc -o "$test_dir/tmp" "$test_dir/tmp.s"

  # binaryを実行
  $test_dir/tmp
}

function assert() {
  local -r input=$1
  local -r expected=$2

  exeute "$input"
  local -r actual=$?

  if [ "$actual" = "$expected" ]; then
    echo -e "\e[32m$input --> $actual\e[m"
  else
    echo -e "\e[31m$input --> $expected expected, but got $actual\e[m"
    # exit 1
  fi
}

function test() {

  input1="1+2;"
  expected1="3"
  assert $input1 $expected1

  input2="1+2-4+20;"
  expected2="19"
  assert $input2 $expected2

  input3="-2*+10+21;"
  expected3="1"
  assert $input3 $expected3

  input4="2*(-3+4);"
  expected4="2"
  assert $input4 $expected4

  input5="2==2;"
  expected5="1"
  assert $input5 $expected5

  input6="2==3;"
  expected6="0"
  assert $input6 $expected6

  input7="2!=2;"
  expected7="0"
  assert $input7 $expected7

  input8="2!=3;"
  expected8="1"
  assert $input8 $expected8

  input9="0<(3<=3)==1;"
  expected9="1"
  assert $input9 $expected9

  # 先に右側から評価される
  input10="0<(3==3)==1;"
  expected10="1"
  assert $input10 $expected10

  input11="a=3;a+3;"
  expected11="6"
  assert $input11 $expected11

  input12="hogeFuga_h=3;foo=3;hogeFuga_h+foo;"
  expected12="6"
  assert $input12 $expected12

  input13="hoge=3;foo=3;hoge-foo;"
  expected13="0"
  assert $input13 $expected13

  input14="hoge=3;hoge*3;"
  expected14="9"
  assert $input14 $expected14

  input15="hoge=3;piyo=20;3*(hoge+piyo);"
  expected15="69"
  assert $input15 $expected15

  # echo OK
}

## main
function main() {
  cleanup
  build
  test
}

main

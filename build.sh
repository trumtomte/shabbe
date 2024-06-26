#!/bin/bash

FLAGS="-g3 -Wall -Wextra -pedantic -Wconversion"
FILES="sha1.c sha1_linux.c -o sha1"

_run_test() {
    result=$(echo -n $1 | ./sha1)
    echo "========="
    echo "Testing:  $1"
    echo "Result:   $result"
    echo "Expected: $2"
    if echo -n "$result" | grep -q "$2"; then
	echo "Success!"
    else
	echo "Fail!"
    fi
}

case $1 in
    dev)
	gcc $FLAGS $FILES;;
    tests)
        gcc $FLAGS $FILES
	_run_test "The quick brown fox jumps over the lazy dog" "2fd4e1c67a2d28fced849ee1bb76e7391b93eb12  -"
	_run_test "" "da39a3ee5e6b4b0d3255bfef95601890afd80709  -"
	_run_test "abcdefg" "2fb5e13419fc89246865e7a324f476ec624e8740  -"
	;;
    *)
	gcc -O3 $FILES;;
esac

#!/bin/bash

if [ "$#" -ne 1 ]; then
	echo "Usage: ./tester.bash [request header file]"
	exit 1
fi

cat $1 | nc localhost 8080
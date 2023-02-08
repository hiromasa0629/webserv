#!/bin/bash

PID=$(lsof -i:8080 | awk '{print $2}')
echo $PID
if [[ "$PID" ]]; then
	kill $PID
fi
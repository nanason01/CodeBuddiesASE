#!/bin/bash

g++ -c helpers.cc exchanges.cc -std=c++17
# g++ -c base64.c -o base64.o
# g++ exchanges.o helpers.o base64.o -lssl -lcrypto -lcurl
g++ exchanges.o helpers.o -lssl -lcrypto -lcurl

./a.out

rm *.o a.out

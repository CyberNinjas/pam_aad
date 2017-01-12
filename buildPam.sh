#!/bin/bash

gcc -fPIC -fno-stack-protector -c src/aad.c

sudo ld -x --shared -o /lib/security/aad.so -lssl -lcrypto aad.o

rm aad.o

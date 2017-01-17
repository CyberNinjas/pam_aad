#!/bin/bash

gcc -fPIC -fno-stack-protector -c src/aad.c src/rest.c src/cJSON.c

sudo ld -x --shared -o /lib/security/aad.so -lssl -lcrypto -lm aad.o rest.o cJSON.o

rm aad.o

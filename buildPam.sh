#!/bin/bash

gcc -fPIC -fno-stack-protector -c src/aad.c src/rest.c src/cJSON.c src/utils.c src/jwt.c -w

sudo ld -x --shared -o /lib/security/aad.so -lssl -lcrypto -lm -ljwt aad.o rest.o cJSON.o utils.o jwt.o

rm aad.o

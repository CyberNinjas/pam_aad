#!/bin/bash

sudo g++ -g -Wall -shared -fPIC -o /lib/security/aad-pam.so src/aad-pam.o src/jwt.o src/utils.o -lrestclient-cpp -lcryptopp /usr/lib/x86_64-linux-gnu/libpthread.a -lssl -lcurl -lldap

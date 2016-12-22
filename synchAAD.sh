#!/bin/bash

sudo g++ -g -Wall -shared -fPIC -o /lib/security/aad-pam.so src/connection.o src/helpers.o src/jwt.o src/utils.o /usr/lib/librestclient-cpp.a -lcryptopp /usr/lib/x86_64-linux-gnu/libpthread.a -lpam_misc -lpam -lssl -lcurl -lldap

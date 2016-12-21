#!/bin/bash

sudo g++ -g -Wall -shared -o /src/connection.o /src/helpers.o /src/jwt.o src/utils.o /usr/lib/librestclient-cpp.a /usr/lib/libcryptopp.a /usr/lib/x86_64-linux-gnu/libpthread.a /usr/lib/x86_64-linux-gnu/libpam.a /usr/lib/x86_64-linux-gnu/libpam_misc.a /usr/lib/x86_64-linux-gnu/libcurl.a

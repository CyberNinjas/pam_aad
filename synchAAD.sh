#!/bin/bash

sudo g++ -shared -o /lib/security/add-pam.so src/aad-pam.o
rm src/aad-pam.o

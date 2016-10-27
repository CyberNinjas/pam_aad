# pam_aad
Azure Active Directory PAM Module

This PAM module aims to provide Azure Active Directory login over SSH to linux boxes.

## Requirements
1. libcurl must be installed on the server, as well as libcurl-dev package that provides cpp headers and shared libraries.
2. Didn't have luck building mratz's rest client from scratch, but he provides a package on packagecloud.io here: https://packagecloud.io/mrtazz/restclient-cpp/packages/ubuntu/trusty/restclient-cpp_0.4.4.1_amd64.deb. This got rid of any of my compilation issues.
3. Nhlohmann's brilliant [json][json_link] library for parsing json from Microsoft's REST api.

## Building the project
```make``` should be sufficient to build the binary, ```test-aad.out``` and the object file ```aad.o```. Then run:
```
sudo ld -x --shared -o /lib/security/aad.so add.o
```
To put your shared library where PAM expects it to be. 

##Example run
```

```

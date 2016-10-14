# pam_aad
Azure Active Directory PAM Module

# Requirements
1. libcurl must be installed on the server, as well as libcurl-dev package that provides cpp headers and shared libraries.
2. I didn't have look building mratz's rest client from scratch, but he provides a package on packagecloud.io here: https://packagecloud.io/mrtazz/restclient-cpp/packages/ubuntu/trusty/restclient-cpp_0.4.4.1_amd64.deb. This got rid of any of my compilation issues.

# Command to compile for testing

```g++ main.cc  -lcurl -lrestclient-cpp -I ../include/ -std=c++11 -o main.out```

# Command to compile for SSH 
```g++ main.cc -fPIC  -lcurl -lrestclient-cpp -I ../include/ -std=c++11 -o pam_ad.so```


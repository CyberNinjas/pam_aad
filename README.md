# pam_aad
Azure Active Directory PAM Module

This PAM module aims to provide Azure Active Directory login over SSH to linux boxes.

## Design Goals

1. Few (if any) external dependencies to maximize portability.

2. High stability.

3. Should be able to build on all systems with:

```
./bootstrap.sh
./configure
make
sudo make install
```









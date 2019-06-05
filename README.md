# pam_aad [![Build Status][travis-badge]][travis-url] [![GPL-3.0-or-later][gpl-badge]][gpl-license] [![Download](https://api.bintray.com/packages/jnchi/aad/libpam-aad/images/download.svg) ](https://bintray.com/jnchi/aad/libpam-aad/_latestVersion) 

Azure Active Directory PAM Module

_This PAM module aims to provide Azure Active Directory authentication for Linux._

## Installation

```
./bootstrap.sh
./configure --with-pam-dir=/lib/x86_64-linux-gnu/security/
make
sudo make install
```

## Configuration

Edit `/etc/pam.d/{{service}}` and add the following line:

```
auth required pam_aad.so
``` 

### Configuration File

Create the file ```/etc/pam_aad.conf``` and fill it with:

```mustache
{ 
  "client": {
    "id": "{{client_id}}"
  },
  "domain": "{{domain}}",
  "group": {
    "id": "{{group_id}}"
  },
  "smtp_server": "{{smtp_server}}",
  "tenant": {
    "name": "{{organization}}.onmicrosoft.com",
    "address": "{{organization_email_address}}"
  }
}
```

## Current behavior

[![asciicast](https://asciinema.org/a/250072.svg)](https://asciinema.org/a/250072)

[gpl-badge]: https://img.shields.io/badge/license-GPL-green.svg
[gpl-license]: COPYING
[travis-badge]: https://travis-ci.org/CyberNinjas/pam_aad.svg?branch=c-dev
[travis-url]: https://travis-ci.org/CyberNinjas/pam_aad

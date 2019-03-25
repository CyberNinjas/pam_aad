# pam_aad [![Build Status][travis-badge]][travis-url] [![GPL-3.0-or-later][gpl-badge]][gpl-license]

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

```terminal
An email with a one-time passcode was sent to your email.
Enter the code at https://aka.ms/devicelogin, then press enter.

Linux debian 4.9.0-8-amd64 #1 SMP Debian 4.9.144-3.1 (2019-02-19) x86_64

The programs included with the Debian GNU/Linux system are free software;
the exact distribution terms for each program are described in the
individual files in /usr/share/doc/*/copyright.

Debian GNU/Linux comes with ABSOLUTELY NO WARRANTY, to the extent
permitted by applicable law.
Last login: Thu Mar 21 11:54:47 2019 from 192.168.1.125
```

[gpl-badge]: https://img.shields.io/badge/license-GPL-green.svg
[gpl-license]: COPYING
[travis-badge]: https://travis-ci.org/CyberNinjas/pam_aad.svg?branch=c-dev
[travis-url]: https://travis-ci.org/CyberNinjas/pam_aad

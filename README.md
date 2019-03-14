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
   "tenant": "{{organization}}.onmicrosoft.com>"
}
```

## Module options

### client_id

This is the id of your application. Once you have created an application through <https://portal.azure.com>,
you will recieve a code in the form of: `xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx`.

### tenant

Your organization, e.g., `{{organization}}.onmicrosoft.com`, replaced by your 0365 organization name. 

### Current behavior

```terminal
Enter the following code at https://aka.ms/devicelogin : B8EYXPJQF
Please hit enter to begin polling...

The programs included with the Debian GNU/Linux system are free software;
the exact distribution terms for each program are described in the
individual files in /usr/share/doc/*/copyright.

Debian GNU/Linux comes with ABSOLUTELY NO WARRANTY, to the extent
permitted by applicable law.
```

[gpl-badge]: https://img.shields.io/badge/license-GPL-green.svg
[gpl-license]: COPYING
[travis-badge]: https://travis-ci.org/CyberNinjas/pam_aad.svg?branch=c-dev
[travis-url]: https://travis-ci.org/CyberNinjas/pam_aad

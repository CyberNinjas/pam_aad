# pam_aad [![Build Status][travis-badge]][travis-url] [![GPL-3.0-or-later][gpl-badge]][gpl-license]

Azure Active Directory PAM Module

_This PAM module aims to provide Azure Active Directory login to Linux over SSH._

## Installation

```
./bootstrap.sh
./configure --with-pam-dir=/lib/x86_64-linux-gnu/security/
make
sudo make install
```

## Configuration

Edit ```/etc/pam.d/sshd``` with your favorite text editor and add the following line at the top:

```mustache
auth required pam_aad.so
``` 

### Configuration File

Create the file ```/etc/pam.conf``` and fill it with:
```
{ 
  "client": {
      "id": "<client_id_here">
   },
   "domain": "<@mycompany.com>",
   "tenant": "<mycompany.onmicrosoft.com>"
}
```

## Module options

### client_id

This is the id of your application. Once you have create an application through <https://portal.azure.com>.
When you create your app through your Azure portal you will recieve a code in the form of 
`xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx`.

### tenant

Your organization. `[xxxxxx].onmicrosoft.com`, where `[xxxxxx]` is replaced by your 0365 organization name. 

### required_group_id

Checks if the user authenticating to the application is part of the group specified. This allows you to 
restrict access to certain machines to specific members of your organization.

### Current behavior

```
ssh me@host
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

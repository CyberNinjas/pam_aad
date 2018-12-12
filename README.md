# pam_aad [![Build Status][travis-badge]][travis-url] [![GPL-3.0-or-later][gpl-badge]][gpl-license]

Azure Active Directory PAM Module

_This PAM module aims to provide Azure Active Directory login to Linux over SSH._

## Installation

```
./bootstrap.sh
./configure
make
sudo make install
```

## Configuration

Edit ```/etc/pam.d/sshd``` with your favorite text editor and add the following line at the top:

```auth required aad.so client_id=yourid resource_id=resourceid tenant=YourOffice365Tenant required_group_id=xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx``` 

## Module options

### client_id

This is the id of your application. Once you have create an application through apps.dev.microsoft.com. When you create your app through your Azure portal you will recieve a code in the form of xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx 

### tenant

Your organization. [xxxxxx].onmicrosoft.com, where [xxxxxx] is replaced by your 0365 organization name. 

### required_group_id

Checks if the user authenticating to the application is part of the group specified. This allows you to restrict access to certain machines to specific members of your organization.

### Current behavior

```
login as: captain@digipirates.onmicrosoft
Using keyboard-interactive authentication.
Enter the following code at https://aka.ms/devicelogin : B8EYXPJQF
Please hit enter after you have logged in.

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

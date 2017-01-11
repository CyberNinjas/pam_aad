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

## Installation

```
./buildPam.sh
```

## Configuration
Edit ```/etc/pam.d/sshd``` with your favorite text editor and add the following line at the top:

```auth required aad.so client_id=yourid resource_id=resourceid tenant=YourOffice365Tenant``` 

## Module options

### client_id

This is the id of your application. Once you have create an application through apps.dev.microsoft.com. When you create your app through your Azure portal you will recieve a code in the form of xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx 

### resource_id
 
 Depicts the identifier of the WebAPI your client wants to access on behalf of the user. For PAM, that will most likely be 00000002-0000-0000-c000-000000000000. 

### tenant

Your organization. [xxxxxx].onmicrosoft.com, where [xxxxxx] is replaced by your 0365 organization name. 

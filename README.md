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

```auth required aad.so client_id=yourid resource_id=resourceid tenant=YourOffice365Tenant required_group_id=xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx``` 

## Module options

### client_id

This is the id of your application. Once you have create an application through apps.dev.microsoft.com. When you create your app through your Azure portal you will recieve a code in the form of xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx 

### resource_id
 
 Depicts the identifier of the WebAPI your client wants to access on behalf of the user. For PAM, that will most likely be 00000002-0000-0000-c000-000000000000. 

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

#### Issues with the above:

1. Note that I still have to have the user truncate their own usernames. 
2. It prompts the user, letting them know they're using keyboard-interactive authentication.
3. Requires a user to hit enter, instead of just displaying the message instead of polling for a set amount of time and logging in when the token is acquired and validated. 

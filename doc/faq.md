## User Provisioning

User provisioning is out of scope for this module. 

See: [libnss_aad](https://github.com/CyberNinjas/libnss_aad)

### Could not chdir to home directory /home/captain: No such file or directory

`/etc/pam.d/sshd`
```
session required pam_mkhomedir.so skel=/etc/skel/ umask=0022
```

## Username Mismatch
> ...the claimed username is tested against the actual username. --[#3](https://github.com/CyberNinjas/pam_aad/issues/3)

Please ensure that the user you are attempting to authenticate as, matches the username in Azure Active Directory e.g. `ssh captain@pam_aad` will check that `captain@digipirates.onmicrosoft` actually exists.

## PAM: Module is unknown

Please ensure that there are no typos in `/etc/pamd.d/sshd` as this can cause this error.

Also, check `/var/log/auth.log` to see if there are issues with the module itself,

```terminal
cat /var/log/auth.log
Feb 21 14:36:52 935854ca0b1b sshd[19286]: debug1: sshd version OpenSSH_7.4, OpenSSL 1.0.2q  20 Nov 2018
Feb 21 14:36:52 935854ca0b1b sshd[19286]: debug1: private host key #0: ssh-rsa SHA256:ZcCt3t//ArCoR2JBSFYPa3L9mNIu+3LWHC7M2YPem5g
Feb 21 14:36:52 935854ca0b1b sshd[19286]: debug1: private host key #1: ecdsa-sha2-nistp256 SHA256:f0t6r9hImgKdQ+bsKaM8xeXkFAOudPmZks4ZuIfDvtY
Feb 21 14:36:52 935854ca0b1b sshd[19286]: debug1: private host key #2: ssh-ed25519 SHA256:AqoDbiFHVyyhTL6Kqiwm0vLbq1lHYjdQtOvD8RuDVQc
Feb 21 14:36:52 935854ca0b1b sshd[19286]: PAM unable to dlopen(pam_aad.so): /lib/security/pam_aad.so: cannot open shared object file: No such file or directory
Feb 21 14:36:52 935854ca0b1b sshd[19286]: PAM adding faulty module: pam_aad.so
```

## Performance issues with OpenVPN auth-pam plugin

> Multiple users are unable to login using pam_aad in combination with the openvpn pam plugin. --[#39](https://github.com/CyberNinjas/pam_aad/issues/39)

This is due to an issue with OpenVPN itself,

> OpenVPN is single threaded authentication scripts and plugins would block and traffic to all clients stall until the auth-verify call returns. --[OpenVPN Ticket #1194](https://community.openvpn.net/openvpn/ticket/1194)

An OpenVPN Azure Active Directory plugin has been created to address this issue.

See: [openvpn-auth-aad](https://github.com/CyberNinjas/openvpn-auth-aad).


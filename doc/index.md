# pam_aad

## Compiling

1) Fetch source code:

```terminal
git clone https://github.com/CyberNinjas/pam_aad

cd pam_aad
```

2) Configure:

```terminal
./bootstrap.sh

./configure --with-pam-dir=/lib/x86_64-linux-gnu/security
```

3) Compile:

```terminal
make
```

4) Install:

```terminal
sudo make install
```

## Configuration

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
  },
  "debug": true # to optionally enable debugging mode
}
```

1) Azure:

- Login to the [Microsoft Azure Portal](portal.azure.com).

- In the sidebar on the left, navigate to "Azure Active Directory", then choose "App registrations (Preview)", then select "New registration".

  - Choose a Name e.g. `Azure Active Directory PAM Module for Linux`.

  - For Supported account types select `Accounts in this organizational directory only (Organization Name)`.

- Next click "Register", down at the bottom.

- From the "Overview" page, under "Manage", select "Authentication".

  - For "Redirect URIs":

    - Select `Public client (mobile & desktop)` from the "Type" drop-down field.

    - Enter `https://linux-auth.org` in the "Redirect URI" field.

  - For "Advanced settings":

    - Select `ID tokens`, under "Implicit grant".

  - For "Supported account types":

    - Select `Accounts in this organizational directory only (Organization Name)`.

- Next, click "Save", back up near the top.

- From the "Overview" page, under "Manage", select "API permissions".

  - Delete any existing permissions (The delegated permission, `Microsoft Graph (1)`, `User.Read` seems to be added by default).

  **NOTE: This module makes use of the `Windows Azure Active Directory` API, not the `Microsoft Graph` API** ([#8](https://github.com/CyberNinjas/pam_aad/issues/8)).

  - Select "Add a permission", then under "Supported legacy APIs", choose `Azure Active Directory Graph`.

    - Choose "Delegated permissions".

    - Under "Select permissions", choose `User.Read`.

2) PAM:

**NOTE: PAM configuration for module must be set to `required`.**

`/etc/pam.d/sshd`

```mustache
# Azure Active Directory authentication.
auth required pam_aad.so
```

3) OpenSSH:

`/etc/ssh/sshd_config`
```
ChallengeResponseAuthentication yes
UsePAM yes
PasswordAuthentication no
AuthenticationMethods keyboard-interactive
```

See also: [debian - How to use the ssh server with PAM but disallow password auth? - Server Fault](https://serverfault.com/questions/783082/how-to-use-the-ssh-server-with-pam-but-disallow-password-auth)

## Tools

- [pamtester](http://pamtester.sourceforge.net)

Example:

    pamtester -v sshd $(whoami) authenticate

## Resources

- [Azure Active Directory Documentation](https://docs.microsoft.com/en-us/azure/active-directory)

- [Azure Active Directory v2.0 and the OAuth 2.0 device code flow](https://docs.microsoft.com/en-us/azure/active-directory/develop/v2-oauth2-device-code)

- [Azure AD v2.0 Protocols (Postman Collection)](https://app.getpostman.com/view-collection/8f5715ec514865a07e6a?referrer=https%3A%2F%2Fapp.getpostman.com%2Frun-collection%2F8f5715ec514865a07e6a)

- [Basic PAM module and test application (GitHub)](https://github.com/beatgammit/simple-pam)

- [Linux-PAM (Pluggable Authentication Modules for Linux)](http://www.linux-pam.org)

- [pam_debug - debug the PAM stack](http://linux-pam.org/Linux-PAM-html/sag-pam_debug.html)

## See also

- [puppet-aad](https://github.com/Jnchi/puppet-aad)

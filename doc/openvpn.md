# OpenVPN Configuration

## Configuration

`/etc/openvpn/server.conf`

```txt
plugin /usr/lib/openvpn/openvpn-plugin-auth-pam.so openvpn
client-cert-not-required
username-as-common-name
```

`/etc/pam.d/openvpn`

```txt
auth required pam_aad.so client_id= resource_id= tenant= required_group_id=
@include common-auth
account required pam_nologin.so
@include common-account
@include common-session
@include common-password
```

-Or-

```terminal
cp /etc/pam.d/sshd /etc/pam.d/openvpn
```

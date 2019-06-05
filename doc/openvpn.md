# OpenVPN Configuration

## Configuration

### Server

`/etc/openvpn/server.conf`

```txt
plugin /usr/lib/openvpn/openvpn-plugin-auth-pam.so openvpn
client-cert-not-required
username-as-common-name
```

Source: [contrib/openvpn/server.conf](../contrib/openvpn/server.conf)

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

### Client

`/etc/openvpn/client.conf`, (or `C:\Program Files\OpenVPN\config\client.ovpn` on Windows)

```txt
# OpenVPN Client Configuration
client
dev tun
proto udp
remote 192.168.1.128 1194
nobind
;user nobody
;group nobody
persist-key
persist-tun
;mute-replay-warnings
ca ca.crt # from server
verb 5
auth-user-pass
```

Source: [contrib/openvpn/client.conf](../contrib/openvpn/client.conf)

## Resources

- [Debian Wiki - OpenVPN](https://wiki.debian.org/OpenVPN)
- [OpenVPN - 2x HOW TO](https://openvpn.net/community-resources/how-to)
- [Ubuntu Documentation - OpenVPN](https://help.ubuntu.com/lts/serverguide/openvpn.html)


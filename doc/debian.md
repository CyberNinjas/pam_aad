# Debian

## Dependencies

To install on Debian, the following package repository,
must be installed:

- [Azure Active Directory for Debian][bintray]

This can be installed via the following command:

```terminal
# Azure Active Directory for Debian
echo "deb https://dl.bintray.com/jnchi/aad unstable main" | tee -a /etc/apt/sources.list.d/aad.list
```

## Installation

```terminal
apt update && apt install -y libpam-aad
```

[bintray]: https://bintray.com/jnchi/aad

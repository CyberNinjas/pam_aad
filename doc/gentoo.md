# Gentoo

## Dependencies

To install on Gentoo, the following overlay must be installed:

- [Rage Overlay](https://gitlab.com/oxr463/overlay)

This can be installed via the following command:

```terminal
layman -a rage
```

## Installation

```terminal
ACCEPT_KEYWORDS="~amd64" emerge --ask sys-auth/pam_aad
```

## Troubleshooting

If `layman` is missing from your system,
it can be installed via the following command:

```terminal
emerge --ask app-portage/layman
```

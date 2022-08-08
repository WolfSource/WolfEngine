# GPG Setup

```shell
$ brew install gnupg
$ gpg --full-generate-key
$ gpg --list-secret-keys --keyid-format LONG ${EMAIL_ADDRESS}
$ gpg --armor --export ${SEC_ID}
$ git config --global commit.gpgsign true
$ git config --global gpg.program gpg
$ git config --global tag.gpgsign true
$ git config --global user.email "${EMAIL_ADDRESS}"
$ git config --global user.name "${FIRST_NAME} ${LAST_NAME}"
$ git config --global user.signingkey ${SEC_ID}
```

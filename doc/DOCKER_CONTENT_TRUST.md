# Docker Content Trust

```shell
$ docker trust key generate {NAME}

# LOAD
$ docker trust key load {NAME}.pem --name {NAME}

$ docker trust signer add --key {NAME}.pem {NAME} 127.0.0.1:5000/fip-api
$ docker trust sign 127.0.0.1:5000/fip-api:0.1.0-nonroot
$ export DOCKER_CONTENT_TRUST=1
$ docker push 127.0.0.1:5000/fip-api:0.1.0-nonroot

# TEST
$ docker trust inspect --pretty 127.0.0.1:5000/fip-api:0.1.0-nonroot

# REMOVE
$ docker trust revoke 127.0.0.1:5000/fip-api:0.1.0-nonroot
```

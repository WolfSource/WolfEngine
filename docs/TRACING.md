# Tracing

## Jaeger

```shell
$ docker pull jaegertracing/all-in-one:1.28.0
$ docker run --detach --publish 6831:6831/udp --publish 6832:6832/udp --publish 16686:16686 jaegertracing/all-in-one:1.28.0
```

## Zipkin

```shell
$ docker pull openzipkin/zipkin:2.23.6
$ docker run --detach --publish 9411:9411 openzipkin/zipkin:2.23.6
```

# Kubernetes

## Kubernetes Cluster Create

```shell
# 1st WAY
$ brew install kind
$ ./kind-with-registry.sh

# 2nd WAY
$ Docker for Desktop

# 3rd WAY
$ brew install ubuntu/microk8s/microk8s
$ microk8s install

# 4th WAY
$ brew cask install virtualbox
$ brew install minikube
$ minikube start

# 5th WAY
$ brew install k3d
```

## Kubernetes Cluster Working

```shell
$ brew install helm
$ brew install kube-ps1
$ brew install kubectx
$ brew install kubernetes-cli
```

## Kubernetes Cluster Working with UI

```shell
$ brew cask install lens
$ brew cask install kui
```

## Kubernetes Linkerd Service Mesh

```shell
$ brew install linkerd
$ curl https://kind.sigs.k8s.io/examples/kind-with-registry.sh > ./script/kind-with-registry.sh
$ chmod 777 ./script/kind-with-registry.sh
$ ./script/kind-with-registry.sh

# [https://linkerd.io/2.11/getting-started/]
$ kubectl version --short
$ curl --location --show-error --silent https://run.linkerd.io/install | sh [BY PASS IF YOU USED BREW]
$ linkerd version
$ linkerd check --pre
$ linkerd install | kubectl apply --filename -
$ linkerd check
$ linkerd viz install | kubectl apply --filename -

## optional
$ linkerd jaeger install | kubectl apply --filename -
$ linkerd multicluster install | kubectl apply --filename -
$ linkerd check
$ linkerd viz dashboard &
$ linkerd --namespace linkerd-viz viz top deployment/web [ERROR]

$ docker build . --file ./wolf-system/Dockerfile --tag wolf-system:0.1.0-nonroot
$ docker tag wolf-system:0.1.0-nonroot 127.0.0.1:5000/wolf-system:0.1.0-nonroot
$ docker push 127.0.0.1:5000/wolf-system:0.1.0-nonroot
$ cat ./wolf-system/wolf-system.yml | kubectl apply --filename -

$ kubectl --namespace wolf-system-namespace port-forward service/wolf-system 8080:8080

$ kubectl get --namespace wolf-system-namespace deployment --output yaml | linkerd inject - | kubectl apply --filename -
$ linkerd --namespace wolf-system-namespace check --proxy
$ linkerd --namespace wolf-system-namespace viz stat deployment
```

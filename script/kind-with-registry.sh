#!/bin/bash

set -eux
set -o errexit
set -o pipefail
set -o nounset

# create registry container unless it already exists
reg_name="kind-registry"
reg_port="5000"
running="$(docker inspect --format '{{.State.Running}}' "${reg_name}" 2>/dev/null || true)"
if [ "${running}" != "true" ]; then
  docker run --detach --name "${reg_name}" --publish "127.0.0.1:${reg_port}:5000" --restart always registry:2.7.1
fi

# create a cluster with the local registry enabled in containerd
cat <<EOF | kind create cluster --config=-
kind: Cluster
apiVersion: kind.x-k8s.io/v1alpha4
containerdConfigPatches:
- |-
  [plugins."io.containerd.grpc.v1.cri".registry.mirrors."127.0.0.1:${reg_port}"]
    endpoint = ["http://${reg_name}:${reg_port}"]
EOF

# connect the registry to the cluster network
# (the network may already be connected)
docker network connect "kind" "${reg_name}" || true

# Document the local registry
# https://github.com/kubernetes/enhancements/tree/master/keps/sig-cluster-lifecycle/generic/1755-communicating-a-local-registry
cat <<EOF | kubectl apply --filename -
apiVersion: v1
kind: ConfigMap
metadata:
  name: local-registry-hosting
  namespace: kube-public
data:
  localRegistryHosting.v1: |
    host: "127.0.0.1:${reg_port}"
    help: "https://kind.sigs.k8s.io/docs/user/local-registry/"
EOF


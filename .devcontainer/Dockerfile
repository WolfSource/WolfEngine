# syntax=docker.io/docker/dockerfile:1.3.1

ARG VARIANT="1.57.0-bullseye"
FROM rust:${VARIANT}

ARG ADD_NON_FREE_PACKAGES="true"
ARG CARGO_HOME="/usr/local/cargo"
ARG DOCKER_VERSION="latest"
ARG ENABLE_NONROOT_DOCKER="true"
ARG FIX_ENVIRONMENT="true"
ARG GIT_VERSION="latest"
ARG HELM_SHA256="automatic"
ARG HELM_VERSION="latest"
ARG INSTALL_OH_MYS="true"
ARG INSTALL_ZSH="true"
ARG KUBECTL_SHA256="automatic"
ARG KUBECTL_VERSION="latest"
ARG MINIKUBE_SHA256="automatic"
ARG MINIKUBE_VERSION="none"
ARG NEW_PASSWORD="vscode"
ARG RUSTUP_HOME="/usr/local/rustup"
ARG RUSTUP_PROFILE="default"
ARG RUST_VERSION="latest"
ARG SOURCE_SOCKET="/var/run/docker-host.sock"
ARG SSHD_PORT="2222"
ARG START_SSHD="true"
ARG TARGET_SOCKET="/var/run/docker.sock"
ARG UPDATE_RC="true"
ARG UPDATE_RUST="true"
ARG UPGRADE_PACKAGES="true"
ARG USERNAME="vscode"
ARG USER_GID="${USER_UID}"
ARG USER_UID="1000"
ARG USE_MOBY="true"
ARG USE_PPA_IF_AVAILABLE="latest"

RUN set -eux \
    && export DEBIAN_FRONTEND="noninteractive" \
    && apt-get update \
    # Remove imagemagick due to https://security-tracker.debian.org/tracker/CVE-2019-10131
    && apt-get purge --yes imagemagick imagemagick-6-common \
    && bash -c "$(curl --fail --location --output /tmp/library-scripts/meta.env --show-error --silent https://raw.githubusercontent.com/microsoft/vscode-dev-containers/main/containers/rust/.devcontainer/library-scripts/meta.env)" \
    && bash -c "$(curl --fail --location --show-error --silent https://raw.githubusercontent.com/microsoft/vscode-dev-containers/main/script-library/common-debian.sh)" -- "${INSTALL_ZSH}" "${USERNAME}" "${USER_UID}" "${USER_GID}" "${UPGRADE_PACKAGES}" "${INSTALL_OH_MYS}" "${ADD_NON_FREE_PACKAGES}" \
    && bash -c "$(curl --fail --location --show-error --silent https://raw.githubusercontent.com/microsoft/vscode-dev-containers/main/script-library/docker-debian.sh)" -- "${ENABLE_NONROOT_DOCKER}" "${SOURCE_SOCKET}" "${TARGET_SOCKET}" "${USERNAME}" "${USE_MOBY}" "${DOCKER_VERSION}" \
    # && bash -c "$(curl --fail --location --show-error --silent https://raw.githubusercontent.com/microsoft/vscode-dev-containers/main/script-library/docker-in-docker-debian.sh)" -- "${ENABLE_NONROOT_DOCKER}" "${USERNAME}" "${USE_MOBY}" "${DOCKER_VERSION}" \
    && bash -c "$(curl --fail --location --show-error --silent https://raw.githubusercontent.com/microsoft/vscode-dev-containers/main/script-library/git-from-src-debian.sh)" -- "${GIT_VERSION}" "${USE_PPA_IF_AVAILABLE}" \
    && bash -c "$(curl --fail --location --show-error --silent https://raw.githubusercontent.com/microsoft/vscode-dev-containers/main/script-library/kubectl-helm-debian.sh)" -- "${KUBECTL_VERSION}" "${HELM_VERSION}" "${MINIKUBE_VERSION}" "${KUBECTL_SHA256}" "${HELM_SHA256}" "${MINIKUBE_SHA256}" \
    && bash -c "$(curl --fail --location --show-error --silent https://raw.githubusercontent.com/microsoft/vscode-dev-containers/main/script-library/rust-debian.sh)" -- "${CARGO_HOME}" "${RUSTUP_HOME}" "${USERNAME}" "${UPDATE_RC}" "${UPDATE_RUST}" "${RUST_VERSION}" "${RUSTUP_PROFILE}" \
    && bash -c "$(curl --fail --location --show-error --silent https://raw.githubusercontent.com/microsoft/vscode-dev-containers/main/script-library/sshd-debian.sh)" -- "${SSHD_PORT}" "${USERNAME}" "${START_SSHD}" "${NEW_PASSWORD}" "${FIX_ENVIRONMENT}" \
    && bash -c "$(curl --fail --location --output /usr/local/share/copy-kube-config.sh --show-error --silent https://raw.githubusercontent.com/microsoft/vscode-dev-containers/main/containers/kubernetes-helm/.devcontainer/copy-kube-config.sh)" \
    && apt-get install --no-install-recommends --yes \
        binutils-aarch64-linux-gnu \
        binutils-arm-linux-gnueabihf \
        binutils-x86-64-linux-gnu \
        docker.io \
        musl-tools \
    && apt-get autoremove --yes \
    && apt-get clean --yes \
    && rm -fr /tmp/* /var/lib/apt/lists/* /var/tmp/*

RUN set -eux \
    && chown ${USERNAME}:root /usr/local/share/copy-kube-config.sh \
    && echo "source /usr/local/share/copy-kube-config.sh" | tee -a /root/.bashrc /root/.zshrc /home/${USERNAME}/.bashrc >> /home/${USERNAME}/.zshrc

ENV CROSS_DOCKER_IN_DOCKER=true
USER ${USERNAME}
RUN cargo install --locked cross
USER root

ENTRYPOINT ["/usr/local/share/docker-init.sh", "/usr/local/share/ssh-init.sh"]

CMD ["sleep", "infinity"]

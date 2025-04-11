#!/usr/bin/env bash

set -o errexit
set -o pipefail
set -o nounset
# set -o xtrace

GO_VERSION=1.24.1

remove_go() {
    rm -rf /usr/local/go
}

install_go() {
    remove_go
    ARCH=$(uname -m)
    
    if [ "$ARCH" = "x86_64" ]; then
        ARCH="amd64"
    elif [ "$ARCH" = "aarch64" ]; then
        ARCH="arm64"
    else
        echo "Unsupported architecture: ${ARCH}"
        exit 1
    fi
    
    echo "Installing Go ${GO_VERSION} for ${ARCH}"
    curl -sSfL https://go.dev/dl/go${GO_VERSION}.linux-${ARCH}.tar.gz | tar -C /usr/local -xz
}

install_go

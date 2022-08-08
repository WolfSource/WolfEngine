#!/bin/bash

set -eux
set -o errexit
set -o pipefail
set -o nounset

VERSION=${1:-"0.1.0"}
COMMIT_SHA=${2:-""}

GIT_COMMITTER_DATE=$(git --no-pager log --format="%aD" --max-count=1 "${COMMIT_SHA}") git tag -a -m "version ${VERSION}" "v${VERSION}" "${COMMIT_SHA}"
git push --tags

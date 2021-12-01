#!/bin/bash

set -eux
set -o errexit
set -o pipefail
set -o nounset

BRANCH_NAME=${1:-"main"}

for COMMIT_SHA in $(git log --format="%H" ${BRANCH_NAME} ^main); do
	git log --format="%B" ${COMMIT_SHA} | make conventional-commits-linter
done

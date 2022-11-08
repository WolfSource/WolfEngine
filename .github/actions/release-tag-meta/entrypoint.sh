#!/bin/sh

set -eux
set -o errexit
set -o pipefail
set -o nounset

ref=${1:-"refs/tags/v0.1.0"}
if [ -z "${ref##*'rc'*}" ] ;then
    prerelease=true
else
    prerelease=false
fi
tag=${ref#"refs/tags/"}
version=${ref#"refs/tags/v"}

echo "::set-output name=name::version ${version}"
echo "::set-output name=prerelease::${prerelease}"
echo "::set-output name=tag::${tag}"
echo "::set-output name=version::${version}"

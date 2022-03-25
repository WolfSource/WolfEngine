#!/bin/sh

set -eux
set -o errexit
set -o pipefail
set -o nounset

ref=${1:-"wolfengine"}
package=${ref#"wolf_"}

echo "::set-output name=package::${package}"

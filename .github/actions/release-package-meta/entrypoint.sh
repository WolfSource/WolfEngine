#!/bin/sh

set -eux
set -o errexit
set -o pipefail
set -o nounset

ref=${1:-"fip_api"}
package=${ref#"fip_"}

echo "::set-output name=package::${package}"

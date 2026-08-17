#!/usr/bin/env bash
set -euo pipefail
if [[ $# -ne 1 ]]; then echo "usage: $0 /path/to/nift" >&2; exit 2; fi
root=$(cd "$(dirname "$0")/.." && pwd)
nift=$(cd "$1" && pwd)
compare() { cmp -s "$1" "$2" || { echo "Jsonic++ sync mismatch: $1 != $2" >&2; exit 1; }; }
compare "$root/include/json.h" "$nift/jsonic/json.h"
compare "$root/include/json.h" "$nift/minifypp/src/Json.h"
grep -q '#include "../jsonic/json.h"' "$nift/src/Json.h" || { echo "Nift src/Json.h is not the Jsonic++ compatibility wrapper" >&2; exit 1; }
echo "Jsonic++ / Nift sync OK"

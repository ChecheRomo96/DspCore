#!/bin/sh
set -eu

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
. "$SCRIPT_DIR/common.sh"

PRESET=documentation
case "${1:-}" in
    ""|--*)
        ;;
    *)
        PRESET=$1
        shift
        ;;
esac

set -- "$SCRIPT_DIR/build.sh" "$PRESET" --target docs "$@"
"$@"

printf '%s\n' "Documentation: $DSPCORE_BUILD_ROOT/$PRESET/docs/html/index.html"

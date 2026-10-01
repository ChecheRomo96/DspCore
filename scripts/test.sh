#!/bin/sh
set -eu

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
. "$SCRIPT_DIR/romodular-adapter.sh"

ROMODULAR_COMMAND_NAME=$0
export ROMODULAR_COMMAND_NAME
exec "$DSPCORE_ROMODULAR_SCRIPTS/test.sh" "$@"

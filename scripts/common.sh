#!/bin/sh

. "$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)/romodular-adapter.sh"
. "$DSPCORE_ROMODULAR_SCRIPTS/common.sh"

dspcore_die() {
    romodular_die "$@"
}

dspcore_require_command() {
    romodular_require_command "$@"
}

dspcore_require_value() {
    romodular_require_value "$@"
}

dspcore_require_preset() {
    romodular_require_preset "$@"
}

dspcore_build_dir() {
    romodular_build_dir "$@"
}

dspcore_configuration() {
    romodular_configuration "$@"
}

dspcore_require_configuration() {
    romodular_require_configuration "$@"
}

dspcore_require_configured() {
    romodular_require_configured "$@"
}

dspcore_absolute_path() {
    romodular_absolute_path "$@"
}

dspcore_require_safe_dist_child() {
    romodular_require_safe_dist_child "$@"
}

#!/bin/sh
set -eu

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
. "$SCRIPT_DIR/common.sh"

usage() {
    printf '%s\n' "Usage: $0 <preset> [--output <path>] [--parallel <jobs>] [--fresh] [--keep] [--examples-on] [-- <cmake arguments>]"
}

PRESET=""
OUTPUT=""
PARALLEL=""
FRESH=0
KEEP=0
EXAMPLES_ON=0
CUSTOM_OUTPUT=0

while [ "$#" -gt 0 ]; do
    case "$1" in
        --output)
            dspcore_require_value "$1" "${2:-}"
            OUTPUT=$2
            CUSTOM_OUTPUT=1
            shift 2
            ;;
        --parallel)
            dspcore_require_value "$1" "${2:-}"
            PARALLEL=$2
            shift 2
            ;;
        --fresh)
            FRESH=1
            shift
            ;;
        --keep)
            KEEP=1
            shift
            ;;
        --examples-on)
            EXAMPLES_ON=1
            shift
            ;;
        --)
            shift
            break
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        -*)
            dspcore_die "unknown option: $1"
            ;;
        *)
            [ -z "$PRESET" ] || dspcore_die "only one preset may be specified"
            PRESET=$1
            shift
            ;;
    esac
done

if [ "$EXAMPLES_ON" -eq 1 ]; then
    set -- "$@" -DDSPCORE_EXAMPLES=ON
else
    set -- "$@" -DDSPCORE_EXAMPLES=OFF
fi

dspcore_require_preset "$PRESET"

[ -n "$OUTPUT" ] || OUTPUT="$DSPCORE_DIST_ROOT/$PRESET"
OUTPUT=$(dspcore_absolute_path "$OUTPUT")

if [ "$KEEP" -eq 0 ] && [ "$CUSTOM_OUTPUT" -eq 1 ]; then
    dspcore_die "custom export paths require --keep; use clean.sh to remove them explicitly"
fi

if [ "$FRESH" -eq 1 ]; then
    "$SCRIPT_DIR/configure.sh" "$PRESET" --fresh -- "$@"
else
    "$SCRIPT_DIR/configure.sh" "$PRESET" -- "$@"
fi

BUILD_DIR=$(dspcore_build_dir "$PRESET")
set -- cmake --build "$BUILD_DIR" --config Release --target DspCoreExportArtifacts
[ -z "$PARALLEL" ] || set -- "$@" --parallel "$PARALLEL"
"$@"

if [ "$KEEP" -eq 0 ]; then
    dspcore_require_safe_dist_child "$OUTPUT"
    cmake -E remove_directory "$OUTPUT"
fi

"$SCRIPT_DIR/install.sh" "$PRESET" --prefix "$OUTPUT"

printf '%s\n' "Exported DspCore (Release) to $OUTPUT"

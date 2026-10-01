#!/bin/sh
set -eu

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
. "$SCRIPT_DIR/common.sh"

usage() {
    printf '%s\n' "Usage: $0 <preset> [--fresh]"
    printf '%s\n' "Runs clang-tidy (.clang-tidy) over the DspCore sources of a Ninja preset."
    printf '%s\n' "Set CLANG_TIDY to choose the executable."
}

PRESET=""
FRESH=0

while [ "$#" -gt 0 ]; do
    case "$1" in
        --fresh)
            FRESH=1
            shift
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

dspcore_require_preset "$PRESET"
CLANG_TIDY=${CLANG_TIDY:-clang-tidy}
dspcore_require_command "$CLANG_TIDY"

set -- "$SCRIPT_DIR/configure.sh" "$PRESET"
[ "$FRESH" -eq 0 ] || set -- "$@" --fresh
"$@"

BUILD_DIR=$(dspcore_build_dir "$PRESET")
[ -f "$BUILD_DIR/compile_commands.json" ] || \
    dspcore_die "$BUILD_DIR/compile_commands.json not found; use a Ninja preset"

# A standalone clang-tidy on macOS does not know the SDK location.
EXTRA=""
if [ "$(uname -s)" = "Darwin" ] && command -v xcrun >/dev/null 2>&1; then
    SDK=$(xcrun --show-sdk-path 2>/dev/null || true)
    [ -z "$SDK" ] || EXTRA="--extra-arg=-isysroot$SDK"
fi

STATUS=0
for SOURCE in $(find "$DSPCORE_ROOT/src" -name '*.cpp' | sort); do
    printf '%s\n' "clang-tidy ${SOURCE#$DSPCORE_ROOT/}"
    "$CLANG_TIDY" -p "$BUILD_DIR" --quiet ${EXTRA:+"$EXTRA"} "$SOURCE" || STATUS=1
done

[ "$STATUS" -eq 0 ] || dspcore_die "clang-tidy reported findings"
printf '%s\n' "clang-tidy found no issues in the DspCore sources"

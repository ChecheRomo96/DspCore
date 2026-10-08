#!/bin/sh
set -eu

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
. "$SCRIPT_DIR/common.sh"

usage() {
    printf '%s\n' "Usage: $0 [--fqbn <board>] [--foundation <source-directory>] [--cpstl <source-directory>]"
    printf '%s\n' "Foundation defaults to DSPCORE_FOUNDATION_SOURCE or the sibling ../Foundation."
    printf '%s\n' "CPSTL defaults to DSPCORE_CPSTL_SOURCE or the sibling ../CPSTL."
}

# The validated Arduino source-mode board. Other cores are not validated here.
FQBN=arduino:avr:uno
FOUNDATION=${DSPCORE_FOUNDATION_SOURCE:-$DSPCORE_ROOT/../Foundation}
CPSTL=${DSPCORE_CPSTL_SOURCE:-$DSPCORE_ROOT/../CPSTL}

while [ "$#" -gt 0 ]; do
    case "$1" in
        --fqbn)
            dspcore_require_value "$1" "${2:-}"
            FQBN=$2
            shift 2
            ;;
        --foundation)
            dspcore_require_value "$1" "${2:-}"
            FOUNDATION=$2
            shift 2
            ;;
        --cpstl)
            dspcore_require_value "$1" "${2:-}"
            CPSTL=$2
            shift 2
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            dspcore_die "unknown argument: $1"
            ;;
    esac
done

command -v arduino-cli >/dev/null 2>&1 || dspcore_die "arduino-cli not found"
[ -f "$FOUNDATION/library.properties" ] || \
    dspcore_die "Foundation Arduino library not found at $FOUNDATION"
FOUNDATION=$(dspcore_absolute_path "$FOUNDATION")
[ -f "$CPSTL/library.properties" ] || \
    dspcore_die "CPSTL Arduino library not found at $CPSTL"
CPSTL=$(dspcore_absolute_path "$CPSTL")

# DspCore requires C++17. The stock Arduino AVR core compiles with gnu++11, and
# its avr-gcc 7.3 supports C++17 when asked; other cores keep their flags.
set --
case "$FQBN" in
    arduino:avr:*)
        set -- --build-property "compiler.cpp.extra_flags=-std=gnu++17"
        ;;
esac

BUILD_ROOT="$DSPCORE_ROOT/build/arduino/$(printf '%s' "$FQBN" | tr ':' '_')"
rm -rf "$BUILD_ROOT"

# Compile each sketch against the repository and Foundation as libraries,
# exactly as an Arduino user who installed both would.
COUNT=0
for SKETCH in "$DSPCORE_ROOT"/examples/DspCore/*/*/*.ino; do
    SKETCH_DIR=$(dirname -- "$SKETCH")
    NAME=${SKETCH_DIR#"$DSPCORE_ROOT/examples/DspCore/"}
    LOG="$BUILD_ROOT/$NAME.log"
    mkdir -p -- "$(dirname -- "$LOG")"
    printf '%s\n' "== $NAME ($FQBN)"

    STATUS=0
    arduino-cli compile \
        --fqbn "$FQBN" \
        --library "$DSPCORE_ROOT" \
        --library "$FOUNDATION" \
        --library "$CPSTL" \
        --build-path "$BUILD_ROOT/$NAME" \
        --warnings default \
        "$@" \
        "$SKETCH_DIR" >"$LOG" 2>&1 || STATUS=$?
    cat -- "$LOG"
    [ "$STATUS" -eq 0 ] || dspcore_die "$NAME failed to compile"

    # The stock AVR core passes -fpermissive, which demotes real type errors
    # to warnings; any warning in DspCore or its examples fails the gate.
    if grep -F "$DSPCORE_ROOT/" "$LOG" | grep -q "warning:"; then
        dspcore_die "$NAME compiled with DspCore warnings"
    fi
    COUNT=$((COUNT + 1))
done

[ "$COUNT" -gt 0 ] || dspcore_die "no Arduino sketches found"
printf '%s\n' "All $COUNT Arduino sketches compiled for $FQBN."

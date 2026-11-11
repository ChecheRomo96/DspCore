#!/bin/sh
set -eu

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
. "$SCRIPT_DIR/common.sh"

usage() {
    printf '%s\n' "Usage: $0 <preset> [--parallel <jobs>] [--fresh]"
}

PRESET=""
PARALLEL=""
FRESH=0

while [ "$#" -gt 0 ]; do
    case "$1" in
        --parallel)
            dspcore_require_value "$1" "${2:-}"
            PARALLEL=$2
            shift 2
            ;;
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
case "$PRESET" in
    macos_*|linux_*|windows_*)
        ;;
    *)
        dspcore_die "package consumer tests require a runnable desktop preset"
        ;;
esac

set -- "$SCRIPT_DIR/export.sh" "$PRESET"
[ "$FRESH" -eq 0 ] || set -- "$@" --fresh
[ -z "$PARALLEL" ] || set -- "$@" --parallel "$PARALLEL"
"$@"

DSPCORE_PREFIX="$DSPCORE_DIST_ROOT/$PRESET"
CONSUMER_SOURCE="$DSPCORE_ROOT/tests/PackageConsumer"
CONSUMER_BUILD="$DSPCORE_BUILD_ROOT/package-consumer/$PRESET"

DSPCORE_CACHE="$(dspcore_build_dir "$PRESET")/CMakeCache.txt"

dspcore_cache_value() {
    sed -n "s/^$1:[^=]*=//p" "$DSPCORE_CACHE" | sed -n '1p'
}

GENERATOR=$(dspcore_cache_value CMAKE_GENERATOR)
GENERATOR_PLATFORM=$(dspcore_cache_value CMAKE_GENERATOR_PLATFORM)
CXX_COMPILER=$(dspcore_cache_value CMAKE_CXX_COMPILER)
TOOLCHAIN_FILE=$(dspcore_cache_value CMAKE_TOOLCHAIN_FILE)
OSX_ARCHITECTURES=$(dspcore_cache_value CMAKE_OSX_ARCHITECTURES)
CROSSCOMPILING=$(dspcore_cache_value CMAKE_CROSSCOMPILING)

[ "$CROSSCOMPILING" != "TRUE" ] || \
    dspcore_die "package execution requires a native preset"
[ -n "$GENERATOR" ] || dspcore_die "configured preset has no CMake generator"

# The package consumer must use the same ABI and compiler family as the
# package. Recreate it so a previous run cannot retain another generator.
cmake -E remove_directory "$CONSUMER_BUILD"

set -- cmake \
    -S "$CONSUMER_SOURCE" \
    -B "$CONSUMER_BUILD" \
    -G "$GENERATOR" \
    -DDspCore_DIR="$DSPCORE_PREFIX/lib/cmake/DspCore" \
    -DCMAKE_PREFIX_PATH="$DSPCORE_PREFIX"

[ -z "$GENERATOR_PLATFORM" ] || set -- "$@" -A "$GENERATOR_PLATFORM"
[ -z "$OSX_ARCHITECTURES" ] || \
    set -- "$@" "-DCMAKE_OSX_ARCHITECTURES=$OSX_ARCHITECTURES"

if [ -n "$TOOLCHAIN_FILE" ]; then
    set -- "$@" "-DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN_FILE"
else
    case "$GENERATOR" in
        "Visual Studio"*|Xcode)
            ;;
        *)
            [ -z "$CXX_COMPILER" ] || \
                set -- "$@" "-DCMAKE_CXX_COMPILER=$CXX_COMPILER"
            ;;
    esac
fi

"$@"

set -- cmake --build "$CONSUMER_BUILD" --config Release
[ -z "$PARALLEL" ] || set -- "$@" --parallel "$PARALLEL"
"$@"

set -- ctest --test-dir "$CONSUMER_BUILD" --output-on-failure \
    --no-tests=error --build-config Release
[ -z "$PARALLEL" ] || set -- "$@" --parallel "$PARALLEL"
"$@"

printf '%s\n' "Verified installed DspCore package and its transitive Foundation dependency"

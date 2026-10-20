#!/bin/sh
set -eu

usage() {
    printf '%s\n' "Usage: $0 <signal.csv> [output.png]"
}

[ "$#" -ge 1 ] && [ "$#" -le 2 ] || { usage >&2; exit 2; }

CSV_PATH=$1
if [ "$#" -eq 2 ]; then
    PNG_PATH=$2
else
    case "$CSV_PATH" in
        *.csv) PNG_PATH=${CSV_PATH%.csv}.png ;;
        *) PNG_PATH=${CSV_PATH}.png ;;
    esac
fi

SCRIPT_DIRECTORY=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
if command -v python3 >/dev/null 2>&1; then
    PYTHON=python3
elif command -v python >/dev/null 2>&1; then
    PYTHON=python
else
    printf '%s\n' "Python 3 is required to render DspCore signal plots." >&2
    exit 1
fi

exec "$PYTHON" "$SCRIPT_DIRECTORY/plot_signal.py" "$CSV_PATH" --save "$PNG_PATH"

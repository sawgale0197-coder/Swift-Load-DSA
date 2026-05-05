#!/usr/bin/env sh
set -eu

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
cd "$SCRIPT_DIR"

OUTPUT=${OUTPUT:-main}

if [ -f "$OUTPUT" ]; then
    rm -f "$OUTPUT"
    echo "Removed ./$OUTPUT"
else
    echo "No built binary to remove."
fi

#!/usr/bin/env sh
set -eu

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
cd "$SCRIPT_DIR"

"$SCRIPT_DIR/end.sh"
"$SCRIPT_DIR/start.sh"

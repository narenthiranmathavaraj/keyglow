#!/usr/bin/env bash
# Adds the two third-party dependencies as pinned git submodules (idempotent).
set -euo pipefail
cd "$(dirname "$0")/.."

FREERTOS_TAG="V11.3.2"
UNITY_TAG="v2.7.0"

add_sub() {  # url path tag
  if [ ! -e "$2/.git" ] && ! git config -f .gitmodules --get "submodule.$2.url" >/dev/null 2>&1; then
    git submodule add "$1" "$2"
  fi
  git submodule update --init "$2" || true
  (cd "$2" && git fetch --tags --quiet && git checkout --quiet "$3")
}

if [ -d .git ]; then
  add_sub https://github.com/FreeRTOS/FreeRTOS-Kernel.git third_party/FreeRTOS-Kernel "$FREERTOS_TAG"
  add_sub https://github.com/ThrowTheSwitch/Unity.git      third_party/Unity          "$UNITY_TAG"
  echo "submodules ready: FreeRTOS-Kernel $FREERTOS_TAG, Unity $UNITY_TAG"
else
  echo "not a git repo yet: run 'git init' first" >&2
fi

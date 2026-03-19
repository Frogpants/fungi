#!/bin/bash
set -euo pipefail

rm -rf dist
rm -rf build-win
rm -rf build
rm -rf third_party
rm -rf src/external/include
rm -rf src/external/src

echo "Project reset complete."
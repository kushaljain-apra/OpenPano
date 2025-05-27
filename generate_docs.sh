#!/bin/bash

# Exit on any error
set -e

# 1. Create output directories if they don't exist
echo "Creating output directories..."
mkdir -p docs/doxygen
mkdir -p docs/sphinx/_build/html # Sphinx typically creates its own _build and html subdirs, but good to ensure parent exists

# 2. Run Doxygen
echo "Generating C++ documentation with Doxygen..."
doxygen Doxyfile

# 3. Run Sphinx
echo "Generating Python documentation with Sphinx..."
# Ensure the local sphinx-build is in PATH if installed via pip --user
export PATH="$HOME/.local/bin:$PATH"
sphinx-build -b html docs/sphinx docs/sphinx/_build/html

echo "Documentation generation complete."
echo "Main landing page: docs/index.html"
echo "Doxygen C++ docs: docs/doxygen/html/index.html"
echo "Sphinx Python docs: docs/sphinx/_build/html/index.html"

#!/bin/bash
git clone https://github.com/JGCRI/hector
cd hector
git checkout 417a174d2df2f3bbe928c0e7bccf8ca7c43bd700

# Apply a patch that effectively lets us disable logging,
# which removes the need for Boost Filesystem
cp ../no_logging.patch .
git apply no_logging.patch

# Remove all logging statements
# Saves about 60KB in the compiled WASM
sed -i 's/^.*H_LOG[^}]*//' src/*.cpp
sed -i 's/^.*<<.*//' src/*.cpp
# Fast Installer - For GNU+Linux
#!/usr/bin/env bash

set -e

echo "🔧 Installing zupt..."

# Create temp directory
TMP_DIR=$(mktemp -d)

# Clone repo
git clone https://github.com/cristiancmoises/zupt.git "$TMP_DIR/zupt"

cd "$TMP_DIR/zupt"

# Build
make clean
make

# Install
sudo cp zupt /usr/local/bin/

echo "✅ Installed to /usr/local/bin/zupt"

# Cleanup
cd ~
rm -rf "$TMP_DIR"

echo "🧹 Cleanup done"
echo "🔒 You can now run: zupt"

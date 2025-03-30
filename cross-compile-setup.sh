#!/bin/bash

# === Configuration ===
PI_HOST="q2"   # Replace with your Pi's IP or hostname
SYSROOT=".rpi-sysroot"      # Change if you want a different location
#!/bin/bash

# Sync only the needed directories

mkdir -p .rpi-sysroot

rsync -avz --delete \
  $PI_HOST:/usr/include/ $SYSROOT:usr/include/

rsync -avz --delete \
  $PI_HOST:/usr/lib/aarch64-linux-gnu/ $SYSROOT:usr/lib/aarch64-linux-gnu/

rsync -avz --delete \
  $PI_HOST:/lib/aarch64-linux-gnu/ $SYSROOT:lib/aarch64-linux-gnu/

rsync -avz --delete \
  $PI_HOST:/usr/lib/ $SYSROOT:usr/lib/

rsync -avz --delete \
  $PI_HOST:/lib/ $SYSROOT:lib/

echo "✅ Sysroot synced successfully!"
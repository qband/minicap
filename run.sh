#!/usr/bin/env bash

# Fail on error, verbose output
set -exo pipefail

# Build project
ndk-build 1>&2

# Figure out which ABI and SDK the device has
abi=$(adb shell getprop ro.product.cpu.abi | tr -d '\r')
sdk=$(adb shell getprop ro.build.version.sdk | tr -d '\r')
rel=$(adb shell getprop ro.build.version.release | tr -d '\r')

# PIE is only supported since SDK 16
if (($sdk >= 16)); then
  bin=minicap
else
  bin=minicap-nopie
fi

args=
if [ "$1" = "autosize" ]; then
  set +o pipefail
  size=$(adb shell dumpsys window | grep -Eo 'init=\d+x\d+' | head -1 | cut -d= -f 2)
  if [ "$size" = "" ]; then
    w=$(adb shell dumpsys window | grep -Eo 'DisplayWidth=\d+' | head -1 | cut -d= -f 2)
    h=$(adb shell dumpsys window | grep -Eo 'DisplayHeight=\d+' | head -1 | cut -d= -f 2)
    size="${w}x${h}"
  fi
  args="-P $size@$size/0"
  set -o pipefail
  shift
fi

# Create a directory for our resources
dir=/data/local/tmp/minicap-devel
adb shell "mkdir $dir 2>/dev/null"

# Upload the binary
adb push libs/$abi/$bin $dir
adb push policy.xml $dir
adb push DroidSansFallback.ttc $dir

# Upload the shared library
if [ -e jni/minicap-shared/aosp/libs/android-$rel/$abi/minicap.so ]; then
  adb push jni/minicap-shared/aosp/libs/android-$rel/$abi/minicap.so $dir
else
  adb push jni/minicap-shared/aosp/libs/android-$sdk/$abi/minicap.so $dir
fi

adb shell pkill minicap

# trap ctrl-c and call ctrl_c()
trap ctrl_c INT
function ctrl_c() {
  # Clean up
  adb shell rm -r $dir
}

# Run!
#adb shell LD_LIBRARY_PATH=$dir $dir/$bin $args "$@"
adb shell "export LD_LIBRARY_PATH=$dir; export MAGICK_CONFIGURE_PATH=$dir; export MAGICK_FONT_PATH=/system/fonts; export MAGICK_DEBUG=Exception; $dir/$bin $args -S -P 768x1280@277x461/0 -m \"AAAAAAA        BBBBBBBB        CCCCCCCC        DDDDDDDD        EEEEEEEE\"" #277 461

# NAME  data format width height stride bpp size
# AVD   0xbfd12c74 6 295 525 295 4 619500 FORMAT_RGBA_8888
# PHONE 0xbee28aa0 10 295 525 296 4 621600 FORMAT_BGRA_8888

#!/usr/bin/env bash

rm minicap.jpg 2>/dev/null
dir=/data/local/tmp/minicap-devel
bin=minicap
adb shell "export LD_LIBRARY_PATH=$dir; export MAGICK_CONFIGURE_PATH=$dir; export MAGICK_FONT_PATH=/system/fonts; export MAGICK_DEBUG=Exception; $dir/$bin -S -P 768x1280@277x461/0 -m 汉字:,test -s >$dir/minicap.jpg"
adb pull $dir/minicap.jpg
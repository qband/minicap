# minicap

## util
sed -n '36,60p' ./jni/vendor/libimagemagick/jni/vendor/libimagemagick/ImageMagick-6.7.3-0/wand/magick-wand-private.h | sed '/^$/d' | grep ",\|;" | sed 's~ *\|,\|;\|\*~~g' | sed 's~\[.*$~~' | awk '{print "            << \"\\nmagick_wand->"$1" \" << magick_wand->"$1}'
sed -n '92,152p' jni/vendor/libimagemagick/jni/vendor/libimagemagick/ImageMagick-6.7.3-0/wand/drawing-wand.c | sed '/^$/d' | grep ",\|;" | sed 's~ *\|,\|;\|\*~~g' | sed 's~\[.*$~~' | sed 's~/.*~~' | awk '{print "            << \"\\ndrawing_wand->"$1" \" << drawing_wand->"$1}'

## reference
- [minicap](https://github.com/openstf/minicap)
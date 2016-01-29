//
// Created by qband on 1/20/16.
//
#include "Watermark.hpp"
#include "util/debug.h"

#define ThrowAPIException(wand) \
{ \
  description=MagickGetException(wand,&severity); \
  (void) FormatLocaleFile(stderr,"%s %s %lu %s\n",GetMagickModule(), \
    description); \
  description=(char *) MagickRelinquishMemory(description); \
  exit(-1); \
}

Watermark::Watermark(unsigned int prePadding, unsigned int postPadding)
  : mTjHandle(tjInitCompress()),
    mSubsampling(TJSAMP_420),
    mEncodedData(NULL),
    mPrePadding(prePadding),
    mPostPadding(postPadding),
    mMaxWidth(0),
    mMaxHeight(0)
{
}

Watermark::~Watermark() {
  tjFree(mEncodedData);
}

bool
Watermark::add(Minicap::Frame* frame) {
  // declare variables
  ExceptionType severity;
  char *description;
  MagickWand *magick_wand;
  unsigned char pixels[frame->size];
  unsigned int status;
  const char *format=convertFormat(frame->format);
  ExceptionInfo *exception;
  DrawingWand *drawing_wand;
  PixelWand *fill;

  // initialize environment
  MagickWandGenesis();
  magick_wand=NewMagickWand();
  status=MagickConstituteImage(magick_wand,frame->width,frame->height,format,CharPixel, frame->data);
  if (status == MagickFalse)
    ThrowAPIException(magick_wand);
  drawing_wand=NewDrawingWand();
  fill=NewPixelWand();

  // set font style
  //MagickSetFont(magick_wand, "/system/fonts/Roboto-Regular.ttf");
  DrawSetFont(drawing_wand,"/system/fonts/Roboto-Regular.ttf");
  DrawSetFontSize(drawing_wand,18);
  PixelSetColor(fill,"white");
  DrawSetFillColor(drawing_wand,fill);

  // apply the watermark into image
  status=MagickAnnotateImage(magick_wand,drawing_wand,20,5,90,"Image");
  if (status == MagickFalse)
    ThrowAPIException(magick_wand);

  // export result
  status=MagickExportImagePixels(magick_wand,0,0,frame->width,frame->height,format,CharPixel,pixels);
  if (status == MagickFalse)
    ThrowAPIException(magick_wand);
  frame->data=pixels;

  // release mem resource
  if(fill) fill=DestroyPixelWand(fill);
  if(drawing_wand) drawing_wand=DestroyDrawingWand(drawing_wand);
  if(magick_wand) magick_wand=DestroyMagickWand(magick_wand);
  MagickWandTerminus();

  return true;
}

const char*
Watermark::convertFormat(Minicap::Format format) {
  switch (format) {
  case Minicap::FORMAT_RGBA_8888:
    return "RGBA";
  case Minicap::FORMAT_RGBX_8888:
    return "RGBX";
  case Minicap::FORMAT_RGB_888:
    return "RGB";
  case Minicap::FORMAT_BGRA_8888:
    return "BGRA";
  default:
    throw std::runtime_error("Unsupported pixel format");
  }
}

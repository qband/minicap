//
// Created by qband on 1/20/16.
//
#include <stdexcept>

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
  std::cout << "A"
              << " " << frame->data
              << " " << frame->format
              << " " << frame->width
              << " " << frame->height
              << " " << frame->stride
              << " " << frame->bpp
              << " " << frame->size
              << std::endl;

  // declare variables
  ExceptionType severity;
  char *description;
  MagickWand *magick_wand;
  unsigned char pixels[frame->size];
  unsigned int status;
  const char *format=convertFormat(frame->format);
  Image *image;
  ExceptionInfo *exception;
  DrawingWand *drawing_wand;
  PixelWand *fill;

  // initialize environment
  MagickWandGenesis();
  image=ConstituteImage(
    frame->width,
    frame->height,
    format,
    CharPixel,
    frame->data,
    exception);
  magick_wand=NewMagickWandFromImage(image);
  drawing_wand=NewDrawingWand();
  fill=NewPixelWand();

  (void) DrawRotate(drawing_wand,45);
  (void) DrawSetFontSize(drawing_wand,18);
  (void) PixelSetColor(fill,"green");
  (void) DrawSetFillColor(drawing_wand,fill);
  (void) DrawAnnotation(drawing_wand,15,5,(const unsigned char *) "Magick");
  //status=MagickDrawImage(magick_wand,drawing_wand);
  //if (status == MagickFalse)
  //  ThrowAPIException(magick_wand);
  //status=MagickAnnotateImage(magick_wand,drawing_wand,70,5,90,"Image");
  //if (status == MagickFalse)
  //  ThrowAPIException(magick_wand);

  status=MagickExportImagePixels(magick_wand,0,0,frame->width,frame->height,format,CharPixel,pixels);
  if (status == MagickFalse)
    ThrowAPIException(magick_wand);
  frame->data=pixels;

  image=DestroyImage(image);
  //fill=DestroyPixelWand(fill);
  drawing_wand=DestroyDrawingWand(drawing_wand);
  magick_wand=DestroyMagickWand(magick_wand);

  std::cout << "C"
            << " " << frame->data
            << " " << frame->format
            << " " << frame->width
            << " " << frame->height
            << " " << frame->stride
            << " " << frame->bpp
            << " " << frame->size
            << std::endl;
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

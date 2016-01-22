//
// Created by qband on 1/20/16.
//
#include <stdexcept>

#include "Watermark.hpp"
#include "util/debug.h"

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
  ExceptionInfo exception;
  Image *image;
  DrawInfo *draw_info;

  image=ConstituteImage(
    frame->width,
    frame->height,
    convertFormat(frame->format),
    CharPixel,
    frame->data,
    &exception);
  draw_info=AcquireDrawInfo();

  AnnotateImage(image, draw_info);
  //DestroyExceptionInfo(&exception);
  DestroyDrawInfo(draw_info);
  DestroyImage(image);

  std::cout << frame->data
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

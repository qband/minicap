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
  std::cout << "A"
              << " " << frame->data
              << " " << frame->format
              << " " << frame->width
              << " " << frame->height
              << " " << frame->stride
              << " " << frame->bpp
              << " " << frame->size
              << std::endl;

  ExceptionInfo *exception;
  Image *image;
  DrawInfo *draw_info;
  char label[]="test";
  ImageInfo *image_info;
  char geometry[MaxTextExtent];

  exception=AcquireExceptionInfo();

  image_info=AcquireImageInfo();
  image_info->signature=MagickSignature;
  image_info->debug=MagickTrue;

  image=ConstituteImage(
    frame->width,
    frame->height,
    convertFormat(frame->format),
    CharPixel,
    frame->data,
    exception);
  //image = BlobToImage(image_info, frame->data, frame->size, exception);
  //draw_info=AcquireDrawInfo();
  draw_info=CloneDrawInfo(image_info,(DrawInfo *) NULL);
  CloneString(&draw_info->text, label);
  FormatLocaleString(geometry, MaxTextExtent, "0x0%+ld%+ld", (long) 40, (long) 40);
  CloneString(&draw_info->geometry,geometry);

  std::cout << "B"
              << " " << (image==NULL)
              << " " << image->blob
              << " " << image->columns
              << " " << image->magick
              << std::endl;

  //RotateImage(image, 180, exception);
  //AnnotateImage(image, draw_info);
  //frame->data=image->blob;
  //frame->data = ImageToBlob(image_info, image, &(frame->size), exception);
  //DestroyExceptionInfo(&exception);
  //DrawGradientImage(image, draw_info);

  //DestroyDrawInfo(draw_info);
  //DestroyImage(image);
  MagickWand *wand;
  DrawingWand *drawing_wand;

  wand=NewMagickWandFromImage(image);
  drawing_wand=NewDrawingWand();

  MagickAnnotateImage(wand, drawing_wand, (double)10, (double)10, (double)10, ConstantString(label));

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

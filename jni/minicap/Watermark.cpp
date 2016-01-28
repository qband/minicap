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

  std::cout << "B"
            << "\nmagick_wand->id " << magick_wand->id
            << "\nmagick_wand->name " << magick_wand->name
            << "\nmagick_wand->exception " << magick_wand->exception
            << "\nmagick_wand->image_info " << magick_wand->image_info
            << "\nmagick_wand->quantize_info " << magick_wand->quantize_info
            << "\nmagick_wand->images " << magick_wand->images
            << "\nmagick_wand->active " << magick_wand->active
            << "\nmagick_wand->pend " << magick_wand->pend
            << "\nmagick_wand->debug " << magick_wand->debug
            << "\nmagick_wand->signature " << magick_wand->signature
            << "\ndrawing_wand->id " << drawing_wand->id
            << "\ndrawing_wand->name " << drawing_wand->name
            << "\ndrawing_wand->image " << drawing_wand->image
            << "\ndrawing_wand->exception " << drawing_wand->exception
            << "\ndrawing_wand->mvg " << drawing_wand->mvg
            << "\ndrawing_wand->mvg_alloc " << drawing_wand->mvg_alloc
            << "\ndrawing_wand->mvg_length " << drawing_wand->mvg_length
            << "\ndrawing_wand->mvg_width " << drawing_wand->mvg_width
//            << "\ndrawing_wand->pattern_id " << drawing_wand->pattern_id
//            << "\ndrawing_wand->pattern_bounds " << drawing_wand->pattern_bounds
//            << "\ndrawing_wand->pattern_offset " << drawing_wand->pattern_offset
            << "\ndrawing_wand->index " << drawing_wand->index
            << "\ndrawing_wand->graphic_context " << drawing_wand->graphic_context
            << "\ndrawing_wand->filter_off " << drawing_wand->filter_off
            << "\ndrawing_wand->indent_depth " << drawing_wand->indent_depth
            << "\ndrawing_wand->path_operation " << drawing_wand->path_operation
            << "\ndrawing_wand->path_mode " << drawing_wand->path_mode
            << "\ndrawing_wand->destroy " << drawing_wand->destroy
            << "\ndrawing_wand->debug " << drawing_wand->debug
            << "\ndrawing_wand->signature " << drawing_wand->signature
            << std::endl;

  status=MagickExportImagePixels(magick_wand,0,0,frame->width,frame->height,format,CharPixel,pixels);
  if (status == MagickFalse)
    ThrowAPIException(magick_wand);
  frame->data=pixels;

  if(image) image=DestroyImage(image);
  if(fill) fill=DestroyPixelWand(fill);
  if(drawing_wand) drawing_wand=DestroyDrawingWand(drawing_wand);
  if(magick_wand) magick_wand=DestroyMagickWand(magick_wand);

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

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
    pixels(NULL),
    mPrePadding(prePadding),
    mPostPadding(postPadding),
    mMaxWidth(0),
    mMaxHeight(0)
{
}

Watermark::~Watermark() {
  tjFree(mEncodedData);
  free(pixels);
}

bool
Watermark::add(Minicap::Frame* frame, const char* mark) {
  //std::cout << frame->data << " " << frame->format << " " << frame->width << " " << frame->height << " " << frame->stride << " " << frame->bpp << " " << frame->size << std::endl;

  // declare variables
  ExceptionType severity;
  char *description;
  MagickWand *magick_wand;
  if( pixels == NULL ) {
    MCINFO("Allocating %ld bytes for Watermark maker", frame->size);
    pixels = (unsigned char *)malloc(frame->size);
  }
  unsigned int status;
  const char *format=convertFormat(frame->format);
  ExceptionInfo *exception;
  DrawingWand *drawing_wand;
  PixelWand *fill;

  // initialize environment
  MagickWandGenesis();
  magick_wand=NewMagickWand();
  status=MagickConstituteImage(magick_wand,frame->stride,frame->height,format,CharPixel, frame->data);
  if (status == MagickFalse)
    ThrowAPIException(magick_wand);
  drawing_wand=NewDrawingWand();
  fill=NewPixelWand();
  // set the transparency
  MagickSetImageAlphaChannel(magick_wand,ActivateAlphaChannel);
  PixelSetColor(fill,"rgba(0,0,0,0.1)");
  MagickTransparentPaintImage(magick_wand,fill,1,10,MagickFalse);

  // Turn antialias on - not sure this makes a difference
  DrawSetTextAntialias(drawing_wand,MagickTrue);

  // set font style
  int byte_width = frame->width/50;
  int font_size = byte_width * 3;
  //DrawSetFont(drawing_wand, "/system/fonts/DroidSansFallback.ttf");
  DrawSetFont(drawing_wand, "/data/local/tmp/minicap-devel/DroidSansFallback.ttc");
  DrawSetFontSize(drawing_wand, font_size);
  PixelSetColor(fill,"rgba(80,80,80,1)");
  DrawSetFillColor(drawing_wand,fill);
  DrawSetGravity(drawing_wand,NorthWestGravity);

  // draw text
  double step = (frame->height/4.0)*0.8660254037844386;//python -c "import math;print(math.sin(math.pi/3));"
  double offset_x = frame->width / 3;
  double offset_y = 0;
  double move_x;
  double move_y;
  for(int i = 0; i < 5; i++) {
    move_x = -(2*i)*(8*byte_width)*0.8660254037844386;
    move_y = -(2*i)*(8*byte_width)*0.5;
    MagickAnnotateImage(magick_wand, drawing_wand, offset_x + move_x, offset_y + i * step + move_y, 30 ,mark);
  }

  // export result
  status=MagickExportImagePixels(magick_wand,0,0,frame->stride,frame->height,format,CharPixel,pixels);
  if (status == MagickFalse)
    ThrowAPIException(magick_wand);
  frame->data=pixels;

  // destroy mem resource
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

bool
Watermark::addStegano(Minicap::Frame* frame) {
  // declare variables
  char *description;
  ExceptionType severity;
  ExceptionInfo *exception;
  unsigned int status;
  MagickWand *magick_wand;
  MagickWand *watermark_wand;
  unsigned char pixels[frame->size];
  DrawingWand *drawing_wand;
  const char *format=convertFormat(frame->format);
  PixelWand *fill;

  // initialize environment
  MagickWandGenesis();
  magick_wand=NewMagickWand();
  status=MagickConstituteImage(magick_wand,frame->stride,frame->height,format,CharPixel, frame->data);
  if (status == MagickFalse)
    ThrowAPIException(magick_wand);
  drawing_wand=NewDrawingWand();
  watermark_wand=NewMagickWand();
  fill=NewPixelWand();
  status=MagickNewImage(watermark_wand,(frame->stride)/8,(frame->height)/8,fill);
  if (status == MagickFalse)
    ThrowAPIException(watermark_wand);

  // set font style
  DrawSetFont(drawing_wand, "/system/fonts/Roboto-Regular.ttf");
  DrawSetFontSize(drawing_wand,12);
  //PixelSetColor(fill,"rgba(192,192,192,0.01)");
  //DrawSetStrokeColor(drawing_wand,fill);
  PixelSetColor(fill,"rgba(255,255,255)");
  DrawSetFillColor(drawing_wand,fill);
  DrawSetGravity(drawing_wand,CenterGravity);

  // draw text on watermark_wand
  status=MagickAnnotateImage(watermark_wand,drawing_wand,0,0,0,"mark\nmark\nmark");
  if (status == MagickFalse)
    ThrowAPIException(magick_wand);

  // draw hiden watermark (watermark_wand) on magick_wand
  magick_wand=MagickSteganoImage(magick_wand,watermark_wand,+15);

  // export result
  status=MagickExportImagePixels(magick_wand,0,0,frame->stride,frame->height,format,CharPixel,pixels);
  if (status == MagickFalse)
    ThrowAPIException(magick_wand);
  frame->data=pixels;

//  MagickWriteImage(magick_wand,"/data/local/tmp/minicap-devel/test.png");

//  unsigned char* offset;

//  std::cout << "A" << std::endl;
//  MagickSetImageFormat(magick_wand,"jpg");
//  std::cout << "B" << std::endl;
//  offset=MagickGetImageBlob(magick_wand,(size_t*)&mEncodedSize);
//  std::cout << "C" << " " << offset << " " << mEncodedSize << std::endl;
//  mEncodedData=offset-mPrePadding;

  // destroy the resource
  if(fill)fill=DestroyPixelWand(fill);
  if(drawing_wand)drawing_wand=DestroyDrawingWand(drawing_wand);
  if(watermark_wand)watermark_wand = DestroyMagickWand(watermark_wand);
  if(magick_wand)magick_wand = DestroyMagickWand(magick_wand);
  MagickWandTerminus();

  return true;
}

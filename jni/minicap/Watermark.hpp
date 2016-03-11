//
// Created by qband on 1/20/16.
//

#ifndef MINICAP_WATERMARK_HPP
#define MINICAP_WATERMARK_HPP

#include <stdio.h>
#include <iostream>
#include <turbojpeg.h>
#include <stdexcept>
#include "Minicap.hpp"
#include <magick/api.h>
#include <wand/MagickWand.h>
#include <wand/magick-wand-private.h>

/*
  Typedef declarations.
*/
typedef enum
{
  PathDefaultOperation,
  PathCloseOperation,                        /* Z|z (none) */
  PathCurveToOperation,                      /* C|c (x1 y1 x2 y2 x y)+ */
  PathCurveToQuadraticBezierOperation,       /* Q|q (x1 y1 x y)+ */
  PathCurveToQuadraticBezierSmoothOperation, /* T|t (x y)+ */
  PathCurveToSmoothOperation,                /* S|s (x2 y2 x y)+ */
  PathEllipticArcOperation,                  /* A|a (rx ry x-axis-rotation large-arc-flag sweep-flag x y)+ */
  PathLineToHorizontalOperation,             /* H|h x+ */
  PathLineToOperation,                       /* L|l (x y)+ */
  PathLineToVerticalOperation,               /* V|v y+ */
  PathMoveToOperation                        /* M|m (x y)+ */
} PathOperation;

typedef enum
{
  DefaultPathMode,
  AbsolutePathMode,
  RelativePathMode
} PathMode;

struct _DrawingWand
{
  size_t
    id;

  char
    name[MaxTextExtent];

  /* Support structures */
  Image
    *image;

  ExceptionInfo
    *exception;

  /* MVG output string and housekeeping */
  char
    *mvg;               /* MVG data */

  size_t
    mvg_alloc,          /* total allocated memory */
    mvg_length;         /* total MVG length */

  size_t
    mvg_width;          /* current line width */

  /* Pattern support */
  char
    *pattern_id;

  RectangleInfo
    pattern_bounds;

  size_t
    pattern_offset;

  /* Graphic wand */
  size_t
    index;              /* array index */

  DrawInfo
    **graphic_context;

  MagickBooleanType
    filter_off;         /* true if not filtering attributes */

  /* Pretty-printing depth */
  size_t
    indent_depth;       /* number of left-hand pad characters */

  /* Path operation support */
  PathOperation
    path_operation;

  PathMode
    path_mode;

  MagickBooleanType
    destroy,
    debug;

  size_t
    signature;
};

class Watermark {
public:
  Watermark(unsigned int prePadding, unsigned int postPadding);

  ~Watermark();

  bool
  add(Minicap::Frame* frame, const char* mark);

  bool
  addStegano(Minicap::Frame* frame);

private:
  tjhandle mTjHandle;
  int mSubsampling;
  unsigned int mPrePadding;
  unsigned int mPostPadding;
  unsigned int mMaxWidth;
  unsigned int mMaxHeight;
  unsigned char* mEncodedData;
  unsigned long mEncodedSize;

  const char*
  convertFormat(Minicap::Format format);
};

#endif //MINICAP_WATERMARK_HPP

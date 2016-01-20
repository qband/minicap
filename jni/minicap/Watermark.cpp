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
  std::cout << frame->width << " " << frame->height << " " << frame->stride << " " << frame->bpp << " " << frame->format << std::endl;
  return true;
}
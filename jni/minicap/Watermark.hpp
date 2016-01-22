//
// Created by qband on 1/20/16.
//

#ifndef MINICAP_WATERMARK_HPP
#define MINICAP_WATERMARK_HPP

#include <stdio.h>
#include <iostream>
#include <turbojpeg.h>
#include "Minicap.hpp"
#include <magick/api.h>

class Watermark {
public:
  Watermark(unsigned int prePadding, unsigned int postPadding);

  ~Watermark();

  bool
  add(Minicap::Frame* frame);

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

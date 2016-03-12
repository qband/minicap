#!/usr/bin/env bash

sed -i 's~/data/local/tmp/minicap-devel/DroidSansFallback.ttc~/data/local/tmp/DroidSansFallback.ttc~' jni/minicap/Watermark.cpp
ndk-build 1>&2
sed -i 's~/data/local/tmp/DroidSansFallback.ttc~/data/local/tmp/minicap-devel/DroidSansFallback.ttc~' jni/minicap/Watermark.cpp

ARTIFACT_DIR=tmp/vendor/minicap
rm -rf $ARTIFACT_DIR
ls libs | xargs -I{} mkdir -p $ARTIFACT_DIR/bin/{}/
ls libs | xargs -I{} cp libs/{}/minicap{-nopie,} $ARTIFACT_DIR/bin/{}/
mkdir -p $ARTIFACT_DIR/shared/
cp -r jni/minicap-shared/aosp/libs/* $ARTIFACT_DIR/shared/
cp policy.xml $ARTIFACT_DIR
cp DroidSansFallback.ttc $ARTIFACT_DIR
cp minicap.js $ARTIFACT_DIR

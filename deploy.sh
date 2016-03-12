#!/usr/bin/env bash

DEST_DIR=$(readlink -f `which stf` | xargs dirname | xargs dirname)
MINICAP_VENDOR_DIR=$DEST_DIR/vendor/minicap
MINICAP_JS_PATH=$DEST_DIR/lib/units/device/resources/minicap.js

BACKUP_DIR=tmp/backup/`date +%Y%m%d-%H%M%S`
ARTIFACT_DIR=tmp/vendor/minicap

# backup
mkdir -p $BACKUP_DIR
cp -r $MINICAP_VENDOR_DIR $BACKUP_DIR
cp $MINICAP_JS_PATH $BACKUP_DIR

# deploy
sudo cp -r $ARTIFACT_DIR/* $MINICAP_VENDOR_DIR
sudo mv $ARTIFACT_DIR/minicap.js $MINICAP_JS_PATH
#!/usr/bin/env sh
DIR=$(dirname `which $0`)
if [ -z "$DIR" ]; then
  echo "\$DIR variable is empty"
  exit 1
fi

rm $DIR/*.log >/dev/null 2>&1
adb forward --remove tcp:1717 >/dev/null 2>&1
adb forward --remove tcp:5039 >/dev/null 2>&1
adb shell ps -ef | awk '$8=="minicap" {print $2}' | xargs -I{} sh -c "adb shell kill -9 {}" >/dev/null 2>&1
adb shell ps -ef | awk '$8=="minicap" {print $2}' | xargs -I{} sh -c "adb shell kill -9 {}" >/dev/null 2>&1

adb forward tcp:1717 localabstract:minicap
adb forward tcp:5039 tcp:5039

#nohup $DIR/run.sh -P 1080x1920@540x960/0 >$DIR/minicap.`date +%Y%m%d-%H%M%S`.log 2>&1 &
$DIR/run.sh -P 1080x1920@540x960/0
MINICAP_PID=`adb shell ps -ef | awk '$8=="minicap" {print $2}'`
FLAG=""
while [ -z "$MINICAP_PID" ]
do
  [[ -z "$FLAG" ]] && echo "wait for minicap startup" && FLAG="done"
  MINICAP_PID=`adb shell ps -ef | awk '$8=="minicap" {print $2}'`
  sleep 3
done
echo "minicap pid: $MINICAP_PID"

GDBSERVER_CMD="gdbserver :5039 --attach $MINICAP_PID" >$DIR/gdbserver.`date +%Y%m%d-%H%M%S`.log 2>&1 &
nohup adb shell $GDBSERVER_CMD >/dev/null 2>&1 &
GDBSERVER_PID=`adb shell ps -ef | awk '$8=="gdbserver" {print $2}'`
FLAG=""
while [ -z "$GDBSERVER_PID" ]
do
  [[ -z "$FLAG" ]] && echo "wait for gdbserver startup" && FLAG="done"
  GDBSERVER_PID=`adb shell ps -ef | awk '$8=="gdbserver" {print $2}'`
  sleep 3
done
echo "gdbserver pid: $GDBSERVER_PID"

gdb $DIR/libs/`adb shell getprop ro.product.cpu.abi | tr -d '\r'`/minicap
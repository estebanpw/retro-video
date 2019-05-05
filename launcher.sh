#!/bin/bash


if [ $# != 4 ]; then
	echo "***ERROR*** Use: launcher.sh video outvideo [date MM/DD/YYYY] [daytime hh:mm:ss]"
	exit -1
fi

VIDEO=$1
RESULT=$2
STRING=$3
STRINGHOURS=$4

SUPERSTRING=$(echo "$STRING $STRINGHOURS")

DATE=$(date -d "$SUPERSTRING" +"%s") # format MM/DD/AAAA hh:mm:ss

echo "using date $DATE"

(ffmpeg -i $VIDEO -hide_banner) &> info.log

INFO=$(cat info.log | grep "Stream")

INFOVIDEO=$(echo "$INFO" | grep "Video:")
INFOAUDIO=$(echo "$INFO" | grep "Audio:")

RESX=$(echo "$INFOVIDEO" | awk -F "," '{print $3}' | awk '{print $1}' | sed 's/x/ /g' | awk '{ print $1}')
RESY=$(echo "$INFOVIDEO" | awk -F "," '{print $3}' | awk '{print $1}' | sed 's/x/ /g' | awk '{ print $2}')
FPS=$(echo "$INFOVIDEO" | awk -F "," '{print $5}' | awk '{print $1}')

AUDIOCODEC=$(echo "$INFOAUDIO" | awk -F " " '{print $4}' | sed 's/,//g')


#echo "RESX: $RESX RESY: $RESY"
#echo "AUDIOCODEC: $AUDIOCODEC"
#echo "fps: $FPS"

echo "Launching ./retro-video $VIDEO $RESX $RESY $FPS $RESULT $AUDIOCODEC $DATE"
./retro-video $VIDEO $RESX $RESY $FPS $RESULT $AUDIOCODEC $DATE

rm info.log

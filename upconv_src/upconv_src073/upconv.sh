#!/bin/sh


dir=${1%/*}
fname=${1##*/}
echo -s:88200 -w:24 -hfa:3 -hfc:20500 -adjBE -cutLowData -smLowData -overSamp:6 -thread:4 -deEmphasis:0 -postABE -sig1:1,4,0 -sig2:1,0 -sig3:1,0 -hfaNB:0 -hfaWide -hfaDiffMin:1 -m:2 -n:0 -ditherLv:0 -ch:2 -dsf:0 -fio:200 > $dir/${fname%.*}_upconv.param
#wav2raw $1 $dir/${fname%.*}_upconv.wav
#upconv $dir/${fname%.*}_upconv.wav $dir/${fname%.*}_upconv.r1
#upconv $dir/${fname%.*}_upconv.wav $dir/${fname%.*}_upconv.r2
#raw2wav $1 $dir/${fname%.*}_upconv.wav

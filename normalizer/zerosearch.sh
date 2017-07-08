#!/bin/sh
for i in `seq 0 10`
do
#for j in `seq 0 10`
#do
#for k in `seq 0 10`
#do
  #Z1=`echo "0.969100+(2-2)*0.0000001" | bc`
  Z1=`echo "0.939+($i-5)*0.001" | bc`
  Z2=`echo "0.724+(5-5)*0.001" | bc`
  Z3=`echo "0.392+(5-5)*0.001" | bc`
  ./normalizer test.wav ./zero/test.$i.$j.$k.wav -0.1 -z $Z1 $Z2 $Z3 0.85 -b 32
  #sox --norm test.wav -t wavpcm ./zero/test.$i.$j.$k.wav
done
#done
#done

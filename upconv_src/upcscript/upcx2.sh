#!/bin/bash
IFS="
"

#dir=$(dirname $1)/
#tmp=${1##*/}
#fname=`basename $1`
#echo ${fname}
#echo $dir
args=$*
num=0

trap 'rm /tmp/tmpfile*; exit 1' 1 2 3 15

while read i
do
num=$(( num+1 ))

cp $i /tmp/tmpfile.wav
echo -n '-s:176400 -w:24 -hfa:3 -hfc:41000 -adjBE -cutLowData -smLowData -overSamp:2 -thread:4 -deEmphasis:0 -postABE -sig1:1,4,-3 -sig2:1,-5 -sig3:1,-9 -hfaNB:0 -hfaWide -hfaDiffMin:1 -m:2 -n:0 -ditherLv:0 -ch:2 -dsf:0 -fio:200 ' > /tmp/tmpfile_upconv.param
echo "wav2raw start $i" >> /tmp/upconv.log
wav2raw /tmp/tmpfile.wav /tmp/tmpfile_upconv.wav | 
while read line 
do
	clear
	echo "wav2raw($i)"	
#echo -n "^[[1;1H"
	echo -n $line
done

echo "upconv_r1 start $i" >> /tmp/upconv.log
upconv /tmp/tmpfile_upconv.wav /tmp/tmpfile_upconv.r1 | 
while read line
do                                                                                                                                                                 
        clear
	if [[  line =~ ABE* ]]; then
		str=ABE	
	elif [[ line =~ SRC* ]]; then
		str=SRC
	elif [[ line =~ HFA3 ]]; then
		str=HFA3
	else

        echo "upconv_r1_$str($i)"
#echo -n "^[[1;1H"
        echo -n $line       
	fi                                                                                                                                       
done 

echo "upconv r2 start $i" >> /tmp/upconv.log
upconv /tmp/tmpfile_upconv.wav /tmp/tmpfile_upconv.r2 |
while read line
do
	clear
	if [[ line =~ ABE* ]]; then
                str=ABE
        elif [[ line =~ SRC* ]]; then                                                                                                                                
                str=SRC
        elif [[ line =~ HFA3* ]]; then
                str=HFA3                                                                                                                                           
        else
	echo "upconv_r2_$str($i)" 
	echo -n $line
	fi
done

echo "raw2wav start $i" >> /tmp/upconv.log
raw2wav /tmp/tmpfile.wav /tmp/tmpfile_upconv.wav
mv /tmp/tmpfile_upconv.wav $num.wav
mv $num.wav $i.upconv.wav
rm /tmp/tmpfile*

done

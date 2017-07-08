#!/bin/sh
IFS="
"

#dir=$(dirname $1)/
#tmp=${1##*/}
#fname=`basename $1`
#echo ${fname}
#echo $dir
args=$*



cp $1 /tmp/tmpfile.wav
echo -n '-s:88200 -w:24 -hfa:3 -hfc:20500 -adjBE -cutLowData -smLowData -overSamp:6 -thread:4 -deEmphasis:0 -postABE -sig1:1,4,0 -sig2:1,0 -sig3:1,0 -hfaNB:0 -hfaWide -hfaDiffMin:1 -m:2 -n:0 -ditherLv:0 -ch:2 -dsf:0 -fio:200 ' > /tmp/tmpfile_upconv.param
wav2raw /tmp/tmpfile.wav /tmp/tmpfile_upconv.wav | 
while read line 
do
	clear
	echo "wav2raw"	
#echo -n "^[[1;1H"
	echo -n $line
done

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

        echo "upconv_r1_$str"
#echo -n "^[[1;1H"
        echo -n $line       
	fi                                                                                                                                       
done 

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
	echo "upconv_r2_$str" 
	echo -n $line
	fi
done


raw2wav /tmp/tmpfile.wav /tmp/tmpfile_upconv.wav
mv /tmp/tmpfile_upconv.wav $1.upconv.wav
rm /tmp/tmpfile*

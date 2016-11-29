#!/bin/bash

START=2
STOP=268435456
STEP=1000

set -e

for ((i=$START;i<=$STOP-$STEP;i+=$STEP)); do
	EXP_LO=$i
	EXP_HI=$(($i+$STEP))
	echo "Iteration ${EXP_LO} ${EXP_HI}..."
	URL="http://www.mersenne.org/report_exponent/?exp_lo=${EXP_LO}&exp_hi=${EXP_HI}&text=1"
	wget "${URL}" -q -O - | hxnormalize -ex -i0 | hxselect -c 'div#text_output' | ./reader
done

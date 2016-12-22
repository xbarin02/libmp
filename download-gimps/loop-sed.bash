#!/bin/bash

EXP_LO=$1
EXP_HI=$2

URL="http://www.mersenne.org/report_exponent/?exp_lo=${EXP_LO}&exp_hi=${EXP_HI}&text=1"

wget "${URL}" -q -O - | tr '\n\r' '  ' | sed 's|.*<div id="text_output"[^>]*>||' | sed 's|</div>.*||'

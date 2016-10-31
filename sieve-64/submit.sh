#!/bin/bash
#
#$ -N sieve-64
#$ -o sieve/sieve-64/logs/$JOB_NAME.$JOB_ID.out
#$ -e sieve/sieve-64/logs/$JOB_NAME.$JOB_ID.err

# log a node name
uname -n

# change working directory to store a state/record to the right place
cd $HOME/sieve/sieve-64/

# limit job to one hour
./sieve-64 -t 3600

# submit again
qsub -q all.q@@stable -l ram_free=512M $HOME/sieve/sieve-64/submit.sh

#!/bin/bash
#
#$ -N sieve-128r
#$ -o sieve/sieve-128r/logs/$JOB_NAME.$JOB_ID.out
#$ -e sieve/sieve-128r/logs/$JOB_NAME.$JOB_ID.err

# log a node name
uname -n

# change working directory to store a state/record to the right place
cd $HOME/sieve/sieve-128r/

# limit job to one hour
./sieve-128r -t 3600

# submit again
qsub -q all.q@@stable -l ram_free=512M $HOME/sieve/sieve-128r/submit.sh

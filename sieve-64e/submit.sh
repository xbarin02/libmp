#!/bin/bash
#
#$ -N sieve-64e
#$ -o sieve/sieve-64e/logs/$JOB_NAME.$JOB_ID.out
#$ -e sieve/sieve-64e/logs/$JOB_NAME.$JOB_ID.err

# log a node name
uname -n

# change working directory to store a state/record to the right place
cd $HOME/sieve/sieve-64e/

# limit job to one hour
./sieve-64e -t 3600

# submit again
qsub -q all.q@@stable -l ram_free=512M $HOME/sieve/sieve-64e/submit.sh

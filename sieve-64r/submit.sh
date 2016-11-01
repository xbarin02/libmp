#!/bin/bash
#
#$ -N sieve-64r
#$ -o sieve/sieve-64r/logs/$JOB_NAME.$JOB_ID.out
#$ -e sieve/sieve-64r/logs/$JOB_NAME.$JOB_ID.err

# log a node name
uname -n

# change working directory to store a state/record to the right place
cd $HOME/sieve/sieve-64r/

# limit job to one hour
./sieve-64r -t 3600

# submit again
qsub -q all.q@@stable -l ram_free=512M $HOME/sieve/sieve-64r/submit.sh

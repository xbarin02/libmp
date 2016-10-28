#!/bin/bash
#
#$ -N sieve-64
#$ -o sieve/sieve-64/logs/$JOB_NAME.$JOB_ID.out
#$ -e sieve/sieve-64/logs/$JOB_NAME.$JOB_ID.err

# one hour
$HOME/sieve/sieve-64/sieve-64 -t 3600

# submit again
qsub -q all.q@@stable -l ram_free=512M $HOME/sieve/sieve-64/submit.sh

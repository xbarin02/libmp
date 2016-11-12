#!/bin/bash
#
#$ -N gimps
#$ -o sieve/gimps/logs/$JOB_NAME.$JOB_ID.out
#$ -e sieve/gimps/logs/$JOB_NAME.$JOB_ID.err

# log a node name
uname -n

# change working directory to store a state/record to the right place
cd $HOME/sieve/gimps/

# limit job to one hour
timeout -s SIGTERM 3600 ./mprime -d

# submit again
qsub -q all.q@@stable -l ram_free=512M $HOME/sieve/gimps/submit.sh

#!/bin/bash

calc(){ awk "BEGIN { print "$*" }"; }

for b in `cat bs`
do
	echo $b
	prefix=`pwd`
	for beta in `cat betas`
	do
		echo `calc 1/$beta` `../app/sanalysation ${prefix}/2_100_${beta}_${b}_100000_9.mdat 100000 1000` >> magnetisation_over_beta_b${b}.dat
	done
done

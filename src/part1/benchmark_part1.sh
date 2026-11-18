#!/usr/bin/bash
MAKEFILE_DIR="../../"
PHILOSOPHES="philosophes.csv"
TEMP="current_mesure.txt"

if [ -f $PHILOSOPHES ]; then
	rm $PHILOSOPHES
fi

for cores in 2 4 8 16 32; do
	echo "Cores : $cores"
	for iter in {1..5}; do
		make -C $MAKEFILE_DIR clean -s #2>/dev/null

		{ /usr/bin/time -f %e -o ${TEMP} make -C $MAKEFILE_DIR -j ${cores} -s ;} #2>/dev/null
		cat $TEMP | tr -d '\n' >> $PHILOSOPHES
		echo -n "," >> $PHILOSOPHES
	done
	echo "" >> $PHILOSOPHES
done






rm $TEMP
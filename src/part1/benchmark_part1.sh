#!/usr/bin/bash

CFLAGS="-O2 -pthread"
THREADS_LIST="2 4 8 16 32"
NB_RUNS=5
TEMP="current_mesure.txt"

PHILOSOPHES_CSV="philosophes.csv"
PRODCONS_CSV="producer_consumer.csv"
READWRITE_CSV="reader_writer.csv"

# Compilation
gcc $CFLAGS -o philosophes philosophes.c || exit 1
gcc $CFLAGS -o producer_consumer producer_consumer.c || exit 1
gcc $CFLAGS -o reader_writer reader_writer.c || exit 1

header="threads"
for i in $(seq 1 $NB_RUNS); do header="$header,run$i"; done
for f in $PHILOSOPHES_CSV $PRODCONS_CSV $READWRITE_CSV; do
	echo "$header" > "$f"
done

measure() {
	local csv=$1
	local total=$2
	shift 2

	local line="$total"
	for iter in $(seq 1 $NB_RUNS); do
		/usr/bin/time -f %e -o "$TEMP" "$@" > /dev/null
		line="$line,$(tr -d '\n' < $TEMP)"
	done
	echo "$line" >> "$csv"
}

for total in $THREADS_LIST; do
	half=$((total / 2))
	echo "Threads totaux : $total"

	# Philosophes
	measure $PHILOSOPHES_CSV $total ./philosophes $total

	# Producteurs/consommateurs
	measure $PRODCONS_CSV $total ./producer_consumer $half $half

	# Lecteurs/écrivains 
	measure $READWRITE_CSV $total ./reader_writer $half $half
done

rm -f $TEMP

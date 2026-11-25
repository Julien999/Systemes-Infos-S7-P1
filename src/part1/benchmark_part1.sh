#!/usr/bin/bash
cd "$(dirname "$0")" || exit 1
 
MAKEFILE_DIR="../.."
TEMP="current_mesure.txt"
 
PHILOSOPHES_CSV="philosophes.csv"
PRODCONS_CSV="producer_consumer.csv"
READWRITE_CSV="reader_writer.csv"
 
# Compile via le Makefile 
if [ ! -x ./philosophes ] || [ ! -x ./producer_consumer ] || [ ! -x ./reader_writer ]; then
	make -C $MAKEFILE_DIR all -s || exit 1
fi
 
# Fichiers de sortie
header="threads"
for i in {1..5}; do
	header="$header,run$i";
done
for f in $PHILOSOPHES_CSV $PRODCONS_CSV $READWRITE_CSV; do
	echo "$header" > "$f"
done
 
measure() {
	local csv=$1
	local total=$2
	shift 2 # Ignore les deux premiers paramètres donnés à la fonction (le csv et le total de threads)
 
	local line="$total"
	for iter in {1..5}; do
		{ /usr/bin/time -f %e -o "$TEMP" "$@" ;} > /dev/null
		line="$line,$(tr -d '\n' < $TEMP)"
	done
	echo "$line" >> "$csv"
}
 
for total in 2 4 8 16 32; do
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
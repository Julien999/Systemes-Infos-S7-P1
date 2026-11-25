#!/usr/bin/bash
set -e
cd "$(dirname "$0")"
 
for venv in venv ../venv; do
	if [ -f "$venv/bin/activate" ]; then
		source "$venv/bin/activate"
		break
	fi
done
 
echo "=== Compilation ==="
make clean
make all
 
echo "=== Benchmark (partie 1) ==="
make test
 
echo "=== Graphiques (partie 1) ==="
(cd src/graphes && python3 generate_plot.py)
 
echo "Terminé."
 
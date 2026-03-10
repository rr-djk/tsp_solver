#!/usr/bin/env bash
# Mesure le speedup du thread pool sur différents nombres de threads.
# Usage : ./scripts/benchmark_speedup.sh [instance.tsp] [algo]
# Défaut : data/berlin52.tsp  algo=2opt
set -euo pipefail

BINARY=./build/tsp_parser_profile
INPUT="${1:-data/berlin52.tsp}"
ALGO="${2:-2opt}"
THREADS=(1 2 4 6 8 12)

# Construction du binaire optimisé
echo "==> make profile"
make profile --silent

echo ""
echo "Benchmark : $INPUT  --algo $ALGO  --all-start"
echo "Binaire   : $BINARY"
echo ""
printf "%-9s  %-12s  %-8s\n" "threads" "temps (s)" "speedup"
printf "%-9s  %-12s  %-8s\n" "---------" "------------" "--------"

ref_time=""

for t in "${THREADS[@]}"; do
    elapsed=$( { /usr/bin/time -f "%e" \
        "$BINARY" \
        --input-file "$INPUT" \
        --algo "$ALGO" \
        --all-start \
        --threads "$t" \
        --quiet \
        --output-file /dev/null; } 2>&1 | tail -1 )

    if [ -z "$ref_time" ]; then
        ref_time="$elapsed"
        speedup="1.00"
    else
        speedup=$(awk "BEGIN {printf \"%.2f\", $ref_time / $elapsed}")
    fi

    printf "%-9d  %-12s  ×%s\n" "$t" "${elapsed}s" "$speedup"
done

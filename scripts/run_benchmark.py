#!/usr/bin/env python3
"""Exécute les 3 heuristiques sur berlin52.tsp et écrit les temps dans speed_results.md."""

import json
import subprocess
import sys
from pathlib import Path

PROJECT_ROOT = Path(__file__).parent.parent
BUILD_DIR = PROJECT_ROOT / "build"
DATA_DIR = PROJECT_ROOT / "data"
RESULTS_DIR = PROJECT_ROOT / "results"
SPEED_RESULTS = PROJECT_ROOT / "scripts" / "speed_results.md"

ALGOS = {
    "nn": "nn",
    "2opt": "2opt",
    "insertion": "insertion",
}


def run_algo(algo_key: str) -> float:
    """Exécute le binaire C++ et retourne le temps en ms."""
    output_file = RESULTS_DIR / f"bench_berlin52_{algo_key}.json"
    cmd = [
        str(BUILD_DIR / "tsp_parser"),
        "--input-file", str(DATA_DIR / "berlin52.tsp"),
        "--algo", algo_key,
        "--threads", "8",
        "--output-file", str(output_file),
    ]
    result = subprocess.run(cmd, capture_output=True, text=True)
    if result.returncode != 0:
        print(f"Erreur {algo_key}:\n{result.stderr}", file=sys.stderr)
        sys.exit(1)

    with open(output_file) as f:
        data = json.load(f)
    return data["time_ms"]


def main() -> None:
    if not (BUILD_DIR / "tsp_parser").exists():
        print("Binaire non trouvé. Compilation...", file=sys.stderr)
        subprocess.run(["make", "-C", str(PROJECT_ROOT)], check=True)

    RESULTS_DIR.mkdir(exist_ok=True)

    times = {}
    for name, key in ALGOS.items():
        print(f"Exécution de {name}...")
        times[name] = run_algo(key)
        print(f"  → {times[name]:.3f} ms")

    # Format markdown tableau
    line = f"| {times['nn']:.3f} | {times['2opt']:.3f} | {times['insertion']:.3f} |\n"

    with open(SPEED_RESULTS, "a") as f:
        f.write(line)

    print(f"\nRésultat ajouté à {SPEED_RESULTS}")
    print(line.strip())


if __name__ == "__main__":
    main()

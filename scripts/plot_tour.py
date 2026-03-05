#!/usr/bin/env python3
"""Trace le meilleur tour TSP depuis un fichier de résultats JSON."""

import argparse
import json
import sys
from pathlib import Path

import matplotlib.pyplot as plt


def parse_tsp_coords(tsp_path: Path) -> list[tuple[float, float]]:
    """Parse un fichier TSPLIB et retourne les coordonnées dans l'ordre de lecture.

    Chaque entrée correspond à l'index interne utilisé dans best_tour (0-based).
    """
    coords: list[tuple[float, float]] = []
    in_section = False

    with tsp_path.open() as f:
        for line in f:
            line = line.strip()
            if line == "NODE_COORD_SECTION":
                in_section = True
                continue
            if line in ("EOF", "DISPLAY_DATA_SECTION"):
                break
            if in_section and line:
                parts = line.split()
                if len(parts) >= 3:
                    coords.append((float(parts[1]), float(parts[2])))

    return coords


def plot_tour(json_path: Path, data_dir: Path, output: Path | None) -> None:
    """Charge le résultat JSON, parse le .tsp correspondant et trace le tour."""
    with json_path.open() as f:
        result = json.load(f)

    file_name: str = result["file_name"]
    algo_name: str = result["algo_name"]
    cost: float = result["cost"]
    tour: list[int] = result["best_tour"]

    tsp_path = data_dir / Path(file_name).name
    if not tsp_path.exists():
        sys.exit(f"Fichier TSP introuvable : {tsp_path}")

    coords = parse_tsp_coords(tsp_path)

    if len(tour) != len(coords):
        sys.exit(
            f"Taille du tour ({len(tour)}) != nombre de villes ({len(coords)})"
        )

    # Coordonnées dans l'ordre du tour, boucle fermée vers la ville de départ
    ordered = [coords[i] for i in tour] + [coords[tour[0]]]
    xs = [p[0] for p in ordered]
    ys = [p[1] for p in ordered]

    all_x = [p[0] for p in coords]
    all_y = [p[1] for p in coords]

    fig, ax = plt.subplots(figsize=(9, 7))

    ax.plot(xs, ys, "-", color="#4C72B0", linewidth=0.9, zorder=1)
    ax.scatter(all_x, all_y, s=18, color="#DD8452", zorder=2, label="Villes")

    start_x, start_y = coords[tour[0]]
    ax.scatter(start_x, start_y, s=70, color="#C44E52", zorder=3, label="Départ")

    stem = Path(file_name).stem
    ax.set_title(f"{stem} — {algo_name}\nCoût : {cost:,.2f}", fontsize=12)
    ax.set_xlabel("x")
    ax.set_ylabel("y")
    ax.legend(fontsize=9)
    ax.set_aspect("equal")
    fig.tight_layout()

    if output:
        fig.savefig(output, dpi=150)
        print(f"Graphique sauvegardé : {output}")
    else:
        plt.show()


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Trace le meilleur tour TSP depuis un résultat JSON."
    )
    parser.add_argument(
        "json_file",
        type=Path,
        help="Fichier JSON produit par tsp_solver",
    )
    parser.add_argument(
        "--data-dir",
        type=Path,
        default=Path("data"),
        metavar="DIR",
        help="Dossier contenant les fichiers .tsp (défaut : data/)",
    )
    parser.add_argument(
        "--output",
        type=Path,
        default=None,
        metavar="FILE",
        help="Chemin de sortie image (.png/.svg). Si absent, affiche la fenêtre.",
    )
    args = parser.parse_args()

    if not args.json_file.exists():
        sys.exit(f"Fichier JSON introuvable : {args.json_file}")

    plot_tour(args.json_file, args.data_dir, args.output)


if __name__ == "__main__":
    main()

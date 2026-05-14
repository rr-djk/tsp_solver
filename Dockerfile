# =============================================================================
# Étape 1 — Compilation & Tests
# =============================================================================
FROM ubuntu:24.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
        g++-14 make libgtest-dev cmake ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# Assure que g++-14 est le g++ par défaut
RUN update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-14 100

WORKDIR /app

# Copie du code source
COPY Makefile ./
COPY include/ include/
COPY src/ src/
COPY tests/ tests/
COPY data/ data/

# Compilation du binaire principal
RUN make -B -j"$(nproc)"

# Compilation et exécution des tests pour valider le binaire
RUN make test

# =============================================================================
# Étape 2 — Exécution (image allégée)
# =============================================================================
FROM ubuntu:24.04 AS runtime

ENV DEBIAN_FRONTEND=noninteractive

# Seule la bibliothèque standard C++ est nécessaire à l'exécution
RUN apt-get update && apt-get install -y --no-install-recommends \
        libstdc++6 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copie du binaire compilé depuis l'étape de compilation
COPY --from=builder /app/build/tsp_parser ./tsp_parser

# Copie des fichiers de données TSPLIB pour une utilisation immédiate
COPY --from=builder /app/data/ ./data/

# Création du répertoire results
RUN mkdir -p results

# Point d'entrée par défaut : le solveur TSP
ENTRYPOINT ["./tsp_parser"]

# Commande par défaut : afficher l'aide
CMD ["help"]

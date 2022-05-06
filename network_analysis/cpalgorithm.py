import argparse
import os
import json

import cpnet
import networkx as nx


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("datasetDirectory", type=str)
    parser.add_argument("dataset", type=str)
    parser.add_argument("outputDirectory", type=str)
    return parser.parse_args()


args = parse_args()
DATASET_DIR = args.datasetDirectory
DATASET = args.dataset
CPNET_OUTPUT_DIR = args.outputDirectory

G_file = os.path.join(DATASET_DIR, f"{DATASET}.txt")
G = nx.read_edgelist(G_file)
"""
One can detect

- a single pair of a core and a periphery using
  - `cpnet.BE`, `cpnet.Lip`, `cpnet.LapCore`, `cpnet.LapSgnCore`,
    `cpnet.Surprise`, `cpnet.LowRankCore`
- multiple pairs of a core and a periphery using
  - `cpnet.KM_ER`, `cpnet.KM_config`, `cpnet.Divisive`
- a continuous spectrum between a core and a periphery using
  - `cpnet.MINRES`, `cpnet.Rombach`, `cpnet.Rossa`

The following algorithms take the edge weight into account:
- `cpnet.KM_ER`, `cpnet.KM_config`, `cpnet.Divisive`, `cpnet.Rombach`,
  `cpnet.Rossa`, `cpnet.LapCore`, `cpnet.LapSgnCore`, `cpnet.LowRankCore`
"""
algorithms = [
    ["BE", cpnet.BE()],
    ["Lip", cpnet.Lip()],
    ["LapCore", cpnet.LapCore()],
    ["LapSgnCore", cpnet.LapSgnCore()],
    ["Surprise", cpnet.Surprise()],
    ["LowRankCore", cpnet.LowRankCore()],
]

for _, algo in algorithms:
    algo.detect(G)

output = [(algo.get_pair_id(), algo.get_coreness()) for _, algo in algorithms]

for algo_output, algo_metadata in zip(output, algorithms):
    c, x = algo_output
    algo_name, algo_struct = algo_metadata

    for key in x:
        x[key] = int(x[key])

    with open(os.path.join(CPNET_OUTPUT_DIR, f"{algo_name}.json"), "w") as fp:
        json.dump(x, fp)

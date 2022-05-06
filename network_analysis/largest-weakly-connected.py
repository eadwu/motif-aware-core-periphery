import os
import networkx as nx

PWD = os.path.abspath(".")
DATASET_DIR = os.path.join(PWD, "..", "datasets", "postprocessed")

G = nx.read_edgelist(os.path.join(DATASET_DIR, "polblogs.txt"))
G = G.to_directed()

wcc_list = nx.weakly_connected_components(G)
max_wcc = max(wcc_list, key=len)
max_wcc = nx.subgraph(G, max_wcc)

with open(os.path.join(DATASET_DIR, "polblogs_largest-wcc.txt.orig"),
          "wb") as fp:
    nx.write_edgelist(max_wcc, fp)

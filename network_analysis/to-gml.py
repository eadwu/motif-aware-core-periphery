import os
import networkx as nx

SOURCE_DIR = os.path.dirname(os.path.realpath(__file__))

ROOT = os.path.abspath(os.path.join(SOURCE_DIR, ".."))
DATASET_DIR = os.path.join(ROOT, "datasets")

G_file_vertex = os.path.join(DATASET_DIR, "orig", "faculty-hiring",
                             "Dataset 4. ComputerScience_vertexlist.txt")
G_file_edge = os.path.join(DATASET_DIR, "orig", "faculty-hiring",
                           "Dataset 3. ComputerScience_edgelist.txt")

with open(G_file_vertex, "r") as fp:
    G_vertex = fp.readlines()

with open(G_file_edge, "r") as fp:
    G_edge = fp.readlines()

G = nx.parse_edgelist(G_edge,
                      nodetype=int,
                      data=(("rank", str), ("gender", str)))
# G = G.to_directed()

import json
# with open(os.path.join(ROOT, "partitions", "faculty-hiring", "AdvHits.json")) as fp:
with open(os.path.join(ROOT, "core_output", "faculty-hiring", "acyclic", "QuarkD6.json")) as fp:
    cores = json.load(fp)

for line in G_vertex:
    # Comment
    if '#' in line:
        continue

    u, pi, USN2010, NRC95, Region, institution = line.strip().split(None,
                                                                    maxsplit=5)

    attrs = {
        int(u): {
            "pi": float(pi),
            "USN2010": USN2010,
            "NRC95": NRC95,
            "Region": Region,
            "institution": institution,
            "core": u in cores and (cores[u] == 1 or cores[u] == 2),
        }
    }
    # print(u, pi, USN2010, NRC95, Region, institution)
    nx.set_node_attributes(G, attrs)

# print("\n".join(nx.generate_gml(G)))
nx.write_gml(G, os.path.join(DATASET_DIR, "gml", "faculty-hiring.gml"))

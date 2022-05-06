import os
import json

SOURCE_DIR = os.path.dirname(os.path.realpath(__file__))
ROOT = os.path.abspath(os.path.join(SOURCE_DIR, ".."))

DATASET = "faculty-hiring"
# DATASET = "polblogs_largest-wcc"
# DATASET = "nber-un"

# 10m+
# DATASET = "soc-pokec-relationships"
# DATASET = "soc-LiveJournal1"
# DATASET = "enwiki-2013"
# 1m+
# DATASET = "amazon0601"
# DATASET = "web-NotreDame"
# DATASET = "wiki-Talk"
# <1m
# DATASET = "foodweb"
# DATASET = "eat"
# DATASET = "email-EuAll"
# DATASET = "cit-HepPh"
# DATASET = "soc-Slashdot0811"

PARTITION_DIR = os.path.join(ROOT, "output", "partitions", DATASET)
QUARK_PARTITION_DIR = os.path.join(ROOT, "output", "quark_partitions", DATASET)

print(PARTITION_DIR, QUARK_PARTITION_DIR)


def read_json(filepath):
    with open(filepath, "r") as fp:
        data = json.load(fp)

    return data


REFERENCE_PARTITIONS = {
    file.split('.')[0]: read_json(os.path.join(root, file))
    for root, dirs, files in os.walk(PARTITION_DIR) for file in files
}


def is_core_node(value):
    value = value * 1
    return value == 1 or value == 2


def count_cores_in(node_mapping, node_mapping_reference=None):
    n_cores = 0

    if node_mapping_reference is None:
        node_mapping_reference = node_mapping

    for k in node_mapping_reference.keys():
        is_k_core_in_reference = is_core_node(node_mapping_reference[k])
        is_k_core_in_mapping = k in node_mapping and is_core_node(
            node_mapping[k])

        if is_k_core_in_reference and is_k_core_in_mapping:
            n_cores = n_cores + 1

    return n_cores


def generate_confusion_matrix(dict_list, other_list, keys, ratio=True):
    confusion_matrix = []

    for i, i_entry in enumerate(dict_list):
        matrix_row = []

        matrix_row.append(keys[i])

        for j in range(len(other_list)):
            j_entry = other_list[j]

            # Given C[i][j] it should be
            #   cores nodes in Partition I that are also core nodes in
            #   Partition J
            i_cores = count_cores_in(i_entry)
            j_cores = count_cores_in(j_entry)
            i_j_cores = count_cores_in(i_entry, node_mapping_reference=j_entry)

            if dict_list == other_list and i == j:
                matrix_row.append("----")
            else:
                # core_ratio = i_j_cores / i_cores
                core_ratio = i_j_cores / j_cores
                matrix_row.append(core_ratio if ratio else i_j_cores)

        confusion_matrix.append(matrix_row)
        print(matrix_row)

    return confusion_matrix


MOTIFS = ["acyclic", "cycle", "cyclep", "cyclepp", "inp", "outp"]

r_keys = sorted(REFERENCE_PARTITIONS.keys())
REFERENCE_PART = [REFERENCE_PARTITIONS[k] for k in r_keys]

MOTIF_PARTITIONS = {}
for motif in MOTIFS:
    QUARK_PARTITIONS = {
        file.split('.')[0]: read_json(os.path.join(root, file))
        for root, dirs, files in os.walk(
            os.path.join(QUARK_PARTITION_DIR, motif)) for file in files
    }

    MOTIF_PARTITIONS[motif] = QUARK_PARTITIONS

    q_keys = sorted(QUARK_PARTITIONS.keys())
    QUARK_PART = [QUARK_PARTITIONS[k] for k in q_keys]

    print(motif, q_keys, r_keys)
    # generate_confusion_matrix(QUARK_PART, QUARK_PART, q_keys)
    generate_confusion_matrix(QUARK_PART, REFERENCE_PART, q_keys)
    # generate_confusion_matrix(REFERENCE_PART, QUARK_PART, r_keys)

# generate_confusion_matrix(REFERENCE_PART, REFERENCE_PART, r_keys)

# a = MOTIF_PARTITIONS["outp"]["QuarkD1"]
# b = REFERENCE_PARTITIONS["BE"]
# print(
#     "| QuarkD1", count_cores_in(a),
#     "| BE", count_cores_in(b),
#     "| SHARED NODES", count_cores_in(a, b),
#     "| SHARED NODES / QuarkD1", count_cores_in(a, b) / count_cores_in(a),
#     "| SHARED NODES / BE", count_cores_in(a, b) / count_cores_in(b),
# )

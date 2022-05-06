#! /usr/bin/env bash

PWD="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")"
CSE640_ROOT="$(readlink -f "${PWD}/..")"
DATASET_ROOT="${CSE640_ROOT}/datasets/directed-cp-datasets/postprocessed"

OUTPUT_DIR="${CSE640_ROOT}/output"
PARTITION_DIR="${OUTPUT_DIR}/partitions"

QUARK="${CSE640_ROOT}/build/quark_decomposition/quark"
METRIC="${CSE640_ROOT}/build/metric/metric"

DATASET="faculty-hiring"
# DATASET="nber-un"

DATASET_FILE="${DATASET_ROOT}/${DATASET}.txt"

for partition in "${PARTITION_DIR}/${DATASET}"/*; do
  filename="$(basename "${partition}")"
  file="${filename%.*}"

  printf "%s\n" "${file}"
  "${METRIC}" "${DATASET_FILE}" "" "${partition}" | rg CSV
  rm QuarkD*.json
done


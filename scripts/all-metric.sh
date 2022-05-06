#! /usr/bin/env bash

# usage: all-metric.sh <metric-dir>

N=4

PWD="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")"
CSE640_ROOT="$(readlink -f "${PWD}/..")"
DATASET_ROOT="${CSE640_ROOT}/datasets/directed"
# DATASET_ROOT="${CSE640_ROOT}/datasets/directed-cp-datasets/postprocessed"

QUARK="${CSE640_ROOT}/build/quark_decomposition/quark"
METRIC="${CSE640_ROOT}/build/metric/metric"

DATASETS=(
  # 10m+
  "soc-pokec-relationships" "soc-LiveJournal1" "enwiki-2013"
  # 1m+
  "amazon0601" "web-NotreDame" "wiki-Talk"
  # <1m
  "foodweb" "eat" "email-EuAll" "cit-HepPh" "soc-Slashdot0811"
)
# DATASETS=("faculty-hiring" "nber-un" "polblogs_largest-wcc")

# MOTIFS=( "cycle" "acyclic" "out+" "in+" "cycle+" "cycle++" "reciprocal" )
MOTIFS=( "cycle" "acyclic" "outp" "inp" "cyclep" "cyclepp" )

OUTPUT_DIR="${CSE640_ROOT}/output/quark"
METRIC_CHECKSUM="${CSE640_ROOT}/output/checksums.sha512sum"

if (cd "${OUTPUT_DIR}" && sha512sum --check --status "${METRIC_CHECKSUM}"); then
  printf "integrity of metric output has been verified\n"
else
  printf "integrity of metric output has been comprising, exiting\n"
  exit 1
fi

METRIC_DIR="${1}"
QUARK_PARTITION_DIR="${2}"

METRIC_DIR="$(readlink -f "${METRIC_DIR}")"
QUARK_PARTITION_DIR="$(readlink -f "${QUARK_PARTITION_DIR}")"

# if [ -d "${METRIC_DIR}" ]
# then
#   rm -r "${METRIC_DIR}"
# fi

# if [ -d "${QUARK_PARTITION_DIR}" ]
# then
#   rm -r "${QUARK_PARTITION_DIR}"
# fi

# mkdir "${METRIC_DIR}" "${QUARK_PARTITION_DIR}"

for dataset in "${DATASETS[@]}"
do
  dataset_path="${DATASET_ROOT}/${dataset}.txt"

  if [ ! -f "${dataset_path}" ];
  then
    printf "missing %s\n" "${dataset}"
    continue
  fi

  for motif in "${MOTIFS[@]}"
  do
    printf "%s - %s\n" "${dataset}" "${motif}"
    nuclei_path="${OUTPUT_DIR}/${dataset}.txt_${motif}_NUCLEI"
    metric_path="${METRIC_DIR}/${dataset}.txt_${motif}"

    (
      if [ -f "${metric_path}" ]
      then
        printf "found metric output, skipping\n"
        continue
      fi

      if [ ! -f "${nuclei_path}" ]
      then
        printf "missing nuclei output, skipping\n"
        continue
      fi

      mkdir -p "${QUARK_PARTITION_DIR}/${dataset}/${motif}"

      (cd "${QUARK_PARTITION_DIR}/${dataset}/${motif}" && "${METRIC}" "${dataset_path}" "${nuclei_path}" &> "${metric_path}")
      # Skip first two columns and output the rest
      rg "CSV" "${metric_path}" | awk '{$1=$2=""; print $0}' &> "${metric_path}.csv"
    ) &

    # allow to execute up to $N jobs in parallel
    while [[ $(jobs -r -p | wc -l) -ge $N ]];
    do
      # now there are $N jobs already running, so wait here for any job
      # to be finished so there is a place to start next one.
      wait -n
    done
  done
done

# no more jobs to be started but wait for pending jobs
# (all need to be finished)
wait

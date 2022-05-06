#! /usr/bin/env bash

# usage: all-cpalgorithm.sh

N=4

PWD="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")"
CSE640_ROOT="$(readlink -f "${PWD}/..")"
DATASET_ROOT="${CSE640_ROOT}/datasets/directed"

PYTHON="${CSE640_ROOT}/../../.direnv/python-CSE640/bin/python"
CPALGO="${CSE640_ROOT}/network_analysis/cpalgorithm.py"

DATASETS=(
  # 10m+
  "soc-pokec-relationships" "soc-LiveJournal1" "enwiki-2013"
  # 1m+
  "amazon0601" "web-NotreDame" "wiki-Talk"
  # <1m
  "foodweb" "eat" "email-EuAll" "cit-HepPh" "soc-Slashdot0811"
)

# MOTIFS=( "cycle" "acyclic" "out+" "in+" "cycle+" "cycle++" "reciprocal" )
MOTIFS=( "cycle" "acyclic" "outp" "inp" "cyclep" "cyclepp" )

OUTPUT_DIR="${CSE640_ROOT}/output/quark_partitions"

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
    partition_path="${OUTPUT_DIR}/${dataset}/${motif}"

    (
      if [ -d "${partition_path}" ]
      then
        rm -rf "${partition_path}"
      fi

      mkdir -p "${partition_path}"
      "${PYTHON}" "${CPALGO}" "${DATASET_ROOT}" "${dataset}" "${partition_path}"
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

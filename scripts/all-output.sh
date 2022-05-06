#! /usr/bin/env bash

N=6

PWD="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")"
ROOT="$(readlink -f "${PWD}/..")"
DATASET_ROOT="${ROOT}/datasets/directed"

QUARK="${ROOT}/quark_decomposition/qd/quark"

DATASETS=(
  "foodweb" "eat" "email-EuAll" "cit-HepPh" "soc-Slashdot0811"
  # 1m+
  "amazon0601" "web-NotreDame" "wiki-Talk"
  # 10m+
  "soc-pokec-relationships" "soc-LiveJournal1" "enwiki-2013"
)

# MOTIFS=( "cycle" "acyclic" "out+" "in+" "cycle+" "cycle++" "reciprocal" )
MOTIFS=( "cycle" "acyclic" "outp" "inp" "cyclep" "cyclepp" )

rm -f *_Hierarchy
rm -f *_NUCLEI

rm -f ../checksums.sha512sum
touch ../checksums.sha512sum

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

    (
      echo ""${QUARK}" "${dataset_path}" "${motif}" YES"
      "${QUARK}" "${dataset_path}" "${motif}" YES &> /dev/null

      hierarchy_path="${dataset}.txt_${motif}_Hierarchy"
      nuclei_path="${dataset}.txt_${motif}_NUCLEI"

      {
        flock -x 3                         # grab a lock on file descriptor #3
        sha512sum "${hierarchy_path}" >&3  # redirect to behavior of FD 3
        sha512sum "${nuclei_path}" >&3
      } 3>>../checksums.sha512sum          # do all this with FD 3 open to checksums.sha512sum
    ) &

    # allow to execute up to $N jobs in parallel
    if [[ $(jobs -r -p | wc -l) -ge $N ]]; then
      # now there are $N jobs already running, so wait here for any job
      # to be finished so there is a place to start next one.
      wait -n
    fi
  done
done

# no more jobs to be started but wait for pending jobs
# (all need to be finished)
wait


#! /usr/bin/env bash

# usage: merge-output-csv.sh <output-dir> <output-file>

PWD="$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")"

METRIC_DIR="${1}"
OUTPUT="${2}"

rm -f "${OUTPUT}"

for file in `ls "${METRIC_DIR}"/*.csv | sort`
do
  filename="$(basename "${file}")"
  printf "%s\n" "${filename}" >> "${OUTPUT}"
  sed 's@^\s*@,@g' "${file}" >> "${OUTPUT}"
done

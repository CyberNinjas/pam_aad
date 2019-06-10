#!/bin/sh
set -eu

# Recursively build docker images
recursive_build() {
  path=$1
  tag=$2
  extra=$3
  for dir in $path/docker/*; do
    if [ -d "${dir}" ]; then
      distro=$(basename "${dir}")
      image="${tag}:${distro}" # org/image:tag
      if [ ! -z "${extra}" ]; then
        image="${tag}:${distro}-${extra}" # org/image:tag-extra
      fi
      docker build -t "${image}" "${path}" \
                   -f "${dir}/Dockerfile"
    fi
  done
}

main() {
  DEFAULT_IMAGE="cyberninjas/pam_aad"

  # Build all docker images
  recursive_build . "${DEFAULT_IMAGE}" ''

  # Build all testing docker images
  recursive_build ./test "${DEFAULT_IMAGE}" 'testing'
}

main

#!/usr/bin/env bash

cd $(dirname "$0")

cd ../build && cmake .. && make && cd ..

./tools/generate_big_file.py

sort tools/output_file > tools/output_file.expected

./build/src/externalsort --input tools/output_file --output tools/output_file.got

(diff tools/output_file.expected tools/output_file.got && printf "\033[1;32mOK\n") || echo -e "\033[1;31mFAIL\n"

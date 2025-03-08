#!/usr/bin/env bash

if [ -d "json/machine" ]; then
  echo ''
else
  mkdir -p json/machine
fi
if [ -d "json/macro" ]; then
  echo ''
else
  mkdir -p json/macro
fi


python liu-machine/generate.py json
if [ -d "grammar/generated/machine" ]; then
  echo ''
else
  mkdir -p grammar/generated/machine
fi
if [ -d "grammar/generated/macro" ]; then
  echo ''
else
  mkdir -p grammar/generated/macro
fi
python script/codegen.py json template grammar/generated

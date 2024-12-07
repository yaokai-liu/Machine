#!/usr/bin/env bash

if [ -d "json" ]; then
  echo ''
else
  mkdir json
fi
python liu-machine/generate.py json
if [ -d "grammar/generated" ]; then
  echo ''
else
  mkdir -p grammar/generated
fi
python script/codegen.py

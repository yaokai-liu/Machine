#!/usr/bin/env bash

if [ -d "json" ]; then
  echo ''
else
  mkdir json
fi
python liu-machine/generate.py json
if [ -d "grammar" ]; then
  echo ''
else
  mkdir grammar
fi
python script/codegen.py

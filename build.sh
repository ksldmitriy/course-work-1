#!/bin/bash

rm debug-logs
rm full-logs

echo === CMAKE ===

set -e

cmake -S ./src -B ./out  

mv out/compile_commands.json .

echo === BUILD ===
make -j 16 -C ./out;

echo === SHADERS ===

glslc shaders/shader.vert -o shaders/vert.spv
glslc shaders/shader.frag -o shaders/frag.spv

echo === RUN ===
 ./out/best_program

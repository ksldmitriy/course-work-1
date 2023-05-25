#!/bin/bash

rm debug-logs
rm full-logs
rm imgui.ini 

echo === CMAKE ===

set -e

cmake -S ./src -B ./out  

mv out/compile_commands.json .

echo === BUILD ===
make -j 16 -C ./out;

echo === SHADERS ===

glslc shaders/shader.vert -o shaders/vert.spv
glslc shaders/shader.frag -o shaders/frag.spv
glslc shaders/debug.vert -o shaders/debug_vert.spv
glslc shaders/debug.frag -o shaders/debug_frag.spv
glslc shaders/mesh.vert -o shaders/mesh_vert.spv
glslc shaders/mesh.frag -o shaders/mesh_frag.spv

echo === RUN ===
 ./out/best_program

#!/bin/bash

for vert in ./*.vert; do
  glslc $vert -o "${vert}.spv"
done

for frag in ./*.frag; do
  glslc $frag -o "${frag}.spv"
done

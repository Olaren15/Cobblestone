#!/bin/bash

for vert in ./*.vert; do
  glslc $vert -o $vert + '.spv'
done

for frag in ./*.vert; do
  glslc $frag -o $frag + '.spv'
done
#!/bin/bash

find . -name '*.vert' -exec glslc '{}' -o '{}.spv' \;
find . -name '*.frag' -exec glslc '{}' -o '{}.spv' \;
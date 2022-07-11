#!/bin/bash
cd build/
cmake -DCMAKE_BUILD_TYPE=Debug .. && cmake --build . -- -j 4 && ./PCE localhost 2000
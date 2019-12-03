#!/bin/bash

echo "Compiling $1!"

if [ -f "out.exe" ]
then
  rm out.exe
fi

nvcc -Xcompiler -fopenmp -o out.exe $1
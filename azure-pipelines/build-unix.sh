#!/bin/bash


export PWD=$(pwd)
ls

cd azure-pipelines
mkdir root
cd root
git clone https://github.com/UG4/ughub.git
ughub/ughub init .
ughub/ughub install SmallStrainMechanics ConvectionDiffusion LuaShell ProMesh
cp -r ../../plugins/SlicerPlugin ./plugins/SlicerPlugin
mkdir build
cd build
export CXX_FLAGS="-std=c++11"
cmake .. -DSlicerPlugin=ON -DProMesh=ON -DConvectionDiffusion=ON -DLuaShell=ON -DSmallStrainMechanics=ON -DSTATIC_BUILD=ON
make -j2
../bin/ugshell -ex ../plugins/SlicerPlugin/test.lua
ls *.ugx
REM #!/bin/bash


REM export PWD=$(pwd)
REM ls

cd azure-pipelines
mkdir root
cd root
git clone https://github.com/UG4/ughub.git
call ughub\ughub init .
call ughub\ughub install SmallStrainMechanics ConvectionDiffusion LuaShell ProMesh
REM cp -r ../../plugins/SlicerPlugin ./plugins/SlicerPlugin
echo d | xcopy ..\..\plugins\SlicerPlugin plugins\SlicerPlugin /s /e
mkdir build
cd build
cmake .. -DSlicerPlugin=ON -DProMesh=ON -DConvectionDiffusion=ON -DLuaShell=ON -DSmallStrainMechanics=ON -DSTATIC_BUILD=ON
REM MSBuild .\ug4.sln  /property:Configuration=Release /property:Platform=x64
REM ..\bin\ugshell -ex ..\plugins\SlicerPlugin\test.lua
dir
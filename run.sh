#!/bin/bash
BASE=$(pwd)

function Format {
   STYLE=Mozilla
   clang-format --style=${STYLE} ${1} > "${1}.tmp.gwf"
   mv "${1}.tmp.gwf" "${1}"
}
function Clean {
  rm -rf build
  rm *.puml
  rm *.png
}
function Build {
   if [ ! -d "build" ]; then
      mkdir build
   fi
   cd build
   cmake -DCMAKE_BUILD_TYPE="${1}" ..
   make
   cd ..
}
function CreateTmpDir {
   mkdir "${1}"
}
function DeleteTmpDir {
   rm -rf "${1}"
}
function CopyFiles2Tmp {
   #we need only the source to generate tags
   cd "${FULL_PATH}"
   find "$FULL_PATH/${1}" -name "*.cpp" -exec cp "{}" "${BASE}/${1}/" \; -print
   find "$FULL_PATH/${1}" -name "*.h" -exec cp "{}" "${BASE}/${1}/" \; -print
   find "$FULL_PATH/${1}" -name "*.hpp" -exec cp "{}" "${BASE}/${1}/" \; -print
   cd "${BASE}"
}
function GenerateTags {
   cd "${1}"
   #dont need the recurstion '-R' since we copied the files...
   ctags -n --fields=+aim --c++-types=+cegmpstuvx-dnf --if0=yes -R .
   cd ..
}
function GenPlantuml {
   export PLANTUML_LIMIT_SIZE=20000000
   java -Xmx8192m -jar ./plantuml.jar -verbose $1.puml
}
function GenPuml {
   CreateTmpDir "${1}"
   CopyFiles2Tmp "${1}"
   GenerateTags "${1}"
   echo "running..."
   ./build/genGraph "${1}"
   DeleteTmpDir "${1}"
   GenPlantuml "${1}"
}
if [ "${1}" = "Clean" ]; then
   Clean
   exit 1
fi
if [ "${1}" = "Format" ]; then
   Format main.cpp
   exit 1
fi
if [ "${1}" = "Build" ]; then
   BUILD_TYPE=$2
   Build $BUILD_TYPE
   exit 1
fi
################################################################################
BUILD_TYPE=Release
#BUILD_TYPE=Debug

rm build/genGraph
Build $BUILD_TYPE

#FULL PATH TO YOUR SOURCE HERE!
FULL_PATH=~/mamba/pos-mamba

#ADD YOUR SUBDIR HERE
GenPuml kernel
GenPuml launcher
GenPuml mal
GenPuml sci
GenPuml sdk

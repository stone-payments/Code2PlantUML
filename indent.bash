#!/bin/bash
STYLE=Mozilla
clang-format --style=$STYLE $1 > $1.tmp.gwf
mv $1.tmp.gwf $1



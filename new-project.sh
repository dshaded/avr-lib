#!/bin/bash

git submodule add git@github.com:dshaded/avr-lib.git lib
git submodule init
cp -r lib/skel/* lib/skel/.gitignore .

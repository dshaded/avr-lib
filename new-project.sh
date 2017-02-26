#!/bin/bash

if [ "$(ls -A)" ] ; then
	echo "Directory must be empty"
else
	git init
	git submodule add git@github.com:dshaded/avr-lib.git lib
	git submodule init
	cp -r lib/skel/* .
	git add .
	git commit -m 'Initial commit'
fi

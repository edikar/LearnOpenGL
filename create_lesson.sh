#!/bin/bash

if [ $# != 2 ]; then
    echo "Usage:"
    echo "./create_lesson from to"
    exit;
fi

if [ ! -d $1 ]; then
	echo "dir $1 does not exist"
	exit;
fi


if [ -d $2 ]; then
	echo "dir $2 already exist"
	exit;
fi

cp -r $1 $2; cd $2; make clean; mv *.cpp `basename $(pwd)`.cpp
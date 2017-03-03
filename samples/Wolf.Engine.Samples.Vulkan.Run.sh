#!/bin/bash

EXAMPLE_NAME_TO_BE_RUN="03_scene"

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
DIR_PATH="$DIR"/../bin/x64/Debug/GNU-Linux
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DIR_PATH
export LD_LIBRARY_PATH
cd $DIR_PATH
./$EXAMPLE_NAME_TO_BE_RUN

#!/bin/sh

srcdir=$(pwd)
export LD_LIBRARY_PATH="$srcdir/src/.libs"${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}

GTK_MODULES=gtkinspector "$@"

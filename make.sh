#!/bin/env sh
gcc -g -Wall -lX11 -lcairo -o bdialog bdialog.c
if [ "$1" = "run" ]; then echo $'this is a\nvery long message split into several lines\nin order to test this app' | ./bdialog; fi
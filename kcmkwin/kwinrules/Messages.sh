#! /usr/bin/env bash
$EXTRACTRC *.ui >> rc.cpp || exit 11
$XGETTEXT *.cpp *.h -o $podir/kcmkwinrules.pot
rm -f rc.cpp

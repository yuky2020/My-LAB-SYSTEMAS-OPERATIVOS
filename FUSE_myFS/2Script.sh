#!/bin/bash

#Create directory ./temp

if [ -d ./temp ] ; then
	rm -r ./temp
fi

mkdir temp

#A
cp src/fuseLib.c temp/fuseLib.c
cp src/myFS.h temp/myFS.h

cp src/fuseLib.c mount-point/fuseLib.c
cp src/myFS.h mount-point/myFS.h

#B

./my-fsck virtual-disk #Check Integrity

truncate -s 512 temp/fuseLib.c
truncate -s 512 mount-point/fuseLib.c


#C

./my-fsck virtual-disk #Check Integrity

diff src/fuseLib.c mount-point/fuseLib.c > diffs.txt

#D

cp third.txt mount-point/third.txt

#E

./my-fsck virtual-disk #Check Integrity

diff third.txt mount-point/third.txt >> diffs.txt

#F

truncate --size=+512 temp/myFS.h
truncate --size=+512 mount-point/myFS.h

#G

./my-fsck virtual-disk #Check Integrity g

diff src/myFS.h mount-point/myFS.h >> diffs.txt

#fusermount -u mount-point

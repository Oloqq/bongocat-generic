#!/bin/bash

name=$1
if [ -z "$name" ]
then
    name="bongo"
fi

rm -rf bin/deploy
rm -f  bin/*.exe
rm -f  bin/*.zip

cmd.exe /c "make"

mkdir            bin/deploy
cp bin/bongo.exe bin/deploy
cp config.json   bin/deploy
cp changelog     bin/deploy
cp WhyFork.md    bin/deploy
cp -r img        bin/deploy
cp -r font       bin/deploy

zip -rq bin/$name bin/deploy

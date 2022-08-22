#!/bin/bash
rm -rf bin/deploy
rm -f  bin/bongo.exe

cmd.exe /c "make"

mkdir            bin/deploy
cp bin/bongo.exe bin/deploy
cp config.json   bin/deploy
cp WhyFork.md    bin/deploy
cp -r img        bin/deploy
cp -r font       bin/deploy

zip -r bin/bongo.zip bin/deploy

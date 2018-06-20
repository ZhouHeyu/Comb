#!/bin/bash

# first need to clean
make clean
#second need to pull remote code into branch old
echo "the Ubuntu 10.04(lab2o8) git password is 123456"
git pull origin old
#cd ../ to make
cd ../ ; make
#if make is ok please to runtest to check code is ok
cd ./test.release ; . runtest
#final return to src dir
cd ../src

#!/bin/bash


root="/Users/app/eosio"
src_dir="eos"

if [ $# -ge 1 ]
then
    src_dir=$1
fi

src_path="$root/$src_dir"
build_path="$src_path/build"

echo
echo "Will going to build eosio under directory <$src_path>"

if ! read -t 30 -p "Are you sure?[y/n] " yORn
then
    echo
    echo "Wait reading timeout(10 secends)."
    exit 1
fi

sudo_pw="123456"
if [ "Xy" == "X$yORn" -o "X" == "X$yORn" ]
then
    cd $src_path && ./eosio_build.sh && cd $build_path && echo "$sudo_pw" | sudo -S make install
else
    echo "Please modify your eosio source path and sudo password in this script."
    exit 1
fi

if [ $? -eq 0 ]
then
    echo -e "\nALL DONE($(pwd)) [succeed]"
else
    echo -e "\nALL DONE($(pwd)) [failed]"
fi
echo

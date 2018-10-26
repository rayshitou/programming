#!/bin/bash

EOSIO=/Users/app/eosio/eos

git_pull="cd $EOSIO && git pull && git submodule update --recursive"

echo "$git_pull"

eval $git_pull

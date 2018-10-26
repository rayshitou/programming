#!/bin/bash

root_path="/Users/app/eosio/config/testnet"
genesis_json="--genesis-json $root_path/genesis.json"
config_dir="--config-dir $root_path"

NODE_DIR=~/Library/Application\ Support/eosio/nodeos
NODE_REPLAY_OPT=""

if [ "X$1" == "Xdelndir" -a -d "${NODE_DIR}"  ]; then
    rm -rf "${NODE_DIR}"
    echo -e "\nDeleted node data directory: ${NODE_DIR}"
fi

if [ -d "${NODE_DIR}"  ]; then
    NODE_REPLAY_OPT="--hard-replay-blockchain"
    genesis_json=""
fi

function conn_testnet(){
    echo "nodeos -e $NODE_REPLAY_OPT $genesis_json $config_dir"
    nohup nodeos -e $NODE_REPLAY_OPT $genesis_json $config_dir &

    if [ -f nohup.out ]
    then
        tail -f nohup.out
    fi
}

conn_testnet

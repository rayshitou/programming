#!/bin/bash

# ===========================================
#
# 说  明：启动单节点，指定delndir参数会删除旧的块数据
# 前  提：NODE_DIR，修改块数据所在目录，否则不能正确启动
# author: Chenchunyun
# Date  : 2018.06.28
#
# ===========================================

WAIT_SEC=5
NODE_DIR=~/Library/Application\ Support/eosio/nodeos
NODE_BASE_PLUGINS="--plugin eosio::chain_api_plugin --plugin eosio::history_api_plugin --plugin eosio::net_api_plugin --plugin eosio::producer_plugin"
NODE_EXT_PLUGINS="--plugin eosio::nxsd_api_plugin"
NODE_REPLAY_OPT=""
OPTIONS="--max-transaction-time=1000 --contracts-console"
# --genesis-json /Users/app/eosio/dev/genesis.json"
node_config="config.ini"

function copy_config_from_node()
{
    cp -rf "$NODE_DIR/config/$node_config" .
}

function copy_config_to_node()
{
    cp -rf ./$node_config "$NODE_DIR/config"
    cp -rf ~/eosio-wallet/default.wallet "$NODE_DIR/data"
}

if [ "X$1" == "Xdelndir" -a -d "${NODE_DIR}" ]; then
#    copy_config_from_node

	rm -rf "${NODE_DIR}"
	echo -e "\nDeleted node data directory: ${NODE_DIR}"
fi

if [ -d "${NODE_DIR}" ]; then
    NODE_REPLAY_OPT="--hard-replay-blockchain"
fi

function start_nodeos(){
#    nodeos -e -p eosio --config-dir ~/dev ${NODE_REPLAY_OPT} &
  #if [ "X$1" == "Xdelndir" ]; then
    copy_config_to_node
  #fi
    node_cmd="nodeos -e -p eosio ${NODE_BASE_PLUGINS} ${NODE_EXT_PLUGINS} ${NODE_REPLAY_OPT} ${OPTIONS} &"
    echo $node_cmd
    eval $node_cmd
}

function restart(){
  if [ "X$1" == "Xdelndir" ]; then
    echo ".................waiting........."
    sleep $WAIT_SEC  ## 需要等到上一次nodeos启动出块后才能重放成功，这个等待时间可能不同机器上不同
    ./kill_eosall.sh
    copy_config_to_node
    NODE_REPLAY_OPT="--hard-replay-blockchain"
    start_nodeos
  fi
}

start_nodeos
#restart

#!/bin/bash

# ===========================================
#
# 说  明：启动nodeos后，顺序完成以下动作：创建eosio.token和accuser账号->部署build/contracts/eosio.token合约到eosio.token账号上->发10亿EOS币到eosio账号->转1万EOS到accuser账号
# author: Chenchunyun
# Date  : 2018.06.28
#
# ===========================================

set -o errexit

# 建系统账号并给eosio.token设置合约
sys_accounts="eosio.token eosio.bpay eosio.msig eosio.names eosio.ram eosio.ramfee eosio.saving eosio.stake eosio.upay"
function sys_accounts_create(){
	for acc in $sys_accounts
	do
	    ./eos_create_account.sh $acc 
	    sleep 1
	done
}

# 创建测试账号并购买内存及抵押带宽和CPU,需在eosio发币完成后执行
test_accounts="accuser accuser1"
function test_accounts_create(){
	for acc in $test_accounts
	do
        ./eos_nxsd_api.sh account eosio $acc
	    sleep 1
	done
}

# 发10亿EOS给eosio账号
function issue_token(){
    ./eos_issue_token.sh
}

# 转1万EOS到accuser账号
function transfer_token(){
    ./eos_nxsd_api.sh transfer eosio ${test_accounts##* } 30 90000.0000
}

start_keosd_daemon(){
    keosd --http-server-address=127.0.0.1:8900 &
}

start_keosd_daemon
sys_accounts_create
issue_token
test_accounts_create
transfer_token

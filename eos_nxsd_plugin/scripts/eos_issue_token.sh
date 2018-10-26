#!/bin/bash

# ===========================================
#
# 说  明：发币
# author: Chenchunyun
# Date  : 2018.06.28
#
# ===========================================

#set -x
set -o errexit

function Usage(){
echo "Usage: $0 [currency_symbol(default: SYS)] [issuer(default: eosio)] [token_amount(default: 1000000000.0000=十亿)]"
}

if [ "X$1" == "X--help" -o "X$1" == "X-h" ];then
     Usage
     exit 1
fi

SYMBOL="SYS"
token_amount=1000000000.0000 # 十亿
issuer="eosio"

if [ $# -ge 3 ];then
     token_amount=$3
fi

if [ $# -ge 2 ];then
     issuer=$2
fi

if [ $# -ge 1 ];then
     SYMBOL=$1
fi

contract_account="eosio.token"

echo
echo "Create $token_amount $SYMBOL to <$issuer>"
cleos push action $contract_account create '{"issuer":"'"$issuer"'", "maximum_supply":"'"$token_amount $SYMBOL"'", "can_freeze":1, "can_recall":1, "can_whitelist":1}' -p $contract_account

echo
echo "Issue $token_amount $SYMBOL to <$issuer>"
cleos push action $contract_account issue '["'"$issuer"'","'"$token_amount $SYMBOL"'","deposit started"]' -p $issuer


BUILDED_CONTRACTS_ROOT=~/eosio/eos/build/contracts/
cd ${BUILDED_CONTRACTS_ROOT}
echo
echo "Deploy system contract to <eosio> with CL: cd ~/eosio/eos/build/contracts/; cleos set contract eosio eosio.system -p eosio"
cleos set contract eosio eosio.system -p eosio

echo
echo "Show the balance for <$issuer>:"
cleos get currency balance $contract_account $issuer
echo

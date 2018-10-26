#!/bin/bash

# ===========================================
#
# 说明：创建账号或显示公钥下的所有账号
#       显示账号：$0 list $pub_key
#       创建账号：$0 $account_name $pub_key
# author: Chenchunyun
# Date  : 2018.06.28
#
# ===========================================

set -o nounset
#set -x
#set -o errexit

owner_pub_key=EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
active_pub_key=EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV
wallet_8900_pw=PW5J4iTWH4JSHZRbu6LC9YEmYpWN3rGtCKVHaR4sFzAtVF3HZ8igs

accounts=""
function list_accounts_for_pubkey()
{
    accounts=$(cleos get accounts $owner_pub_key)

    echo "[SH_DEBUG] Show the accounts with pubic key $owner_pub_key"
    echo "$accounts"
}

function create_account_by_faucet(){
    curl http://127.0.0.1:8888/v1/faucet/create_account -X POST -d '{
        "account": "'"$account_name"'"
        "keys":{
               "owner": "'"$owner_pub_key"'",
               "active": "'"$active_pub_key"'"
                }
        }'    
}

function create_account_by_cleos(){
    cleos create account eosio $account_name $owner_pub_key $active_pub_key
}

echo
if [ $# -ge 1 ]; then
    if [ $# -eq 2 ]; then
        owner_pub_key=$2
        active_pub_key=$2
    fi

    if [ "X$1" = "Xlist" ]; then
         list_accounts_for_pubkey
         exit 1
    fi

    if [ $# -eq 3 ]; then
        active_pub_key=$3
    fi

    search_ret=$(cleos wallet list |grep -E "default\s+\*")

    if [ "${search_ret}X" == "X" ]; then
        cleos wallet open -n default
        
        echo "Unlock wallet 'default'"
        cleos wallet unlock --password $wallet_8900_pw
    fi

    echo "[SH_Debug]Creating user '$1'"
    echo "[SH_Debug] owner pub key: $owner_pub_key"
    echo "[SH_Debug] active pub key: $active_pub_key"

    account_name=$1
#    create_account_by_faucet
    create_account_by_cleos

    list_accounts_for_pubkey

    contract_account="eosio.token"
    grepret=$(echo $accounts | grep $contract_account | wc -l)
    if [ "X$account_name" == "X$contract_account" -a $grepret -ge 1 ];then
        BUILDED_CONTRACTS_ROOT=~/eosio/eos/build/contracts/
        cd ${BUILDED_CONTRACTS_ROOT}

        echo "Deploy system contract to <$contract_account>"
        cleos set contract $contract_account eosio.token -p $contract_account
    fi
else
    echo "Usage: $0 <account_name> [public key for owner and active]"
fi

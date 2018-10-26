#!/bin/bash

set -o nounset
#set -o errexit
#set -x

phost="$(grep trx-broadcast-url config.ini | grep -v "#" | cut -d/ -f3 | cut -d: -f1)"
host="localhost"
nodeos_port="8888"
wallet="default"
wallet_pw="PW5JQMamnjnd4uniDzND23XfBvzwarftFb1mhgvGX6CKEhhU2JHs5"

if [ $# -lt 3 ]; then
    echo "Usage: $0 <from> <to> <public_key for signing transaction> [expiration seconds for transaction] [EOS count(like: 3.0000)]"
else
    u_from=$1
    u_to=$2
    sign_pub_key=$3

    trx_expiration_sec=30
    if [ $# -ge 4 ]; then
        trx_expiration_sec=$4
    fi

    trx_eos_cunt="1.0000 EOS"
    if [ $# -eq 5 ]; then
        trx_eos_cunt="$5 EOS"
    fi

	echo "---------------------------------"
	echo "[SH_Debug] RPC API: v1/chain/abi_json_to_bin"
	echo "[SH_Debug] User <$u_from> transfer <$trx_eos_cunt> to user <$u_to>"
	
	binary_data=$(curl http://$phost:$nodeos_port/v1/chain/abi_json_to_bin -X POST -d '{
	   "code": "eosio.token",
	   "action": "transfer",
	   "args": {
	       "from": "'"${u_from}"'",
	       "to": "'"${u_to}"'",
	       "quantity": "'"${trx_eos_cunt}"'",
	       "memo": "'"transfer ${trx_eos_cunt}"'"
	   }}' -s | jq '.binargs')
	
	echo "[SH_Debug] abi_json_to_bin data: $binary_data"
	echo "---------------------------------"
	echo "[SH_Debug] RPC API: v1/wallet/sign_transaction"
	
	json_get_info=$(cleos -u http://$phost:$nodeos_port get info)
	block_num=$(echo $json_get_info | jq '.head_block_num')
	block_timestamp=$(echo $json_get_info | jq '.head_block_time')
	block_prefix=$(cleos -u http://$phost:$nodeos_port get block ${block_num} | jq '.ref_block_prefix')
	chain_id=$(echo $json_get_info | jq '.chain_id')
	
	echo "[SH_Debug] Block num: $block_num, Block prefix: $block_prefix"
	
	now_seconds=`date +%s` # the seconds of now
	serv_seconds=$((${now_seconds} - 8 * 3600 + ${trx_expiration_sec})) # 服务器时间比电脑时间慢8小时，同时事务时间要比当前时间晚1分钟
    if [ "Xmac" == "X$(who | head -1 | cut -f1 -d" ")" ];then
    	serv_seconds_humman_format=`date -r ${serv_seconds} "+%Y-%m-%dT%H:%M:%S"`
    else
	    serv_seconds_humman_format=`date -d @${serv_seconds} "+%Y-%m-%dT%H:%M:%S"`
    fi
	
	echo "[SH_Debug] The delay time: ${trx_expiration_sec}s, transaction time: \"$serv_seconds_humman_format\", block time: $block_timestamp"
	echo "[SH_Debug] The public key for signing transaction: ${sign_pub_key}"
	
	cleos wallet list |grep -q -E "$wallet\s+\*"
	if [ $? -ne 0 ]; then
	    cleos wallet open -n $wallet 
	
	    echo "[SH_Debug] Unlock wallet <$wallet>"
	    cleos wallet unlock -n $wallet --password $wallet_pw
	fi
	
	sign_trx_ret=$(curl http://$host:8900/v1/wallet/sign_transaction -X POST -d '[
	    {
	     	"ref_block_num": '"${block_num}"', 
	        "ref_block_prefix": '"${block_prefix}"',
	        "expiration": "'"${serv_seconds_humman_format}"'",
	        "max_net_usage_words": 100,
	 	    "max_cpu_usage_ms": 100,
	        "actions": [
	            {
	                "account": "eosio.token",
	                "name": "transfer",
	                "authorization": [
	                    {
	                        "actor": "'"${u_from}"'",
	                        "permission": "active"
	                    }
	                ],
	                "data": '"${binary_data}"'
	            }
	        ],
	        "signatures": []
	    },
	    [
	  	"'"${sign_pub_key}"'"
	    ],
	    '"${chain_id}"'
	]' -s)
	
	pushed_trx=$(echo $sign_trx_ret | sed -e 's/\([{]\)/\1\"transaction\":\{/' -e 's/\(\"signatures\":\)/\}\,\1/' -e 's/\,\}/\}/'  -e 's/\([{]\)/\{\"compression\": \"none\"\,/')
	
	err_code=$(echo $sign_trx_ret | jq '.error.code')
	
	# If signature succeed, then push the transaction
	if [ "null" == "${err_code}" ]; then
		echo "---------------------------------"
		echo "[SH_Debug] RPC API: v1/chain/push_transaction"
		echo "[SH_Debug] Pushed transaction body:"
		echo $pushed_trx | jq .
		
		push_trx_ret=$(curl http://$phost:$nodeos_port/v1/chain/push_transaction -X POST -d "${pushed_trx}" -s)
		
		echo "================="
		echo "[SH_Debug] Push transaction results:"
		echo ${push_trx_ret} | jq '.'
		
    else
		echo "---------------------------------"
		echo "[SH_Debug] Signature error messages: `echo $sign_trx_ret | jq '.error.details[0].message'`"
	fi

	echo
	echo "---------------------------------"
fi

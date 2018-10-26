#!/bin/bash

#set -x
set -o nounset
#set -o errexit

owner_pub_key="EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV"
active_pub_key="EOS6MRyAjQq8ud7hVNYcfnVPJqcVpscN5So8BhtHuGYqET5GDW5CV"
wallet_8888_pw="PW5J4iTWH4JSHZRbu6LC9YEmYpWN3rGtCKVHaR4sFzAtVF3HZ8igs"
wallet_8900_pw="PW5J4iTWH4JSHZRbu6LC9YEmYpWN3rGtCKVHaR4sFzAtVF3HZ8igs"
wt_default_8900="default"
contract_code_default="eosio.token"

host="127.0.0.1"
#host="192.168.2.232"
phost="$(grep trx-broadcast-url config.ini | grep -v "#" | cut -d/ -f3 | cut -d: -f1)"
nodeos_port="8888"

function Usage(){
    echo
    echo "Usage: $0 <key|balance|transfer|buyram> [options]"
    echo "subcommands"
    echo "   key: Create public and private keys"
    echo "   balance <account_name> <contract> [sysmbol]: Get the currency balance for account"
    echo "   transfer <from> <to> [expiration in seconds] [currency count(like: 3.0000, default: 1.0000)] [symbol(default: SYS)]: Transfer currency from <from> to <to> account"
    echo "   buyram <payer> <receiver> [quant(like: "1 SYS")] [symbol(default: SYS)]: payer buy rams for receiver"
    echo "   sellram <receiver> <ram bytes>: receiver sell its rams and receive corresponding tokens"
    echo "   delegate <from> <receiver> <stake_net_quantity> <stake_cpu_quantity> [transfer(default: false)]"
    echo "   undelegate <from> <receiver> <stake_net_quantity> <stake_cpu_quantity>"
    echo "   account <creator> <account_name> [buy_ram_eos] [stake_net] [stake_cpu] [transfer] [owner_key] [active_key]"
    echo "    wallet <name>"
    echo "    rpcacc <name> <password>"
    echo "    rpcaccdel <name>"
    echo "    rpcacclist"
    echo "    table <contract_account> <action> <table_name> <record_data>"
    echo
}

function unlock_wallet(){
    search_ret=$(cleos wallet list |grep -E "${wt_default_8900}\s+\*")
    search_ret=$(cleos wallet list |grep -E "${wt_default_8900}\s+\*")

    if [ "${search_ret}X" == "X"  ]; then
         cleos wallet open -n $wt_default_8900
         echo "Unlock wallet <$wt_default_8900>"
         cleos wallet unlock -n $wt_default_8900 --password ${wallet_8900_pw}
    fi
}

rpc_account="nxsdchicken"
rpc_pw="123456"
sys_name="syszero"
sys_pw="NX99_sD#@32"

if [ $# -ge 1 ]; then
    if [ "X$1" == "Xtable"  ]; then
        if [ $# -lt 5  ];then
    	    echo "Usage: $0 table <contract_account> <action> <table_name> <record_data>"
            exit 1
        fi
        contract_account=$2
        action=$3
        table=$4
        data=$5

        echo "[SH_Debug] $action table <$table> with record data <$data>"
        cret=$(curl http://$host:$nodeos_port/v1/nxsd/handle_table -X POST -d '{
             "wallet": "'"$wt_default_8900"'",
             "ppwww": "'"${wallet_8900_pw}"'",
             "contract_account": "'"${contract_account}"'",
             "action": "'"${action}"'",
             "table": "'"${table}"'",
             "data": "'"${data}"'",
             "rpc_account": {
                 "name": "'"$rpc_account"'",
                 "pw": "'"$rpc_pw"'",
             }
            }' -s)
        echo $cret | jq "."
        echo

    elif [ "X$1" == "Xrpcaccdel"  ]; then
        if [ $# -lt 2  ];then
            echo "Usage: $0 rpcaccdel <name>"
            exit 1
        fi

        acc_name="$2"
        cret=$(curl http://$host:$nodeos_port/v1/nxsd/delete_rpc_account -X POST -d '{
                    "admin": {
                        "name": "'"${sys_name}"'",
                        "pw": "'"${sys_pw}"'",
                    },
                    "account_name": "'"${acc_name}"'"
                }' -s)
        echo $cret | jq "."
        echo

    elif [ "X$1" == "Xrpcacclist"  ]; then
        cret=$(curl http://$host:$nodeos_port/v1/nxsd/list_rpc_account -X POST -d '{
            "name": "'"$sys_name"'",
            "pw": "'"$sys_pw"'",
        }' -s)
        echo $cret | jq "."
        echo

    elif [ "X$1" == "Xrpcacc"  ]; then
        if [ $# -lt 3  ];then
            echo "Usage: $0 rpcacc <name> <password>"
            exit 1
        fi

        acc_name="$2"
        acc_pw="$3"
        cret=$(curl http://$host:$nodeos_port/v1/nxsd/create_rpc_account -X POST -d '{
                    "creator": {
                        "name": "'"${sys_name}"'",
                        "pw": "'"${sys_pw}"'",
                    },
                    "account": {
                        "name": "'"${acc_name}"'",
                        "pw": "'"${acc_pw}"'",
                    }

                }' -s)
        echo $cret | jq "."
        echo

    elif [ "X$1" == "Xwallet"  ]; then
        if [ $# -lt 2  ];then
            echo "Usage: $0 wallet <name>"
            exit 1
        fi

        wallet_name="$2"
        cret=$(curl http://$host:$nodeos_port/v1/nxsd/create_wallet -X POST -d '{
                    "wallet_name": "'"${wallet_name}"'",
                    "rpc_account": {
                        "name": "'"$rpc_account"'",
                        "pw": "'"$rpc_pw"'",
                    }
                }' -s)
        echo $cret | jq "."
        echo

    elif [ "X$1" == "Xaccount"  ]; then
        if [ $# -lt 3  ];then
            echo "Usage: $0 account <creator> <account_name> [buy_ram_eos] [stake_net] [stake_cpu] [transfer] [owner_key] [active_key]"
            exit 1
        fi

        creator=$2
        account_name=$3
        symbol="SYS"

        buy_ram_eos="900.0000"
        if [ $# -ge 4 ];then
            buy_ram_eos="$4"
        fi

        stake_net="450.0000"
        if [ $# -ge 5  ];then
            stake_net="$5"
        fi

        stake_cpu="450.0000"
        if [ $# -ge 6  ];then
            stake_cpu="$6"
        fi

        transfer="false"
        if [ $# -ge 7  ];then
            transfer=$7
        fi

        owner="$owner_pub_key"
        if [ $# -ge 8  ];then
            owner="$8"
        fi

        active="$owner"
        if [ $# -ge 9  ];then
            active="$9"
        fi

        echo "[SH_Debug] <$creator> create account <$account_name> with net:$stake_net cpu:$stake_cpu ram:$buy_ram_eos"
        cret=$(curl http://$host:$nodeos_port/v1/nxsd/create_account -X POST -d '{
             "wallet": "'"$wt_default_8900"'",
             "ppwww": "'"${wallet_8900_pw}"'",
             "creator": "'"${creator}"'",
             "account_name": "'"${account_name}"'",
             "buy_ram_eos": "'"${buy_ram_eos} $symbol"'",
             "stake_net": "'"${stake_net} $symbol"'",
             "stake_cpu": "'"${stake_cpu} $symbol"'",
             "transfer": '"${transfer}"',
             "keys": {
                 "owner": "'"$owner"'",
                 "active": "'"$active"'"
             },
             "rpc_account": {
                 "name": "'"$rpc_account"'",
                 "pw": "'"$rpc_pw"'",
             }
            }' -s)

        echo $cret | jq "."
        echo

    elif [ "X$1" == "Xundelegate"  ]; then
        if [ $# -lt 5  ];then
            echo "Usage: $0 undelegate <from> <receiver> <stake_net_quantity> <stake_cpu_quantity>"
            exit 1
        fi
        from=$2
        receiver=$3
        unstake_net_quantity=$4
        unstake_cpu_quantity=$5

        echo "[SH_Debug] User <$from> undelegate net:$unstake_net_quantity cpu:$unstake_cpu_quantity to <$receiver>"
        cret=$(curl http://$host:$nodeos_port/v1/nxsd/undelegatebw -X POST -d '{
             "wallet": "'"$wt_default_8900"'",
             "ppwww": "'"${wallet_8900_pw}"'",
             "from": "'"${from}"'",
             "receiver": "'"${receiver}"'",
             "unstake_net_quantity": "'"${unstake_net_quantity}"'",
             "unstake_cpu_quantity": "'"${unstake_cpu_quantity}"'",
             "rpc_account": {
                 "name": "'"$rpc_account"'",
                 "pw": "'"$rpc_pw"'",
             }
            }' -s)
        echo $cret | jq "."
        echo

    elif [ "X$1" == "Xdelegate"  ]; then
        if [ $# -lt 5  ];then
            echo "Usage: $0 delegate <from> <receiver> <stake_net_quantity> <stake_cpu_quantity> [transfer(default: false)]"
            exit 1
        fi
        from=$2
        receiver=$3
        stake_net_quantity=$4
        stake_cpu_quantity=$5
        transfer="false"

        if [ $# -ge 6  ];then
            transfer=$6
        fi

        echo "[SH_Debug] User <$from> delegate net:$stake_net_quantity cpu:$stake_cpu_quantity to <$receiver> [transfer]"
        cret=$(curl http://$host:$nodeos_port/v1/nxsd/delegatebw -X POST -d '{
             "wallet": "'"$wt_default_8900"'",
             "ppwww": "'"${wallet_8900_pw}"'",
             "from": "'"${from}"'",
             "receiver": "'"${receiver}"'",
             "stake_net_quantity": "'"${stake_net_quantity}"'",
             "stake_cpu_quantity": "'"${stake_cpu_quantity}"'",
             "transfer": '"${transfer}"'
            }' -s)
        echo $cret | jq "."
        echo

    elif [ "X$1" == "Xsellram"  ]; then
        if [ $# -lt 3  ];then
            echo "Usage: $0 sellram <receiver> <ram bytes>: receiver sell its rams and receive corresponding tokens"
            exit 1
        fi
        receiver=$2
        amount=$3

        echo "[SH_Debug] User <$receiver> sell $amount bytes ram"
        cret=$(curl http://$host:$nodeos_port/v1/nxsd/sell_ram -X POST -d '{
             "wallet": "'"$wt_default_8900"'",
             "ppwww": "'"${wallet_8900_pw}"'",
             "receiver": "'"${receiver}"'",
             "amount": '"${amount}"',
             "rpc_account": {
                 "name": "'"$rpc_account"'",
                 "pw": "'"$rpc_pw"'",
             }
            }' -s)
        echo $cret | jq "."
        echo

    elif [ "X$1" == "Xbuyram"  ]; then
        if [ $# -lt 3  ];then
            echo "Usage: $0 buyram <payer> <receiver> [quant(eg: 1)] [symbol(default: SYS)]"
            exit 1
        fi
        payer=$2
        receiver=$3
        quant=1
        symbol="SYS"

        if [ $# -ge 4  ];then
            quant=$4
        fi

        if [ $# -ge 5  ];then
            symbol=$5
        fi

        echo "[SH_Debug] User <$payer> buy $quant $symbol rams for <$receiver>"
        cret=$(curl http://$host:$nodeos_port/v1/nxsd/buy_ram -X POST -d '{
             "wallet": "'"$wt_default_8900"'",
             "ppwww": "'"${wallet_8900_pw}"'",
             "payer": "'"${payer}"'",
             "receiver": "'"${receiver}"'",
             "quant": "'"${quant} ${symbol}"'",
             "rpc_account": {
                 "name": "'"$rpc_account"'",
                 "pw": "'"$rpc_pw"'",
             }
           }' -s)
        echo $cret | jq "."
        echo

    elif [ "X$1" == "Xkey" ]; then
        echo "[SH_Debug] Create public and private keys:"
        cret=$(curl http://$host:$nodeos_port/v1/nxsd/create_key -s)
        echo $cret | jq "."
        echo

    elif [ "X$1" == "Xbalance" ]; then
        if [ $# -lt 2 ];then
                echo "Usage: $0 balance <account> [contract(default value is eosio.token)] [symbol(default value is SYS)]"
            exit 1
        fi
        blc_account=$2
        blc_contract="eosio.token"
        blc_symbol="ALL"
        if [ $# -ge 3 ];then
            blc_contract=$3
        fi
        if [ $# -ge 4 ];then
            blc_symbol=$4
        fi
        echo "[SH_Debug] Get balance for account <$blc_account>:"

        cret=$(curl http://$host:$nodeos_port/v1/nxsd/get_balance -X GET -d '{
          "code": "'"${blc_contract}"'",
          "account": "'"${blc_account}"'",
          "symbol": "'"${blc_symbol}"'"
          }' -s)
        echo $cret | jq "."
        echo

    elif [ "X$1" == "Xtransfer" ]; then
        if [ $# -lt 3 ];then
            echo "Usage: $0 transfer <from> <to> [expiration in seconds] [currency count(like: 3.0000)] [symbol(default: SYS)]"
            exit 1
        fi
        u_from=$2
        u_to=$3
        symbol="SYS"
        trx_eos_cunt="1.0000"
        expiration=10

        if [ $# -ge 4 ]; then
            expiration="$4"
        fi

        if [ $# -ge 5 ]; then
            trx_eos_cunt="$5"
        fi

        if [ $# -ge 6 ]; then
            symbol="$6"
        fi

        echo "[SH_Debug] Transfer currency '${trx_eos_cunt} $symbol' from <$u_from> to <$u_to>."
        tx_ret=$(curl http://$host:$nodeos_port/v1/nxsd/token_transfer -X POST -d '{
                    "wallet": "'"$wt_default_8900"'",
                    "code": "'"$contract_code_default"'",
                    "ppwww": "'"${wallet_8900_pw}"'",
                    "args": {
	                    "from": "'"${u_from}"'",
	                    "to": "'"${u_to}"'",
	                    "quantity": "'"${trx_eos_cunt} ${symbol}"'",
                        "expiration": '"${expiration}"',
	                    "memo": "'"transfer ${trx_eos_cunt}"'"
                    },
                    "rpc_account": {
                        "name": "'"$rpc_account"'",
                        "pw": "'"$rpc_pw"'",
                    }
                }' -s)

        echo "[SH_Debug] Transfer result:"
        echo ${tx_ret} | jq "."

        echo "[SH_DEBUG] The transaction expiration after $expiration seconds!"

        echo "[SH_Debug] Show balance for users:"
        echo "${u_to}'s balance on $phost:$nodeos_port >>>"
        cleos -u http://$phost:$nodeos_port get currency balance eosio.token ${u_to} $symbol
        echo
        echo "${u_from}'s balance on $phost:$nodeos_port >>>"
        cleos -u http://$phost:$nodeos_port get currency balance eosio.token ${u_from} $symbol

    else
        Usage;
    fi

else
    Usage;
fi

# eosio-nxsd-plugin
An open source plugin for eosio blockchain

# 部署插件步骤
1：下载插件源码，执行插件配置脚本install_nxsd_plugin.sh，如果脚本执行成功则跳到第9步；执行失败则从第2步开始配置。  
2：将eosio-nxsd-plugin文件夹复制到eosio源码根目录下的plugins目录下  
3：将eosio-nxsd-plugin目录名为nxsd_api_plugin  
4：修改programs/nodeos/CMakeLists.txt文件，在target_link_libraries条目里添加一行"PRIVATE -Wl,${whole_archive_flag} nxsd_api_plugin            -Wl,${no_whole_archive_flag}"（不要双引号）  
5：修改plugins/CMakeLists.txt文件，添加一行“add_subdirectory(nxsd_api_plugin)”（不要双引号）  
6：修改contracts/CMakeLists.txt文件，添加一行“add_subdirectory(nxsd.farm)”（不要双引号）  
7: 修改nodeos使用的config.ini文件，改动如下：  
trx-broadcast-url =             # 新增，交易广播的节点url，如：http://127.0.0.1:8888  
rsa-priv-file =                 # 新增，pem格式的私钥文件（绝对路径），如：/temp/priv.pem  
rsa-pub-file =                  # 新增，pem格式的公钥文件（绝对路径），如：/temp/pub.pem  
rpc-account-file =              # 新增，rpc接口调用账号文件（绝对路径），如：/temp/rpc_acc.dat  
token-inc-meta =                # 新增，ICO增发规则，如：wallet:wallet-passwd:sender:to:total:token_symbol:inc_ratio:y|m|w|d|h|n:inc_num  
plugin = eosio::nxsd_api_plugin # 新增   
8: 编译eosio源码  
9: 配置好第7步骤中的选项值，启动nodeos，完成。  

# 新增RPC接口调用事例 
1. 创建账户  
curl http://$host:$port/v1/faucet/create_account -X POST -d '{  
  "account": "${ACCOUNT_NAME}"  ## 账户名   
  "keys":{  
      "owner": "${OWNER_PERMISSION_PUBLIC_KEY}",   ## owner权限关联的公钥  
      "active": "${ACTIVE_PERMISSION_PUBLIC_KEY}"  ## active权限关联的公钥  
  },   
  "rpc_account": {  
      "name": "'"$rpc_account"'", ## RPC帐号  
      "pw": "'"$rpc_pw"'",        ## RPC帐号的密码  
  }  
}'  
调用成功：  
{}  ## 返回空  
调用失败：返回错误码及错误描述说明   

2. 创建密钥对  
curl http://$host:$port/v1/nxsd/create_key -X POST  
调用成功：  
{  
    "pub_key":"<base64> or <plain text>", ## RSA加密后的公钥或公钥明文   
    "prv_key":"<base64> or <plain text>"  ## RSA加密后的私钥或私钥明文  
}  
调用失败：返回错误码及错误描述说明   

3. 转账，币由A账户转到B账户   
curl http://$host:$port/v1/nxsd/coin_transfer -X POST -d '{   
  "wallet": "${WALLET_NAME}",      ## 钱包名称   
  "code": "${TRANSFER_CONTRACT}",  ## 转账合约，如eosio.token  
  "ppwww": "${WALLET_PASSWORD}",   ## 钱包密码  
  "args": {  
      "from": "${ACCOUNT_FROM}",   ## 币转出账户名  
      "to": "${ACCOUNT_TO}",       ## 币转入账户名  
      "quantity": "${CURRENCY_COUNT}",  ## 币数量，比如23.00000 EOS  
      "expiration": '"${expiration}"',  ## 交易过期时长，秒为单位  
      "memo": "${TRANSFER_MEMO}"   ## 备注  
  },  
  "rpc_account": {  
      "name": "'"$rpc_account"'", ## RPC帐号  
      "pw": "'"$rpc_pw"'",        ## RPC帐号的密码  
  }  
}'  
调用成功：  
{  
    "transaction_id": "ef764ae22aced4fc5e7aae30cd8afb35418e7e79fb0753d694454aff78b4f7c9",  
    "processed": {  
      "id": "ef764ae22aced4fc5e7aae30cd8afb35418e7e79fb0753d694454aff78b4f7c9",  
      "receipt": {  
        "status": "executed",  ## executed表示成功，其他表示交易为生效  
        "cpu_usage_us": 2509,  
        "net_usage_words": 18  
      },  
      "elapsed": 2509,  
      .......   
}  
调用失败：返回错误码及错误描述说明   
  
4. 获取账户的余额  
curl http://$host:$port/v1/nxsd/get_balance -X GET -d '{  
     "code": "'"${CONTRACT_ACCOUNT}"'",  ## 拥有合约的账号，如eosio.token  
     "account": "'"${ACCOUNT_NAME}"'",   ## 账户名称  
     "symbol": "'"${CURRENCY_SYMBOL}"'"  ## 币的符号，如EOS，SYS, ALL(表示获取所有币种余额)  
}'  
调用成功：  
{  
  "blc_asset":["999999965.3885 EOS"]  
}  ## 或返回空也是成功，如：{}  
调用失败：返回错误码及错误描述说明   

5. 购买内存  
curl http://$host:$port/v1/nxsd/buy_ram -X POST -d '{  
     "wallet": "'"${WALLET_NAME}"'",   ## 钱包名称  
     "ppwww": "'"${WALLET_PASSWORD}"'",## 钱包密码  
     "payer": "'"${PAYER}"'",        ## 付费者  
     "receiver": "'"${RECEIVER}"'",  ## 内存接受者  
     "quant": "'"${QUANT}"'",         ## 购买内存的量，以币度量，如："32 EOS"  
     "rpc_account": {  
        "name": "'"$rpc_account"'", ## RPC帐号  
        "pw": "'"$rpc_pw"'",        ## RPC帐号的密码  
  }  
}'  
调用成功：  
{}  
调用失败：返回错误码及错误描述说明   

6. 卖出内存  
curl http://$host:$port/v1/nxsd/sell_ram -X POST -d '{  
     "wallet": "'"${WALLET_NAME}"'",   ## 钱包名称  
     "ppwww": "'"${WALLET_PASSWORD}"'",## 钱包密码  
     "receiver": "'"${RECEIVER}"'",  ## 售出内存代币的接收者  
     "amount": "'"${AMOUNT}"'",      ## 内存数量，字节为单位，如：1024，0表示卖出全部剩余未使用的内存(谨慎操作)  
     "rpc_account": {  
       "name": "'"$rpc_account"'", ## RPC帐号  
       "pw": "'"$rpc_pw"'",        ## RPC帐号的密码  
  }  
}'  
调用成功：  
{}  
调用失败：返回错误码及错误描述说明   

7. 抵押网络和CPU带宽   
curl http://$host:$port/v1/nxsd/delegatebw -X POST -d '{   
  "wallet": "'"${WALLET_NAME}"'",   ## 钱包名称  
  "ppwww": "'"${WALLET_PASSWORD}"'",## 钱包密码  
  "from": "'"${ACCOUNT_FROM}"'",                  ## 抵押账户  
  "receiver": "'"${ACCOUNT_RECEIVER}"'",          ## 带宽接收账户  
  "stake_net_quantity": "'"${CURRENCY_COUNT}"'",  ## 网络抵押数量，比如23.00000 EOS  
  "stake_cpu_quantity": "'"${CURRENCY_COUNT}"'",  ## CPU抵押数量，比如23.0000 EOS  
  "transfer": '"${TRANSFER_FLAG}"',               ## 带宽接收账户是否可以解除抵押标志，允许：true，禁止：false   
  "rpc_account": {  
      "name": "'"$rpc_account"'", ## RPC帐号  
      "pw": "'"$rpc_pw"'",        ## RPC帐号的密码  
  }  
  }'  
调用成功：  
{}  
调用失败：返回错误码及错误描述说明   


8. 释放网络和CPU带宽   
curl http://$host:$port/v1/nxsd/undelegatebw -X POST -d '{   
  "wallet": "'"${WALLET_NAME}"'",   ## 钱包名称  
  "ppwww": "'"${WALLET_PASSWORD}"'",## 钱包密码  
  "from": "'"${ACCOUNT_FROM}"'",                  ## 释放网路和CPU资源的账户  
  "receiver": "'"${ACCOUNT_RECEIVER}"'",          ## 接收释放的带宽和CPU的账户  
  "unstake_net_quantity": "'"${CURRENCY_COUNT}"'",  ## 网络抵押数量，比如23.00000 EOS  
  "unstake_cpu_quantity": "'"${CURRENCY_COUNT}"'",  ## CPU抵押数量，比如23.0000 EOS  
  "rpc_account": {  
      "name": "'"$rpc_account"'", ## RPC帐号  
      "pw": "'"$rpc_pw"'",        ## RPC帐号的密码  
  }  
  }'  
调用成功：  
{}  
调用失败：返回错误码及错误描述说明   
     

9. 创建账号，支持创建时购买内存、抵押cpu和带宽  
curl http://$host:$port/v1/nxsd/create_account -X POST -d '{  
     "wallet": "'"$wt_default_8900"'",  ## 钱包名称  
     "ppwww": "'"${wallet_8900_pw}"'",  ## 钱包密码  
     "creator": "'"${creator}"'",       ## 新账号的创建者账号  
     "account_name": "'"${account_name}"'",  ## 新账号名  
     "buy_ram_eos": "'"${buy_ram_eos} $symbol"'",  ## 内存数量，以代币为单位，如：1.0000 EOS  
     "stake_net": "'"${stake_net} $symbol"'",      ## 抵押的带宽数量，以代币为单位，如：1.0000 EOS  
     "stake_cpu": "'"${stake_cpu} $symbol"'",    ## 抵押的内存数量，以代币为单位，如：1.0000 EOS  
     "transfer": '"${transfer}"',  ## 新建的账号是否可以释放带宽和cpu，true：可以，false，不允许
     "keys": {  
            "owner": "'"$owner"'",  ## owner角色权限使用的公钥  
            "active": "'"$active"'" ## active角色权限使用的公钥  
     },  
     "rpc_account": {  
        "name": "'"$rpc_account"'", ## RPC帐号  
        "pw": "'"$rpc_pw"'",        ## RPC帐号的密码  
     }  
   }'  
调用成功：  
{ 
    "transaction_id": "ef764ae22aced4fc5e7aae30cd8afb35418e7e79fb0753d694454aff78b4f7c9",  
         "processed": {  
            "id": "ef764ae22aced4fc5e7aae30cd8afb35418e7e79fb0753d694454aff78b4f7c9",  
            "receipt": {  
               "status": "executed",  ## executed表示成功，其他表示交易为生效  
               "cpu_usage_us": 2509,  
               "net_usage_words": 18  
             },  
             "elapsed": 2509,  
             .......  
         }  
}  
调用失败：返回错误码及错误描述说明  

10. 创建钱包  
curl http://$host:$port/v1/nxsd/create_wallet -X POST -d '{   
     "wallet_name": "'"${wallet_name}"'", ## 钱包名称   
     "rpc_account": {  
        "name": "'"$rpc_account"'", ## RPC帐号  
        "pw": "'"$rpc_pw"'",        ## RPC帐号的密码  
  }  
    }'  
调用成功：  
{  
    "wallet_pw":"<base64> or <plain text>", ## RSA加密后的钱包密码或密码明文   
}  
调用失败：返回错误码及错误描述说明  

11. 创建RPC账号  
curl http://$host:$port/v1/nxsd/create_rpc_account -X POST -d '{   
    "creator": {  
         "name": "'"${sys_name}"'", ## RPC系统账号  
         "pw": "'"${sys_pw}"'",     ## 密码  
    },  
    "account": {   
         "name": "'"${acc_name}"'", ## RPC普通账号  
         "pw": "'"${acc_pw}"'",     ## 密码  
    }  
    }'    
调用成功：  
{  
  "code": 200,  ## 返回码为200，代表成功  
  "message": "Create account succeed."  
}  
调用失败：返回错误码及错误描述说明  

12. 查询RPC普通账号  
curl http://$host:$port/v1/nxsd/list_rpc_account -X POST -d '{  
    "name": "'"${sys_name}"'", ## RPC系统账号  
    "pw": "'"${sys_pw}"'",     ## 密码  
}'
调用成功：  
{  
  "ncount": 1,  
   "accounts": [  
     {  
        "name": "acc1",  
        "pw": "pass"  
     }  
  ]  
}  
调用失败：返回错误码及错误描述说明  
  
13. 数据上链  
curl http://$host:$nodeos_port/v1/nxsd/handle_table -X POST -d '{  
             "wallet": "'"${wallet_name}"'",  
             "ppwww": "'"${wallet_passwd}"'",  
             "contract_account": "'"${contract_account}"'",  
             "action": "'"${action}"'",  
             "table": "'"${table_name}"'",  
             "data": "'"${data}"'",  
             "rpc_account": {  
                 "name": "'"$rpc_account"'",  
                 "pw": "'"$rpc_pw"'",  
             }  
            }'  
调用成功：  
{  
  "transaction_id": "e70b19fb3dab9b7cd09c35a7c7b35c641592ff46b2b91cc7018067a243122ec1",  
  "processed": {  
    "id": "e70b19fb3dab9b7cd09c35a7c7b35c641592ff46b2b91cc7018067a243122ec1",  
    "receipt": {  
      "status": "executed",  ## 状态为executed表明是执行成功  
	........  
     }  
   }  
}  
调用失败：返回错误码及错误描述说明  
  

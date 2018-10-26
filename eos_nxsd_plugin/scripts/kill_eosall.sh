#!/bin/bash

wait_sec=2

NODEOS=nodeos
KEOSD=keosd

nodeos_pid=$(ps -ef |grep "${NODEOS} -e" | grep -v grep | awk '{print $2}')
keosd_pid=$(ps -ef |grep "${KEOSD}" | grep -v grep | awk '{print $2}')

last_block_number=$(cleos get info | jq '.' | grep head_block_num | awk -F":" '{print $2}' | cut -f1 -d,)

if [ "X" == "X${last_block_number}" ];then
     last_block_number="-1 (nodeos is not running)" 
fi

echo 
echo "The lastest block num: $last_block_number"
echo

if [ ! "${nodeos_pid}X" == "X" ];then
	pkill nodeos
	echo "Killing ${NODEOS}, wait ${wait_sec}s .."
	sleep ${wait_sec}
	ps -ef |grep "${NODEOS} -e"
else
	echo -e "No ${NODEOS} process running."
fi

if [ ! "${keosd_pid}X" == "X" ];then
	pkill keosd
	echo "Killing ${KEOSD}, waiting ${wait_sec}s .."
	sleep ${wait_sec}
	ps -ef |grep "${KEOSD}"
else
	echo -e "No ${KEOSD} process running."
fi

echo

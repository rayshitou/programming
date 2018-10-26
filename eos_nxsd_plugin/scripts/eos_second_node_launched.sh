#!/bin/bash

nodeos --producer-name producernde1 --plugin eosio::chain_api_plugin --plugin eosio::net_api_plugin --http-server-address 127.0.0.1:8889 --p2p-listen-endpoint 127.0.0.1:9877 --p2p-peer-address 127.0.0.1:9876 --config-dir node2 --data-dir node2 --private-key ["EOS7EDefEkTjp9LRHBWbwEoNLBeiosHwDGS7GsZEgjgYasNV5AG5e","5JKjSnw1spPtYgne6TAL2iQvWZpnpCzWsba8XfbciYNTwYyVG54"] &

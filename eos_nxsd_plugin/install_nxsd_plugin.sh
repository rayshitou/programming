#!/bin/bash

# ****************************************************************
# This script is to automatically configure nxsd plugin for building EOSIO.
#
# author: chenchunyun
# date:   2018.8.22
# file:   install_nxsd_plugin.sh
# ****************************************************************

# set -o errexit

function usage()
{
  if [ "X$1" == "Xp" ]
  then
    echo "$2 is not valid path"
  elif [ "X$1" == "Xf" ]
  then
    echo "$2 is not valid file"
  else
    echo "Usage: $0 <eos_path(like: /tmp/eos)> <nodeos_config_file(like: /tmp/config.ini)>"
  fi
  exit 1
}

function check_args()
{
  if [ $# -lt 2 ]
  then
    usage
  elif [ ! -d $1 ]
  then
    usage p $1
  elif [ ! -f $2 ]
  then
    usage f $2
  fi
}

check_args $@

eos_dir=$1

eos_nxsd_plugin_dir="${eos_dir}/plugins/nxsd_api_plugin"
eos_contract_dir="${eos_dir}/contracts"
eos_build_dir="${eos_dir}/build"

eos_nodeos_cmakelist_file="${eos_dir}/programs/nodeos/CMakeLists.txt"
eos_plugins_cmakelist_file="${eos_dir}/plugins/CMakeLists.txt"
eos_contracts_cmakelist_file="${eos_dir}/contracts/CMakeLists.txt"

config_ini=$2

function change_config_ini_file()
{
  grep -q "eosio::nxsd_api_plugin" ${config_ini}
  if [ $? -ne 0 ]; then
    echo "plugin = eosio::nxsd_api_plugin" >> ${config_ini}
  fi

  grep -q "rsa-priv-file" ${config_ini}
  if [ $? -ne 0 ]; then
    sed -i '1i # Additonal entities (eosio::nxsd_api_plugin)\ntrx-broadcast-url = http://127.0.0.1:8888\nrsa-priv-file =\nrsa-pub-file =\nrpc-account-file =\ntoken-inc-meta =\n' ${config_ini}
  fi
}

function try_install_sed_on_macos()
{
  sysOS=$(uname -s)
  if [ "$sysOS" == "Darwin" ];then
    which sed | grep -q local
    if [ $? -ne 0 ];then
      echo "Installing gun-sed, please wait 1-3 minutes ..."
      brew install gnu-sed --with-default-names
    fi
  fi
}

function change_cmakelist_files()
{
  grep -q "nxsd_api_plugin" ${eos_nodeos_cmakelist_file}
  if [ $? -ne 0 ]; then
    sed -i '/PRIVATE appbase/a\        PRIVATE -Wl,${whole_archive_flag} nxsd_api_plugin            -Wl,${no_whole_archive_flag}' ${eos_nodeos_cmakelist_file}
  fi

  grep -q "nxsd_api_plugin" ${eos_plugins_cmakelist_file}
  if [ $? -ne 0 ]; then
    sed -i '/add_subdirectory(chain_plugin)/a\add_subdirectory(nxsd_api_plugin)' ${eos_plugins_cmakelist_file}
  fi

  grep -q "nxsd.farm" ${eos_contracts_cmakelist_file}
  if [ $? -ne 0 ]; then
    sed -i '/add_subdirectory(eosiolib)/a\add_subdirectory(nxsd.farm)' ${eos_contracts_cmakelist_file}
  fi
}

function del_irrelevant_dir_under_github_folder()
{
  rm -rf $*/scripts
  rm -rf $*/nx_wallet
}

function del_irrelevant_entity()
{
  del_irrelevant_dir_under_github_folder ${eos_nxsd_plugin_dir}

  rm -rf ${eos_nxsd_plugin_dir}/contracts
  rm -f  ${eos_nxsd_plugin_dir}/install_nxsd_plugin.sh
}

function copy_plugin_dir()
{
  mkdir -p ${eos_nxsd_plugin_dir}
  cp -rp ./* ${eos_nxsd_plugin_dir} 

  del_irrelevant_entity
}

function copy_contract_dir()
{
  cp -rp ./contracts/* ${eos_contract_dir}
}

function copy_plugin_dir_to_github()
{
  github_plugin_dir=/Users/app/eosio/nxsd-eos
  if [ -d ${github_plugin_dir} ]; then
    cp -rp ./* ${github_plugin_dir}
    
    del_irrelevant_dir_under_github_folder ${github_plugin_dir}
  fi
}

function make_install()
{
  echo "****************************************"
  echo "Be going to make install EOSIO programs"
  echo "Please input the password for SUDO"
  echo "****************************************"

  cd ${eos_build_dir} && sudo make install

  if [ $? -eq 0 ]; then
    echo 
    echo "!!SUCCESS FOR INSTALL EOSIO!!"
    echo
  fi
}

function build_eosio()
{
  if ! read -t 23 -p "The nxsd plugin source is ready, do you want to build eosio now?[y/n] " yORn
  then
    echo
    echo "Wait reading timeout(23 secends)."
    exit 1
  fi

  if [ "Xy" == "X$yORn" -o "X" == "X$yORn" ]; then
    cd ${eos_dir} && ./eosio_build.sh

    if [ $? -eq 0 ]; then
      echo
      echo "!!SUCCESS FOR BUILD EOSIO!!"
      echo

      make_install
    fi
  fi
}

function main()
{
  copy_plugin_dir
  copy_contract_dir
  copy_plugin_dir_to_github

  try_install_sed_on_macos

  change_config_ini_file
  change_cmakelist_files

  echo "!!SUCCESS FOR NXSD CONFIGURATION!!"
  
  build_eosio
}

main

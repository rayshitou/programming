#!/bin/bash

commit_msg="$*"

function check_commit_msg()
{
  if [ $# -lt 1 ]
  then
    echo "Usage: $0 <commit_message>"
    exit 1
  fi
}

function push_gitlab()
{
  gitlab_dir=/Users/app/eosio/eos/nxsd_plugin.gitlab
  
  cd ${gitlab_dir}
  git add . && git commit -m "${commit_msg}" && git push
}

function push_github()
{
  github_dir=/Users/app/eosio/nxsd-eos
  
  cd ${github_dir}
  git add . && git commit -m "${commit_msg}" && git push
}

check_commit_msg $@
push_gitlab
push_github

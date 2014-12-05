#!/bin/bash

SCRIPTDIR=$( cd $(dirname $0) ; pwd -P )
TOPDIR=$( cd ${SCRIPTDIR}/../.. ; pwd -P )

BRANCH=efl/crosswalk-10/39.0.2171.19
BRANCH_XWALK=efl/crosswalk-10/39.0.2171.19

URLBASE='git://github.com/crosswalk-project'

REPOS=( "src|$URLBASE/chromium-crosswalk-efl.git|$BRANCH" \
        "src/third_party/WebKit|$URLBASE/blink-crosswalk-efl.git|$BRANCH" \
        "src/xwalk|$URLBASE/crosswalk-efl.git|$BRANCH_XWALK" \
      )

pushd $TOPDIR 2>&1 > /dev/null

for K in ${REPOS[@]} ; do
  L=(${K//\|/ })
  REPOPATH=${L[0]}
  REPO=${L[1]}
  BRANCH=${L[2]}

  if [[ ! -d $REPOPATH || ! -d $REPOPATH/.git ]] ; then
    echo Cloning branch $BRANCH of $REPO into $REPOPATH
    mkdir -p $REPOPATH
    pushd $REPOPATH 2>&1 > /dev/null
    git init .
    git remote add -t \* -f origin $REPO
    git fetch $REPO $BRANCH:$BRANCH
    git checkout $BRANCH
    popd 2>&1 >/dev/null
  else
    pushd $REPOPATH 2>&1 > /dev/null
    CURRENT_BRANCH=`git rev-parse --abbrev-ref HEAD`
    if [ "x$CURRENT_BRANCH" == "x$BRANCH" ] ; then
      echo Updating branch $BRANCH of $REPO in $REPOPATH
      git fetch $REPO $BRANCH
      git rebase FETCH_HEAD
    else
      echo Fetching branch $BRANCH of $REPO into $REPOPATH
      git fetch $REPO $BRANCH:$BRANCH
      git checkout $BRANCH
    fi
    popd 2>&1 >/dev/null
  fi
done

popd 2>&1 >/dev/null

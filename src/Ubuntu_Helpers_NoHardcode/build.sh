#!/bin/bash

checkout-svn.sh

import-win32-bins.sh
build-makerelease.sh
build-replacements.sh

create-repo.sh

cd1-build.sh
cd2-build.sh

#!/bin/bash

ERR_OK=0
ERR_UNKNOWN_REP_TYPE=1
ERR_UNKNOWN_REPOS_SRC_FORM=2
ERR_APT=3
ERR_DOWNLOAD=4
ERR_DPKG_INSTALL=5
ERR_MAKE=6

SPACE_SED='s/  */ /g; s/^ *//g; s/ *$//g'

PKG_NAME=$(echo "$1" | sed "$SPACE_SED") # Package Name
REPOS_SRC=$(echo "$2" | sed "$SPACE_SED") # repository source (URL, freeform)
REPOS=$(echo "$3" | sed "$SPACE_SED") # repository (woody, sarge, ...)
REPOS_TYPE=$(echo "$4" | sed "$SPACE_SED") # repository type (numeric: 1 - package, 2 - link to source, 3 - archive)
MIN_VER=$(echo "$5" | sed "$SPACE_SED") # minimum version (text)
BIN_EXEC=$(echo "$6" | sed "$SPACE_SED") # binary executables
SRC_INCLUDES=$(echo "$7" | sed "$SPACE_SED") # source includes
SRC_IMPL=$(echo "$8" | sed "$SPACE_SED") # source implementation
BIN_LIB=$(echo "$9" | sed "$SPACE_SED") # binary library
CONFIG=$(echo "${10}" | sed "$SPACE_SED") # configuration
USERNAME=$(echo "${11}" | sed "$SPACE_SED") # CVS/SVN username
PASSWORD=$(echo "${12}" | sed "$SPACE_SED") # CVS/SVN password
PARAMETER=$(echo "${13}" | sed "$SPACE_SED") # Parameter

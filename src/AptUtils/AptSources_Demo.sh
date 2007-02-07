#!/bin/bash
. ./AptSources.sh

AptSrc_ParseSourcesList "${1:-./sample-sources.list}"
AptSrc_WriteSourcesList
echo "-------"
AptSrc_AddSource "deb http://ftp.ro.debian.org/debian sarge main"
AptSrc_AddSource "deb http://ftp.ro.debian.org/debian etch main"
AptSrc_AddSource "deb http://ftp.ro.debian.org/debian sid main"
AptSrc_AddSource "deb http://ftp.ro.debian.org/debian sid non-free"
AptSrc_WriteSourcesList

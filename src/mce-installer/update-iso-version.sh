#!/bin/bash

if [[ "$Version" == "" ]] ;then
	Version="1.0"
fi

sed -i "s/_SNR_VERSION_/$Version/g" src/wget-wrapper.sh

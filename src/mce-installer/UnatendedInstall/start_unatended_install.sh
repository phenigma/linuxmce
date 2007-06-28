#!/bin/bash

## Copy fake frontend configuration file in /tmp
cp ./mce_wizard_data.sh /tmp

## Run the mce-installer scripts
./mce-installer-InstallDependencies.sh 
./mce-installer-core.sh

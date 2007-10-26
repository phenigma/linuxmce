#!/bin/bash

. /usr/share/debconf/confmodule

db_set "exim4/dc_noalias_regenerate" "false"
db_fset "exim4/dc_noalias_regenerate" seen true
db_set "exim4/dc_smarthost" "mail.plutohome.com::587"
db_fset "exim4/dc_smarthost" seen true
db_set "exim4/dc_relay_domains" ""
db_fset "exim4/dc_relay_domains" seen true
db_set "exim4/dc_relay_nets" ""
db_fset "exim4/dc_relay_nets" seen true
db_set "exim4/mailname" "dcerouter"
db_fset "exim4/mailname" seen true
db_set "exim4/dc_localdelivery" "mbox format in /var/mail/"
db_fset "exim4/dc_localdelivery" seen true
db_set "exim4/dc_local_interfaces" "127.0.0.1"
db_fset "exim4/dc_local_interfaces" seen true
db_set "exim4/dc_minimaldns" "false"
db_fset "exim4/dc_minimaldns" seen true
db_set "exim4/dc_other_hostnames" "dcerouter"
db_fset "exim4/dc_other_hostnames" seen true
db_set "exim4/dc_eximconfig_configtype" "mail sent by smarthost; no local mail"
db_fset "exim4/dc_eximconfig_configtype" seen true
db_set "exim4/no_config" "true"
db_fset "exim4/no_config" seen true
db_set "exim4/hide_mailname" ""
db_fset "exim4/hide_mailname" seen true
db_set "exim4/dc_postmaster" ""
db_fset "exim4/dc_postmaster" seen true
db_set "exim4/dc_readhost" ""
db_fset "exim4/dc_readhost" seen true
db_set "exim4/use_split_config" "false"
db_fset "exim4/use_split_config" seen true
db_set "exim4/exim4-config-title" ""
db_fset "exim4/exim4-config-title" seen true
			  

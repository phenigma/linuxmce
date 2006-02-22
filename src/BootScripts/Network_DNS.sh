#!/bin/bash

cat >/etc/bind/named.conf.forwarders <<END
forwarders {
$(grep nameserver /etc/resolv.conf | grep -v '#' | sed 's/nameserver//g; s/ *//g; s/^.*$/\t&;/')
};
END

rndc reload

#!/bin/sh
./tests/net-snmp-release/sbin/snmpd -f -Lo -m "" -C -c tests/snmpd.conf

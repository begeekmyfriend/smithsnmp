#!/bin/sh
.tests/net-snmp-release/sbin/snmptrapd -f -Lo -m "" -C -c tests/snmptrapd.conf

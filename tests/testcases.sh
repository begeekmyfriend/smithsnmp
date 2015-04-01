#!/bin/bash
PATH=$PATH:$(pwd)/tests/net-snmp-release/bin

# SNMPv2 cases
# Option -m "" means do not load any mib file in Net-SNMP.

snmpget -m "" -v2c -cpublic localhost .0 
snmpget -m "" -v2c -cpublic localhost .1.3
snmpget -m "" -v2c -cpublic localhost .1.4
snmpget -m "" -v2c -cpublic localhost .1.5.6.7.8.100

snmpget -m "" -v2c -cpublic localhost .1.3.6.1.2.1.1.9.1.2.1
snmpget -m "" -v2c -cpublic localhost .1.3.6.1.2.1.1.1.0 .1.3.6.1.2.1.1.2.0
snmpget -m "" -v2c -cpublic localhost .1.3.6.1.2.1.1.9.1.1
snmpget -m "" -v2c -cpublic localhost .1.3.6.1.2.1.1.9.1.2
snmpget -m "" -v2c -cpublic localhost .1.3.6.1.2.1.1.9.1.5
snmpget -m "" -v2c -cpublic localhost .1.3.6.1.2.1.1.0

snmpgetnext -m "" -v2c -cpublic localhost .0 
snmpgetnext -m "" -v2c -cpublic localhost .1.3
snmpgetnext -m "" -v2c -cpublic localhost .1.4
snmpgetnext -m "" -v2c -cpublic localhost .1.5.6.7.8.100

snmpbulkget -m "" -v2c -cpublic localhost .1.3.6.1.2.1.1

# Error test (no access)
snmpset -m "" -v2c -cpublic localhost .1.3.6.1.2.1.1.9.1.1 i 1
# Error test (no access)
snmpset -m "" -v2c -cpublic localhost .1.3.6.1.2.1.4.1.0 s "This agent is really smith!"
# Error test (no access)
snmpset -m "" -v2c -cpublic localhost .1.3.6.1.2.1.4.1.0 i 8888

# Error test (no access)
snmpset -m "" -v2c -cprivate localhost .1.3.6.1.2.1.1.9.1.1 i 1
# Error test (wrong type)
snmpset -m "" -v2c -cprivate localhost .1.3.6.1.2.1.4.1.0 s "This agent is really smith!"
# OK
snmpset -m "" -v2c -cprivate localhost .1.3.6.1.2.1.4.1.0 i 8888

snmpwalk -m "" -v2c -cpublic localhost .1.3.6.1.2.1.1
snmpwalk -m "" -v2c -cpublic localhost .1.3.6.1

# SNMPv3 cases for no authentication
# Option -m "" means do not load any mib file in Net-SNMP.

snmpget -m "" -u roNoAuthUser -l noAuthNoPriv localhost .0 
snmpget -m "" -u roNoAuthUser -l noAuthNoPriv localhost .1.3
snmpget -m "" -u roNoAuthUser -l noAuthNoPriv localhost .1.4
snmpget -m "" -u roNoAuthUser -l noAuthNoPriv localhost .1.5.6.7.8.100

snmpget -m "" -u roNoAuthUser -l noAuthNoPriv localhost .1.3.6.1.2.1.1.9.1.2.1
snmpget -m "" -u roNoAuthUser -l noAuthNoPriv localhost .1.3.6.1.2.1.1.1.0 .1.3.6.1.2.1.1.2.0
snmpget -m "" -u roNoAuthUser -l noAuthNoPriv localhost .1.3.6.1.2.1.1.9.1.1
snmpget -m "" -u roNoAuthUser -l noAuthNoPriv localhost .1.3.6.1.2.1.1.9.1.2
snmpget -m "" -u roNoAuthUser -l noAuthNoPriv localhost .1.3.6.1.2.1.1.9.1.5
snmpget -m "" -u roNoAuthUser -l noAuthNoPriv localhost .1.3.6.1.2.1.1.0

snmpgetnext -m "" -u roNoAuthUser -l noAuthNoPriv localhost .0 
snmpgetnext -m "" -u roNoAuthUser -l noAuthNoPriv localhost .1.3
snmpgetnext -m "" -u roNoAuthUser -l noAuthNoPriv localhost .1.4
snmpgetnext -m "" -u roNoAuthUser -l noAuthNoPriv localhost .1.5.6.7.8.100

snmpbulkget -m "" -u roNoAuthUser -l noAuthNoPriv localhost .1.3.6.1.2.1.1

# Error test (no access)
snmpset -m "" -u roNoAuthUser -l noAuthNoPriv localhost .1.3.6.1.2.1.1.9.1.1 i 1
# Error test (no access)
snmpset -m "" -u roNoAuthUser -l noAuthNoPriv localhost .1.3.6.1.2.1.4.1.0 s "This agent is really smith!"
# Error test (no access)
snmpset -m "" -u roNoAuthUser -l noAuthNoPriv localhost .1.3.6.1.2.1.4.1.0 i 8888

# Error test (no access)
snmpset -m "" -u rwNoAuthUser -l noAuthNoPriv localhost .1.3.6.1.2.1.1.9.1.1 i 1
# Error test (wrong type)
snmpset -m "" -u rwNoAuthUser -l noAuthNoPriv localhost .1.3.6.1.2.1.4.1.0 s "This agent is really smith!"
# OK
snmpset -m "" -u rwNoAuthUser -l noAuthNoPriv localhost .1.3.6.1.2.1.4.1.0 i 8888

snmpwalk -m "" -u roNoAuthUser -l noAuthNoPriv localhost .1.3.6.1.2.1.1
snmpwalk -m "" -u roNoAuthUser -l noAuthNoPriv localhost .1.3.6.1

# SNMPv3 cases for authentication only
# Option -m "" means do not load any mib file in Net-SNMP.

snmpget -m "" -u roAuthUser -a MD5 -A "roAuthUser" -l authNoPriv localhost .0 
snmpget -m "" -u roAuthUser -a MD5 -A "roAuthUser" -l authNoPriv localhost .1.3
snmpget -m "" -u roAuthUser -a MD5 -A "roAuthUser" -l authNoPriv localhost .1.4
snmpget -m "" -u roAuthUser -a MD5 -A "roAuthUser" -l authNoPriv localhost .1.5.6.7.8.100

snmpget -m "" -u roAuthUser -a MD5 -A "roAuthUser" -l authNoPriv localhost .1.3.6.1.2.1.1.9.1.2.1
snmpget -m "" -u roAuthUser -a MD5 -A "roAuthUser" -l authNoPriv localhost .1.3.6.1.2.1.1.1.0 .1.3.6.1.2.1.1.2.0
snmpget -m "" -u roAuthUser -a MD5 -A "roAuthUser" -l authNoPriv localhost .1.3.6.1.2.1.1.9.1.1
snmpget -m "" -u roAuthUser -a MD5 -A "roAuthUser" -l authNoPriv localhost .1.3.6.1.2.1.1.9.1.2
snmpget -m "" -u roAuthUser -a MD5 -A "roAuthUser" -l authNoPriv localhost .1.3.6.1.2.1.1.9.1.5
snmpget -m "" -u roAuthUser -a MD5 -A "roAuthUser" -l authNoPriv localhost .1.3.6.1.2.1.1.0

snmpgetnext -m "" -u roAuthUser -a MD5 -A "roAuthUser" -l authNoPriv localhost .0 
snmpgetnext -m "" -u roAuthUser -a MD5 -A "roAuthUser" -l authNoPriv localhost .1.3
snmpgetnext -m "" -u roAuthUser -a MD5 -A "roAuthUser" -l authNoPriv localhost .1.4
snmpgetnext -m "" -u roAuthUser -a MD5 -A "roAuthUser" -l authNoPriv localhost .1.5.6.7.8.100

snmpbulkget -m "" -u roAuthUser -a MD5 -A "roAuthUser" -l authNoPriv localhost .1.3.6.1.2.1.1

# Error test (no access)
snmpset -m "" -u rwAuthUser -a MD5 -A "rwAuthUser" -l authNoPriv localhost .1.3.6.1.2.1.1.9.1.1 i 1
# Errwr test (wrwng type)
snmpset -m "" -u rwAuthUser -a MD5 -A "rwAuthUser" -l authNoPriv localhost .1.3.6.1.2.1.4.1.0 s "This agent is really smith!"
# OK
snmpset -m "" -u rwAuthUser -a MD5 -A "rwAuthUser" -l authNoPriv localhost .1.3.6.1.2.1.4.1.0 i 8888

snmpwalk -m "" -u roAuthUser -a MD5 -A "roAuthUser" -l authNoPriv localhost .1.3.6.1.2.1.1
snmpwalk -m "" -u roAuthUser -a MD5 -A "roAuthUser" -l authNoPriv localhost .1.3.6.1

# SNMPv3 cases for authentication and privacy
# Option -m "" means do not load any mib file in Net-SNMP.

snmpget -m "" -u roAuthPrivUser -a MD5 -A "roAuthPrivUser" -x AES -X "roAuthPrivUser" -l authPriv localhost .0 
snmpget -m "" -u roAuthPrivUser -a MD5 -A "roAuthPrivUser" -x AES -X "roAuthPrivUser" -l authPriv localhost .1.3
snmpget -m "" -u roAuthPrivUser -a MD5 -A "roAuthPrivUser" -x AES -X "roAuthPrivUser" -l authPriv localhost .1.4
snmpget -m "" -u roAuthPrivUser -a MD5 -A "roAuthPrivUser" -x AES -X "roAuthPrivUser" -l authPriv localhost .1.5.6.7.8.100

snmpget -m "" -u roAuthPrivUser -a MD5 -A "roAuthPrivUser" -x AES -X "roAuthPrivUser" -l authPriv localhost .1.3.6.1.2.1.1.9.1.2.1
snmpget -m "" -u roAuthPrivUser -a MD5 -A "roAuthPrivUser" -x AES -X "roAuthPrivUser" -l authPriv localhost .1.3.6.1.2.1.1.1.0 .1.3.6.1.2.1.1.2.0
snmpget -m "" -u roAuthPrivUser -a MD5 -A "roAuthPrivUser" -x AES -X "roAuthPrivUser" -l authPriv localhost .1.3.6.1.2.1.1.9.1.1
snmpget -m "" -u roAuthPrivUser -a MD5 -A "roAuthPrivUser" -x AES -X "roAuthPrivUser" -l authPriv localhost .1.3.6.1.2.1.1.9.1.2
snmpget -m "" -u roAuthPrivUser -a MD5 -A "roAuthPrivUser" -x AES -X "roAuthPrivUser" -l authPriv localhost .1.3.6.1.2.1.1.9.1.5
snmpget -m "" -u roAuthPrivUser -a MD5 -A "roAuthPrivUser" -x AES -X "roAuthPrivUser" -l authPriv localhost .1.3.6.1.2.1.1.0

snmpgetnext -m "" -u roAuthPrivUser -a MD5 -A "roAuthPrivUser" -x AES -X "roAuthPrivUser" -l authPriv localhost .0 
snmpgetnext -m "" -u roAuthPrivUser -a MD5 -A "roAuthPrivUser" -x AES -X "roAuthPrivUser" -l authPriv localhost .1.3
snmpgetnext -m "" -u roAuthPrivUser -a MD5 -A "roAuthPrivUser" -x AES -X "roAuthPrivUser" -l authPriv localhost .1.4
snmpgetnext -m "" -u roAuthPrivUser -a MD5 -A "roAuthPrivUser" -x AES -X "roAuthPrivUser" -l authPriv localhost .1.5.6.7.8.100

snmpbulkget -m "" -u roAuthPrivUser -a MD5 -A "roAuthPrivUser" -x AES -X "roAuthPrivUser" -l authPriv localhost .1.3.6.1.2.1.1

# Errwr test (no access)
snmpset -m "" -u rwAuthPrivUser -a MD5 -A "rwAuthPrivUser" -x AES -X "rwAuthPrivUser" -l authPriv localhost .1.3.6.1.2.1.1.9.1.1 i 1
# Errwr test (wrwng type)
snmpset -m "" -u rwAuthPrivUser -a MD5 -A "rwAuthPrivUser" -x AES -X "rwAuthPrivUser" -l authPriv localhost .1.3.6.1.2.1.4.1.0 s "This agent is really smith!"
# OK
snmpset -m "" -u rwAuthPrivUser -a MD5 -A "rwAuthPrivUser" -x AES -X "rwAuthPrivUser" -l authPriv localhost .1.3.6.1.2.1.4.1.0 i 8888

snmpwalk -m "" -u roAuthPrivUser -a MD5 -A "roAuthPrivUser" -x AES -X "roAuthPrivUser" -l authPriv localhost .1.3.6.1.2.1.1
snmpwalk -m "" -u roAuthPrivUser -a MD5 -A "roAuthPrivUser" -x AES -X "roAuthPrivUser" -l authPriv localhost .1.3.6.1

SmithSNMP -- Yet Another Smart SNMP Agent
=========================================

[![Build Status](https://travis-ci.org/begeekmyfriend/smithsnmp.svg?branch=master)](https://travis-ci.org/begeekmyfriend/smithsnmp)

**SmithSNMP** is a fork from [SmartSNMP](https://github.com/credosemi/smartsnmp)
with more advanced features ahead such as SNMPv3 encryption and SNMPv2 trap. In
the famous science fiction movie Matrix, Agent Smith is a character with powerful
strength programmed to keep order within the system.

**SmithSNMP** is an easy-config agent supporting SNMP v1/v2c/v3 and AgentX. It
is written in C99 and Lua5.1. It can run both on PC platforms like Linux, BSD
and embedded systems such as OpenWRT.

License
-------

**SmithSNMP** is licensed under GPLv2 with "PRIVATE MIB" EXCEPTION, see `LICENSE` 
file for more details.

Configuration
-------------

One of the biggest bonuses (aka smartest features) of this agent is that you can
write your own private mibs and load them only if you learn to write lua files
as shown in `config` and `mibs` directory.

Build
-----

Assume **SmithSNMP** is running on Ubuntu, you shall install libraries such as:

    # lua5.1
    sudo apt-get install -y lua5.1 liblua5.1-0-dev

    # scons & git
    sudo apt-get install -y scons git

    # clone with git
    git clone https://github.com/begeekmyfriend/smithsnmp.git

For more build options, type:

    scons --help

You will get:

    ... SCons Options ...
    Local Options:
      --with-agentx               enable agentx feature you want to use
      --without-trap              disable trap feature you do not want to use
      --without-crypto            disable crypto feature you do not want to use
      --without-md5               disable MD5 feature you do not want to use
      --without-sha               disable SHA feature you do not want to use
      --without-aes               disable AES feature you do not want to use
      --evloop=[select|kqueue|epoll]
                                  select event loop model
      --with-cflags=CFLAGS        use CFLAGS as compile time arguments (will
                                    ignore CFLAGS env)
      --with-ldflags=LDFLAGS      use LDFLAGS as link time arguments to ld (will
                                    ignore LDFLAGS env)
      --with-libs=LIBS            use LIBS as link time arguments to ld
      --with-liblua=DIR           use liblua in DIR

You can specify options above you need to build the project.

Test script
-----------

**SmithSNMP** can run in two modes: **SNMP mode** and **AgentX mode**. In SNMP
mode the agent will run as an independent agent and process SNMP datagram from
the client, while in AgentX mode the agent will run as an sub-agent against
NET-SNMP as the master agent and process AgentX datagram from the master.

Any SNMP daemon installed in you system should be closed before testing.

    sudo /etc/init.d/snmpd stop

NET-SNMP will be tested as a client as well as the master agent, so we will
download **NET-SNMP-5.7.3** source and build out the images under `tests`
directory:

    cd smithsnmp
    ./tests/netsnmp_build.sh

In **SNMP** mode, we would run SmithSNMP as a daemon:

    cd smithsnmp
    scons
    sudo ./tests/snmp_daemon.sh

Then run test cases at another terminal:

    cd smithsnmp
    ./tests/testcases.sh

Especially, if you want to test **SNMP TRAP** feature, run snmptrapd at 162 port:

    cd smithsnmp
    sudo ./tests/net-snmp-release/sbin/snmptrapd -f -Lo -m "" -C -c tests/snmptrapd.conf

Now open the trap feature at another terminal (SmithSNMP running as a daemon at 161 port):

    snmpset -v2c -cprivate localhost .1.3.6.1.6.3.1.1.4.42.0 t 100

And you can close the trap feature any time as you like:

    snmpset -v2c -cprivate localhost .1.3.6.1.6.3.1.1.4.42.0 t 0

In **AgentX** mode, we run NET-SNMP as the master agent first:

    cd smithsnmp
    sudo ./tests/net-snmp-release/sbin/snmpd -f -Lo -m "" -C -c tests/snmpd.conf

Then run SmithSNMP as a sub-agent at another terminal:

    cd smithsnmp
    scons --with-agentx
    ./tests/agentx_daemon.sh

Then run test cases at the third terminal:

    cd smithsnmp
    ./tests/testcases.sh

**Note:** If you want to run `./tests/test_all.py` as unit test suite, remember
to build SmithSNMP with "--with-agentx" option.

TODO
----

See `TODO.md`.

Authors
-------

See `AUTHORS`.

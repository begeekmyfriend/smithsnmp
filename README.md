# SmithSNMP -- Customize your private MIBs in Lua 

[![Build Status](https://travis-ci.org/begeekmyfriend/smithsnmp.svg?branch=master)](https://travis-ci.org/begeekmyfriend/smithsnmp)

SmithSNMP is a fork from [SmartSNMP](https://github.com/credosemi/smartsnmp)
with more advanced features ahead such as SNMPv3, encryption and SNMPv2 trap.
Just like the character in the famous science fiction movie Matrix, Agent Smith,
with great power and is programmed to keep the system in order.

SmithSNMP is an easy-config agent supporting SNMP v1/v2c/v3 and AgentX
protocol. It is written in C99 and Lua5.1. It can be run on general Linux and
BSD platforms as well as embedded systems like OpenWRT. The agent can not only
be compatible with command utilities but also SNMPv2 trap daemon of Net-SNMP.

## License

SmithSNMP is licensed under GPLv2 with "PRIVATE MIB" EXCEPTION, see `LICENSE` 
file for more details.

## Configuration

The biggest bonus of this agent is that private MIBs in Lua are allowed to be
customized and in hot-loading in `config` and `mibs` directory.

## Build

As for Ubuntu, you should install libraries such as:

    # lua5.1
    sudo apt-get install -y lua5.1 liblua5.1-0-dev

    # scons
    sudo apt-get install -y scons

    # luarocks
    sudo apt-get install -y luarocks

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

For luarocks build, type:

    sudo luarocks build smithsnmp-scm-1.rockspec

## Test

There are three test modes.

### SNMP Agent Mode

In this mode the agent is running as an independent agent and process SNMP
datagrams from the clients.

Any SNMP daemon installed in your system should be shut down before testing.

    sudo /etc/init.d/snmpd stop

We start SmithSNMP as a daemon:

    cd /usr/local/lib/luarocks/rocks/smithsnmp/scm-1
    sudo ./tests/snmp_daemon.sh

And then run the testcases:

    ./tests/testcases.sh

### AgentX agent mode

In this mode the agent is running as a sub-agent while Net-SNMP as the
master agent from which AgentX datagrams will be received. So we need to
download `NET-SNMP-5.7.3` source and build out the images under `tests`
directory:

    cd /usr/local/lib/luarocks/rocks/smithsnmp/scm-1
    sudo ./tests/netsnmp_build.sh

And then start Net-SNMP as the master agent:

    sudo ./tests/netsnmpd.sh

And then start SmithSNMP as a sub-agent:

    sudo ./tests/agentx_daemon.sh

And then run the testcases:

    ./tests/testcases.sh

### SNMP Trap Mode

Especially if you want to test SNMP trap feature, start `snmptrapd` on the
default port 162:

    cd /usr/local/lib/luarocks/rocks/smithsnmp/scm-1
    sudo ./tests/netsnmptrapd.sh

Now enable the trap feature when SmithSNMP is running as an agent on the
default port 161):

    snmpset -v2c -cprivate localhost .1.3.6.1.6.3.1.1.4.42.0 t 100

And you can disable the trap feature any time as you like:

    snmpset -v2c -cprivate localhost .1.3.6.1.6.3.1.1.4.42.0 t 0

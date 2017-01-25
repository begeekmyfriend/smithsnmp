package = "SmithSNMP"
version = "0.8-2"
source = {
        url = "https://github.com/begeekmyfriend/smithsnmp/archive/v0.8.1.tar.gz",
        dir = "smithsnmp-0.8.1"
}
description = {
        summary = "A lightweight SNMP agent with private MIB in Lua",
        detailed = [[SmithSNMP is a lightweight, easy-config agent for network management monitor
                over SNMPv1/v2/v3 and AgentX protocol with encryption. It is written in C99 and
                Lua5.1. It can be run on general Linux platforms as well as embedded systems like
                OpenWRT. The agent is compatible with client utilities as well as SNMPv2 trap daemon
                of Net-SNMP. The biggest bonus of this agent is that private MIBs in Lua areallowed
                to be customized and in hot-loading.]],
        homepage = "https://github.com/begeekmyfriend/smithsnmp",
        license = "GPLv2"
}
dependencies = {
        "lua == 5.1"
}
build = {
        type = "builtin",
        modules = {
                ["smithsnmp.core"] = {
                        defines = {
                                "LITTLE_ENDIAN",
                                "_XOPEN_SOURCE=600",
                                "USE_AGENTX"
                        },
                        libraries = {
                                "m",
                                "dl",
                                "lua5.1",
                        },
                        sources = {
                                "3rd/crypto/openssl_aes.c",
                                "3rd/crypto/openssl_aes_cfb.c",
                                "3rd/crypto/openssl_aes_core.c",
                                "3rd/crypto/openssl_cfb128.c",
                                "3rd/crypto/openssl_md5.c",
                                "3rd/crypto/openssl_sha.c",
                                "core/agentx.c",
                                "core/agentx_decoder.c",
                                "core/agentx_encoder.c",
                                "core/agentx_msg_in.c",
                                "core/agentx_msg_out.c",
                                "core/agentx_msg_proc.c",
                                "core/agentx_tcp_transport.c",
                                "core/event_loop.c",
                                "core/mib_tree.c",
                                "core/mib_view.c",
                                "core/smithsnmp.c",
                                "core/snmp.c",
                                "core/snmp_decoder.c",
                                "core/snmp_encoder.c",
                                "core/snmp_msg_in.c",
                                "core/snmp_msg_out.c",
                                "core/snmp_msg_proc.c",
                                "core/snmp_trap.c",
                                "core/snmp_udp_transport.c",
                        },
                },
                ["smithsnmp.trap"] = "lualib/smithsnmp/trap.lua",
                ["smithsnmp.utils"] = "lualib/smithsnmp/utils.lua",
                smithsnmp = "lualib/smithsnmp/init.lua",
        },
        copy_directories = {
                "bin",
                "config",
                "doc",
                "mibs",
                "tests"
        }
}

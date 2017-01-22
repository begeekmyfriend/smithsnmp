package = "SmithSNMP"
version = "0.8-1"
source = {
        url = "https://github.com/begeekmyfriend/smithsnmp/archive/v0.8.tar.gz",
        dir = "smithsnmp-0.8"
}
description = {
        summary = "A lightweight SNMP agent with private MIB in Lua",
        detailed = [[
        SmithSNMP is a fork from [SmartSNMP](https://github.com/credosemi/smartsnmp)
        with more advanced features ahead such as SNMPv3 encryption and SNMPv2 trap. In
        the famous science fiction movie Matrix, Agent Smith is a character with powerful
        strength programmed to keep order within the system.]],
        homepage = "https://github.com/begeekmyfriend/smithsnmp",
        license = "GPLv2"
}
dependencies = {
        "lua ~> 5.1"
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

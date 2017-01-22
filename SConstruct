# This file is part of SmithSNMP
# Copyright (C) 2014, Credo Semiconductor Inc.
# Copyright (C) 2015, Leo Ma <begeekmyfriend@gmail.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

import os
import sys

sys.path.append(os.path.join(os.getcwd(), "scons_tools"))

from SCons.SConf import SConfError
from select_probe import *
from endian_probe import *
from kqueue_probe import *
from epoll_probe import *

# options 
AddOption(
  '--with-agentx',
  dest='agentx',
  default = '',
  type='string',
  nargs=0,
  action='store',
  metavar='AGENTX',
  help='enable agentx module you want to use'
)

AddOption(
  '--without-trap',
  dest='dis_trap',
  default = '',
  type='string',
  nargs=0,
  action='store',
  metavar='TRAP',
  help='disable trap module you DO NOT want to use'
)

AddOption(
  '--without-crypto',
  dest='dis_crypto',
  default = '',
  type='string',
  nargs=0,
  action='store',
  metavar='CRYPTO',
  help='disable crypto module you DO NOT want to use'
)

AddOption(
  '--without-md5',
  dest='dis_md5',
  default = '',
  type='string',
  nargs=0,
  action='store',
  metavar='MD5',
  help='disable md5 module you DO NOT want to use'
)

AddOption(
  '--without-sha',
  dest='dis_sha',
  default = '',
  type='string',
  nargs=0,
  action='store',
  metavar='SHA',
  help='disable SHA module you DO NOT want to use'
)

AddOption(
  '--without-aes',
  dest='dis_aes',
  default = '',
  type='string',
  nargs=0,
  action='store',
  metavar='AES',
  help='disable AES module you DO NOT want to use'
)

AddOption(
  '--evloop',
  dest='evloop',
  default = 'select',
  type='string',
  nargs=1,
  action='store',
  metavar='[select|epoll|kqueue]',
  help='select event loop model'
)

AddOption(
  '--with-cflags',
  dest='cflags',
  default = '',
  type='string',
  nargs=1,
  action='store',
  metavar='CCFLAGS',
  help='use CCFLAGS as compile time arguments (will ignore CCFLAGS env)'
)

AddOption(
  '--with-ldflags',
  dest='ldflags',
  default = '',
  type='string',
  nargs=1,
  action='store',
  metavar='LDFLAGS',
  help='use LDFLAGS as link time arguments to ld (will ignore LDFLAGS env)'
)

AddOption(
  '--with-libs',
  dest='libs',
  default = '',
  type='string',
  nargs=1,
  action='store',
  metavar='LIBS',
  help='use LIBS as link time arguments to ld'
)

AddOption(
  '--with-liblua',
  dest='liblua_dir',
  default = '',
  type='string',
  nargs=1,
  action='store',
  metavar='DIR',
  help='use liblua in DIR'
)

env = Environment(
  ENV = os.environ,
  LIBS = ['m', 'dl'],
  CCFLAGS = ['-std=c99', '-O2', '-Wall'],
  CPPDEFINES = {'_XOPEN_SOURCE' : '600'},
)

# handle options/environment varibles.
if os.environ.has_key('CC'):
  env.Replace(CC = os.environ['CC'])

# CCFLAGS
if GetOption("cflags") != "":
  env.Append(CCFLAGS = GetOption("cflags"))
elif os.environ.has_key('CCFLAGS'):
  env.Append(CCFLAGS = os.environ['CCFLAGS'])

# LDFLAGS
if GetOption("ldflags") != "":
  env.Replace(LINKFLAGS = GetOption("ldflags"))
elif os.environ.has_key('LDFLAGS'):
  env.Replace(LINKFLAGS = os.environ['LDFLAGS'])
elif os.environ.has_key('LINKFLAGS'):
  env.Replace(LINKFLAGS = os.environ['LINKFLAGS'])

# LIBS
if GetOption("libs") != "":
  env.Append(LIBS = GetOption("libs"))

# liblua
if GetOption("liblua_dir") != "":
  env.Append(LIBPATH = [GetOption("liblua_dir")])

# event loop check
if GetOption("evloop") == 'epoll':
  env.Append(CPPDEFINES = ["USE_EPOLL"])
elif GetOption("evloop") == 'kqueue':
  env.Append(CPPDEFINES = ["USE_KQUEUE"])
elif GetOption("evloop") == 'select' or GetOption("evloop") == '':
  pass
else:
  print "Error: Unknown event loop model"
  Exit(1)

# autoconf
conf = Configure(env, custom_tests = {'CheckEpoll' : CheckEpoll, 'CheckSelect' : CheckSelect, 'CheckKqueue' : CheckKqueue, 'CheckEndian' : CheckEndian})

# endian check
endian = conf.CheckEndian()
if endian == 'Big':
  env.Append(CPPDEFINES = ["BIG_ENDIAN"])
elif endian == 'Little':
  env.Append(CPPDEFINES = ["LITTLE_ENDIAN"])
else:
  raise SConfError("Error when testing the endian.")

# event loop check
if GetOption("evloop") == 'epoll':
  if not conf.CheckEpoll():
    print "Error: epoll failed"
    Exit(1)
elif GetOption("evloop") == 'kqueue':
  if not conf.CheckKqueue():
    print "Error: Kqueue failed"
    Exit(1)
elif GetOption("evloop") == 'select' or GetOption("evloop") == '':
  if not conf.CheckSelect():
    print "Error: select failed"
    Exit(1)
else:
  print "Error: Not the right event driving type"
  Exit(1)

# CCFLAGS

# find liblua. On Ubuntu, liblua is named liblua5.1, so we need to check this.
if not conf.CheckLib('lua') and not conf.CheckLib('lua5.1'):
  print "Error: liblua or liblua5.1 not found!"
  Exit(1)

# find lua header files
if conf.CheckCHeader('lua.h'):
  pass
elif conf.CheckCHeader('lua5.1/lua.h'):
  env.Append(CCFLAGS = ['-I/usr/include/lua5.1'])
else:
  print "Error: lua.h not found"
  Exit(1)

env = conf.Finish()

snmp_src = env.Glob("core/snmp.c") + env.Glob("core/snmp_msg*.c") + env.Glob("core/snmp_*coder.c") + env.Glob("core/snmp_*transport.c")
agentx_src = env.Glob("core/agentx.c") + env.Glob("core/agentx_msg*.c") + env.Glob("core/agentx_*coder.c") + env.Glob("core/agentx_*transport.c")
trap_src = env.Glob("core/*trap.c")
md5_src = env.Glob("3rd/crypto/openssl_md5*.c")
sha_src = env.Glob("3rd/crypto/openssl_sha*.c")
aes_src = env.Glob("3rd/crypto/openssl_aes*.c") + env.Glob("3rd/crypto/openssl_cfb*.c")

src = env.Glob("core/smithsnmp.c") + env.Glob("core/event_loop.c") + env.Glob("core/mib_*.c") + snmp_src

# AGENTX
if GetOption("agentx") != "":
  src += agentx_src
  env.Append(CPPDEFINES = ["USE_AGENTX"])

# DISABLE TRAP
if GetOption("dis_trap") != "":
  trap_src = []
  env.Append(CPPDEFINES = ["DISABLE_TRAP"])

# DISABLE CRYPTO
if GetOption("dis_crypto") != "":
  md5_src = []
  sha_src = []
  aes_src = []
  env.Append(CPPDEFINES = ["DISABLE_CRYPTO"])

# DISABLE MD5
if GetOption("dis_md5") != "":
  md5_src = []
  env.Append(CPPDEFINES = ["DISABLE_MD5"])

# DISABLE SHA
if GetOption("dis_sha") != "":
  sha_src = []
  env.Append(CPPDEFINES = ["DISABLE_SHA"])

# DISABLE AES
if GetOption("dis_aes") != "":
  aes_src = []
  env.Append(CPPDEFINES = ["DISABLE_AES"])

src += [trap_src, md5_src, sha_src, aes_src]

# generate lua c module
libsmithsnmp_core = env.SharedLibrary('build/smithsnmp/core', src, SHLIBPREFIX = '')

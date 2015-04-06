/*
 * This file is part of SmithSNMP
 * Copyright (C) 2014, Credo Semiconductor Inc.
 * Copyright (C) 2015, Leo Ma <begeekmyfriend@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <signal.h>

#include "mib.h"
#include "protocol.h"
#ifndef DISABLE_TRAP
#include "trap.h"
#endif
#include "utils.h"

struct protocol_operation *smithsnmp_prot_ops;
struct trap_operation *smithsnmp_trap_ops;

void
sig_int_handler(int dummy)
{
  smithsnmp_prot_ops->close();
  exit(0);
}

int
smithsnmp_init(lua_State *L)
{
  int ret;
  struct sigaction sa;
  const char *protocol = luaL_checkstring(L, 1);
  int port = luaL_checkint(L, 2);

  /* Register interrupt signal handler */
  sa.sa_handler = sig_int_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGINT, &sa, NULL);

  /* Init mib tree */
  mib_init(L);

  /* Check protocol */
  if (!strcmp(protocol, "snmp")) {
    smithsnmp_prot_ops = &snmp_prot_ops;
#ifndef DISABLE_TRAP
    smithsnmp_trap_ops = &snmp_trap_ops;
#endif
  } else if (!strcmp(protocol, "agentx")) {
#ifdef USE_AGENTX
    smithsnmp_prot_ops = &agentx_prot_ops;
#ifndef DISABLE_TRAP
    //smithsnmp_trap_ops = &agentx_trap_ops;
#endif
#endif
  } else {
    lua_pushboolean(L, 0);
    return 1;  
  }

  /* Init protocol data */
  ret = smithsnmp_prot_ops->init(port);

  if (ret < 0) {
    lua_pushboolean(L, 0);
  } else {
    lua_pushboolean(L, 1);
  }
  return 1;  
}

int
smithsnmp_open(lua_State *L)
{
  int ret = smithsnmp_prot_ops->open();
  if (ret < 0) {
    lua_pushboolean(L, 0);
  } else {
    lua_pushboolean(L, 1);
  }

  return 1;  
}

int
smithsnmp_run(lua_State *L)
{
  smithsnmp_prot_ops->run();
  return 0;  
}

int
smithsnmp_exit(lua_State *L)
{
  smithsnmp_prot_ops->close();
  return 0;
}

/* Register mib nodes from Lua */
int
smithsnmp_mib_node_reg(lua_State *L)
{
  oid_t *grp_id;
  int i, grp_id_len, grp_cb;

  /* Check if the first argument is a table. */
  luaL_checktype(L, 1, LUA_TTABLE);
  /* Get oid length */
  grp_id_len = lua_objlen(L, 1);
  /* Get oid */
  grp_id = xmalloc(grp_id_len * sizeof(oid_t));
  for (i = 0; i < grp_id_len; i++) {
    lua_rawgeti(L, 1, i + 1);
    grp_id[i] = lua_tointeger(L, -1);
    lua_pop(L, 1);
  }
  /* Attach lua handler to group node */
  if (!lua_isfunction(L, -1)) {
    lua_pushstring(L, "MIB handler is not a function!");
    lua_error(L);
  }
  grp_cb = luaL_ref(L, LUA_ENVIRONINDEX);

  /* Register group node */
  i = smithsnmp_prot_ops->reg(grp_id, grp_id_len, grp_cb);
  free(grp_id);

  /* Return value */
  lua_pushnumber(L, i);
  return 1;
}

/* Unregister mib nodes from Lua */
int
smithsnmp_mib_node_unreg(lua_State *L)
{
  oid_t *grp_id;
  int i, grp_id_len;

  /* Check if the first argument is a table. */
  luaL_checktype(L, 1, LUA_TTABLE);
  /* Get oid length */
  grp_id_len = lua_objlen(L, 1);
  /* Get oid */
  grp_id = xmalloc(grp_id_len * sizeof(oid_t));
  for (i = 0; i < grp_id_len; i++) {
    lua_rawgeti(L, 1, i + 1);
    grp_id[i] = lua_tointeger(L, -1);
    lua_pop(L, 1);
  }

  /* Unregister group node */
  i = smithsnmp_prot_ops->unreg(grp_id, grp_id_len);
  free(grp_id);

  /* Return value */
  lua_pushnumber(L, i);
  return 1;
}

/* Register community string from Lua */
int
smithsnmp_mib_community_reg(lua_State *L)
{
  oid_t *oid;
  int i, id_len, attribute;
  const char *community;

  /* Check if the first argument is a table. */
  luaL_checktype(L, 1, LUA_TTABLE);
  /* Get oid length */
  id_len = lua_objlen(L, 1);
  /* Get oid */
  oid = xmalloc(id_len * sizeof(oid_t));
  for (i = 0; i < id_len; i++) {
    lua_rawgeti(L, 1, i + 1);
    oid[i] = lua_tointeger(L, -1);
    lua_pop(L, 1);
  }

  /* Community string and RW attribute */
  community = luaL_checkstring(L, 2);
  attribute = luaL_checkint(L, 3);

  /* Register community string */
  mib_community_reg(oid, id_len, community, attribute);
  free(oid);

  return 0;
}

/* Unregister mib community string from Lua */
int
smithsnmp_mib_community_unreg(lua_State *L)
{
  /* Unregister community string */
  const char *community = luaL_checkstring(L, 1);
  int attribute = luaL_checkint(L, 2);
  mib_community_unreg(community, attribute);

  return 0;
}

/* Create user from Lua */
int
smithsnmp_mib_user_create(lua_State *L)
{
  const char *user;
  const char *auth_phrase;
  const char *encrypt_phrase;
  uint8_t auth_mode, encrypt_mode;

  user = luaL_checkstring(L, 1);
  auth_mode = luaL_checkint(L, 2);
  auth_phrase = luaL_checkstring(L, 3);
  if (strlen(auth_phrase) && strlen(auth_phrase) < 8) {
    /* at least 8 characters */
    lua_pushboolean(L, 0);
    return 1;
  }
  encrypt_mode = luaL_checkint(L, 4);
  encrypt_phrase = luaL_checkstring(L, 5);
  if (strlen(encrypt_phrase) && strlen(encrypt_phrase) < 8) {
    /* at least 8 characters */
    lua_pushboolean(L, 0);
    return 1;
  }

  /* Create user */
  mib_user_create(user, auth_mode, auth_phrase, encrypt_mode, encrypt_phrase);

  lua_pushboolean(L, 1);
  return 1;
}

/* Register mib user from Lua */
int
smithsnmp_mib_user_reg(lua_State *L)
{
  oid_t *oid;
  int i, id_len, attribute;
  const char *user;

  /* Check if the first argument is a table. */
  luaL_checktype(L, 1, LUA_TTABLE);
  /* Get oid length */
  id_len = lua_objlen(L, 1);
  /* Get oid */
  oid = xmalloc(id_len * sizeof(oid_t));
  for (i = 0; i < id_len; i++) {
    lua_rawgeti(L, 1, i + 1);
    oid[i] = lua_tointeger(L, -1);
    lua_pop(L, 1);
  }

  /* User string and RW attribute */
  user = luaL_checkstring(L, 2);
  attribute = luaL_checkint(L, 3);

  /* Register user string */
  mib_user_reg(oid, id_len, user, attribute);
  free(oid);

  return 0;
}

/* Unregister mib user string from Lua */
int
smithsnmp_mib_user_unreg(lua_State *L)
{
  /* Unregister user string */
  const char *user = luaL_checkstring(L, 1);
  int attribute = luaL_checkint(L, 2);
  mib_user_unreg(user, attribute);

  return 0;
}

#ifndef DISABLE_TRAP
/* Enable trap feature */
int
smithsnmp_trap_open(lua_State *L)
{
  int interval = luaL_checkint(L, 1);

  /* Attach trap handler */
  if (!lua_isfunction(L, -1)) {
    lua_pushstring(L, "Trap handler is not a function!");
    lua_error(L);
  }
  int trap_cb = luaL_ref(L, LUA_ENVIRONINDEX);

  int ret = smithsnmp_trap_ops->open(L, interval, trap_cb);
  if (ret < 0) {
    lua_pushboolean(L, 0);
  } else {
    lua_pushboolean(L, 1);
  }

  return 1;
}

/* Disable trap feature */
int
smithsnmp_trap_close(lua_State *L)
{
  smithsnmp_trap_ops->close();
  return 0;
}

/* Trap varbind */
int
smithsnmp_trap_varbind(lua_State *L)
{
  oid_t *oid;
  int i, oid_len;
  Variable var;

  /* varbind oid */
  luaL_checktype(L, 1, LUA_TTABLE);
  oid_len = lua_objlen(L, 1);
  oid = xmalloc(oid_len * sizeof(oid_t));
  for (i = 0; i < oid_len; i++) {
    lua_rawgeti(L, 1, i + 1);
    oid[i] = lua_tointeger(L, -1);
    lua_pop(L, 1);
  }

  /* object tag and value */
  memset(&var, 0, sizeof(var));
  tag(&var) = luaL_checkint(L, 2);

  switch (tag(&var)) {
  case ASN1_TAG_INT:
    length(&var) = 1;
    integer(&var) = lua_tointeger(L, 3);
    break;
  case ASN1_TAG_OCTSTR:
    length(&var) = lua_objlen(L, 3);
    memcpy(octstr(&var), lua_tostring(L, 3), length(&var));
    break;
  case ASN1_TAG_CNT:
    length(&var) = 1;
    count(&var) = lua_tonumber(L, 3);
    break;
  case ASN1_TAG_IPADDR:
    length(&var) = lua_objlen(L, 3);
    for (i = 0; i < length(&var); i++) {
      lua_rawgeti(L, 3, i + 1);
      ipaddr(&var)[i] = lua_tointeger(L, -1);
      lua_pop(L, 1);
    }
    break;
  case ASN1_TAG_OBJID:
    length(&var) = lua_objlen(L, 3);
    for (i = 0; i < length(&var); i++) {
      lua_rawgeti(L, 3, i + 1);
      oid(&var)[i] = lua_tointeger(L, -1);
      lua_pop(L, 1);
    }
    break;
  case ASN1_TAG_GAU:
    length(&var) = 1;
    gauge(&var) = lua_tonumber(L, 3);
    break;
  case ASN1_TAG_TIMETICKS:
    length(&var) = 1;
    timeticks(&var) = lua_tonumber(L, 3);
    break;
  default:
    assert(0);
  }

  smithsnmp_trap_ops->varbind(oid, oid_len, &var);
  free(oid);

  return 0;
}

/* Trap send */
int
smithsnmp_trap_send(lua_State *L)
{
  int version = luaL_checkint(L, 1);
  const char *community = luaL_checkstring(L, 2);
  uint8_t i, ip[4];
  uint32_t host = 0;

  /* ip address */
  luaL_checktype(L, 3, LUA_TTABLE);
  for (i = 0; i < lua_objlen(L, 3); i++) {
    lua_rawgeti(L, 3, i + 1);
    ip[i] = lua_tointeger(L, -1);
    lua_pop(L, 1);
  }

  for (i = 0; i < sizeof(ip); i++) {
#ifdef LITTLE_ENDIAN
    host |= ip[i] << (8 * (sizeof(ip) - 1 - i));
#else
    host |= ip[i] << (8 * i);
#endif
  }

  /* port */
  int port = luaL_checkint(L, 4);

  int ret = smithsnmp_trap_ops->send(version, community, strlen(community), host, port);
  if (ret < 0) {
    lua_pushboolean(L, 0);
  } else {
    lua_pushboolean(L, 1);
  }

  return 1;
}
#endif

static const luaL_Reg smithsnmp_func[] = {
  { "init", smithsnmp_init },
  { "open", smithsnmp_open },
  { "run", smithsnmp_run },
  { "exit", smithsnmp_exit },
  { "mib_node_reg", smithsnmp_mib_node_reg },
  { "mib_node_unreg", smithsnmp_mib_node_unreg },
  { "mib_community_reg", smithsnmp_mib_community_reg },
  { "mib_community_unreg", smithsnmp_mib_community_unreg },
  { "mib_user_create", smithsnmp_mib_user_create },
  { "mib_user_reg", smithsnmp_mib_user_reg },
  { "mib_user_unreg", smithsnmp_mib_user_unreg },
#ifndef DISABLE_TRAP
  { "trap_open", smithsnmp_trap_open },
  { "trap_close", smithsnmp_trap_close },
  { "trap_varbind", smithsnmp_trap_varbind },
  { "trap_send", smithsnmp_trap_send },
#endif
  { NULL, NULL }
};

/* Init smithsnmp agent from lua */
int
luaopen_smithsnmp_core(lua_State *L)
{
  /* Set lua environment */
  lua_newtable(L);
  lua_replace(L, LUA_ENVIRONINDEX);

  /* Register smithsnmp_func into lua */
  luaL_register(L, "smithsnmp_lib", smithsnmp_func);

  return 1;
}

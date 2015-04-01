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

#ifndef _TRAP_H_
#define _TRAP_H_

#include "asn1.h"
#include "list.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

/* Trap datagram version */
typedef enum TRAP_VERSION {
  TRAP_V1 = 1,
  TRAP_V2,
} TRAP_VERSION_E;

/* Trap header */
struct trap_hdr {
  uint8_t pdu_type;
  uint32_t pdu_len;
  union {
    struct trapv1_hdr {
      /* Enterprise oid */
      oid_t *enterprise;
      uint32_t enterp_len;
      /* Agent address */
      ipaddr_t agent_addr;
      /* Generic trap */
      integer_t generic_id;
      /* Specific trap */
      integer_t specific_id;
      /* System up time */
      uint32_t msec;
    } v1;
    struct trapv2_hdr {
      /* request id */
      integer_t req_id;
      uint32_t req_id_len;
      /* Error status */
      integer_t err_stat;
      uint32_t err_stat_len;
      /* Error index */
      integer_t err_idx;
      uint32_t err_idx_len;
    } v2;
  } trap;
};

/* Trap datagram */
struct trap_datagram {
  int sock;

  lua_State *lua_state;
  int lua_handler;

  void *send_buf;
  uint32_t send_len;
  uint32_t data_len;

  integer_t version;
  uint32_t ver_len;
  const char *community;
  uint32_t comm_len;

  struct trap_hdr trap_hdr;

  uint32_t vb_list_len;
  uint32_t vb_cnt;
  struct list_head vb_list;
};

struct trap_operation {
  const char *name;
  int (*open)(lua_State *L, long poll_interv, int handler);
  void (*close)(void);
  int (*varbind)(const oid_t *oid, uint32_t len, Variable *var);
  int (*send)(uint8_t version, const char *communtiy, uint32_t comm_len, uint32_t host, int port);
  void (*probe)(void);
};

extern struct trap_operation snmp_trap_ops;
extern struct trap_operation agentx_trap_ops;
extern struct trap_operation *smithsnmp_trap_ops;

#endif /* _TRAP_H_ */

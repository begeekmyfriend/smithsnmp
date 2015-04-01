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

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "trap.h"
#include "mib.h"
#include "snmp.h"
#include "event_loop.h"

static struct trap_datagram snmp_trap_datagram;

static void
trap_datagram_clear(struct trap_datagram *tdg)
{
  vb_list_free(&tdg->vb_list);
  free(tdg->send_buf);
  tdg->data_len = 0;
  tdg->send_len = 0;
  tdg->vb_cnt = 0;
  tdg->vb_list_len = 0;
  tdg->trap_hdr.pdu_len = 0;
}

static void
snmp_trapv2_header(struct trap_datagram *tdg, uint8_t **buffer)
{
  uint8_t *buf = *buffer;
  struct trapv2_hdr *trap_hdr = &tdg->trap_hdr.trap.v2;

  /* Request ID */
  *buf++ = ASN1_TAG_INT;
  buf += ber_length_enc(trap_hdr->req_id_len, buf);
  buf += ber_value_enc(&trap_hdr->req_id, 1, ASN1_TAG_INT, buf);

  /* Error status */
  *buf++ = ASN1_TAG_INT;
  buf += ber_length_enc(trap_hdr->err_stat_len, buf);
  buf += ber_value_enc(&trap_hdr->err_stat, 1, ASN1_TAG_INT, buf);

  /* Error index */
  *buf++ = ASN1_TAG_INT;
  buf += ber_length_enc(trap_hdr->err_idx_len, buf);
  buf += ber_value_enc(&trap_hdr->err_idx, 1, ASN1_TAG_INT, buf);

  *buffer = buf;
}

static void
snmp_trap_encode(struct trap_datagram *tdg)
{
  uint8_t *buf;
  uint32_t oid_len, len_len;
  const uint32_t tag_len = 1;
  struct var_bind *vb;
  struct list_head *curr;
  struct trap_hdr *trap_hdr = &tdg->trap_hdr;
  struct trapv2_hdr *pdu_hdr = &tdg->trap_hdr.trap.v2;

  /* varbind list len */
  len_len = ber_length_enc_try(tdg->vb_list_len);
  trap_hdr->pdu_len += tag_len + len_len + tdg->vb_list_len;

  /* request id len */
  len_len = ber_length_enc_try(pdu_hdr->req_id_len);
  trap_hdr->pdu_len += tag_len + len_len + pdu_hdr->req_id_len;

  /* error status len */
  len_len = ber_length_enc_try(pdu_hdr->err_stat_len);
  trap_hdr->pdu_len += tag_len + len_len + pdu_hdr->err_stat_len;

  /* error index len */
  len_len = ber_length_enc_try(pdu_hdr->err_idx_len);
  trap_hdr->pdu_len += tag_len + len_len + pdu_hdr->err_idx_len;

  /* PDU len */
  len_len = ber_length_enc_try(trap_hdr->pdu_len);
  tdg->data_len += tag_len + len_len + trap_hdr->pdu_len;

  /* community len */
  len_len = ber_length_enc_try(tdg->comm_len);
  tdg->data_len += tag_len + len_len + tdg->comm_len;

  /* version len */
  len_len = ber_length_enc_try(tdg->ver_len);
  tdg->data_len += tag_len + len_len + tdg->ver_len;

  /* send buffer len */
  len_len = ber_length_enc_try(tdg->data_len);
  tdg->send_len += tag_len + len_len + tdg->data_len;

  /* allocate trap buffer */
  tdg->send_buf = xmalloc(tdg->send_len);
  buf = tdg->send_buf;

  /* sequence tag */
  *buf++ = ASN1_TAG_SEQ;
  buf += ber_length_enc(tdg->data_len, buf);

  /* version */
  int version = tdg->version - 1;
  *buf++ = ASN1_TAG_INT;
  buf += ber_length_enc(tdg->ver_len, buf);
  buf += ber_value_enc(&version, tdg->ver_len, ASN1_TAG_INT, buf);

  /* community */
  *buf++ = ASN1_TAG_OCTSTR;
  buf += ber_length_enc(tdg->comm_len, buf);
  buf += ber_value_enc(tdg->community, tdg->comm_len, ASN1_TAG_OCTSTR, buf);

  /* trap header */
  *buf++ = tdg->trap_hdr.pdu_type;
  buf += ber_length_enc(trap_hdr->pdu_len, buf);

  switch (tdg->version) {
  /*
  case TRAP_V1:
    snmp_trapv1_header(tdg);
    break;
  */
  case TRAP_V2:
    snmp_trapv2_header(tdg, &buf);
    break;
  default:
    SMARTSNMP_LOG(L_INFO, "Sorry, only support Trap v2!\n");
    free(tdg->send_buf);
    return;
  }

  /* varbind list */
  *buf++ = ASN1_TAG_SEQ;
  buf += ber_length_enc(tdg->vb_list_len, buf);

  list_for_each(curr, &tdg->vb_list) {
    vb = list_entry(curr, struct var_bind, link);

    *buf++ = ASN1_TAG_SEQ;
    buf += ber_length_enc(vb->vb_len, buf);

    /* oid */
    *buf++ = ASN1_TAG_OBJID;
    oid_len = ber_value_enc_try(vb->oid, vb->oid_len, ASN1_TAG_OBJID);
    buf += ber_length_enc(oid_len, buf);
    buf += ber_value_enc(vb->oid, vb->oid_len, ASN1_TAG_OBJID, buf);

    /* value */
    *buf++ = vb->value_type;
    buf += ber_length_enc(vb->value_len, buf);
    memcpy(buf, vb->value, vb->value_len);
    buf += vb->value_len;
  }
}

static int
snmp_trap_socket(struct trap_datagram *tdg, uint32_t host, int port)
{
  struct sockaddr_in sin;

  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
#ifdef LITTLE_ENDIAN
  sin.sin_addr.s_addr = htonl(host);
  sin.sin_port = htons(port);
#else
  sin.sin_addr.s_addr = host;
  sin.sin_port = port;
#endif

  /* There is no need to bind socket because the host may well just running on the
   * port 162, and there is also no expecting to receive response from the host. */
  return sendto(tdg->sock, tdg->send_buf, tdg->send_len, 0, (struct sockaddr *)&sin, sizeof(sin));
}

/* Add varbind(s) into trap datagram */
static int
snmp_trap_varbind(const oid_t *oid, uint32_t oid_len, Variable *var)
{
  struct var_bind *vb;
  struct trap_datagram *tdg = &snmp_trap_datagram;
  uint32_t id_len, len_len, val_len;
  const uint32_t tag_len = 1;

  val_len = ber_value_enc_try(value(var), length(var), tag(var));
  vb = vb_new(oid_len, val_len);
  if (vb != NULL) {
    /* new varbind */
    oid_cpy(vb->oid, oid, oid_len);
    vb->oid_len = oid_len;
    vb->value_type = tag(var);
    vb->value_len = ber_value_enc(value(var), length(var), tag(var), vb->value);

    /* oid length encoding */
    id_len = ber_value_enc_try(vb->oid, vb->oid_len, ASN1_TAG_OBJID);
    len_len = ber_length_enc_try(id_len);
    vb->vb_len = tag_len + len_len + id_len;

    /* value length encoding */
    len_len = ber_length_enc_try(vb->value_len);
    vb->vb_len += tag_len + len_len + vb->value_len;

    /* varbind length encoding */
    len_len = ber_length_enc_try(vb->vb_len);
    tdg->vb_list_len += tag_len + len_len + vb->vb_len;

    /* add into list */
    list_add_tail(&vb->link, &tdg->vb_list);
    tdg->vb_cnt++;

    return 0;
  }

  return -1;
}

/* Send SNMP trap datagram to NMS host */
static int
snmp_trap_send(uint8_t version, const char *community, uint32_t comm_len, uint32_t host, int port)
{
  int ret;
  struct trap_datagram *tdg = &snmp_trap_datagram;
  struct trap_hdr *trap_hdr = &tdg->trap_hdr;
  struct trapv2_hdr *pdu_hdr = &tdg->trap_hdr.trap.v2;

  tdg->version = version;
  tdg->ver_len = 1;
  tdg->community = community;
  tdg->comm_len = comm_len;

  pdu_hdr->req_id = random();
  pdu_hdr->req_id_len = ber_value_enc_try(&pdu_hdr->req_id, 1, ASN1_TAG_INT);
  pdu_hdr->err_stat = 0;
  pdu_hdr->err_stat_len = ber_value_enc_try(&pdu_hdr->err_stat_len, 1, ASN1_TAG_INT);
  pdu_hdr->err_idx = 0;
  pdu_hdr->err_idx_len = ber_value_enc_try(&pdu_hdr->err_idx_len, 1, ASN1_TAG_INT);
  switch (version) {
  case TRAP_V1:
    trap_hdr->pdu_type = SNMP_TRAP_V1;
    break;
  case TRAP_V2:
    trap_hdr->pdu_type = SNMP_TRAP_V2;
    break;
  default:
    break;
  }

  /* Encode SNMP trap datagram */
  snmp_trap_encode(tdg);

  /* SNMP trap socket */
  ret = snmp_trap_socket(tdg, host, port);

  /* clear trap datagram */
  trap_datagram_clear(tdg);

  return ret;
}

/* Enable SNMP trap feature */
static int
snmp_trap_open(lua_State *L, long poll_interv, int handler)
{
  struct trap_datagram *tdg = &snmp_trap_datagram;

  tdg->sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (tdg->sock < 0) {
    return -1;
  }

  tdg->lua_state = L;
  tdg->lua_handler = handler;
  INIT_LIST_HEAD(&tdg->vb_list);

  snmp_event_timeout(poll_interv);
  return 0;
}

/* Disable SNMP trap feature */
static void
snmp_trap_close(void)
{
  struct trap_datagram *tdg = &snmp_trap_datagram;

  lua_State *L = tdg->lua_state;
  if (L != NULL) {
    snmp_event_timeout(0);
    luaL_unref(L, LUA_ENVIRONINDEX, tdg->lua_handler);
    close(snmp_trap_datagram.sock);
  }
}

static void
snmp_trap_probe(void)
{
  struct trap_datagram *tdg = &snmp_trap_datagram;

  lua_State *L = tdg->lua_state;
  if (L != NULL) {
    /* Empty lua stack. */
    lua_pop(L, -1);
    /* Get trap handler. */
    lua_rawgeti(L, LUA_ENVIRONINDEX, tdg->lua_handler);
    /* Invoke trap lua handler*/
    if (lua_pcall(L, 0, 0, 0) != 0) {
      SMARTSNMP_LOG(L_ERROR, "SNMP trap hander %d fail: %s\n", tdg->lua_handler, lua_tostring(L, -1));
    }
  }
}

struct trap_operation snmp_trap_ops = {
  "snmp",
  snmp_trap_open,
  snmp_trap_close,
  snmp_trap_varbind,
  snmp_trap_send,
  snmp_trap_probe,
};

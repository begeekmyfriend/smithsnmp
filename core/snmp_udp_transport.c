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

#include <sys/socket.h>
#include <netinet/in.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "transport.h"
#include "protocol.h"
#include "event_loop.h"
#include "utils.h"

struct snmp_data_entry {
  int sock;
  uint8_t *buf;
  int len;
  struct sockaddr_in client_sin;
};

static struct snmp_data_entry snmp_entry;

static void
snmp_write_handler(int sock, unsigned char flag, void *ud)
{
  struct snmp_data_entry *entry = ud;

  if (sendto(sock, entry->buf, entry->len, 0, (struct sockaddr *)&entry->client_sin, sizeof(struct sockaddr_in)) == -1) {
    perror("sendto()");
    snmp_event_done();
  }

  free(entry->buf);

  snmp_event_remove(sock, flag);
}

static void
snmp_read_handler(int sock, unsigned char flag, void *ud)
{
  socklen_t server_sz = sizeof(struct sockaddr_in);
  int len;
  uint8_t *buf;

  buf = xmalloc(TRANSP_BUF_SIZ);

  /* Receive UDP data, store the address of the sender in client_sin */
  len = recvfrom(sock, buf, TRANSP_BUF_SIZ, 0, (struct sockaddr *)&snmp_entry.client_sin, &server_sz);
  if (len == -1) {
    perror("recvfrom()");
    snmp_event_done();
  }

  /* Parse SNMP PDU in decoder */
  snmp_prot_ops.receive(buf, len);
}

/* Send snmp datagram as a UDP packet to the remote */
static void
transport_send(uint8_t *buf, int len)
{
  snmp_entry.buf = buf;
  snmp_entry.len = len;
  snmp_event_add(snmp_entry.sock, SNMP_EV_WRITE, snmp_write_handler, &snmp_entry);
}

static void
transport_running(void)
{
  snmp_event_init();
  snmp_event_add(snmp_entry.sock, SNMP_EV_READ, snmp_read_handler, NULL);
  snmp_event_run();
}

static void
transport_stop(void)
{
  snmp_event_done();
  close(snmp_entry.sock);
}

static int
transport_init(int port)
{
  struct sockaddr_in sin;

  snmp_entry.sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (snmp_entry.sock < 0) {
    perror("usock");
    return -1;
  }

  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
#ifdef LITTLE_ENDIAN
  sin.sin_addr.s_addr = htonl(INADDR_ANY);
  sin.sin_port = htons(port);
#else
  sin.sin_addr.s_addr = INADDR_ANY;
  sin.sin_port = port;
#endif

  if (bind(snmp_entry.sock, (struct sockaddr *)&sin, sizeof(sin))) {
    perror("bind()");
    close(snmp_entry.sock);
    return -1;
  }

  return 0;
}

struct transport_operation snmp_transp_ops = {
  "snmp_udp",
  transport_init,
  transport_running,
  transport_stop,
  transport_send,
};

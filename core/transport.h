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

#ifndef _TRANSPORT_H_
#define _TRANSPORT_H_

#include <stdint.h>

#define TRANSP_BUF_SIZ  (65536)

struct transport_operation {
  const char *name;
  int (*init)(int port);
  void (*running)(void);
  void (*close)(void);
  void (*send)(uint8_t *buf, int len);
  int (*step)(long timeout);
};

extern struct transport_operation snmp_transp_ops;
extern struct transport_operation agentx_transp_ops;

#endif /* _TRANSPORT_H_ */

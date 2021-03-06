/*
 * Copyright (c) 2015 Michael Stuart.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the Appleseed project, <https://github.com/drmetal/app-l-seed>
 *
 * Author: Michael Stuart <spaceorbot@gmail.com>
 *
 */

#include "netconf.h"

#ifndef DRIVERS_NET_LWIP_NET_H_
#define DRIVERS_NET_LWIP_NET_H_

void net_init(netconf_t* netconf);
void net_deinit(netconf_t* netconf);
bool wait_for_address(netconf_t* netconf);

bool net_is_up();

const char* net_hostname();
const char* net_lip();
const char* net_mac();

unsigned long net_ip_packets_sent();
unsigned long net_ip_packets_received();
unsigned long net_ip_packets_dropped();
unsigned long net_ip_errors();
unsigned short net_mtu();
unsigned char* net_hwaddr();
ip_addr_t net_ipaddr();
ip_addr_t net_gwaddr();
ip_addr_t net_netmask();

#endif /* DRIVERS_NET_LWIP_NET_H_ */

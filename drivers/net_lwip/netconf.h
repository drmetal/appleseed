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
 * This file is part of the stm32-device-support project, <https://github.com/drmetal/stm32-device-support>
 *
 * Author: Michael Stuart <spaceorbot@gmail.com>
 *
 */

#include "lwipopts.h"
#include "netif.h"
#include "logger.h"
#include "net_config.h"

#ifndef NET_CONFIG_H_
#define NET_CONFIG_H_

#define MAX_HOSTNAME_LENGTH	64

#define DEFAULT_RESOLV_CONF_PATH        "/etc/network/resolv"
#define DEFAULT_NETIF_CONF_PATH         "/etc/network/interface"

typedef enum {
    NET_RESOLV_DHCP,
    NET_RESOLV_STATIC
} net_resolv_prot_t;

typedef enum {
    DHCP_STATE_INIT,
    DHCP_STATE_DISCOVER,
    DHCP_STATE_DONE=2
} dhcp_state_t;

typedef struct {
	logger_t log;
	struct netif netif;
	net_resolv_prot_t resolv;
	SemaphoreHandle_t address_ok;
#ifdef LWIP_DHCP
	uint32_t dhcp_fine_timer;
	uint32_t dhcp_coarse_timer;
	dhcp_state_t dhcp_state;
#endif
	uint8_t hostname[MAX_HOSTNAME_LENGTH];
	struct ip_addr addr_cache[3]; // ip, netmask,gw
#if NO_SYS
	uint32_t tcp_timer;
	uint32_t arp_timer;
#endif
}netconf_t;

void net_config(netconf_t* netconf, const char* resolv, const char* interface);
bool string_to_mac_address(uint8_t* address, const uint8_t* macaddr);
bool string_to_address(uint8_t* address, const uint8_t* addr);

#endif // NET_CONFIG_H_

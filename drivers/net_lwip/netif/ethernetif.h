#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__


#include "lwip/err.h"
#include "netif.h"

err_t ethernetif_incoming();
err_t ethernetif_init(struct netif *netif);
err_t ethernetif_input(struct netif *netif);

#endif

/*
 * Inspiration from eth_dwmac_mmu.c, which is:
 * Copyright (c) 2021 BayLibre SAS
 *
 * Copyright (C) 2025 Alif Semiconductor.
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT snps_designware_ethernet

#include <zephyr/kernel.h>
#include <zephyr/net/ethernet.h>
#include "eth.h"
#include <zephyr/drivers/pinctrl.h>
#include <zephyr/irq.h>

#include "eth_dwmac_priv.h"

int dwmac_bus_init(struct dwmac_priv *p)
{
	p->base_addr = DT_INST_REG_ADDR(0);

	return 0;
}

static struct dwmac_dma_desc __alif_ns_section dwmac_tx_descs[NB_TX_DESCS];
static struct dwmac_dma_desc __alif_ns_section dwmac_rx_descs[NB_RX_DESCS];

static const uint8_t dwmac_mac_addr[6] = DT_INST_PROP(0, local_mac_address);

void dwmac_platform_init(struct dwmac_priv *p)
{
	p->tx_descs = dwmac_tx_descs;
	p->rx_descs = dwmac_rx_descs;

	/* basic configuration for this platform */
	REG_WRITE(MAC_CONF,
			MAC_CONF_PS |
			MAC_CONF_FES |
			MAC_CONF_DM);
	REG_WRITE(DMA_SYSBUS_MODE,
			DMA_SYSBUS_MODE_AAL |
			DMA_SYSBUS_MODE_FB);

	/* set up IRQs */
	IRQ_CONNECT(DT_INST_IRQN(0), DT_INST_IRQ(0, priority), dwmac_isr,
			DEVICE_DT_INST_GET(0), 0);
	irq_enable(DT_INST_IRQN(0));

	/* retrieve MAC address */
	memcpy(p->mac_addr, dwmac_mac_addr, sizeof(p->mac_addr));
}

static struct dwmac_priv dwmac_instance;

ETH_NET_DEVICE_DT_INST_DEFINE(0,
				dwmac_probe,
				NULL,
				&dwmac_instance,
				NULL,
				CONFIG_ETH_INIT_PRIORITY,
				&dwmac_api,
				NET_ETH_MTU);

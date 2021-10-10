/**
 * @file
 * Ethernet Interface Skeleton
 *
 */

 /*
  * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
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
  * This file is part of the lwIP TCP/IP stack.
  *
  * Author: Adam Dunkels <adam@sics.se>
  *
  */

  /*
   * This file is a skeleton for developing Ethernet network interface
   * drivers for lwIP. Add code to the low_level functions and do a
   * search-and-replace for the word "ethernetif" to replace it with
   * something that better describes your network interface.
   */
#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include <lwip/stats.h>
#include <lwip/snmp.h>
#include "lwip/prot/etharp.h"
#include "netif/etharp.h"
#include <t_stddef.h>
#include <sil.h>
#include <kernel.h>
#include <t_syslog.h>
#include <prc_rename.h>
#include "if_rx65n.h"
#include "if_rx65nreg.h"
#include <string.h>
#include "ether_phy.h"
#include "kernel_cfg.h"

   /* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 'n'

extern uint8_t mac_addr[ETHARP_HWADDR_LEN];

typedef struct t_rx65n_buf
{
  uint8_t rx_buff[NUM_IF_RX65N_RXBUF][32 * ((IF_RX65N_BUF_PAGE_SIZE + 31) / 32)];
  uint8_t tx_buff[NUM_IF_RX65N_TXBUF][32 * ((IF_RX65N_BUF_PAGE_SIZE + 31) / 32)];
  T_RX65N_RX_DESC rx_desc[NUM_IF_RX65N_RXBUF];
  T_RX65N_TX_DESC tx_desc[NUM_IF_RX65N_TXBUF];
} T_RX65N_BUF;

#if defined(__RX)
#pragma	section	ETH_MEMORY
#endif
T_RX65N_BUF rx65n_buf __attribute__((aligned(32)));
#if defined(__RX)
#pragma	section
#endif

/**
 * Helper struct to hold private data used to operate your ethernet interface.
 * Keeping the ethernet address of the MAC in this struct is not necessary
 * as it is already kept in the struct netif.
 * But this is only an example, anyway...
 */
struct ethernetif
{
  struct netif *netif;
  struct eth_addr *ethaddr;
  /* Add whatever per-interface state that is needed here. */
  T_RX65N_TX_DESC *tx_write;
  T_RX65N_RX_DESC *rx_read;
  PHY_STATE_T state;
  bool_t link_pre;
  bool_t link_now;
  bool_t over_flow;
};

/* Forward declarations. */
void  ethernetif_input(struct netif *netif);

static void rx65n_set_ecmr(struct netif *netif, PHY_MODE_T mode);

/*
 *  rx65n_bus_init -- ターゲット依存部のバスの初期化
 */

void
rx65n_bus_init (void)
{
  /* イーサネット・コントローラの動作を許可 */
  sil_wrh_mem((uint16_t *)SYSTEM_PRCR_ADDR, (uint16_t)0xA502);  /* 書込み許可 */
  sil_wrw_mem((uint32_t *)SYSTEM_MSTPCRB_ADDR,
    sil_rew_mem((uint32_t *)SYSTEM_MSTPCRB_ADDR) & ~SYSTEM_MSTPCRB_MSTPB15_BIT);
  sil_wrh_mem((uint16_t *)SYSTEM_PRCR_ADDR, (uint16_t)0xA500);  /* 書込み禁止 */

  /* EtherNET有効 */
  /* PA3～5 RMII_MDIO, RMII_MDC, RMII_LINKSTA */
  sil_wrb_mem((uint8_t *)PORTA_PMR_ADDR,
    sil_reb_mem((uint8_t *)PORTA_PMR_ADDR) | (1 << 3) | (1 << 4) | (1 << 5));
  /* PB0～7 RXD1, RXD0, REF50CK, RX-ER, TXD-EN, TXD0, TXD1, CRS */
  sil_wrb_mem((uint8_t *)PORTB_PMR_ADDR,
    sil_reb_mem((uint8_t *)PORTB_PMR_ADDR) | 0xFF);

  /* 書き込みプロテクトレジスタの設定 PFSWEビットへの書き込みを許可 */
  sil_wrb_mem((uint8_t *)(MPC_PWPR_ADDR) , 0x00);
  /* 書き込みプロテクトレジスタの設定 PxxFSレジスタへの書き込みを許可 */
  sil_wrb_mem((uint8_t *)(MPC_PWPR_ADDR) , 0x40);

  /* PA3をET_MDIOとする */
  sil_wrb_mem((uint8_t *)MPC_PA3PFS_ADDR, 0x11); 
  /* PA4をET_MDCとする */
  sil_wrb_mem((uint8_t *)MPC_PA4PFS_ADDR, 0x11); 
  /* PA5をET_LINKSTAとする */
  sil_wrb_mem((uint8_t *)MPC_PA5PFS_ADDR, 0x11); 

  /* PB0をRXD1とする */
  sil_wrb_mem((uint8_t *)MPC_PB0PFS_ADDR, 0x12);
  /* PB1をRXD0とする */
  sil_wrb_mem((uint8_t *)MPC_PB1PFS_ADDR, 0x12);
  /* PB2をREF50CKとする */
  sil_wrb_mem((uint8_t *)MPC_PB2PFS_ADDR, 0x12);
  /* PB3をRX-ERとする */
  sil_wrb_mem((uint8_t *)MPC_PB3PFS_ADDR, 0x12);
  /* PB4をTXD-ENとする */
  sil_wrb_mem((uint8_t *)MPC_PB4PFS_ADDR, 0x12);
  /* PB5をTXD0とする */
  sil_wrb_mem((uint8_t *)MPC_PB5PFS_ADDR, 0x12);
  /* PB6をTXD1とする */
  sil_wrb_mem((uint8_t *)MPC_PB6PFS_ADDR, 0x12);
  /* PB7をCRSとする */
  sil_wrb_mem((uint8_t *)MPC_PB7PFS_ADDR, 0x12);

  /* PHY MODEをRMIIとする */
  sil_wrb_mem((uint8_t *)MPC_PFENET_ADDR,
    sil_reb_mem((uint8_t *)MPC_PFENET_ADDR) & ~MPC_PFENET_PHYMODE0_BIT);

  /* 書き込みプロテクトレジスタの設定 書き込みを禁止 */
  sil_wrb_mem((uint8_t *)(MPC_PWPR_ADDR) , 0x80);

  /* 割り込みを有効に */
  sil_wrw_mem((uint32_t *)ICU_GENAL1_ADDR,
    sil_rew_mem((uint32_t *)ICU_GENAL1_ADDR) | ICU_GRPAL1_EDMAC0_EINT0);
}

/*
 *  rx65n_init_sub -- ネットワークインタフェースの初期化
 *
 *    注意: NIC 割り込み禁止状態で呼び出すこと。
 */

static void
rx65n_init_sub(struct netif *netif)
{
  struct ethernetif *ethernetif = netif->state;
  PHY_STATE_T state = PHY_STATE_UNINIT;

  /* MAC部ソフトウエア・リセット */
  sil_wrw_mem(EDMAC_EDMR, sil_rew_mem(EDMAC_EDMR) | EDMAC_EDMR_SWR_BIT);

  dly_tsk(1000);

  sil_wrw_mem(ETHERC_MAHR, ((uint32_t)mac_addr[0] << 24)
    | ((uint32_t)mac_addr[1] << 16) | ((uint32_t)mac_addr[2] << 8)
    | (uint32_t)mac_addr[3]);
  sil_wrw_mem(ETHERC_MALR, ((uint32_t)mac_addr[4] << 8)
    | (uint32_t)mac_addr[5]);

  /* PHYリセット */
  while ((state = phy_reset(state, PHY_ADDRESS)) != PHY_STATE_RESET);
  ethernetif->state = state;

  /* Clear all ETHERC status BFR, PSRTO, LCHNG, MPD, ICD */
  sil_wrw_mem(ETHERC_ECSR, 0x00000037);

  /* リンク変化割り込み有効 */
  sil_wrw_mem(ETHERC_ECSIPR, sil_rew_mem(ETHERC_ECSIPR) | ETHERC_ECSIPR_LCHNGIP);

  /* Clear all ETHERC and EDMAC status bits */
  sil_wrw_mem(EDMAC_EESR, 0x47FF0F9F);

  /* 送受信割り込み有効 */
  sil_wrw_mem(EDMAC_EESIPR, (EDMAC_EESIPR_TCIP | EDMAC_EESIPR_FRIP | EDMAC_EESIPR_RDEIP | EDMAC_EESIPR_FROFIP));

  /* 受信フレーム長上限（バッファサイズ） */
  sil_wrw_mem(ETHERC_RFLR, IF_RX65N_BUF_PAGE_SIZE);

  /* 96ビット時間（初期値） */
  sil_wrw_mem(ETHERC_IPGR, 0x00000014);

  /* Set little endian mode */
  sil_wrw_mem(EDMAC_EDMR, sil_rew_mem(EDMAC_EDMR) | EDMAC_EDMR_DE_BIT);

  /* Initialize Rx descriptor list address */
  sil_wrw_mem(EDMAC_RDLAR, (uint32_t)rx65n_buf.rx_desc);
  /* Initialize Tx descriptor list address */
  sil_wrw_mem(EDMAC_TDLAR, (uint32_t)rx65n_buf.tx_desc);
  /* Copy-back status is RFE & TFE only */
  sil_wrw_mem(EDMAC_TRSCER, 0x00000000);
  /* Threshold of Tx_FIFO */
  sil_wrw_mem(EDMAC_TFTR, 0x00000000);
  /* Transmit fifo & receive fifo is 2048 bytes */
  sil_wrw_mem(EDMAC_FDR, 0x00000707);
  /* RR in EDRRR is under driver control */
  sil_wrw_mem(EDMAC_RMCR, 0x00000001);

  /* PHYの初期化を促す */
  ethernetif->link_pre = false;
  ethernetif->link_now = true;
}

/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static void
low_level_init(struct netif *netif)
{
  struct ethernetif *ethernetif = netif->state;

  rx65n_bus_init();

  /* set MAC hardware address length */
  netif->hwaddr_len = ETHARP_HWADDR_LEN;

  /* set MAC hardware address */
  memcpy(netif->hwaddr, mac_addr, sizeof(netif->hwaddr));

  /* maximum transfer unit */
  netif->mtu = IF_RX65N_BUF_PAGE_SIZE - (2 * ETHARP_HWADDR_LEN + /*TYPE*/2 + /*FCS*/4);

  /* device capabilities */
  /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
  netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_IGMP;

  /* Do whatever else is needed to initialize interface. */
  T_RX65N_TX_DESC *tdsc;
  T_RX65N_RX_DESC *rdsc;
  int i;
  ER ret;

  /* NIC からの割り込みを禁止する。*/
  ret = dis_int(INT_IF_RX65N_TRX);
  if (ret != E_OK)
    syslog(LOG_DEBUG, "dis_int");

  tdsc = (T_RX65N_TX_DESC *)rx65n_buf.tx_desc;
  ethernetif->tx_write = tdsc;
  for (i = 0; i < NUM_IF_RX65N_TXBUF; i++) {
    memset(tdsc, 0, sizeof(*tdsc));
    tdsc->tbl = 0;
    tdsc->tba = (uint32_t)&rx65n_buf.tx_buff[i];
    tdsc++;
  }
  tdsc--;
  tdsc->tdle = 1;

  rdsc = (T_RX65N_RX_DESC *)rx65n_buf.rx_desc;
  ethernetif->rx_read = rdsc;
  for (i = 0; i < NUM_IF_RX65N_RXBUF; i++) {
    memset(rdsc, 0, sizeof(*rdsc));
    rdsc->rbl = IF_RX65N_BUF_PAGE_SIZE;
    rdsc->rba = (uint32_t)&rx65n_buf.rx_buff[i];
    rdsc->rfl = 0;
    rdsc->ract = 1;
    rdsc++;
  }
  rdsc--;
  rdsc->rdle = 1;

  /* rx65n_init 本体を呼び出す。*/
  rx65n_init_sub(netif);

  if (sil_rew_mem(EDMAC_EDRRR) == 0) {
    sil_wrw_mem(EDMAC_EDRRR, EDMAC_EDRRR_RR);
  }

  /* 送受信を有効 */
  rx65n_set_ecmr(netif, PHY_MODE_10BASE_FULL);

  /* NIC からの割り込みを許可する。*/
  ret = ena_int(INT_IF_RX65N_TRX);
  if (ret != E_OK)
    syslog(LOG_DEBUG, "ena_int");
}

/*
*  rx65n_set_ecmr -- ECMRレジスタの設定
*/

static void
rx65n_set_ecmr(struct netif *netif, PHY_MODE_T mode)
{
  uint32_t ecmr;

  ecmr = ETHERC_ECMR_RE | ETHERC_ECMR_TE/* | ETHERC_ECMR_PRM*/;

  if ((mode & 0x01) != 0)
    ecmr |= ETHERC_ECMR_DM;
  if ((mode & 0x02) != 0)
    ecmr |= ETHERC_ECMR_RTM;

  /* 動作モード設定 */
  sil_wrw_mem(ETHERC_ECMR, ecmr);
}

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become availale since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */

static err_t
low_level_output(struct netif *netif, struct pbuf *p)
{
  struct ethernetif *ethernetif = netif->state;
  struct pbuf *q;

  T_RX65N_TX_DESC *desc, *next;
  uint8_t *buf = NULL, *end;
  int32_t len, res, pos;
  uint32_t tfp;

  for (res = p->tot_len, pos = 0; res > 0; res -= len, pos += len) {
    desc = ethernetif->tx_write;

    while (desc->tact != 0) {
      tslp_tsk(100);
    }

    buf = (uint8_t *)desc->tba;

    next = desc + 1;
    if (next == &rx65n_buf.tx_desc[NUM_IF_RX65N_TXBUF]) {
      next = rx65n_buf.tx_desc;
    }
    ethernetif->tx_write = next;

    len = res;
    if (len > IF_RX65N_BUF_PAGE_SIZE) {
      len = IF_RX65N_BUF_PAGE_SIZE;
      tfp = 0x0;
    }
    else
      tfp = 0x1;

    if (pos == 0)
      tfp |= 0x2;

#if ETH_PAD_SIZE
    pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

    end = &buf[len];
    for (q = p; (q != NULL) && (buf < end); buf += q->len, q = q->next) {
      /* Send the data from the pbuf to the interface, one pbuf at a
      time. The size of the data in each pbuf is kept in the ->len
      variable. */
      memcpy(buf, (uint8_t *)q->payload, q->len);
    }

    desc->tbl = len;
    desc->tfp = tfp;
    desc->tact = 1;

#if ETH_PAD_SIZE
    pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

    LINK_STATS_INC(link.xmit);
  }

  if (sil_rew_mem(EDMAC_EDTRR) == 0) {
    sil_wrw_mem(EDMAC_EDTRR, EDMAC_EDTRR_TR);
  }

  return ERR_OK;
}

/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
static struct pbuf *
low_level_input(struct netif *netif)
{
  struct ethernetif *ethernetif = netif->state;
  struct pbuf *p, *q;
  u16_t pos, len;
  T_RX65N_RX_DESC *desc;

  desc = ethernetif->rx_read;

  if (desc->ract != 0) {
    return NULL;
  }

  /* Obtain the size of the packet and put it into the "len"
     variable. */
  len = desc->rfl;
  if (len == 0) {
    return NULL;
  }

#if ETH_PAD_SIZE
  len += ETH_PAD_SIZE; /* allow room for Ethernet padding */
#endif

  /* We allocate a pbuf chain of pbufs from the pool. */
  p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);

  if (p != NULL) {

#if ETH_PAD_SIZE
    pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

    /* We iterate over the pbuf chain until we have read the entire
     * packet into the pbuf. */
    for (q = p, pos = 0; q != NULL; pos += q->len, q = q->next) {
      /* Read enough bytes to fill this pbuf in the chain. The
       * available data in the pbuf is given by the q->len
       * variable.
       * This does not necessarily have to be a memcpy, you can also preallocate
       * pbufs for a DMA-enabled MAC and after receiving truncate it to the
       * actually received size. In this case, ensure the tot_len member of the
       * pbuf is the sum of the chained pbuf len members.
       */
      memcpy(q->payload, (uint8_t *)desc->rba + pos, q->len);
    }

    desc->rfp = 0;
    desc->ract = 1;

    desc++;
    if (desc == &rx65n_buf.rx_desc[NUM_IF_RX65N_RXBUF]) {
      desc = rx65n_buf.rx_desc;
    }
    ethernetif->rx_read = desc;

    if (sil_rew_mem(EDMAC_EDRRR) == 0) {
      sil_wrw_mem(EDMAC_EDRRR, EDMAC_EDRRR_RR);
    }

#if ETH_PAD_SIZE
    pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

    LINK_STATS_INC(link.recv);
  }
  else {
    //drop packet();
    LINK_STATS_INC(link.memerr);
    LINK_STATS_INC(link.drop);
  }

  return p;
}

/* 
 * rx65n_link -- リンク状態の変化に対する処理
 */
bool_t
rx65n_link(struct netif *netif)
{
	struct ethernetif *ethernetif = netif->state;
	PHY_MODE_T mode;
	uint8_t phy_addr = PHY_ADDRESS;

	if(ethernetif->state == PHY_STATE_NEGOTIATED){
		ethernetif->link_now = phy_is_link(phy_addr);
		if(!ethernetif->link_now)
			ethernetif->state = PHY_STATE_RESET;
		return true;
	}

	/* PHYの初期化 */
	ethernetif->state = phy_initialize(ethernetif->state, phy_addr, &mode);
	if(ethernetif->state != PHY_STATE_NEGOTIATED){
		return false;
	}

	/* ECMRレジスタの設定 */
	rx65n_set_ecmr(netif, mode);
	return true;
}

/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
void
ethernetif_input(struct netif *netif)
{
  struct ethernetif *ethernetif;
  struct eth_hdr *ethhdr;
  struct pbuf *p;

  ethernetif = netif->state;

  if(ethernetif->link_pre != ethernetif->link_now){
    ethernetif->link_pre = ethernetif->link_now;
    /* リンク状態に変化あり */
    if (rx65n_link(netif)) {
      netif_set_link_up(netif);
    }
    else {
      netif_set_link_down(netif);
    }

    if (ethernetif->state != PHY_STATE_NEGOTIATED) {
      ethernetif->link_pre = !ethernetif->link_now;
      sig_sem(SEM_IF_RX65N_RBUF_READY);
    }

    return;
  }

  for (;;) {
    /* move received packet into a new pbuf */
    p = low_level_input(netif);
    /* no packet could be read, silently ignore this */
    if (p == NULL) return;
    /* points to packet payload, which starts with an Ethernet header */
    ethhdr = p->payload;

    switch (htons(ethhdr->type)) {
      /* IP or ARP packet? */
    case ETHTYPE_IP:
    case ETHTYPE_ARP:
#if PPPOE_SUPPORT
      /* PPPoE packet? */
    case ETHTYPE_PPPOEDISC:
    case ETHTYPE_PPPOE:
#endif /* PPPOE_SUPPORT */
      /* full packet send to tcpip_thread to process */
      if (netif->input(p, netif) != ERR_OK) {
        LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
        pbuf_free(p);
        p = NULL;
      }
      break;

    default:
      pbuf_free(p);
      p = NULL;
      break;
    }
  }
}

struct ethernetif if_rx65n;

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t
ethernetif_init(struct netif *netif)
{
  struct ethernetif *ethernetif;

  LWIP_ASSERT("netif != NULL", (netif != NULL));

  if_rx65n.netif = netif;
  ethernetif = &if_rx65n;
  if (ethernetif == NULL) {
    LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_init: out of memory\n"));
    return ERR_MEM;
  }

#if LWIP_NETIF_HOSTNAME
  /* Initialize interface hostname */
  netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

  /*
   * Initialize the snmp variables and counters inside the struct netif.
   * The last argument should be replaced with your link speed, in units
   * of bits per second.
   */
  NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, LINK_SPEED_OF_YOUR_NETIF_IN_BPS);

  netif->state = ethernetif;
  netif->name[0] = IFNAME0;
  netif->name[1] = IFNAME1;
  /* We directly use etharp_output() here to save a function call.
   * You can instead declare your own function an call etharp_output()
   * from it if you have to do some checks before sending (e.g. if link
   * is available...) */
  netif->output = etharp_output;
  netif->linkoutput = low_level_output;

  ethernetif->ethaddr = (struct eth_addr *)&(netif->hwaddr[0]);

  /* initialize the hardware */
  low_level_init(netif);

  act_tsk(IF_RX65N_TASK);

  return ERR_OK;
}

void
if_rx65n_task(EXINF exinf)
{
	ER ret;

	dly_tsk(2000 * 1000);

	do {
		ethernetif_input(if_rx65n.netif);
		ret = wai_sem(SEM_IF_RX65N_RBUF_READY);
	} while(ret == E_OK);
}

/*
*  RX65N Ethernet Controler 送受信割り込みハンドラ
*/

void
if_rx65n_trx_handler_body (struct netif *netif)
{
  struct ethernetif *ethernetif = netif->state;
  uint32_t ecsr, eesr, psr;

  ecsr = sil_rew_mem(ETHERC_ECSR);

  if (ecsr & ETHERC_ECSR_LCHNG) {
    /* ETHERC部割り込み要因クリア */
    sil_wrw_mem(ETHERC_ECSR, ETHERC_ECSR_LCHNG);

    psr = sil_rew_mem(ETHERC_PSR);
    ethernetif->link_now = (psr & ETHERC_PSR_LMON) != 0;

    /* リンク状態に変化あり */
    if (ethernetif->link_pre != ethernetif->link_now) {
      /* 受信割り込み処理 */
      sig_sem(SEM_IF_RX65N_RBUF_READY);
    }
  }

  eesr = sil_rew_mem(EDMAC_EESR);

  if (eesr & EDMAC_EESR_FR) {
    /* DMA部割り込み要因クリア */
    sil_wrw_mem(EDMAC_EESR, EDMAC_EESR_FR);

    /* 受信割り込み処理 */
    sig_sem(SEM_IF_RX65N_RBUF_READY);
  }
  if (eesr & EDMAC_EESR_TC) {
    /* DMA部割り込み要因クリア */
    sil_wrw_mem(EDMAC_EESR, EDMAC_EESR_TC);

    /* 送信割り込み処理 */
    sig_sem(SEM_IF_RX65N_RBUF_READY);
  }
  if (eesr & (EDMAC_EESR_FROF | EDMAC_EESR_RDE)) {
    /* DMA部割り込み要因クリア */
    sil_wrw_mem(EDMAC_EESR, EDMAC_EESR_FROF | EDMAC_EESR_RDE);

    ethernetif->over_flow = true;

    /* 受信割り込み処理 */
    sig_sem(SEM_IF_RX65N_RBUF_READY);
  }
}

/*
 *  グループAL1割り込みハンドラ
 */

void
if_rx65n_trx_handler(EXINF exinf)
{
	uint32_t grpal1 = sil_rew_mem((void *)ICU_GRPAL1_ADDR);

	if ((grpal1 & ICU_GRPAL1_EDMAC0_EINT0) != 0) {
		if_rx65n_trx_handler_body(if_rx65n.netif);
	}
}

void
if_rx65n_cyclic_handler(void)
{
  /* 受信割り込み処理 */
  sig_sem(SEM_IF_RX65N_RBUF_READY);
}

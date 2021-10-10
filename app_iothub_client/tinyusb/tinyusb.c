/* 
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

/* 
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Peter Lawrence
 *
 * influenced by lrndis https://github.com/fetisov/lrndis
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

/*
this appears as either a RNDIS or CDC-ECM USB virtual network adapter; the OS picks its preference

RNDIS should be valid on Linux and Windows hosts, and CDC-ECM should be valid on Linux and macOS hosts

The MCU appears to the host as IP address 192.168.7.1, and provides a DHCP server, DNS server, and web server.
*/
/*
Some smartphones *may* work with this implementation as well, but likely have limited (broken) drivers,
and likely their manufacturer has not tested such functionality.  Some code workarounds could be tried:

The smartphone may only have an ECM driver, but refuse to automatically pick ECM (unlike the OSes above);
try modifying ./examples/devices/net_lwip_webserver/usb_descriptors.c so that CONFIG_ID_ECM is default.

The smartphone may be artificially picky about which Ethernet MAC address to recognize; if this happens, 
try changing the first byte of tud_network_mac_address[] below from 0x02 to 0x00 (clearing bit 1).
*/

#include <kernel.h>
#include <time.h>
#include "arduino.h"

#include "iodefine.h"

#include "tusb.h"
#include "usb_descriptors.h"

#include "lwip/init.h"
#include "lwip/timeouts.h"
#include "lwip/etharp.h"
#include "lwip/ethip6.h"
#include "lwip/netif.h"
#include "lwip/dhcp.h"
#include "netif/ethernet.h"
#include "lwip/apps/sntp.h"
#include "lwip/apps/httpd.h"
#include "adns5050.h"

//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF PROTYPES
//--------------------------------------------------------------------+

#define ENC_A 2
#define ENC_B 6

uint8_t keycode[6] = { 0 };
adns5050_t mouse;
int8_t delta_x;
int8_t delta_y;
uint8_t encoder;
int8_t vertical;
int8_t horizontal;
uint8_t button_mask;

void hid_task(void);
void enc_a_changed(void);
void enc_b_changed(void);
void board_init(void);

/* lwip context */
static struct netif netif_data;

/* shared between tud_network_recv_cb() and service_traffic() */
static struct pbuf *received_frame;

/* this is used by this code, ./class/net/net_driver.c, and usb_descriptors.c */
/* ideally speaking, this should be generated from the hardware's unique ID (if available) */
/* it is suggested that the first byte is 0x02 to indicate a link-local address */
const uint8_t tud_network_mac_address[6] = {0x02,0x02,0x84,0x6A,0x96,0x00};
static ip_addr_t ipaddr, netmask, gateway;

static err_t linkoutput_fn(struct netif *netif, struct pbuf *p)
{
  (void)netif;

  for (;;)
  {
    /* if TinyUSB isn't ready, we must signal back to lwip that there is nothing we can do */
    if (!tud_ready())
      return ERR_USE;

    /* if the network driver can accept another packet, we make it happen */
    if (tud_network_can_xmit())
    {
      tud_network_xmit(p, 0 /* unused for this example */);
      break;
    }

    /* transfer execution to TinyUSB in the hopes that it will finish transmitting the prior packet */
    tud_task();
  }
  return ERR_OK;
}

static err_t netif_init_cb(struct netif *netif)
{
  LWIP_ASSERT("netif != NULL", (netif != NULL));
  netif->mtu = CFG_TUD_NET_MTU;
  netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP | NETIF_FLAG_UP;
  netif->state = NULL;
  netif->name[0] = 'E';
  netif->name[1] = 'X';
  netif->linkoutput = linkoutput_fn;
  netif->output = etharp_output;
#if LWIP_IPV6
  netif->output_ip6 = ethip6_output;
#endif /* LWIP_IPV6 */
  return ERR_OK;
}

static void netif_status_callback(struct netif *nif)
{
	printf("NETIF: %c%c%d is %s\n", nif->name[0], nif->name[1], nif->num,
		netif_is_up(nif) ? "UP" : "DOWN");
#if LWIP_IPV4
	printf("IPV4: Host at %s ", ip4addr_ntoa(netif_ip4_addr(nif)));
	printf("mask %s ", ip4addr_ntoa(netif_ip4_netmask(nif)));
	printf("gateway %s\n", ip4addr_ntoa(netif_ip4_gw(nif)));
#endif /* LWIP_IPV4 */
#if LWIP_IPV6
	printf("IPV6: Host at %s\n", ip6addr_ntoa(netif_ip6_addr(nif, 0)));
#endif /* LWIP_IPV6 */
#if LWIP_NETIF_HOSTNAME
	printf("FQDN: %s\n", netif_get_hostname(nif));
#endif /* LWIP_NETIF_HOSTNAME */

#if LWIP_MDNS_RESPONDER
	if(tcpip_init_flag)
		mdns_resp_netif_settings_changed(nif);
#endif
}
#ifdef LAN_ADAPTOR
static void init_lwip(void)
{
  struct netif *netif = &netif_data;

  IP_ADDR4(&gateway, 0,0,0,0);
  IP_ADDR4(&ipaddr,  0,0,0,0);
  IP_ADDR4(&netmask, 0,0,0,0);

  lwip_init();

  /* the lwip virtual MAC address must be different from the host's; to ensure this, we toggle the LSbit */
  netif->hwaddr_len = sizeof(tud_network_mac_address);
  memcpy(netif->hwaddr, tud_network_mac_address, sizeof(tud_network_mac_address));
  netif->hwaddr[5] ^= 0x01;

  netif = netif_add(netif, &ipaddr.u_addr.ip4, &netmask.u_addr.ip4, &gateway.u_addr.ip4, NULL, netif_init_cb, ip_input);
  netif_create_ip6_linklocal_address(netif, 1);
  netif->ip6_autoconfig_enabled = 1;
  netif_set_status_callback(netif, netif_status_callback);

  //netif_set_default(netif);
}
#endif
bool tud_network_recv_cb(const uint8_t *src, uint16_t size)
{
  /* this shouldn't happen, but if we get another packet before 
  parsing the previous, we must signal our inability to accept it */
  if (received_frame) return false;

  if (size)
  {
    struct pbuf *p = pbuf_alloc(PBUF_RAW, size, PBUF_POOL);

    if (p)
    {
      /* pbuf_alloc() has already initialized struct; all we need to do is copy the data */
      memcpy(p->payload, src, size);

      /* store away the pointer for service_traffic() to later handle */
      received_frame = p;
    }
  }

  return true;
}

uint16_t tud_network_xmit_cb(uint8_t *dst, void *ref, uint16_t arg)
{
  struct pbuf *p = (struct pbuf *)ref;

  (void)arg; /* unused for this example */

  return pbuf_copy_partial(p, dst, p->tot_len, 0);
}
#ifdef LAN_ADAPTOR
static void service_traffic(void)
{
  /* handle any packet received by tud_network_recv_cb() */
  if (received_frame)
  {
    ethernet_input(received_frame, &netif_data);
    pbuf_free(received_frame);
    received_frame = NULL;
    tud_network_recv_renew();
  }

  //sys_check_timeouts();
}
#endif
void tud_network_init_cb(void)
{
  /* if the network is re-initializing and we have a leftover packet, we must do a cleanup */
  if (received_frame)
  {
    pbuf_free(received_frame);
    received_frame = NULL;
  }
}

/*------------- MAIN -------------*/
void setup(void)
{
  board_init();

  pinMode(PIN_A5, INPUT);
  // Left button
  pinMode(20, INPUT_PULLUP);
  // Right button
  pinMode(21, INPUT_PULLUP);
  // A phase
  attachInterrupt(ENC_A, enc_a_changed, CHANGE);
  // B phase
  attachInterrupt(ENC_B, enc_b_changed, CHANGE);

  encoder = 0;

  adns5050_init(&mouse, 11, 13, 10);
  pinMode(12, OUTPUT);
  digitalWrite(12, HIGH);

  adns5050_begin(&mouse);
  delay(1);
  adns5050_sync(&mouse);

  tusb_init();
#ifdef LAN_ADAPTOR
  /* initialize lwip */
  init_lwip();
#endif
}

void loop(void)
{
    tud_task(); // tinyusb device task

    hid_task();
#ifdef LAN_ADAPTOR
    service_traffic();
#endif
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void)
{
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
  (void) remote_wakeup_en;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
}

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

// Every 10ms, we will sent 1 report for each HID profile (keyboard, mouse etc ..)
// tud_hid_report_complete_cb() is used to send the next report after previous one is complete
void hid_task(void)
{
  // Poll every 10ms
  const uint32_t interval_ms = 10;
  static uint32_t start_ms = 0;

  delta_y += adns5050_read(&mouse, ADNS5050_DELTA_X_REG);
  delta_x -= adns5050_read(&mouse, ADNS5050_DELTA_Y_REG);

  if (digitalRead(20) == 0)
	button_mask |= MOUSE_BUTTON_LEFT;
  else
	button_mask &= ~MOUSE_BUTTON_LEFT;
  if (digitalRead(21) == 0)
    button_mask |= MOUSE_BUTTON_RIGHT;
  else
	button_mask &= ~MOUSE_BUTTON_RIGHT;

  if ( millis() - start_ms < interval_ms) return; // not enough time
  start_ms += interval_ms;

  //printf("delta_x:%d, delta_y:%d, vertical:%d, buttons:%x\r\n", delta_x, delta_y, vertical, button_mask);

  uint32_t const btn = 0;

  // Remote wakeup
  if ( tud_suspended() && digitalRead(PIN_A5) )
  {
    // Wake up host if we are in suspend mode
    // and REMOTE_WAKEUP feature is enabled by host
    tud_remote_wakeup();
  }
  else
  {
    // keyboard interface
    if ( tud_hid_n_ready(ITF_NUM_KEYBOARD) )
    {
      // used to avoid send multiple consecutive zero report for keyboard
      static bool has_keyboard_key = false;

      uint8_t const report_id = 0;
      uint8_t const modifier  = 0;

      if ( btn )
      {
        uint8_t keycode[6] = { 0 };
        keycode[0] = HID_KEY_ARROW_RIGHT;

        tud_hid_n_keyboard_report(ITF_NUM_KEYBOARD, report_id, modifier, keycode);
        has_keyboard_key = true;
      }else
      {
        // send empty key report if previously has key pressed
        if (has_keyboard_key) tud_hid_n_keyboard_report(ITF_NUM_KEYBOARD, report_id, modifier, NULL);
        has_keyboard_key = false;
      }
    }

    // mouse interface
    if ( tud_hid_n_ready(ITF_NUM_MOUSE) )
    {
      uint8_t const report_id   = 0;
      tud_hid_n_mouse_report(ITF_NUM_MOUSE, report_id, button_mask, delta_x, delta_y, vertical, horizontal);
      button_mask = delta_x = delta_y = vertical = horizontal = 0;
    }
  }
}

// Invoked when received SET_PROTOCOL request
// protocol is either HID_PROTOCOL_BOOT (0) or HID_PROTOCOL_REPORT (1)
void tud_hid_set_protocol_cb(uint8_t instance, uint8_t protocol)
{
  (void) instance;
  (void) protocol;

  // nothing to do since we use the same compatible boot report for both Boot and Report mode.
  // TOOD set a indicator for user
}

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint8_t len)
{
  (void) instance;
  (void) report;
  (void) len;

  // nothing to do
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
  // TODO not Implemented
  (void) instance;
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) reqlen;

  return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
  (void) report_id;

  // keyboard interface
  if (instance == ITF_NUM_KEYBOARD)
  {
    // Set keyboard LED e.g Capslock, Numlock etc...
    if (report_type == HID_REPORT_TYPE_OUTPUT)
    {
      // bufsize should be (at least) 1
      if ( bufsize < 1 ) return;

      uint8_t const kbd_leds = buffer[0];

      if (kbd_leds & KEYBOARD_LED_CAPSLOCK)
      {
        // Capslock On: disable blink, turn led on
      }else
      {
        // Caplocks Off: back to normal blink
      }
    }
  }
}

void enc_a_changed(void)
{
  uint8_t prev = encoder & 0b11;

  // An-1:Bn-1:An:Bn
  encoder <<= 2;
  encoder |= (prev ^ 0b10);

  switch (encoder & 0b1111){
  // A:Low->High, B:Low
  case 0b0010:
  // A:High->Low, B:High
  case 0b1101:
    vertical++;
    break;
  // A:Low->High, B:High
  case 0b0111:
  // A:High->Low, B:Low
  case 0b1000:
    vertical--;
    break;
  }
}

void enc_b_changed(void)
{
  uint8_t prev = encoder & 0b11;

  // An-1:Bn-1:An:Bn
  encoder <<= 2;
  encoder |= (prev ^ 0b01);

  switch (encoder & 0b1111){
  // A:High, B:Low->High
  case 0b1011:
  // A:Low, B:High->Low
  case 0b0100:
    vertical++;
    break;
  // A:High, B:High->Low
  case 0b1110:
  // A:Low, B:Low->High
  case 0b0001:
	vertical--;
	break;
  }
}

void tinyusb_task(EXINF exinf)
{
	setup();
	while(1){
		loop();
	}
}

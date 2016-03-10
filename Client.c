/*----------------------------------------------------------------------------
 *      RL-ARM - TCPnet
 *----------------------------------------------------------------------------
 *      Name:    CLIENT.C
 *      Purpose: LED Control Client demo example
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2009 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/
#include "LPC17xx.h"
#include <RTL.h>
#include <stdio.h>
#include <string.h>
#include "IP\EMAC_LPC17xx.h"
#include "GLCD\GLCD.h"
#include "Load_pic.h"


//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------

//   <h>Remote IP Address
//   ====================
//
//     <o>IP1: Address byte 1 <0-255>
//     <i> Default: 192
#define IP1            192

//     <o>IP2: Address byte 2 <0-255>
//     <i> Default: 168
#define IP2            168

//     <o>IP3: Address byte 3 <0-255>
//     <i> Default: 0
#define IP3            0

//     <o>IP4: Address byte 4 <0-255>
//     <i> Default: 100
#define IP4            100

//   </h>

//   <o>Remote Port <1-65535>
//   <i> Do not set number of port too small,
//   <i> maybe it is already used.
//   <i> Default: 1001
#define PORT_NUM       1001

//   <o>Communication Protocol <0=> TCP <1=> UDP
//   <i> Selecet a protocol for sending data.
#define PROTOCOL       1

//   <o>LED Blinking speed <1-100>
//   <i> Blinking speed = SPEED * 100ms
//   <i> Default: 2
#define SPEED          2


//------------- <<< end of configuration section >>> -----------------------


#define BLINKLED 0x01  /* Command for blink the leds on board */
#define SENDLEN  2     /* Number of bytes to send */
#define TCP      0
#define UDP      1

char chh = 0;

BOOL tick;
BOOL flag;
U8 p2val;
U8 socket_tcp;
U8 socket_udp;
U8 Rem_IP[4] = {IP1,IP2,IP3,IP4};

void BUTTON_init(void);

void delay( uint32_t del)
{
	uint32_t i,temp;
	for ( i=0; i<del; i++)
		temp = i;
}

/*--------------------------- init ------------------------------------------*/

static void init () {
  /* Add System initialisation code here */

  /* Set the clocks. */
  SystemInit();

  init_ethernet ();
  /* Configure the GPIO for LEDs. */
  LPC_GPIO1->FIODIR   |= 0xB0000000;
  LPC_GPIO2->FIODIR   |= 0x0000007C;

  /* Configure UART1 for 115200 baud. */

  /* Setup and enable the SysTick timer for 100ms. */
  SysTick->LOAD = (SystemFrequency / 10) - 1;
  SysTick->CTRL = 0x05;




}


/*--------------------------- init_display ----------------------------------*/

static void init_display () {
  /* LCD Module init */
  GLCD_Init();
  GLCD_Clear(Black);
  GLCD_SetBackColor(Black);
  GLCD_SetTextColor(White);
  GLCD_DisplayString(3,1,"Object Recognition");
  GLCD_DisplayString(4,5,"System");
  GLCD_DisplayString(5,2,"Embedded Terminal");

  delay(1<<26);
  GLCD_Clear(Black);
  GLCD_DisplayString(3,0,"System Initializing");
}

/*--------------------------- Button Init ----------------------------------*/

void BUTTON_init(void) {
  LPC_GPIO2->FIODIR      &= ~(1 << 10);    /* PORT2.10 defined as input       */
  LPC_GPIOINT->IO2IntEnF |=  (1 << 10);    /* enable falling edge irq         */

  NVIC_EnableIRQ(EINT3_IRQn);              /* enable irq in nvic              */
}
/*--------------------------- LED_out ---------------------------------------*/

void LED_out (U32 val) {
  const U8 led_pos[8] = { 28, 29, 31, 2, 3, 4, 5, 6 };
  U32 i,mask;

  for (i = 0; i < 8; i++) {
    mask = 1 << led_pos[i];
    if (val & (1<<i)) {
      if (i < 3) LPC_GPIO1->FIOSET = mask;
      else       LPC_GPIO2->FIOSET = mask;
    }
    else {
      if (i < 3) LPC_GPIO1->FIOCLR = mask;
      else       LPC_GPIO2->FIOCLR = mask;
    }
  }
}
/*---------------------------Process---------------------------------------*/
void procrec (U8 *buf) {
  switch (buf[0]) {
    case BLINKLED:
      LED_out (buf[1]);
      break;
  }
}

/*--------------------------- timer_poll ------------------------------------*/

static void timer_poll () {
  /* System tick timer running in poll mode */

  if (SysTick->CTRL & 0x10000) {
    /* Timer tick every 100 ms */
    timer_tick ();
    tick = __TRUE;
  }
}


/*--------------------------- fputc -----------------------------------------*/

int fputc(int ch, FILE *f)  {
  /* Debug output to serial port. */

  if (ch == '\n')  {
    while (!(LPC_UART1->LSR & 0x20));
    LPC_UART1->THR = 0x0D;
  }
  while (!(LPC_UART1->LSR & 0x20));
  LPC_UART1->THR = (ch & 0xFF);
  return (ch);
}


/*--------------------------- UDP socket ------------------------------------*/

U16 udp_callback (U8 soc, U8 *rip, U16 rport, U8 *buf, U16 len) {
  /* This function is called by the UDP module when UDP packet is received. */

  /* Make a reference to suppress compiler warnings. */
  rip  = rip;
  rport= rport;
  len  = len;

  if (soc != socket_udp) {
    /* Check if this is the socket we are connected to */
    return (0);
  }
  GLCD_Clear(Black);
  Load_Pic(buf);
  GLCD_DisplayString(9,3,buf);
  return (0);
}


/*--------------------------- TCP socket ------------------------------------*/

U16 tcp_callback (U8 soc, U8 evt, U8 *ptr, U16 par) {
  /* This function is called by the TCP module on TCP event */
  /* Check the 'Net_Config.h' for possible events.          */
  par = par;

  if (soc != socket_tcp) {
    return (0);
  }

  switch (evt) {
    case TCP_EVT_DATA:
      /* TCP data frame has arrived, data is located at *par1, */
      /* data length is par2. Allocate buffer to send reply.   */
      procrec(ptr);
      break;

    case TCP_EVT_CONREQ:
      /* Remote peer requested connect, accept it */
      return (1);

    case TCP_EVT_CONNECT:
      /* The TCP socket is connected */
      return (1);
  }
  return (0);
}


/*--------------------------- TCP send --------------------------------------*/

void send_data (U8 p2val) {
  U8 *sendbuf;
  U8 p2;

  /* UDP */
  if (socket_udp != 0) {
    /* Start Connection */
    sendbuf = udp_get_buf (SENDLEN);
    sendbuf[0] = p2val;
    udp_send (socket_udp, Rem_IP, 1001, sendbuf, SENDLEN);
  }

  /* TCP */
  if (socket_tcp != 0) {
    /* Start Connection */
    p2 = p2val;
    switch (tcp_get_state(socket_tcp)) {
      case TCP_STATE_FREE:
      case TCP_STATE_LISTEN:
        tcp_connect (socket_tcp, Rem_IP, PORT_NUM, 0);
        break;
      case TCP_STATE_CONNECT:
        if (tcp_check_send (socket_tcp) == __TRUE) {
          sendbuf = tcp_get_buf(SENDLEN);
          sendbuf[0] = p2;
          tcp_send (socket_tcp, sendbuf, SENDLEN);
        }
        break;
    }
  }
}


/*--------------------------- main ------------------------------------------*/

int main (void) {
  /* Main Thread of the TcpNet */
  U8 protocol;

  

  init ();
  init_display ();
  init_TcpNet ();
  BUTTON_init();

  delay(1<<29);
  GLCD_Clear(Black);
  GLCD_DisplayString(3,0,"Initialization Done");

  protocol = PROTOCOL;
  switch (protocol) {
    case TCP:
      socket_tcp = tcp_get_socket (TCP_TYPE_CLIENT_SERVER , 0, 10, tcp_callback);
	  if (socket_tcp != 0)tcp_listen (socket_tcp, PORT_NUM+1);
      break;
    case UDP:
      socket_udp = udp_get_socket (0, UDP_OPT_SEND_CS | UDP_OPT_CHK_CS, udp_callback);
      if (socket_udp != 0) {
        udp_open (socket_udp, PORT_NUM);
      }
      break;
  }
  
  p2val = 1;
  while (1) {
    timer_poll ();
    main_TcpNet ();
    if (tick == __TRUE) {
	  if(flag == __TRUE)
	  {
	  	send_data(p2val);
		flag = __FALSE;
	  }
      tick = __FALSE;
    }
  }
}

 void EINT3_IRQHandler()					   /*INT0 interrupt*/
{
//	uint32_t i,j;
	
	delay(1<<22);							//delay to prevent the vibrate of the button
	LPC_GPIOINT->IO2IntClr |= (1 << 10);	//clear pending interrupt 
	p2val = 'a';
	flag = __TRUE;
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

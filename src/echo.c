/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
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
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

#include <stdio.h>
#include <string.h>
#include "xparameters.h"
#include "xgpio.h"
#include "xiicps.h"

#define TJE_IMPLEMENTATION
#include "jpeg.h"
#include "jpeg2.h"
// #include "jpeg3.h"

#include "lwip/err.h"
#include "lwip/tcp.h"
#if defined (__arm__) || defined (__aarch64__)
#include "xil_printf.h"
#endif

volatile int poslano;
volatile int slanje = 0;
unsigned char slika3[480*640*2];
 unsigned char slika2[480*640*3];

unsigned char picture[480*265*2];
unsigned char picture2[480*265*3];

int velicina;


int transfer_data() {
	return 0;
}

void print_app_header()
{
	xil_printf("\n\r\n\r-----lwIP TCP echo server ------\n\r");
	xil_printf("TCP packets sent to port 6001 will be echoed back\n\r");
}

err_t sent_callback(void *arg, struct tcp_pcb *tpcb, unsigned int len)
{

	velicina = tcp_sndbuf(tpcb);

	if(slanje){
		tcp_arg(tpcb, NULL);

		if((poslano + velicina) >= brojac){

			tcp_write(tpcb, slika + poslano, brojac - poslano, 1);

			tcp_close(tpcb);
			slanje = 0;

			xil_printf("Slanje gotovo\n\r");

		}else{



			tcp_write(tpcb, slika + poslano, velicina, 1);
			poslano += velicina;

		}

	}

}


err_t recv_callback(void *arg, struct tcp_pcb *tpcb,
                               struct pbuf *p, err_t err)
{

	int i, status;

	xil_printf("Received %d \n\r" , p->len);

	/* do not read the packet if we are not in ESTABLISHED state */
	if (!p) {
		tcp_close(tpcb);
		tcp_recv(tpcb, NULL);
		return ERR_OK;
	}

	/* indicate that the packet has been received */
	tcp_recved(tpcb, p->len);


	velicina = tcp_sndbuf(tpcb);

	// druga opcija da se salje slika
	if(p->len != 0){


			if(!strncmp(p->payload,"1",1)){



				   //   status = getFrame2(picture);
				        status = getFrame2(slika3);
				    //     getFrame(slika3);

						if(status  == XST_SUCCESS){
							xil_printf("Picture fetch successfully \r\n");
		    				toRGB(picture,picture2);
							tje_encode_to_file(480,265,3,picture2);

			//				toRGB(slika3,slika2);
			//                tje_encode_to_file(480,640,3,slika2);

							criptImage(slika,brojac);

						}else{
							xil_printf("Picture fetch fail \r\n");
						}

					if (tcp_sndbuf(tpcb) > p->len) {
						err = tcp_write(tpcb, slika, velicina, 1);
						poslano = velicina;
						slanje = 1;
					} else
						xil_printf("no space in tcp_sndbuf\n\r");


			}

	}


	/* free the received pbuf */
	pbuf_free(p);

	return ERR_OK;
}

err_t accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err)
{
	static int connection = 1;

	/* set the receive callback for this connection */
	tcp_recv(newpcb, recv_callback);
	tcp_sent(newpcb, sent_callback);
	/* just use an integer number indicating the connection id as the
	   callback argument */
	tcp_arg(newpcb, (void*)(UINTPTR)connection);

	/* increment for subsequent accepted connections */
	connection++;

	return ERR_OK;
}


int start_application()
{


	struct tcp_pcb *pcb;
	err_t err;
	unsigned port = 7;

	/* create new TCP PCB structure */
	pcb = tcp_new();
	if (!pcb) {
		xil_printf("Error creating PCB. Out of Memory\n\r");
		return -1;
	}

	/* bind to specified @port */
	err = tcp_bind(pcb, IP_ADDR_ANY, port);
	if (err != ERR_OK) {
		xil_printf("Unable to bind to port %d: err = %d\n\r", port, err);
		return -2;
	}

	/* we do not need any arguments to callback functions */
	tcp_arg(pcb, NULL);

	/* listen for connections */
	pcb = tcp_listen(pcb);
	if (!pcb) {
		xil_printf("Out of memory while tcp_listen\n\r");
		return -3;
	}

	/* specify callback to use for incoming connections */
	tcp_accept(pcb, accept_callback);

	xil_printf("TCP echo server started @ port %d\n\r", port);


	return 0;
}

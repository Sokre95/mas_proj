/*
 * camera.c
 *
 *  Created on: 28. pro 2017.
 *      Author: Renchy
 */
/*                          INCLUDE                  */

#include<stdio.h>
#include<malloc.h>
#include<stdlib.h>
#include "xparameters.h"
#include "xgpio.h"
#include "xil_printf.h"
#include "xiicps.h"

/*                          INCLUDE                  */



/*                          DEKLARACIJA FUNKCIJA               */

unsigned char* getFrame();
int getFrame2();
int camera_init();
int iic_write(u8 addr, u8 data);

/*                          DEKLARACIJA FUNKCIJA               */



/*                          DEFINE                  */

#define IIC_DEVICE_ID     XPAR_XIICPS_0_DEVICE_ID
#define IIC_SLAVE_ADDR    0x21
#define IIC_SCLE_RATE     400000

XIicPs Iic;

/*                          DEFINE                  */



/*                          FERWEB                                  */

/*                          REGISTRI                       */


/* Registers */
#define REG_GAIN	0x00	/* Gain lower 8 bits (rest in vref) */
#define REG_BLUE	0x01	/* blue gain */
#define REG_RED		0x02	/* red gain */
#define REG_VREF	0x03	/* Pieces of GAIN, VSTART, VSTOP */
#define REG_COM1	0x04	/* Control 1 */
#define  COM1_CCIR656	  0x40  /* CCIR656 enable */
#define REG_BAVE	0x05	/* U/B Average level */
#define REG_GbAVE	0x06	/* Y/Gb Average level */
#define REG_AECHH	0x07	/* AEC MS 5 bits */
#define REG_RAVE	0x08	/* V/R Average level */
#define REG_COM2	0x09	/* Control 2 */
#define  COM2_SSLEEP	  0x10	/* Soft sleep mode */
#define REG_PID		0x0a	/* Product ID MSB */
#define REG_VER		0x0b	/* Product ID LSB */
#define REG_COM3	0x0c	/* Control 3 */
#define  COM3_SWAP	  0x40	  /* Byte swap */
#define  COM3_SCALEEN	  0x08	  /* Enable scaling */
#define  COM3_DCWEN	  0x04	  /* Enable downsamp/crop/window */
#define REG_COM4	0x0d	/* Control 4 */
#define REG_COM5	0x0e	/* All "reserved" */
#define REG_COM6	0x0f	/* Control 6 */
#define REG_AECH	0x10	/* More bits of AEC value */
#define REG_CLKRC	0x11	/* Clocl control */
#define   CLK_EXT	  0x40	  /* Use external clock directly */
#define   CLK_SCALE	  0x3f	  /* Mask for internal clock scale */
#define REG_COM7	0x12	/* Control 7 */
#define   COM7_RESET	  0x80	  /* Register reset */
#define   COM7_FMT_MASK	  0x38
#define   COM7_FMT_VGA	  0x00
#define	  COM7_FMT_CIF	  0x20	  /* CIF format */
#define   COM7_FMT_QVGA	  0x10	  /* QVGA format */
#define   COM7_FMT_QCIF	  0x08	  /* QCIF format */
#define	  COM7_RGB	  0x04	  /* bits 0 and 2 - RGB format */
#define	  COM7_YUV	  0x00	  /* YUV */
#define	  COM7_BAYER	  0x01	  /* Bayer format */
#define	  COM7_PBAYER	  0x05	  /* "Processed bayer" */
#define REG_COM8	0x13	/* Control 8 */
#define   COM8_FASTAEC	  0x80	  /* Enable fast AGC/AEC */
#define   COM8_AECSTEP	  0x40	  /* Unlimited AEC step size */
#define   COM8_BFILT	  0x20	  /* Band filter enable */
#define   COM8_AGC	  0x04	  /* Auto gain enable */
#define   COM8_AWB	  0x02	  /* White balance enable */
#define   COM8_AEC	  0x01	  /* Auto exposure enable */
#define REG_COM9	0x14	/* Control 9  - gain ceiling */
#define REG_COM10	0x15	/* Control 10 */
#define   COM10_HSYNC	  0x40	  /* HSYNC instead of HREF */
#define   COM10_PCLK_HB	  0x20	  /* Suppress PCLK on horiz blank */
#define   COM10_HREF_REV  0x08	  /* Reverse HREF */
#define   COM10_VS_LEAD	  0x04	  /* VSYNC on clock leading edge */
#define   COM10_VS_NEG	  0x02	  /* VSYNC negative */
#define   COM10_HS_NEG	  0x01	  /* HSYNC negative */
#define REG_HSTART	0x17	/* Horiz start high bits */
#define REG_HSTOP	0x18	/* Horiz stop high bits */
#define REG_VSTART	0x19	/* Vert start high bits */
#define REG_VSTOP	0x1a	/* Vert stop high bits */
#define REG_PSHFT	0x1b	/* Pixel delay after HREF */
#define REG_MIDH	0x1c	/* Manuf. ID high */
#define REG_MIDL	0x1d	/* Manuf. ID low */
#define REG_MVFP	0x1e	/* Mirror / vflip */
#define   MVFP_MIRROR	  0x20	  /* Mirror image */
#define   MVFP_FLIP	  0x10	  /* Vertical flip */

#define REG_AEW		0x24	/* AGC upper limit */
#define REG_AEB		0x25	/* AGC lower limit */
#define REG_VPT		0x26	/* AGC/AEC fast mode op region */
#define REG_HSYST	0x30	/* HSYNC rising edge delay */
#define REG_HSYEN	0x31	/* HSYNC falling edge delay */
#define REG_HREF	0x32	/* HREF pieces */
#define REG_TSLB	0x3a	/* lots of stuff */
#define   TSLB_YLAST	  0x04	  /* UYVY or VYUY - see com13 */
#define REG_COM11	0x3b	/* Control 11 */
#define   COM11_NIGHT	  0x80	  /* NIght mode enable */
#define   COM11_NMFR	  0x60	  /* Two bit NM frame rate */
#define   COM11_HZAUTO	  0x10	  /* Auto detect 50/60 Hz */
#define	  COM11_50HZ	  0x08	  /* Manual 50Hz select */
#define   COM11_EXP	  0x02
#define REG_COM12	0x3c	/* Control 12 */
#define   COM12_HREF	  0x80	  /* HREF always */
#define REG_COM13	0x3d	/* Control 13 */
#define   COM13_GAMMA	  0x80	  /* Gamma enable */
#define	  COM13_UVSAT	  0x40	  /* UV saturation auto adjustment */
#define   COM13_UVSWAP	  0x01	  /* V before U - w/TSLB */
#define REG_COM14	0x3e	/* Control 14 */
#define   COM14_DCWEN	  0x10	  /* DCW/PCLK-scale enable */
#define REG_EDGE	0x3f	/* Edge enhancement factor */
#define REG_COM15	0x40	/* Control 15 */
#define   COM15_R10F0	  0x00	  /* Data range 10 to F0 */
#define	  COM15_R01FE	  0x80	  /*            01 to FE */
#define   COM15_R00FF	  0xc0	  /*            00 to FF */
#define   COM15_RGB565	  0x10	  /* RGB565 output */
#define   COM15_RGB555	  0x30	  /* RGB555 output */
#define REG_COM16	0x41	/* Control 16 */
#define   COM16_AWBGAIN   0x08	  /* AWB gain enable */
#define REG_COM17	0x42	/* Control 17 */
#define   COM17_AECWIN	  0xc0	  /* AEC window - must match COM4 */
#define   COM17_CBAR	  0x08	  /* DSP Color bar */

/*
 * This matrix defines how the colors are generated, must be
 * tweaked to adjust hue and saturation.
 *
 * Order: v-red, v-green, v-blue, u-red, u-green, u-blue
 *
 * They are nine-bit signed quantities, with the sign bit
 * stored in 0x58.  Sign for v-red is bit 0, and up from there.
 */
#define	REG_CMATRIX_BASE 0x4f
#define   CMATRIX_LEN 6
#define REG_CMATRIX_SIGN 0x58


#define REG_BRIGHT	0x55	/* Brightness */
#define REG_CONTRAS	0x56	/* Contrast control */

#define REG_GFIX	0x69	/* Fix gain control */

#define REG_DBLV	0x6b	/* PLL control an debugging */
#define   DBLV_BYPASS	  0x00	  /* Bypass PLL */
#define   DBLV_X4	  0x01	  /* clock x4 */
#define   DBLV_X6	  0x10	  /* clock x6 */
#define   DBLV_X8	  0x11	  /* clock x8 */

#define REG_REG76	0x76	/* OV's name */
#define   R76_BLKPCOR	  0x80	  /* Black pixel correction enable */
#define   R76_WHTPCOR	  0x40	  /* White pixel correction enable */

#define REG_RGB444	0x8c	/* RGB 444 control */
#define   R444_ENABLE	  0x02	  /* Turn on RGB444, overrides 5x5 */
#define   R444_RGBX	  0x01	  /* Empty nibble at end */

#define REG_HAECC1	0x9f	/* Hist AEC/AGC control 1 */
#define REG_HAECC2	0xa0	/* Hist AEC/AGC control 2 */

#define REG_BD50MAX	0xa5	/* 50hz banding step limit */
#define REG_HAECC3	0xa6	/* Hist AEC/AGC control 3 */
#define REG_HAECC4	0xa7	/* Hist AEC/AGC control 4 */
#define REG_HAECC5	0xa8	/* Hist AEC/AGC control 5 */
#define REG_HAECC6	0xa9	/* Hist AEC/AGC control 6 */
#define REG_HAECC7	0xaa	/* Hist AEC/AGC control 7 */
#define REG_BD60MAX	0xab	/* 60hz banding step limit */

/*                               REGISTRI                   */

/*                               STRUKTURE                  */


struct regval_list {
	unsigned char reg_num;
	unsigned char value;
};

static struct regval_list ov7670_default_regs[] = {
	//{ REG_COM7, COM7_RESET },
/*
 * Clock scale: 3 = 15fps
 *              2 = 20fps
 *              1 = 30fps
 */
	{ REG_CLKRC, 0x1F},	/* OV: clock scale (30 fps) */
	{ REG_TSLB,  0x04 },	/* OV */
	{ REG_COM7, 0 },	/* VGA */
	/*
	 * Set the hardware window.  These values from OV don't entirely
	 * make sense - hstop is less than hstart.  But they work...
	 */
	{ REG_HSTART, 0x13 },	{ REG_HSTOP, 0x01 },
	{ REG_HREF, 0xb6 },	{ REG_VSTART, 0x02 },
	{ REG_VSTOP, 0x7a },	{ REG_VREF, 0x0a },

	{ REG_COM3, 0 },	{ REG_COM14, 0 },
	/* Mystery scaling numbers */
	{ 0x70, 0x3a },		{ 0x71, 0x35 },
	{ 0x72, 0x11 },		{ 0x73, 0xf0 },
	{ 0xa2, 0x02 },		{ REG_COM10, 0x0 },

	/* Gamma curve values */
	{ 0x7a, 0x20 },		{ 0x7b, 0x10 },
	{ 0x7c, 0x1e },		{ 0x7d, 0x35 },
	{ 0x7e, 0x5a },		{ 0x7f, 0x69 },
	{ 0x80, 0x76 },		{ 0x81, 0x80 },
	{ 0x82, 0x88 },		{ 0x83, 0x8f },
	{ 0x84, 0x96 },		{ 0x85, 0xa3 },
	{ 0x86, 0xaf },		{ 0x87, 0xc4 },
	{ 0x88, 0xd7 },		{ 0x89, 0xe8 },

	/* AGC and AEC parameters.  Note we start by disabling those features,
	   then turn them only after tweaking the values. */
	{ REG_COM8, COM8_FASTAEC | COM8_AECSTEP | COM8_BFILT },
	{ REG_GAIN, 0 },	{ REG_AECH, 0 },
	{ REG_COM4, 0x40 }, /* magic reserved bit */
	{ REG_COM9, 0x18 }, /* 4x gain + magic rsvd bit */
	{ REG_BD50MAX, 0x05 },	{ REG_BD60MAX, 0x07 },
	{ REG_AEW, 0x95 },	{ REG_AEB, 0x33 },
	{ REG_VPT, 0xe3 },	{ REG_HAECC1, 0x78 },
	{ REG_HAECC2, 0x68 },	{ 0xa1, 0x03 }, /* magic */
	{ REG_HAECC3, 0xd8 },	{ REG_HAECC4, 0xd8 },
	{ REG_HAECC5, 0xf0 },	{ REG_HAECC6, 0x90 },
	{ REG_HAECC7, 0x94 },
	{ REG_COM8, COM8_FASTAEC|COM8_AECSTEP|COM8_BFILT|COM8_AGC|COM8_AEC },

	/* Almost all of these are magic "reserved" values.  */
	{ REG_COM5, 0x61 },	{ REG_COM6, 0x4b },
	{ 0x16, 0x02 },		{ REG_MVFP, 0x07|MVFP_MIRROR },
	{ 0x21, 0x02 },		{ 0x22, 0x91 },
	{ 0x29, 0x07 },		{ 0x33, 0x0b },
	{ 0x35, 0x0b },		{ 0x37, 0x1d },
	{ 0x38, 0x71 },		{ 0x39, 0x2a },
	{ REG_COM12, 0x78 },	{ 0x4d, 0x40 },
	{ 0x4e, 0x20 },		{ REG_GFIX, 0 },
	{ 0x6b, 0x4a },		{ 0x74, 0x10 },
	{ 0x8d, 0x4f },		{ 0x8e, 0 },
	{ 0x8f, 0 },		{ 0x90, 0 },
	{ 0x91, 0 },		{ 0x96, 0 },
	{ 0x9a, 0 },		{ 0xb0, 0x84 },
	{ 0xb1, 0x0c },		{ 0xb2, 0x0e },
	{ 0xb3, 0x82 },		{ 0xb8, 0x0a },

	/* More reserved magic, some of which tweaks white balance */
	{ 0x43, 0x0a },		{ 0x44, 0xf0 },
	{ 0x45, 0x34 },		{ 0x46, 0x58 },
	{ 0x47, 0x28 },		{ 0x48, 0x3a },
	{ 0x59, 0x88 },		{ 0x5a, 0x88 },
	{ 0x5b, 0x44 },		{ 0x5c, 0x67 },
	{ 0x5d, 0x49 },		{ 0x5e, 0x0e },
	{ 0x6c, 0x0a },		{ 0x6d, 0x55 },
	{ 0x6e, 0x11 },		{ 0x6f, 0x9f }, /* "9e for advance AWB" */
	{ 0x6a, 0x40 },		{ REG_BLUE, 0x40 },
	{ REG_RED, 0x60 },
	{ REG_COM8, COM8_FASTAEC|COM8_AECSTEP|COM8_BFILT|COM8_AGC|COM8_AEC|COM8_AWB },

	/* Matrix coefficients */
	{ 0x4f, 0x80 },		{ 0x50, 0x80 },
	{ 0x51, 0 },		{ 0x52, 0x22 },
	{ 0x53, 0x5e },		{ 0x54, 0x80 },
	{ 0x58, 0x9e },

	{ REG_COM16, COM16_AWBGAIN },	{ REG_EDGE, 0 },
	{ 0x75, 0x05 },		{ 0x76, 0xe1 },
	{ 0x4c, 0 },		{ 0x77, 0x01 },
	{ REG_COM13, 0xc3 },	{ 0x4b, 0x09 },
	{ 0xc9, 0x60 },		{ REG_COM16, 0x38 },
	{ 0x56, 0x40 },

	{ 0x34, 0x11 },		{ REG_COM11, COM11_EXP|COM11_HZAUTO },
	{ 0xa4, 0x88 },		{ 0x96, 0 },
	{ 0x97, 0x30 },		{ 0x98, 0x20 },
	{ 0x99, 0x30 },		{ 0x9a, 0x84 },
	{ 0x9b, 0x29 },		{ 0x9c, 0x03 },
	{ 0x9d, 0x4c },		{ 0x9e, 0x3f },
	{ 0x78, 0x04 },

	/* Extra-weird stuff.  Some sort of multiplexor register */
	{ 0x79, 0x01 },		{ 0xc8, 0xf0 },
	{ 0x79, 0x0f },		{ 0xc8, 0x00 },
	{ 0x79, 0x10 },		{ 0xc8, 0x7e },
	{ 0x79, 0x0a },		{ 0xc8, 0x80 },
	{ 0x79, 0x0b },		{ 0xc8, 0x01 },
	{ 0x79, 0x0c },		{ 0xc8, 0x0f },
	{ 0x79, 0x0d },		{ 0xc8, 0x20 },
	{ 0x79, 0x09 },		{ 0xc8, 0x80 },
	{ 0x79, 0x02 },		{ 0xc8, 0xc0 },
	{ 0x79, 0x03 },		{ 0xc8, 0x40 },
	{ 0x79, 0x05 },		{ 0xc8, 0x30 },
	{ 0x79, 0x26 },

	{ 0xff, 0xff },	/* END MARKER */
};


/*
 * Here we'll try to encapsulate the changes for just the output
 * video format.
 *
 * RGB656 and YUV422 come from OV; RGB444 is homebrewed.
 *
 * IMPORTANT RULE: the first entry must be for COM7, see ov7670_s_fmt for why.
 */

//SET YUV MODE
static struct regval_list ov7670_fmt_yuv422[] = {
	{ REG_COM7, 0x0 },  /* Selects YUV mode */
	{ REG_RGB444, 0 },	/* No RGB444 please */
	{ REG_COM1, 0 },	/* CCIR601 */
	{ REG_COM15, COM15_R00FF },
	{ REG_COM9, 0x48 }, /* 32x gain ceiling; 0x8 is reserved bit */
	{ 0x4f, 0x80 }, 	/* "matrix coefficient 1" */
	{ 0x50, 0x80 }, 	/* "matrix coefficient 2" */
	{ 0x51, 0    },		/* vb */
	{ 0x52, 0x22 }, 	/* "matrix coefficient 4" */
	{ 0x53, 0x5e }, 	/* "matrix coefficient 5" */
	{ 0x54, 0x80 }, 	/* "matrix coefficient 6" */
	{ REG_COM13, COM13_GAMMA|COM13_UVSAT },
	{ 0xff, 0xff },
};


//SET RGB 565 MODE
static struct regval_list ov7670_fmt_rgb565[] = {
	{ REG_COM7, COM7_RGB },	/* Selects RGB mode */
	{ REG_RGB444, 0 },	/* No RGB444 please */
	{ REG_COM1, 0x0 },	/* CCIR601 */
	{ REG_COM15, COM15_RGB565 },
	{ REG_COM9, 0x38 }, 	/* 16x gain ceiling; 0x8 is reserved bit */
	{ 0x4f, 0xb3 }, 	/* "matrix coefficient 1" */
	{ 0x50, 0xb3 }, 	/* "matrix coefficient 2" */
	{ 0x51, 0    },		/* vb */
	{ 0x52, 0x3d }, 	/* "matrix coefficient 4" */
	{ 0x53, 0xa7 }, 	/* "matrix coefficient 5" */
	{ 0x54, 0xe4 }, 	/* "matrix coefficient 6" */
	{ REG_COM13, COM13_GAMMA|COM13_UVSAT },
	{ 0xff, 0xff },
};


//SET TEST MODE SHIFT ONE output 01, 01, 02, 02, 04, 04, 08, 08, ....
static struct regval_list ov7670_test_shift[] = {
	{ 0x70, 0xCA },	/* */
	{ 0x71, 0xB5 },	/* */
	{ 0xff, 0xff }
};

//SET TEST MODE: COLOR TEST BAR
static struct regval_list ov7670_test_bar[] = {
	{ 0x70, 0x4A },	/* */
	{ 0x71, 0xB5 },	/* */
	{ 0xff, 0xff }
};

//SET TEST MODE: OFF
static struct regval_list ov7670_image[] = {
	{ 0x70, 0x4A },
	{ 0x71, 0x35 },
	{ 0xff, 0xff }
};

/*                        STRUKTURE                       */

/*                          FERWEB                                  */



static int camera_write_array(struct regval_list *val){

	while(val -> reg_num != 0xff || val -> value != 0xff){

		int ret = iic_write(val -> reg_num, val -> value);
		if (ret == XST_FAILURE){

			xil_printf("I2C write failure \r\n}" );
			return ret;
		}

	//	xil_printf("Num: %x , Val: %x \r\n" , val -> reg_num ,  val -> value);

		val++;
	}

	return 0;

}


int iic_write(u8 addr, u8 data){

	int status;

	u8 buffer[2];

	buffer[0] = addr;
	buffer[1] = data;

	status = XIicPs_MasterSendPolled(&Iic, buffer,2,IIC_SLAVE_ADDR);

    if(status != XST_SUCCESS){
    	xil_printf("I2C failure \r\n" );
    	return XST_FAILURE;
    }

    while(XIicPs_BusIsBusy(&Iic)){ xil_printf("I2C busy \r\n" ); }

    return XST_SUCCESS;

}


int camera_init(){

	XIicPs_Config *config;
	int status;

	u16 deviceId = IIC_DEVICE_ID;

	config = XIicPs_LookupConfig(deviceId);
	if(config == NULL){
		xil_printf("Camera failure \r\n" );
		return XST_FAILURE;
	}

	status = XIicPs_CfgInitialize(&Iic, config, config -> BaseAddress);
	if(status != XST_SUCCESS){
		xil_printf("Camera failure \r\n" );
		return XST_FAILURE;
	}

	status = XIicPs_SelfTest(&Iic);
	if(status != XST_SUCCESS){
		xil_printf("Camera failure \r\n" );
			return XST_FAILURE;
		}

	status = XIicPs_SetSClk(&Iic, IIC_SCLE_RATE);
    if(status != XST_SUCCESS){
    	xil_printf("Camera failure \r\n");
    	     return XST_FAILURE;
    }


	camera_write_array(ov7670_default_regs);
//	camera_write_array(ov7670_fmt_yuv422);
	camera_write_array(ov7670_fmt_rgb565);
	camera_write_array(ov7670_image);
//	camera_write_array(ov7670_test_bar);


	return XST_SUCCESS;
}


#define READ_CAM XGpio_DiscreteRead(&cam,1)

#define CAMERA_isVSYNdown(a)   ((a&0x0400) == 0x0000)
#define CAMERA_isVSYNup(a)     ((a&0x0400) == 0x0400)
#define CAMERA_isHREFdown(a)   ((a&0x0200) == 0x0000)
#define CAMERA_isHREFup(a)     ((a&0x0200) == 0x0200)
#define CAMERA_isPCLKdown(a)   ((a&0x0100) == 0x0000)
#define CAMERA_isPCLKup(a)     ((a&0x0100) == 0x0100)

unsigned char* getFrame(unsigned char *slika){

	printf("On get frame !!!\n");

	unsigned char* Y;
	int z,y,r,tt,tt1;
	XGpio cam;

	XGpio_Initialize(&cam,XPAR_AXI_GPIO_0_DEVICE_ID);
	XGpio_SetDataDirection(&cam,1,0xffffffff);

	Y = (unsigned char*) malloc( 480 * 640 * 2 * sizeof(unsigned char));

	if(Y == NULL){

		xil_printf("Error allocating memory");
		return Y;

	}else{

		xil_printf("Memory ok");
	}

	ll:
				z=0;
				   while(CAMERA_isVSYNdown(READ_CAM));
				   while(CAMERA_isVSYNup(READ_CAM));

					for(y = 0; y<480; y++){  //288
						//Wait line to start
						do{
							tt=READ_CAM;
							if(CAMERA_isVSYNup(tt)){
								xil_printf("VSINC error [%d]\n",y);
								goto ll;
							}
						}while(CAMERA_isHREFdown(tt));

						//Y[z] =(u8)(tt);
						//z++;
						for(r = 0;r<1280;r++){ //352
							tt=READ_CAM;
	  					do{tt=READ_CAM;}
							while(CAMERA_isPCLKdown(tt)); //while(CAMERA_isPCLKup(tt));

							//---------
							do{
								tt1=READ_CAM;
								if(CAMERA_isHREFdown(tt1) && r!=1279){
							     	xil_printf("HREF error [%d] %d  %d \n",r , y , z); goto ll;
									}
							}
							while(CAMERA_isPCLKup(tt1));

							//Write data
							//HMY[z] =(Xuint8)(tt>>3);
							Y[z] =(u8)(tt);
						//	if( z == 1 || z == 1000 || z == 10000 || z == 100000 || z == 196607 || z == 200000)
						//	xil_printf(" Ajmo %c  jos  %d !!!\n" , Y[z] , z);

							z++;
						}
						while(CAMERA_isHREFup(READ_CAM));

					}

     l2:
					    xil_printf("%d",z);

					xil_printf("OK !!!\n");

					slika = Y;
			return Y;

}




int getFrame2(unsigned char* slika){


	int br = 0;
	int z,y,r,tt,tt1;
	XGpio cam;


	XGpio_Initialize(&cam,XPAR_AXI_GPIO_0_DEVICE_ID);
	XGpio_SetDataDirection(&cam,1,0xffffffff);

	ll:
	            br ++;
	            if(br == 10 ){
	               return XST_FAILURE;
	            }
				z=0;
				   while(CAMERA_isVSYNdown(READ_CAM));
				   while(CAMERA_isVSYNup(READ_CAM));

					for(y = 0; y<480; y++){  //288
						//Wait line to start
					do{
							tt=READ_CAM;
							if(CAMERA_isVSYNup(tt)){
								xil_printf("VSINC error [%d]\n",y);
								goto ll;
							}
						}while(CAMERA_isHREFdown(tt));

						//Y[z] =(u8)(tt);
						//z++;
						for(r = 0;r<530;r++){ //352
							tt=READ_CAM;
	  					do{tt=READ_CAM;}
							while(CAMERA_isPCLKdown(tt)); //while(CAMERA_isPCLKup(tt));

							//---------
							do{
								tt1=READ_CAM;
								if(CAMERA_isHREFdown(tt1) && r!=1279){
							    	xil_printf("HREF error [%d] %d  %d \n",r , y , z); goto ll;
						 		}
							}
							while(CAMERA_isPCLKup(tt1));

							//Write data
							//HMY[z] =(Xuint8)(tt>>3);
							slika[z] =(u8)(tt);

							z++;
						}
						while(CAMERA_isHREFup(READ_CAM));

					}

					return XST_SUCCESS;

}



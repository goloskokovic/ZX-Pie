//
// TK-PIE
// Copyright (c) 2016 - Fabio Belavenuto & Victor Trucco
//
// All rights reserved
//
// Redistribution and use in source and synthezised forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// Redistributions in synthesized form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// Neither the name of the author nor the names of other contributors may
// be used to endorse or promote products derived from this software without
// specific prior written permission.
//
// THIS CODE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// You are responsible for any legal issues arising from your use of this code.
//
//-------------------------------------------------------------------------------

#define ARMBASE 0x8000
#define CS 0x20003000
#define CLO 0x20003004
#define C0 0x2000300C
#define C1 0x20003010
#define C2 0x20003014
#define C3 0x20003018

// Timer ----------------

#define ARM_TIMER_LOD 0x2000B400
#define ARM_TIMER_VAL 0x2000B404
#define ARM_TIMER_CTL 0x2000B408
#define ARM_TIMER_CLI 0x2000B40C
#define ARM_TIMER_RIS 0x2000B410
#define ARM_TIMER_MIS 0x2000B414
#define ARM_TIMER_RLD 0x2000B418
#define ARM_TIMER_DIV 0x2000B41C
#define ARM_TIMER_CNT 0x2000B420

// GPIO -----------------

#define GPFSEL0 0x20200000 // GPIO 0 a 9
#define GPFSEL1 0x20200004 // GPIO 10 a 19
#define GPFSEL2 0x20200008 // GPIO 20 a 29
#define GPFSEL3 0x2020000C // GPIO 30 a 39
#define GPSET0  0x2020001C // set bits
#define GPCLR0  0x20200028 // clear bits
#define GPLEV0  0x20200034 // GPIO 0...31     , GPLEV1 has the values for pins 32->53.
#define GPEDS0 	0x20200040 // GPIO Pin Event Detect Status 0 
#define GPFEN0 	0x20200058 // GPIO 0..31 Pin Falling Edge Detect Enable 0 (setar 1 no bit para detectar)
#define GPAFEN0 0x20200088 // GPIO 0..31 Pin Async. Falling Edge Detect 0 (setar 1 no bit para detectar)
#define GPLEN0 	0x20200070 // GPIO 0..31 Pin LOW Detect Enable 0 (setar 1 no bit para detectar)
#define GPPUD		0x20200094 //GPIO Pin Pull-up/down Enable 
#define GPPUDCLK0 	0x20200098 //GPIO Pin Pull-up/down Enable Clock 0 

// IRQ -------------- 

#define IRQ_PENDING2    0x2000B208
#define IRQ_ENABLE2     0x2000B214
#define IRQ_FIQ_CTRL    0x2000B20C
#define IRQ_ENABLE_BASIC 0x2000B218


unsigned int *ptr_GPEDS0 = (unsigned int *)GPEDS0;
unsigned int *ptr_GPLEV0 = (unsigned int *)GPLEV0;

unsigned char scr1[10000];

unsigned int line_num=0;
 
extern void PUT32 ( unsigned int, unsigned int );
extern void PUT16 ( unsigned int, unsigned int );
extern void PUT8 ( unsigned int, unsigned int );
extern unsigned int GET32 ( unsigned int );
extern unsigned int GETPC ( void );
extern void BRANCHTO ( unsigned int );
extern void dummy ( unsigned int );

extern void enable_irq ( void );
extern void enable_fiq ( void );

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define BITS_PER_PIXEL 8

#define BORDER_TOP  24
#define BORDER_LEFT  32

unsigned char flash_counter = 0;
unsigned char flash_state = 0;

typedef struct video_bits_ 
{
    unsigned char x : 5;	 
	unsigned char y2 : 3;
	unsigned char pixel_row : 3;
	unsigned char y1 : 2;
	unsigned char holder : 3;
} video_bits;

typedef union 
{
	unsigned int value;
	video_bits b;
} packed_video;

typedef struct attr_ 
{
    unsigned char ink : 3;	 
	unsigned char paper : 3;
	unsigned char bright : 1;
	unsigned char flash : 1;
} attr;

typedef union 
{
	unsigned char value;
	attr b;
} packed_attr;

typedef struct GPIO_bits_ 
{
	unsigned char DATA  : 8;
                  
	unsigned int ADDR  : 13;

	unsigned char A15_13  : 3;
              
	unsigned char fiqData 			: 1; //GPIO 24
    unsigned char fiqBorder 		: 1; //GPIO 25
    unsigned char fiqULAplus_addr  	: 1; //GPIO 26
    unsigned char fiqULAplus_data 	: 1; //GPIO 27
    unsigned char GP28 : 1;
    unsigned char GP29 : 1;
    unsigned char GP30 : 1;
    unsigned char GP31 : 1;
		
} GPIO_bits;

typedef union 
{
	unsigned long value;
	GPIO_bits b;
} packed_GPIO;

typedef struct ULAplus_register_bits_ 
{
	unsigned char subgroup  : 6;
	unsigned char group  : 2;
		
} ULAplus_register_bits;

typedef union 
{
	unsigned char value;
	ULAplus_register_bits b;
} packed_ULAplus_register;

packed_ULAplus_register ULAplus_register;

//ulaplus
unsigned char ulaPlus_enable = 0;
unsigned char ulaplus_data;

unsigned char ulaplus_palette_table[ 64 ] = 
{
	0x00, 0x02, 0x18, 0x1b, 0xc0, 0xc3, 0xd8, 0xdb, //	; INK
	0x00, 0x02, 0x18, 0x1b, 0xc0, 0xc3, 0xd8, 0xdb, //	; PAPER
	0x00, 0x03, 0x1c, 0x1f, 0xe0, 0xe3, 0xfc, 0xff, //	; +BRIGHT
	0x00, 0x03, 0x1c, 0x1f, 0xe0, 0xe3, 0xfc, 0xff, //	;
	0xdb, 0xd8, 0xc3, 0xc0, 0x1b, 0x18, 0x02, 0x00, //	; +FLASH
	0xdb, 0xd8, 0xc3, 0xc0, 0x1b, 0x18, 0x02, 0x00, //	;
	0xff, 0xfc, 0xe3, 0xe0, 0x1f, 0x1c, 0x03, 0x00, //	; +BRIGHT/
	0xff, 0xfc, 0xe3, 0xe0, 0x1f, 0x1c, 0x03, 0x00 //	; +FLASH
};

 packed_GPIO pGPIO;
 unsigned short border_color = 0;
packed_attr pAttr;
packed_video pVideo;

unsigned char buffer_video[SCREEN_WIDTH * SCREEN_HEIGHT * 2];


void (*ptDraw)(unsigned int);

void memcpy ( unsigned char *d, unsigned char *s, unsigned len )
{
	while(--len)   *d++ = *s++;
}

unsigned int MailboxWrite ( unsigned int fbinfo_addr, unsigned int channel )
{
    unsigned int mailbox;

    mailbox = 0x2000B880;
	
    while( 1 )
    {
        if(( GET32( mailbox + 0x18 ) & 0x80000000 ) == 0 ) break;
    }
	
    PUT32( mailbox + 0x20, fbinfo_addr + channel );
	
    return( 0 );
}

unsigned int MailboxRead ( unsigned int channel )
{
    unsigned int ra;
    unsigned int mailbox;

    mailbox = 0x2000B880;
	
    while( 1 )
    {
        while( 1 )
        {
            ra = GET32( mailbox + 0x18 );
            if (( ra & 0x40000000 ) ==0 ) break;
        }
       
        ra = GET32( mailbox + 0x00 );
        
        if (( ra&0xF ) == channel ) break;
    }
	
    return(ra);
}

unsigned int frameBuffer;

void put_pixel ( int x, int y, int c )
{

    // calculate the pixel's byte offset inside the buffer
    // note: x * 2 as every pixel is 2 consecutive bytes
    unsigned int pix_offset = x + y * SCREEN_WIDTH;
	
	PUT8( frameBuffer + pix_offset, c ); 

}

 unsigned int fb_pos;


void draw_ULA( unsigned int dy ) 
{

	unsigned char ink,paper;
	unsigned int i, attr;
	unsigned int addr;
	
	pVideo.b.x = 0;//dx>>3;
	pVideo.b.y2 = ( dy >> 3 ) & 7;
	pVideo.b.pixel_row = dy & 7;
	pVideo.b.y1 = ( dy & 0xc0 ) >> 6;
	
	attr = 6144 + (( pVideo.b.y1 << 8 ) | ( pVideo.value & 0xff ));

	addr = pVideo.value;
	
	for ( i = 0; i < 32; i++ )
	{	
		pAttr.value = scr1[ attr ];
		
		if(pAttr.b.flash && (flash_state&1)!=1)
		{
			paper = pAttr.b.bright << 3 | pAttr.b.ink;
			ink   = pAttr.b.bright << 3 | pAttr.b.paper;
		}
		else
		{
			ink   = pAttr.b.bright << 3 | pAttr.b.ink;
			paper = pAttr.b.bright << 3 | pAttr.b.paper;
		}		

		//ink = ulaplus_palette_table[(pAttr.b.flash * 2 + pAttr.b.bright) * 16 + pAttr.b.ink ];
		//paper = ulaplus_palette_table[(pAttr.b.flash * 2 + pAttr.b.bright) * 16 + pAttr.b.paper + 8 ];		

		PUT32( fb_pos,  ((( scr1[ addr ] & 16  ) > 0 )  ? ink << 24 : paper << 24 ) | 
						((( scr1[ addr ] & 32  ) > 0 )  ? ink << 16 : paper << 16 ) | 
						((( scr1[ addr ] & 64  ) > 0 )  ? ink << 8  : paper << 8  ) |
						((( scr1[ addr ] & 128 ) > 0 )  ? ink       : paper       ));
		
		fb_pos += 4;
		
		PUT32( fb_pos,  ((( scr1[ addr ] & 1 ) > 0 ) ? ink << 24 : paper << 24 ) | 
						((( scr1[ addr ] & 2 ) > 0 ) ? ink << 16 : paper << 16 ) | 
						((( scr1[ addr ] & 4 ) > 0 ) ? ink << 8  : paper << 8  ) |
						((( scr1[ addr ] & 8 ) > 0 ) ? ink 	  : paper       ));
		
		fb_pos += 4;
		
		attr++;
		addr++;
	}

}


void draw_ULAplus( unsigned int dy ) 
{

	unsigned char ink,paper;
	unsigned int i, attr;
	unsigned int addr;
	
	pVideo.b.x = 0;//dx>>3;
	pVideo.b.y2 = ( dy >> 3 ) & 7;
	pVideo.b.pixel_row = dy & 7;
	pVideo.b.y1 = ( dy & 0xc0 ) >> 6;
	
	attr = 6144 + (( pVideo.b.y1 << 8 ) | ( pVideo.value & 0xff ));

	addr = pVideo.value;
	
	for ( i = 0; i < 32; i++ )
	{	
		pAttr.value = scr1[ attr ];
		
		//ink   = pAttr.b.bright << 3 | pAttr.b.ink;
		//paper = pAttr.b.bright << 3 | pAttr.b.paper;	

		ink   = ulaplus_palette_table[(pAttr.b.flash * 2 + pAttr.b.bright) * 16 + pAttr.b.ink ];
		paper = ulaplus_palette_table[(pAttr.b.flash * 2 + pAttr.b.bright) * 16 + pAttr.b.paper + 8 ];		

		PUT32( fb_pos,  ((( scr1[ addr ] & 16  ) > 0 )  ? ink << 24 : paper << 24 ) | 
						((( scr1[ addr ] & 32  ) > 0 )  ? ink << 16 : paper << 16 ) | 
						((( scr1[ addr ] & 64  ) > 0 )  ? ink << 8  : paper << 8  ) |
						((( scr1[ addr ] & 128 ) > 0 )  ? ink       : paper       ));
		
		fb_pos += 4;
		
		PUT32( fb_pos,  ((( scr1[ addr ] & 1 ) > 0 ) ? ink << 24 : paper << 24 ) | 
						((( scr1[ addr ] & 2 ) > 0 ) ? ink << 16 : paper << 16 ) | 
						((( scr1[ addr ] & 4 ) > 0 ) ? ink << 8  : paper << 8  ) |
						((( scr1[ addr ] & 8 ) > 0 ) ? ink 	  : paper       ));
		
		fb_pos += 4;
		
		attr++;
		addr++;
	}

}


void drawAttr(unsigned int addr) 
{
	int f=0;
	int dx, dy, line;
	unsigned int address;
	
	packed_attr pAttr;
		
	pAttr.value = scr1[addr];
	
	
	address = ((addr&0b0001100000000)<<3) | (f<<8) | (addr & 255);
	pVideo.value = address;
	line = (pVideo.b.y1<<3) | pVideo.b.y2;		
	dy = (line << 3);
			
	for (f=0; f<8; f++)
	{	
		
		address = ((addr&0b0001100000000)<<3) | (f<<8) | (addr & 255);
		
		dx = pVideo.b.x << 3;

		((scr1[address]&128)>0) ? put_pixel(dx++,dy+f,pAttr.b.ink) : put_pixel(dx++,dy+f,pAttr.b.paper);
		((scr1[address]&64)>0)  ? put_pixel(dx++,dy+f,pAttr.b.ink) : put_pixel(dx++,dy+f,pAttr.b.paper);
		((scr1[address]&32)>0)  ? put_pixel(dx++,dy+f,pAttr.b.ink) : put_pixel(dx++,dy+f,pAttr.b.paper);
		((scr1[address]&16)>0)  ? put_pixel(dx++,dy+f,pAttr.b.ink) : put_pixel(dx++,dy+f,pAttr.b.paper);
		((scr1[address]&8)>0)   ? put_pixel(dx++,dy+f,pAttr.b.ink) : put_pixel(dx++,dy+f,pAttr.b.paper);
		((scr1[address]&4)>0)   ? put_pixel(dx++,dy+f,pAttr.b.ink) : put_pixel(dx++,dy+f,pAttr.b.paper);
		((scr1[address]&2)>0)   ? put_pixel(dx++,dy+f,pAttr.b.ink) : put_pixel(dx++,dy+f,pAttr.b.paper);
		((scr1[address]&1)>0)   ? put_pixel(dx++,dy+f,pAttr.b.ink) : put_pixel(dx++,dy+f,pAttr.b.paper);
			
	}

}

void initVideo ( void )
{

	//-------------- configura Video
	
	
	PUT32(0x40040000, 0x000000c0);	//   // FB_STRUCT_END - FB_STRUCT ; Buffer Size In Bytes (Including The Header Values, The End Tag And Padding)
	PUT32(0x40040004, 0x00000000); 	// ; Buffer Request/Response Code
									//; Request Codes: $00000000 Process Request Response Codes: $80000000 Request Successful, $80000001 Partial Response
									//; Sequence Of Concatenated Tags
									
	PUT32(0x40040008, 0x00048003);	//Set_Physical_Display ; Tag Identifier
	PUT32(0x4004000c, 0x00000008); 	//Value Buffer Size In Bytes
	PUT32(0x40040010, 0x00000008);	// 1 bit (MSB) Request/Response Indicator (0=Request, 1=Response), 31 bits (LSB) Value Length In Bytes
	PUT32(0x40040014, SCREEN_WIDTH);//; Value Buffer
	PUT32(0x40040018, SCREEN_HEIGHT);// ; Value Buffer

	PUT32(0x4004001c, 0x00048004);	  //Set_Virtual_Buffer ; Tag Identifier
	PUT32(0x40040020, 0x00000008);    // ; Value Buffer Size In Bytes
	PUT32(0x40040024, 0x00000008);    //; 1 bit (MSB) Request/Response Indicator (0=Request, 1=Response), 31 bits (LSB) Value Length In Bytes
	PUT32(0x40040028, SCREEN_WIDTH);  //; Value Buffer
	PUT32(0x4004002c, SCREEN_HEIGHT); // ; Value Buffer

	PUT32(0x40040030, 0x00048005);	//Set_Depth ; Tag Identifier
	PUT32(0x40040034, 0x00000004);  //Value Buffer Size In Bytes
	PUT32(0x40040038, 0x00000004);  //1 bit (MSB) Request/Response Indicator (0=Request, 1=Response), 31 bits (LSB) Value Length In Bytes
	PUT32(0x4004003c, BITS_PER_PIXEL); //Value Buffer

	PUT32(0x40040040, 0x00048009);	//Set_Virtual_Offset ; Tag Identifier
	PUT32(0x40040044, 0x00000008); 	//Value Buffer Size In Bytes
	PUT32(0x40040048, 0x00000008); 	//1 bit (MSB) Request/Response Indicator (0=Request, 1=Response), 31 bits (LSB) Value Length In Bytes

	PUT32(0x4004004c, 0);			// FB_OFFSET_X:
	PUT32(0x40040050, 0);			// FB_OFFSET_Y:

	PUT32(0x40040054, 0x0004800b);	//Set_Palette ; Tag Identifier
	PUT32(0x40040058, 0x00000048);  //Value Buffer Size In Bytes
	PUT32(0x4004005c, 0x00000048);  //1 bit (MSB) Request/Response Indicator (0=Request, 1=Response), 31 bits (LSB) Value Length In Bytes
	PUT32(0x40040060, 0); 			//Value Buffer (Offset: First Palette Index To Set (0-255))
	PUT32(0x40040064, 16);			// Value Buffer (Length: Number Of Palette Entries To Set (1-256))

	
	PUT32(0x40040068, 0xFF000000);  //  BLACK       	
	PUT32(0x4004006c, 0xFFA00000);  //  BLUE    
	PUT32(0x40040070, 0xFF0000A0);  //  RED        		
	PUT32(0x40040074, 0xFFA000A0);  //  MAGENTA   
	PUT32(0x40040078, 0xFF00A000);	//  GREEN         
	PUT32(0x4004007c, 0xFFA0A000);	//  CYAN       		
	PUT32(0x40040080, 0xFF00A0A0);	//  YELLOW       
	PUT32(0x40040084, 0xFFA0A0A0);	//  WHITE    
                      
	PUT32(0x40040088, 0xFF000000);  //  BRIGHT_BLACK  
	PUT32(0x4004008c, 0xFFFF0000);  //  BRIGHT_BLUE   
	PUT32(0x40040090, 0xFF0000FF);	//  BRIGHT_RED  	
	PUT32(0x40040094, 0xFFFF00FF);	//  BRIGHT_MAGENTA
	PUT32(0x40040098, 0xFF00FF00);	//  BRIGHT_GREEN  
	PUT32(0x4004009c, 0xFFFFFF00);	//  BRIGHT_CYAN 	
	PUT32(0x400400a0, 0xFF00FFFF);	//  BRIGHT_YELLOW 
	PUT32(0x400400a4, 0xFFFFFFFF); 	//  BRIGHT_WHITE  
	
	PUT32(0x400400a8, 0x00040001);	//Allocate_Buffer ; Tag Identifier
	PUT32(0x400400ac, 0x00000008);  //Value Buffer Size In Bytes
	PUT32(0x400400b0, 0x00000008);  //1 bit (MSB) Request/Response Indicator (0=Request, 1=Response), 31 bits (LSB) Value Length In Bytes

	PUT32(0x400400b4, 0) ; 			//Pointer para o Framebuffer
	PUT32(0x400400b8, 0) ; 			

	PUT32(0x400400bc, 0) ; 			//(End Tag)

	MailboxWrite(0x40040000,8);
	MailboxRead(8);
	
	frameBuffer = GET32(0x400400b4);
	
	ptDraw = &draw_ULA;

}

void initVideoULAplus ( void )
{
	unsigned int i;
	
	unsigned int addr = 0x40040000;
	unsigned int addr_ini = 0x40040000;
	unsigned int addr_frameBuffer;
	
	unsigned char color_3_to_8[ 8 ]={ 0, 36, 73, 109, 146, 182, 219, 255 };
	
	
	//-------------- configura Video
	
	//32 entradas * 4 + 256 cores * 4 
	PUT32(addr, 0x00000488); addr+=4;	//   // FB_STRUCT_END - FB_STRUCT ; Buffer Size In Bytes (Including The Header Values, The End Tag And Padding)
	PUT32(addr, 0x00000000); addr+=4; 	// ; Buffer Request/Response Code
										//; Request Codes: $00000000 Process Request Response Codes: $80000000 Request Successful, $80000001 Partial Response
										//; Sequence Of Concatenated Tags
									
	PUT32(addr, 0x00048003); addr+=4;	//Set_Physical_Display ; Tag Identifier
	PUT32(addr, 0x00000008); addr+=4; 	//Value Buffer Size In Bytes
	PUT32(addr, 0x00000008); addr+=4;	// 1 bit (MSB) Request/Response Indicator (0=Request, 1=Response), 31 bits (LSB) Value Length In Bytes
	PUT32(addr, SCREEN_WIDTH); addr+=4;	//; Value Buffer
	PUT32(addr, SCREEN_HEIGHT); addr+=4;// ; Value Buffer

	PUT32(addr, 0x00048004); addr+=4;	//Set_Virtual_Buffer ; Tag Identifier
	PUT32(addr, 0x00000008); addr+=4;	// ; Value Buffer Size In Bytes
	PUT32(addr, 0x00000008); addr+=4;  	//; 1 bit (MSB) Request/Response Indicator (0=Request, 1=Response), 31 bits (LSB) Value Length In Bytes
	PUT32(addr, SCREEN_WIDTH); addr+=4; //; Value Buffer
	PUT32(addr, SCREEN_HEIGHT); addr+=4; // ; Value Buffer
          
	PUT32(addr, 0x00048005); addr+=4;	//Set_Depth ; Tag Identifier
	PUT32(addr, 0x00000004); addr+=4;  	//Value Buffer Size In Bytes
	PUT32(addr, 0x00000004); addr+=4;  	//1 bit (MSB) Request/Response Indicator (0=Request, 1=Response), 31 bits (LSB) Value Length In Bytes
	PUT32(addr, BITS_PER_PIXEL); addr+=4; //Value Buffer
          
	PUT32(addr, 0x00048009); addr+=4;	//Set_Virtual_Offset ; Tag Identifier
	PUT32(addr, 0x00000008); addr+=4; 	//Value Buffer Size In Bytes
	PUT32(addr, 0x00000008); addr+=4; 	//1 bit (MSB) Request/Response Indicator (0=Request, 1=Response), 31 bits (LSB) Value Length In Bytes
          
	PUT32(addr, 0); addr+=4;			// FB_OFFSET_X:
	PUT32(addr, 0); addr+=4;			// FB_OFFSET_Y:
          
	#define QTD_COLORS 256
	
	PUT32(addr, 0x0004800b); addr+=4;	//Set_Palette ; Tag Identifier
	PUT32(addr, (QTD_COLORS *  sizeof(unsigned int)) + (2 *  sizeof(unsigned int)));  addr+=4;  //Value Buffer Size In Bytes
	PUT32(addr, (QTD_COLORS *  sizeof(unsigned int)) + (2 *  sizeof(unsigned int)));  addr+=4;  //1 bit (MSB) Request/Response Indicator (0=Request, 1=Response), 31 bits (LSB) Value Length In Bytes
	PUT32(addr, 0); addr+=4;			//Value Buffer (Offset: First Palette Index To Set (0-255))
	PUT32(addr, QTD_COLORS); addr+=4;	// Value Buffer (Length: Number Of Palette Entries To Set (1-256))

	
	for ( i = 0; i < QTD_COLORS; i++ )
	{
		unsigned char r, g, b, r8, g8, b8;
		
		r = ( i >> 2 ) & 7;
		g = ( i >> 5 ) & 7;
		b = ( i & 3 );
		b = ( b << 1 );
		
		if ( b ) b = b | 1;
		
		r8 = color_3_to_8 [ r ];
		g8 = color_3_to_8 [ g ];
		b8 = color_3_to_8 [ b ];

		PUT32( addr, (0xFF << 24) | (b8 << 16) | (g8 << 8) | r8 ); addr += 4; 
	}
	
	PUT32(addr, 0x00040001); addr+=4;	//Allocate_Buffer ; Tag Identifier
	PUT32(addr, 0x00000008); addr+=4;   //Value Buffer Size In Bytes
	PUT32(addr, 0x00000008); addr+=4;   //1 bit (MSB) Request/Response Indicator (0=Request, 1=Response), 31 bits (LSB) Value Length In Bytes

	addr_frameBuffer = addr;
	PUT32(addr, 0) ; addr+=4;		//Pointer to Framebuffer
	PUT32(addr, 0) ; addr+=4;			

	PUT32(addr, 0) ;				//(End Tag)

	MailboxWrite(addr_ini,8);
	MailboxRead(8);
	
	frameBuffer = GET32(addr_frameBuffer);
	
	//for (i=0;i<64;i++) ulaplus_palette_table[i]=255;
	
	ptDraw = &draw_ULAplus;
}

 unsigned char cor = 0;

void c_fiq_handler ( void )
{
	
	pGPIO.value = ptr_GPLEV0[ 0 ]; //GET32(GPLEV0);	

	if ( ( ptr_GPEDS0[ 0 ] & 0x2000000 ) != 0 )
	{
		//clear the interrupt
		ptr_GPEDS0[ 0 ] |= 0x2000000;

		if ( ulaPlus_enable != 1 )
		{
			border_color = pGPIO.b.DATA & 7;  
		}
		else
		{
			border_color = ulaplus_palette_table[ 8 + ( pGPIO.b.DATA & 7 ) ];  
		}
	}
	else if ( ( ptr_GPEDS0[ 0 ] & 0x1000000 ) != 0 )
	{	
		//clear the interrupt
		ptr_GPEDS0[ 0 ] |= 0x1000000;
		
		scr1[ pGPIO.b.ADDR ] = pGPIO.b.DATA;	
	}
	else if ( ( ptr_GPEDS0[ 0 ] & 0x4000000 ) != 0 ) //BF3B ULAplus addr
	{
		//clear the interrupt
		ptr_GPEDS0[ 0 ] |= 0x4000000;

		ULAplus_register.value = pGPIO.b.DATA;
		
	}
	else  //FF3B ULAplus data
	{
		//clear the interrupt
		ptr_GPEDS0[ 0 ] |= 0x8000000;

		ulaplus_data = pGPIO.b.DATA;
		
		if ( ULAplus_register.b.group == 1 )
		{
			if ( ulaplus_data == 1 )
			{
				if (ulaPlus_enable != 1)
				{
					initVideoULAplus(); 
					ulaPlus_enable = 1;
				}
			}
			else if ( ulaplus_data == 0 )
			{
				if (ulaPlus_enable == 1)
				{
					initVideo(); 
					ulaPlus_enable = 0;
				}
			}
		}
		else if ( ULAplus_register.b.group != 1 )
		{
			ulaplus_palette_table[ ULAplus_register.b.subgroup ] = ulaplus_data;
		}
	}
}

 unsigned int icount=0;
 unsigned int frames=0;

void c_irq_handler ( void )
{
	
	//clear the interrupt. 
    PUT32(ARM_TIMER_CLI,0);
	
	flash_counter++;
	if (flash_counter%16 == 0) flash_state++;

	int x,y;
	
	fb_pos = frameBuffer + 7712; //space for upper and left border
	 
	for ( line_num = 0; line_num < 192; line_num++ )
	{

			ptDraw( line_num ); //draw the line

		unsigned char cor = border_color;
		for ( x = 0; x < 32; x++ ) //right border
		{
			PUT8( fb_pos++, cor ); 
		}
		
		
		for ( x = 0; x < 32; x++ ) //left border
		{
			PUT8( fb_pos++, cor ); 
		}
		
	}
	
	for ( y = 0; y < 24; y++ ) //lower border
	{
		for ( x = 0; x < SCREEN_WIDTH; x++ )
		{
			PUT8( fb_pos++, border_color ); 
		}
		
	}

	fb_pos = frameBuffer;
	for ( y = SCREEN_HEIGHT - 24; y < SCREEN_HEIGHT; y++ )
	{
		for ( x = 0; x < SCREEN_WIDTH; x++ )
		{
			PUT8( fb_pos++, border_color ); //upper border
		}
		
	}
	
	for ( x = 0; x < 32; x++ ) //single line for left border
	{
		PUT8( fb_pos++, border_color ); 
	}

}



//------------------------------------------------------------------------
int notmain ( void )
{
    unsigned int ra;
	int k = 0;

	ULAplus_register.value = 0;
	
	
	initVideo();


	//-------------- config GPIOs
	
	PUT32( GPPUD, 0x02 ); // Enable Pull Up control 
	
	
    PUT32( GPFSEL0, 0 ); // GPIO 0 to 9 in
    PUT32( GPFSEL1, 0 ); // GPIO 10 to 19 in
	PUT32( GPFSEL2, 0 ); // GPIO 20 to 29 in
			
	ra = GET32( GPFSEL3 ); // GPIO 30 to 39
	ra &= 0xFFFFFFC0;      // GPIO 30 and 31 in
    PUT32( GPFSEL3, ra );
	

	PUT32( GPPUDCLK0, 0xF000000 ); //GPIO 24, 25, 26,27 pull up

	PUT32( GPAFEN0, 0xF000000 ); //GPIO 24, 25, 26, 27 falling edge
	
	ra = GET32( GPEDS0 ); 
	ra |= 0xF000000; //GPIO 24, 25, 26 ,27
	PUT32( GPEDS0, ra ); // clear GPIO 
	
	//FIQ
	PUT32( IRQ_FIQ_CTRL, 0xB4 ); //FIQ to gpio_int[3] 
	enable_fiq();
	
	//IRQ
	//ra = GET32( IRQ_ENABLE2 ); 
	//ra |= 1<<20;  //gpio_int[3] ok to GPIO24
	//ra |= 1<<19;  //gpio_int[2] no to gpio_24
	//ra |= 1<<18;  //gpio_int[1] no to gpio_24
	//ra |= 1<<17;  //gpio_int[0] ok to gpio_24
	//ra |= 1<<17;  //gpio_int[0] ok to gpio_24
	//PUT32( IRQ_ENABLE2, ra );
	
	//enable_irq();
	//enable_fiq();
	
	//irq timer
	PUT32(ARM_TIMER_CTL,0x003E0000); //timer disable
    PUT32(ARM_TIMER_LOD,19968-1); 
    PUT32(ARM_TIMER_RLD,19968-1); 
   // PUT32(ARM_TIMER_LOD,1000000-1); //1 sec
    //PUT32(ARM_TIMER_RLD,1000000-1); // 
    PUT32(ARM_TIMER_DIV,0x000000F9); // 249   250mhz/(249+1) = 1mhz
    PUT32(ARM_TIMER_CLI,0);
    PUT32(IRQ_ENABLE_BASIC,1);
    icount=0;

    enable_irq();
    PUT32( ARM_TIMER_CTL, 0x003E00A2 ); //enable timer
    PUT32( ARM_TIMER_CLI, 0 );
	
	//-------------- end config GPIO	
	
	for (k=6144;k<6912; k++)
	{
			drawAttr( k );
	}
	
	
	// loop - just wait
	while(1)
	{
		//if ( ( ptr_GPLEV0[0] & 0x8000000) == 0 ) //hsync
		//{
		//	c_irq_handler ( );
		//}
	}
	
    return(0);
}


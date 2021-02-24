/*
 * Copyright (C) 2018 bzt (bztsrc@github)
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */

// #include "uart.h"
#include "printf.h"
#include "mbox.h"
// #include "homer.h"
//#include "zoom_bg_logo_l2-720p.h"
#include "zoom_bg_logo_l2-1024x768.h"
// #include "tv-test-screen-1024x768.h"

unsigned int width, height, vwidth, vheight, pitch, isrgb;   /* dimensions and channel order */
unsigned char *lfb;                         /* raw frame buffer address */

unsigned int offset_delta = 1;  // speed. higher -> faster

unsigned int width = 600;
unsigned int height = 600;

unsigned int vwidth = 1024;
unsigned int vheight = 768;

static unsigned int offsetx = 0; 
unsigned int offsety = 0; 

/**
 * Set screen resolution to 1024x768
 */
void lfb_init()
{
    mbox[0] = 35*4;
    mbox[1] = MBOX_REQUEST;

    // xzl: viewport 
    mbox[2] = 0x48003;  //set phy wh
    mbox[3] = 8;
    mbox[4] = 8;
    // mbox[5] = 1024;         //FrameBufferInfo.width
    // mbox[6] = 768;          //FrameBufferInfo.height
    mbox[5] = width;         //FrameBufferInfo.width
    mbox[6] = height;          //FrameBufferInfo.height

    // xzl: fb
    mbox[7] = 0x48004;  //set virt wh
    mbox[8] = 8;
    mbox[9] = 8;
    // mbox[10] = 3840;        //FrameBufferInfo.virtual_width
    // mbox[11] = 2560;         //FrameBufferInfo.virtual_height
     mbox[10] = vwidth;        //FrameBufferInfo.virtual_width
     mbox[11] = vheight;         //FrameBufferInfo.virtual_height
    // mbox[10] = 1000;        //FrameBufferInfo.virtual_width
    // mbox[11] = 1000;         //FrameBufferInfo.virtual_height    

    mbox[12] = 0x48009; //set virt offset
    mbox[13] = 8;
    mbox[14] = 8;
    mbox[15] =  offsetx;           //FrameBufferInfo.x_offset
    mbox[16] =  offsety;           //FrameBufferInfo.y.offset    
    // mbox[15] = 400; // 0;           //FrameBufferInfo.x_offset
    // mbox[16] = 400; // 0;           //FrameBufferInfo.y.offset

    mbox[17] = 0x48005; //set depth
    mbox[18] = 4;
    mbox[19] = 4;
    mbox[20] = 32;          //FrameBufferInfo.depth

    mbox[21] = 0x48006; //set pixel order
    mbox[22] = 4;
    mbox[23] = 4;
    mbox[24] = 1;           //1=RGB, not 0=BGR preferably

    mbox[25] = 0x40001; //get framebuffer, gets alignment on request
    mbox[26] = 8;
    mbox[27] = 8;
    mbox[28] = 4096;        //FrameBufferInfo.pointer
    mbox[29] = 0;           //FrameBufferInfo.size

    mbox[30] = 0x40008; //get pitch
    mbox[31] = 4;
    mbox[32] = 4;
    mbox[33] = 0;           //FrameBufferInfo.pitch

    mbox[34] = MBOX_TAG_LAST;

    //this might not return exactly what we asked for, could be
    //the closest supported resolution instead
    if(mbox_call(MBOX_CH_PROP) && mbox[20]==32 && mbox[28]!=0) {
        mbox[28]&=0x3FFFFFFF;   //convert GPU address to ARM address
        width=mbox[5];          //get actual physical width
        height=mbox[6];         //get actual physical height
        pitch=mbox[33];         //get number of bytes per line
        vwidth=mbox[10];
        vheight=mbox[11];
        isrgb=mbox[24];         //get the actual channel order
        lfb=(void*)((unsigned long)mbox[28]);
        printf("width %u height %u pitch %u isrgb %u\r\n", width, height, pitch, isrgb);
        printf("vwidth %u vheight %u offsetx %u offsety %u\r\n", vwidth, vheight, mbox[15], mbox[16]);
    } else {
        printf("Unable to set screen resolution to 1024x768x32\n");
    }
}

#if 0
static unsigned int vx = 1024; 
static unsigned int vy = 768;
static unsigned int px = 1024 * 2; 
static unsigned int py = 768; 

void lfb_update()
{
    static unsigned int offsetx = 0; 

    offsetx += 10; 
    if (offsetx + vx > px)
        offsetx = 0; 

    mbox[0] = 35*4;
    mbox[1] = MBOX_REQUEST;

    mbox[2] = 0x48003;  //set phy wh
    mbox[3] = 8;
    mbox[4] = 8;
    // mbox[5] = 1024;         //FrameBufferInfo.width
    // mbox[6] = 768;          //FrameBufferInfo.height
    mbox[5] = px;         //FrameBufferInfo.width
    mbox[6] = py;          //FrameBufferInfo.height


    mbox[7] = 0x48004;  //set virt wh
    mbox[8] = 8;
    mbox[9] = 8;
    // mbox[10] = 3840;        //FrameBufferInfo.virtual_width
    // mbox[11] = 2560;         //FrameBufferInfo.virtual_height
     mbox[10] = vx;        //FrameBufferInfo.virtual_width
     mbox[11] = vy;         //FrameBufferInfo.virtual_height
    // mbox[10] = 500;        //FrameBufferInfo.virtual_width
    // mbox[11] = 500;         //FrameBufferInfo.virtual_height    

    mbox[12] = 0x48009; //set virt offset
    mbox[13] = 8;
    mbox[14] = 8;
    mbox[15] =  offsetx;           //FrameBufferInfo.x_offset
    mbox[16] =  0;           //FrameBufferInfo.y.offset    
    // mbox[15] = 400; // 0;           //FrameBufferInfo.x_offset
    // mbox[16] = 400; // 0;           //FrameBufferInfo.y.offset

    mbox[17] = 0x48005; //set depth
    mbox[18] = 4;
    mbox[19] = 4;
    mbox[20] = 32;          //FrameBufferInfo.depth

    mbox[21] = 0x48006;   //set pixel order
    mbox[22] = 4;
    mbox[23] = 4;
    mbox[24] = 1;           //RGB, not BGR preferably

    mbox[25] = 0x40001; //get framebuffer, gets alignment on request
    mbox[26] = 8;
    mbox[27] = 8;
    mbox[28] = 4096;        //FrameBufferInfo.pointer
    mbox[29] = 0;           //FrameBufferInfo.size

    mbox[30] = 0x40008; //get pitch
    mbox[31] = 4;
    mbox[32] = 4;
    mbox[33] = 0;           //FrameBufferInfo.pitch

    mbox[34] = MBOX_TAG_LAST;

    //this might not return exactly what we asked for, could be
    //the closest supported resolution instead
    if(mbox_call(MBOX_CH_PROP) && mbox[20]==32 && mbox[28]!=0) {
        mbox[28]&=0x3FFFFFFF;   //convert GPU address to ARM address
        width=mbox[5];          //get actual physical width
        height=mbox[6];         //get actual physical height
        pitch=mbox[33];         //get number of bytes per line
        isrgb=mbox[24];         //get the actual channel order
        lfb=(void*)((unsigned long)mbox[28]);
        printf("width %u height %u pitch %u isrgb %u\r\n", width, height, pitch, isrgb);
        printf("vwidth %u vheight %u offsetx %u offsety %u\r\n", mbox[10], mbox[11], mbox[15], mbox[16]);
    } else {
        printf("Unable to set screen resolution to 1024x768x32\n");
    }
}
#endif

/* return: 0 on success */
int lfb_update2()
{
    offsetx += offset_delta; 
    offsety += offset_delta; 

#if 0 // works, but does not look good
    if (offsetx + width > vwidth)
        offsetx = 0; 
    if (offsety + height > vheight)
        offsety = 0; 
#endif         

#if 1
    if (offsetx + width > vwidth || offsety + height > vheight) 
        offsetx = offsety = 0; 
#endif

    /* NB: these requests belong to one transaction. all or nothing */

#if 0
    mbox[0] = 12*4;
    mbox[1] = MBOX_REQUEST;

    //set virt offset
    mbox[2] = 0x48009; 
    mbox[3] = 8;
    mbox[4] = 8;
    mbox[5] =  offsetx;           //FrameBufferInfo.x_offset
    mbox[6] =  offsety;           //FrameBufferInfo.y.offset    

    mbox[7] = 0x48005; 
    mbox[8] = 4;
    mbox[9] = 4;
    mbox[10] = 32;           

    mbox[11] = MBOX_TAG_LAST;
#endif

    mbox[0] = 8*4;
    mbox[1] = MBOX_REQUEST;

    //set virt offset
    mbox[2] = 0x48009; 
    mbox[3] = 8;
    mbox[4] = 8;
    mbox[5] =  offsetx;           //FrameBufferInfo.x_offset
    mbox[6] =  offsety;           //FrameBufferInfo.y.offset    

    mbox[7] = MBOX_TAG_LAST;

    if(mbox_call(MBOX_CH_PROP)) {
        printf("to set: offsetx %u offsety %u res: offsetx %u offsety %u\r\n", offsetx, offsety, mbox[5], mbox[6]);
        if (mbox[5] != offsetx || mbox[6] != offsety) {
            printf("failed to set. reached screen boundaries?\n\r");
            return -1; 
        }
        return 0; 
    } else
        printf("mbox call failed\n\r");
    return -1; 
}

/**
 * Show a picture
 */

// #define IMG_DATA uva_data       // homer_data
// #define IMG_HEIGHT uva_height
// #define IMG_WIDTH uva_width

#define IMG_DATA img_data       // homer_data
#define IMG_HEIGHT img_height
#define IMG_WIDTH img_width

void lfb_showpicture()
{
    int x,y;
    unsigned char *ptr=lfb;
    char *data=IMG_DATA, pixel[4];
    // fill framebuf. crop img data per the framebuf size
    unsigned int img_fb_height = vheight < IMG_HEIGHT ? vheight : IMG_HEIGHT; 
    unsigned int img_fb_width = vwidth < IMG_WIDTH ? vwidth : IMG_WIDTH; 

    //ptr += (height-img_fb_height)/2*pitch + (width-img_fb_width)*2;
    ptr += (vheight-img_fb_height)/2*pitch + (vwidth-img_fb_width)*2;

    // xzl: copy the image pixels to the center of framebuf
    for(y=0;y<img_fb_height;y++) {
        for(x=0;x<img_fb_width;x++) {
            HEADER_PIXEL(data, pixel);
            // the image is in RGB. So if we have an RGB framebuffer, we can copy the pixels
            // directly, but for BGR we must swap R (pixel[0]) and B (pixel[2]) channels.
            *((unsigned int*)ptr)=isrgb ? *((unsigned int *)&pixel) : (unsigned int)(pixel[0]<<16 | pixel[1]<<8 | pixel[2]);
            ptr+=4;
        }
        ptr+=pitch-img_fb_width*4;
    }
}
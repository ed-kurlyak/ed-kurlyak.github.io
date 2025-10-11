#define _WIN32_WINNT 0x0500

#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include "backbuffer.h"

RGBQUAD RgbPal[256];

HWND g_hWnd = NULL;
HINSTANCE g_hInst;

unsigned char* texture_page_ptrs = NULL;

char * phd_winptr;

int		xgen_ymin;
int		xgen_ymax;

int phd_scrwidth;

#define SINGLE_MASK 0xffff8000

/****************************************************************************
 ****************************************************************************
 *				Generate Polygon X,G,U,V coords
 ****************************************************************************
 ***************************************************************************/
#pragma pack(push,foo,1)

typedef struct uvgp_info
{
	short int	xs;
	short int	ys;
	short int	shade;
	float	ooz;
	float	uoz;
	float	voz;
} UVGP_INFO;

typedef struct xbuf_persp {
			int	Xleft;
			int	Gleft;
			int	UOZleft;
			int	VOZleft;
			int OOZleft;
			int	Xright;
			int	Gright;
			int	UOZright;
			int	VOZright;
			int OOZright;
}XBUF_PERSP;

typedef struct xbuf_persp_fp {
			int	Xleft;
			int	Gleft;
			float	UOZleft;
			float	VOZleft;
			float 	OOZleft;
			int	Xright;
			int	Gright;
			float	UOZright;
			float	VOZright;
			float 	OOZright;
}XBUF_PERSP_FP;

#pragma pack(pop,foo)

#define MAX_SCANLINES 1200 
XBUF_PERSP 	xbuffer[MAX_SCANLINES] = {0};         	// Edge buffer for Polygon Draw

int		xgen_xguvpersp_fp( short int *iptr )
{
	UVGP_INFO		*ptr1,*ptr2;
	XBUF_PERSP_FP	*xptr;
	int				g,g_add;
	int				x,x_add;
	float scale;
	float			uoz,uoz_add;
	float			voz,voz_add;
	float			ooz,ooz_add;
	int				y1,y2,ydif;
	int				min_y,max_y;
	int				numcoords;

	int v = sizeof(UVGP_INFO);

	numcoords = (int)*(iptr++);							// Get Number Coords
	ptr2 = (UVGP_INFO *)iptr;
	ptr1 = ptr2 + numcoords-1;
	min_y = max_y = ptr1->ys;
	for ( ;numcoords>0;numcoords--,ptr1=ptr2,ptr2++ ) 	// For All Side in Clockwise manner
	{
		y1 = ptr1->ys;
		y2 = ptr2->ys;
		if ( y1<y2 )                        			// We are on RHS of Polygon
		{
			if ( y1<min_y )
				min_y = y1;

			ydif = y2-y1;

			x = ptr1->xs;
			x_add = ((ptr2->xs-x)<<16)/ydif;
			x = (x<<16) + 0x0ffff;

			g = ptr1->shade<<8;                         // Get Lerps and Deltas
			scale = 1.0f/ydif;
			g_add = ((ptr2->shade<<8) - g)/ydif;
			uoz = ptr1->uoz;
			uoz_add = (ptr2->uoz - uoz) * scale;
			voz = ptr1->voz;
			voz_add = (ptr2->voz - voz) * scale;
			ooz = ptr1->ooz;
			ooz_add = (ptr2->ooz - ooz) * scale;

			xptr = ((XBUF_PERSP_FP *)xbuffer) + y1;
			for ( ;y1<y2;y1++,xptr++ )
			{
				x += x_add;
				g += g_add;
				uoz += uoz_add;
				voz += voz_add;
				ooz += ooz_add;
				xptr->Xright = x;
				xptr->Gright = g;
				xptr->UOZright = uoz;
				xptr->VOZright = voz;
				xptr->OOZright = ooz;
			}
		}
		else if ( y2<y1 )								// We are on LHS of Polygon
		{
			if ( y1>max_y )
				max_y = y1;

			ydif = y1-y2;

			x = ptr2->xs;
			x_add = ((ptr1->xs-x)<<16)/ydif;
			x = (x<<16) + 0x0001;

			g = ptr2->shade<<8;                         // Get Lerps and Deltas
			scale = 1.0f/ydif;
			g_add = ((ptr1->shade<<8) - g)/ydif;
			uoz = ptr2->uoz;
			uoz_add = (ptr1->uoz - uoz) * scale;
			voz = ptr2->voz;
			voz_add = (ptr1->voz - voz) * scale;
			ooz = ptr2->ooz;
			ooz_add = (ptr1->ooz - ooz) * scale;

			xptr = ((XBUF_PERSP_FP *)xbuffer) + y2;
			for ( ;y2<y1;y2++,xptr++ )
			{
				x += x_add;
				g += g_add;
				uoz += uoz_add;
				voz += voz_add;
				ooz += ooz_add;
				xptr->Xleft = x;
				xptr->Gleft = g;
				xptr->UOZleft = uoz;
				xptr->VOZleft = voz;
				xptr->OOZleft = ooz;
			}
		}
	}
	if ( min_y==max_y )
		return(0);
	xgen_ymin = min_y;				// Insert Global Y bounds..
	xgen_ymax = max_y;
	return(1);
}

/******************************************************************************
 *						   TextureMapper
 * 		 		Perspective Correction every 32 Pixels
 *****************************************************************************/
void	gtmap_persp32_fp( int ymin, int ymax, unsigned char *tptr )
{
	/* This version does 32 pixels at a time linearly, and adaptively double pixels for each 32 pixel span.
		Should only be used in 640x480 though */
	XBUF_PERSP_FP	*dataptr;
	unsigned char	*lineptr,*pixptr;
	float				ooz,uoz,voz,scale;
	float				oozadd32,uozadd32,vozadd32;
	int				start_g,add_g,add_g2;
	int				end_u,end_v;
	int				start_u,start_v;
	int				add_u,add_v;
	int				start_x,end_x;
	int				x;

	dataptr = ((XBUF_PERSP_FP *)xbuffer)+ymin;
	lineptr = (unsigned char*)phd_winptr + ( ymin*phd_scrwidth );
	for (ymax-=ymin; ymax>0; ymax--, dataptr++, lineptr+=phd_scrwidth)
	{
		start_x = dataptr->Xleft >> 16;
		end_x = dataptr->Xright >> 16;
		x = end_x - start_x;
		if (x <= 0)
			continue;
		
		if ( x>32 )
		{
			start_g = dataptr->Gleft;
			add_g = (dataptr->Gright-start_g) / x;
			add_g2 = add_g<<1;

			scale = 32.0f/x;
			uoz = dataptr->UOZleft;
			uozadd32 = (dataptr->UOZright-uoz) * scale;
			voz = dataptr->VOZleft;
			vozadd32 = (dataptr->VOZright-voz) * scale;
			ooz = dataptr->OOZleft;
			oozadd32 = (dataptr->OOZright-ooz) * scale;

			scale = 256.0f/ooz;
			end_u = (int)(uoz * scale);
			end_v = (int)(voz * scale);

			pixptr = lineptr + start_x;
		
			ooz += oozadd32;
			scale = 256.0f/ooz;
			
			for ( ; x>=32; x-=32)
			{
				start_u = end_u;
				uoz += uozadd32;
				end_u = (int)(uoz*scale);
				add_u = (end_u-start_u)>>5;

				start_v = end_v;
				voz += vozadd32;
				end_v = (int)(voz*scale);
				add_v = (end_v-start_v)>>5;

				// FP divide in parallel with the pixel drawing
				ooz += oozadd32;
				scale = 256.0f/ooz;

				if ((ABS(add_u) + ABS(add_v)) & SINGLE_MASK)
				{
					// Single pixel it
					//*(pixptr) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					*(pixptr) = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					
					//*(pixptr+1) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					*(pixptr+1) = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					
					//*(pixptr+2) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					*(pixptr+2) = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					
					//*(pixptr+3) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					*(pixptr+3) = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;

					//*(pixptr+4) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					*(pixptr+4) = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					
					//*(pixptr+5) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					*(pixptr+5) = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					
					//*(pixptr+6) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					*(pixptr+6) = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					
					//*(pixptr+7) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					*(pixptr+7) = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;

					//*(pixptr+8) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					*(pixptr+8) = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					
					//*(pixptr+9) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					*(pixptr+9) = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					
					//*(pixptr+10) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					*(pixptr+10) = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					
					//*(pixptr+11) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					*(pixptr+11) = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;

					//*(pixptr+12) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					*(pixptr+12) = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					
					//*(pixptr+13) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					*(pixptr+13) = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					
					//*(pixptr+14) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					*(pixptr+14) = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					
					//*(pixptr+15) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					*(pixptr+15) = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
					start_g += add_g;
					start_u += add_u;
					start_v += add_v;

					//*(pixptr+16) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					*(pixptr+16) = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					
					//*(pixptr+17) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					*(pixptr+17) = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					
					//*(pixptr+18) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					*(pixptr+18) = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					
					//*(pixptr+19) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					*(pixptr+19) = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;

					//*(pixptr+20) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					*(pixptr+20) = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					
					//*(pixptr+21) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					*(pixptr+21) = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					
					//*(pixptr+22) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					*(pixptr+22) = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					
					//*(pixptr+23) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					*(pixptr+23) = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;

					//*(pixptr+24) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					*(pixptr+24) = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					
					//*(pixptr+25) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					*(pixptr+25) = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					
					//*(pixptr+26) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					*(pixptr+26) = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					
					//*(pixptr+27) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					*(pixptr+27) = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;

					//*(pixptr+28) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					*(pixptr+28) = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					
					//*(pixptr+29) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					*(pixptr+29) = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					
					//*(pixptr+30) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					*(pixptr+30) = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					
					//*(pixptr+31) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					*(pixptr+31) = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
					start_g += add_g;
				}
				else
				{
					// Double pixel it
					add_u <<= 1;
					add_v <<= 1;

					//*(pixptr) = *(pixptr+1) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					*(pixptr) = *(pixptr+1) = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
					start_u += add_u;
					start_v += add_v;
					start_g += add_g2;
					
					//*(pixptr+2) = *(pixptr+3) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					*(pixptr+2) = *(pixptr+3) = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
					start_u += add_u;
					start_v += add_v;
					start_g += add_g2;

					//*(pixptr+4) = *(pixptr+5) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					*(pixptr+4) = *(pixptr+5) = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
					start_u += add_u;
					start_v += add_v;
					start_g += add_g2;
					
					//*(pixptr+6) = *(pixptr+7) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					*(pixptr+6) = *(pixptr+7) = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
					start_u += add_u;
					start_v += add_v;
					start_g += add_g2;

					//*(pixptr+8) = *(pixptr+9) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					*(pixptr+8) = *(pixptr+9) = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
					start_u += add_u;
					start_v += add_v;
					start_g += add_g2;
					
					//*(pixptr+10) = *(pixptr+11) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					*(pixptr+10) = *(pixptr+11) = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
					start_u += add_u;
					start_v += add_v;
					start_g += add_g2;

					//*(pixptr+12) = *(pixptr+13) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					*(pixptr+12) = *(pixptr+13) = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
					start_u += add_u;
					start_v += add_v;
					start_g += add_g2;
					
					//*(pixptr+14) = *(pixptr+15) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					*(pixptr+14) = *(pixptr+15) = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
					start_u += add_u;
					start_v += add_v;
					start_g += add_g2;

					//*(pixptr+16) = *(pixptr+17) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					*(pixptr+16) = *(pixptr+17) = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
					start_u += add_u;
					start_v += add_v;
					start_g += add_g2;

					//*(pixptr+18) = *(pixptr+19) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					*(pixptr+18) = *(pixptr+19) = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
					start_u += add_u;
					start_v += add_v;
					start_g += add_g2;

					//*(pixptr+20) = *(pixptr+21) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					*(pixptr+20) = *(pixptr+21) = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
					start_u += add_u;
					start_v += add_v;
					start_g += add_g2;
					
					//*(pixptr+22) = *(pixptr+23) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					*(pixptr+22) = *(pixptr+23) = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
					start_u += add_u;
					start_v += add_v;
					start_g += add_g2;

					//*(pixptr+24) = *(pixptr+25) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					*(pixptr+24) = *(pixptr+25) = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
					start_u += add_u;
					start_v += add_v;
					start_g += add_g2;
					
					//*(pixptr+26) = *(pixptr+27) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					*(pixptr+26) = *(pixptr+27) = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
					start_u += add_u;
					start_v += add_v;
					start_g += add_g2;

					//*(pixptr+28) = *(pixptr+29) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					*(pixptr+28) = *(pixptr+29) = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
					start_u += add_u;
					start_v += add_v;
					start_g += add_g2;
					
					//*(pixptr+30) = *(pixptr+31) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					*(pixptr+30) = *(pixptr+31) = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
					start_g += add_g2;
				}

				pixptr += 32;
			}

			if (!x)
				continue;

			// Do the odd bit at the end
			scale = 256.0f/dataptr->OOZright;

			start_u = end_u;
			end_u = (int)(dataptr->UOZright*scale);
			add_u = (end_u-start_u)/x;

			start_v = end_v;
			end_v = (int)(dataptr->VOZright*scale);
			add_v = (end_v-start_v)/x;
		}
		else
		{
			/* Special case for spans of 32 pixels or less */
			start_g = dataptr->Gleft;
			add_g = (dataptr->Gright-start_g) / x;

			scale = 256.0f/dataptr->OOZleft;
			start_u = (int)(dataptr->UOZleft * scale);
			start_v = (int)(dataptr->VOZleft * scale);

			scale = 256.0f/dataptr->OOZright;
			end_u = (int)(dataptr->UOZright * scale);
			end_v = (int)(dataptr->VOZright * scale);

			add_u = (end_u-start_u)/x;
			add_v = (end_v-start_v)/x;

			pixptr = lineptr + start_x;
		}

		/* Do bits of span less than 32 pixels */
		if ((ABS(add_u) + ABS(add_v)) & SINGLE_MASK)
		{
			/* Single pixel it */
			for ( ; x>1; x-=2, pixptr+=2)
			{
				//pixptr[0] = depthq_table[start_g>>16][ *(tptr + ((start_v>>16)<<8) + (start_u>>16)) ];
				pixptr[0] = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
				start_u += add_u;
				start_v += add_v;
				start_g += add_g;
				//pixptr[1] = depthq_table[start_g>>16][ *(tptr + ((start_v>>16)<<8) + (start_u>>16)) ];
				pixptr[1] = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
				start_u += add_u;
				start_v += add_v;
				start_g += add_g;
			}
		}
		else
		{
			/* Double pixel it */
			add_u <<= 1;
			add_v <<= 1;
			add_g2 = add_g<<1;

			for ( ; x>1; x-=2, pixptr+=2)
			{
				//pixptr[0] = pixptr[1] = depthq_table[start_g>>16][ *(tptr + ((start_v>>16)<<8) + (start_u>>16)) ];
				pixptr[0] = pixptr[1] = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
				start_u += add_u;
				start_v += add_v;
				start_g += add_g2;
			}
		}

		if (x)
			//pixptr[0] = depthq_table[start_g>>16][ *(tptr + ((start_v>>16)<<8) + (start_u>>16)) ];
			pixptr[0] = *(tptr + ((start_v>>16)<<8) + (start_u>>16) );
	}
}


void draw_poly_gtmap_persp( short int *iptr )
{
	if ( xgen_xguvpersp_fp(iptr) )
		gtmap_persp32_fp( xgen_ymin, xgen_ymax, (unsigned char*)texture_page_ptrs );
}

void Load_BMP()
{
	//открываем BMP файл 64x64 пикселя 8 бит
	//глубина цвета для чтения в бинарном режиме
	FILE *fp = NULL;
	fopen_s(&fp, "texture.bmp", "rb");
	if(fp==NULL) printf("Error Open File");

	//читаем заголовок файла текстуры
	BITMAPFILEHEADER bfh;
	fread(&bfh, sizeof(BITMAPFILEHEADER), 1, fp);

	//читаем заголовок файла текстуры
	BITMAPINFOHEADER bih;
	fread(&bih, sizeof(BITMAPINFOHEADER), 1, fp);

	//одни элемент палитны занимает 4 байта
	//3 байта для R,G,B и один зарезервированный
	//fread(&RgbPal, 256 * 4,1,fp);
	fread(&RgbPal, 256 * sizeof(RGBQUAD),1,fp);

	//сдвигаемся от начала BMP файла до начала данных
	fseek(fp, bfh.bfOffBits, SEEK_SET);

	//указатель на массив байт полученных из BMP файла
	texture_page_ptrs = new unsigned char[bih.biHeight * bih.biWidth];
	
	//читаем из файла цветовые данные изображения
	fread(texture_page_ptrs, bih.biHeight * bih.biWidth, 1, fp);

	//загрузили текстуру закрываем файл
	fclose(fp);
}

int main() 
{

	g_hInst = GetModuleHandle(NULL);

	g_hWnd = GetConsoleWindow();

	RECT Rc;
	GetClientRect(g_hWnd, &Rc);

	phd_scrwidth = Rc.right;

	Load_BMP();

	Create_BackBuffer();

	phd_winptr = (char*)dibdc->surface;

	Clear_BackBuffer();

	/*
	float one  = (256.0f * 8.0f * 16384.0f);
	float z = float ( (1024) << 16) ;
	float ooz = one / z;
	*/

	UVGP_INFO v1 = {100, 100, 1, 1.0f, 0.0f, 0.0f };
	UVGP_INFO v2 = {300, 100, 1, 1.0f, 65535.0f, 0.0f};
	UVGP_INFO v3 = {300, 300, 1, 1.0f, 65535.0f, 65535.0f};
	UVGP_INFO v4 = {100, 300, 1, 1.0f, 0.0f, 65535.0f};

	short int num_verts = 4;

	short int *iptr = (short int *)malloc(sizeof(short int) + sizeof(UVGP_INFO) * num_verts);
	
	short int * iptr_t = iptr;
		
	*iptr_t = num_verts;
	iptr_t++;

	memcpy(iptr_t, &v1, sizeof(UVGP_INFO));
	iptr_t += sizeof(UVGP_INFO) / 2;

	memcpy(iptr_t, &v2, sizeof(UVGP_INFO));
	iptr_t += sizeof(UVGP_INFO) / 2;

	memcpy(iptr_t, &v3, sizeof(UVGP_INFO));
	iptr_t += sizeof(UVGP_INFO) / 2;
	
	memcpy(iptr_t, &v4, sizeof(UVGP_INFO));

	draw_poly_gtmap_persp(iptr);

	Present_BackBuffer();

	while(!_kbhit()){};

	Delete_BackBuffer();

	delete [] texture_page_ptrs;


    return 0;
}

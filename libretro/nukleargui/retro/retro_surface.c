
#include "RSDL_wrapper.h"
#include <stdio.h>
#include <string.h>

static const char *cross[] = {
  "X                               ",
  "XX                              ",
  "X.X                             ",
  "X..X                            ",
  "X...X                           ",
  "X....X                          ",
  "X.....X                         ",
  "X......X                        ",
  "X.......X                       ",
  "X........X                      ",
  "X.....XXXXX                     ",
  "X..X..X                         ",
  "X.X X..X                        ",
  "XX  X..X                        ",
  "X    X..X                       ",
  "     X..X                       ",
  "      X..X                      ",
  "      X..X                      ",
  "       XX                       ",
  "                                ",
};

void DrawPointBmp16(unsigned short int *buffer, int x, int y, unsigned short int color, int rwidth, int rheight)
{
    int idx;

    idx=x+y*rwidth;
    if(idx>=0 && idx<rwidth*rheight)
        buffer[idx]=color;
}

void DrawPointBmp32(unsigned int *buffer, int x, int y, unsigned int color, int rwidth, int rheight)
{
    int idx;

    idx=x+y*rwidth;
    if(idx>=0 && idx<rwidth*rheight)
        buffer[idx]=color;
}

void draw_cross(RSDL_Surface *surface, int x, int y)
{
    int i, j, idx;
    int dx=32, dy=20;
    unsigned short int col_16=0xffff;
    unsigned int col_32=0xffffffff;
    int w = surface->w;
    int h = surface->h;

    for(j=y;j<y+dy;j++){
        idx=0;
        for(i=x;i<x+dx;i++){
            if (pix_bytes == 2)
            {
                if(cross[j-y][idx]=='.')DrawPointBmp16(surface->pixels,i,j,col_16,w,h);
                else if(cross[j-y][idx]=='X')DrawPointBmp16(surface->pixels,i,j,0,w,h);
            }
            else
            {
                if(cross[j-y][idx]=='.')DrawPointBmp32(surface->pixels,i,j,col_32,w,h);
                else if(cross[j-y][idx]=='X')DrawPointBmp32(surface->pixels,i,j,0,w,h);
            }
            idx++;
        }
    }
}

unsigned int Retro_MapRGB(RSDL_PixelFormat *a, int r, int g, int b)
{
    return (r >> a->Rloss) << a->Rshift
         | (g >> a->Gloss) << a->Gshift
         | (b >> a->Bloss) << a->Bshift
         | a->Amask;
}

unsigned int Retro_MapRGBA(RSDL_PixelFormat *a, int r, int g, int b, int alpha)
{
    return (r >> a->Rloss) << a->Rshift
         | (g >> a->Gloss) << a->Gshift
         | (b >> a->Bloss) << a->Bshift
         | ((alpha >> a->Aloss) << a->Ashift & a->Amask);
}

static __inline__
RSDL_bool RSDL_IntersectRect(const RSDL_Rect *A, const RSDL_Rect *B, RSDL_Rect *intersection)
{
	int Amin, Amax, Bmin, Bmax;

	/* Horizontal intersection */
	Amin = A->x;
	Amax = Amin + A->w;
	Bmin = B->x;
	Bmax = Bmin + B->w;
	if(Bmin > Amin)
	        Amin = Bmin;
	intersection->x = Amin;
	if(Bmax < Amax)
	        Amax = Bmax;
	intersection->w = Amax - Amin > 0 ? Amax - Amin : 0;

	/* Vertical intersection */
	Amin = A->y;
	Amax = Amin + A->h;
	Bmin = B->y;
	Bmax = Bmin + B->h;
	if(Bmin > Amin)
	        Amin = Bmin;
	intersection->y = Amin;
	if(Bmax < Amax)
	        Amax = Bmax;
	intersection->h = Amax - Amin > 0 ? Amax - Amin : 0;

	return (intersection->w && intersection->h);
}
/*
 * Set the clipping rectangle for a blittable surface
 */
RSDL_bool RSDL_SetClipRect(RSDL_Surface *surface, const RSDL_Rect *rect)
{
	RSDL_Rect full_rect;

	/* Don't do anything if there's no surface to act on */
	if ( ! surface ) {
		return RSDL_FALSE;
	}

	/* Set up the full surface rectangle */
	full_rect.x = 0;
	full_rect.y = 0;
	full_rect.w = surface->w;
	full_rect.h = surface->h;

	/* Set the clipping rectangle */
	if ( ! rect ) {
		surface->clip_rect = full_rect;
		return 1;
	}
	return RSDL_IntersectRect(rect, &full_rect, &surface->clip_rect);
}

void RSDL_GetClipRect(RSDL_Surface *surface, RSDL_Rect *rect)
{
	if ( surface && rect ) {
		*rect = surface->clip_rect;
	}
}


void Retro_FreeSurface(RSDL_Surface *surf )
{
   if (!surf)
      return;

#ifdef RETRO_DEBUG
   printf("free surf format palette color\n");
#endif
   if(surf->format->palette->colors)
      free(surf->format->palette->colors);

#ifdef RETRO_DEBUG
   printf("free surf format palette\n");
#endif
   if(surf->format->palette)
      free(surf->format->palette);

#ifdef RETRO_DEBUG
   printf("free surf format\n");
#endif
   if(surf->format)
      free(surf->format);

#ifdef RETRO_DEBUG
   printf("free surf pixel\n");
#endif
   if(surf->pixels)
      free(surf->pixels);
}


RSDL_Surface *Retro_CreateRGBSurface32( int w,int h, int d, int rm,int rg,int rb,int ra)
{
#ifdef RETRO_DEBUG
   printf("s(%d,%d,%d) (%x,%x,%x,%x)\n",w,h,d,rm,rg,rb,ra);
#endif

   RSDL_Surface *bitmp;

   bitmp = (RSDL_Surface *) calloc(1, sizeof(*bitmp));
   if (bitmp == NULL)
   {
      printf("tex surface failed");
      return NULL;
   }

   bitmp->format = (RSDL_PixelFormat *) calloc(1,sizeof(*bitmp->format));
   if (bitmp->format == NULL)
   {
      printf("tex format failed");
      return NULL;
   }

   bitmp->format->palette =(RSDL_Palette *) calloc(1,sizeof(*bitmp->format->palette));
   if (bitmp->format->palette == NULL)
   {
      printf("tex format palette failed");
      return NULL;
   }

#ifdef RETRO_DEBUG
  printf("create surface XR8G8B8 libretro\n");
#endif
//FIXME: why pal for 32 bits surface ?
  bitmp->format->palette->ncolors=256;
  bitmp->format->palette->colors=(RSDL_Color *)malloc(1024);
  bitmp->format->palette->version=0;
  bitmp->format->palette->refcount=0;
  memset(bitmp->format->palette->colors,0x0,1024);

   bitmp->format->BitsPerPixel = 32;
   bitmp->format->BytesPerPixel = 4;
   bitmp->format->Rloss=0;
   bitmp->format->Gloss=0;
   bitmp->format->Bloss=0;
   bitmp->format->Aloss=0;

   bitmp->format->Rshift=16;
   bitmp->format->Gshift=8;
   bitmp->format->Bshift=0;
   bitmp->format->Ashift=24;

   bitmp->format->Rmask=0x00ff0000;   
   bitmp->format->Gmask=0x0000ff00;
   bitmp->format->Bmask=0x000000ff;
   bitmp->format->Amask=0xff000000;

/*
   bitmp->format->Rshift=16;
   bitmp->format->Gshift=8;
   bitmp->format->Bshift=0;
   bitmp->format->Ashift=24;

   bitmp->format->Rmask=0x00ff0000;
   bitmp->format->Gmask=0x0000ff00;
   bitmp->format->Bmask=0x000000ff;
   bitmp->format->Amask=0xff000000;
*/   

   bitmp->format->colorkey=0;
   bitmp->format->alpha=255;//0;

   bitmp->flags=0;
   bitmp->w=w;
   bitmp->h=h;
   bitmp->pitch=w*4;
   bitmp->pixels=malloc(sizeof(unsigned char)*h*w*4);
   if (!bitmp->pixels) {
	    printf("failed alloc pixels\n");	
            Retro_FreeSurface(bitmp);            
            return NULL;
   }
   memset(bitmp->pixels,0, h*w*4);

   bitmp->clip_rect.x=0;
   bitmp->clip_rect.y=0;
   bitmp->clip_rect.w=w;
   bitmp->clip_rect.h=h;

   //printf("fin prepare tex:%dx%dx%d\n",bitmp->w,bitmp->h,bitmp->format->BytesPerPixel);
   return bitmp;

}

RSDL_Surface *Retro_CreateRGBSurface16( int w,int h, int d, int rm,int rg,int rb,int ra)
{
#ifdef RETRO_DEBUG
   printf("s(%d,%d,%d) (%x,%x,%x,%x)\n",w,h,d,rm,rg,rb,ra);
#endif

   RSDL_Surface *bitmp;

   bitmp = (RSDL_Surface *) calloc(1, sizeof(*bitmp));
   if (bitmp == NULL)
   {
      printf("tex surface failed");
      return NULL;
   }

   bitmp->format =(RSDL_PixelFormat *) calloc(1,sizeof(*bitmp->format));
   if (bitmp->format == NULL)
   {
      printf("tex format failed");
      return NULL;
   }

   bitmp->format->palette = (RSDL_Palette *)calloc(1,sizeof(*bitmp->format->palette));
   if (bitmp->format->palette == NULL)
   {
      printf("tex format palette failed");
      return NULL;
   }

#ifdef RETRO_DEBUG
  printf("create surface RGB565 libretro\n");
#endif
//FIXME: why pal for 32 bits surface ?
  bitmp->format->palette->ncolors=256;
  bitmp->format->palette->colors=(RSDL_Color *)malloc(256*2);
  bitmp->format->palette->version=0;
  bitmp->format->palette->refcount=0;
  memset(bitmp->format->palette->colors,0x0,256*2);

   bitmp->format->BitsPerPixel = 16;
   bitmp->format->BytesPerPixel = 2;
   bitmp->format->Rloss=3;
   bitmp->format->Gloss=2;
   bitmp->format->Bloss=3;
   bitmp->format->Aloss=0;

   bitmp->format->Rshift=11;
   bitmp->format->Gshift=5;
   bitmp->format->Bshift=0;
   bitmp->format->Ashift=0;

   bitmp->format->Rmask=0x0000F800;
   bitmp->format->Gmask=0x000007E0;
   bitmp->format->Bmask=0x0000001F;
   bitmp->format->Amask=0x00000000;

/*
   bitmp->format->Rshift=16;
   bitmp->format->Gshift=8;
   bitmp->format->Bshift=0;
   bitmp->format->Ashift=24;

   bitmp->format->Rmask=0x00ff0000;
   bitmp->format->Gmask=0x0000ff00;
   bitmp->format->Bmask=0x000000ff;
   bitmp->format->Amask=0xff000000;
*/   

   bitmp->format->colorkey=0;
   bitmp->format->alpha=255;//0;

   bitmp->flags=0;
   bitmp->w=w;
   bitmp->h=h;
   bitmp->pitch=w*2;
   bitmp->pixels=malloc(sizeof(unsigned char)*h*w*2);
   if (!bitmp->pixels) {
	    printf("failed alloc pixels\n");	
            Retro_FreeSurface(bitmp);            
            return NULL;
   }
   memset(bitmp->pixels,0, h*w*2);

   bitmp->clip_rect.x=0;
   bitmp->clip_rect.y=0;
   bitmp->clip_rect.w=w;
   bitmp->clip_rect.h=h;

   //printf("fin prepare tex:%dx%dx%d\n",bitmp->w,bitmp->h,bitmp->format->BytesPerPixel);
   return bitmp;
}


unsigned short blend(unsigned short fg, unsigned short bg, unsigned int alpha)
{
   // Split foreground into components
   unsigned fg_r = fg >> 11;
   unsigned fg_g = (fg >> 5) & ((1u << 6) - 1);
   unsigned fg_b = fg & ((1u << 5) - 1);

   // Split background into components
   unsigned bg_r = bg >> 11;
   unsigned bg_g = (bg >> 5) & ((1u << 6) - 1);
   unsigned bg_b = bg & ((1u << 5) - 1);

   // Alpha blend components
   unsigned out_r = (fg_r * alpha + bg_r * (255 - alpha)) / 255;
   unsigned out_g = (fg_g * alpha + bg_g * (255 - alpha)) / 255;
   unsigned out_b = (fg_b * alpha + bg_b * (255 - alpha)) / 255;

   // Pack result
   return (unsigned short) ((out_r << 11) | (out_g << 5) | out_b);
}

uint32_t blend32(uint32_t fg, uint32_t bg, unsigned int alpha)
{
   // Split foreground into components
   unsigned fg_r = (fg >> 16) & 0xFF;
   unsigned fg_g = (fg >> 8) & 0xFF;
   unsigned fg_b = (fg >> 0) & 0xFF;

   // Split background into components
   unsigned bg_r = (bg >> 16) & 0xFF;
   unsigned bg_g = (bg >> 8) & 0xFF;
   unsigned bg_b = (bg >> 0) & 0xFF;

   // Alpha blend components
   unsigned out_r = (fg_r * alpha + bg_r * (255 - alpha)) / 255;
   unsigned out_g = (fg_g * alpha + bg_g * (255 - alpha)) / 255;
   unsigned out_b = (fg_b * alpha + bg_b * (255 - alpha)) / 255;

   // Pack result
   return (uint32_t) ((out_r << 16) | (out_g << 8) | out_b);
}

void DrawFBoxBmp16(RSDL_Surface *surface, int x, int y, int dx, int dy, unsigned short color, unsigned int alpha)
{
   int i,j,idx;
   unsigned short *mbuffer = (unsigned short*)surface->pixels;

   for(i=x;i<x+dx;i++)
   {
      for(j=y;j<y+dy;j++)
      {
         idx=i+j*retrow;
         if (alpha < 255)
            mbuffer[idx]=blend(color, mbuffer[idx], alpha);
         else
            mbuffer[idx]=color;
      }
   }
}

void DrawFBoxBmp32(RSDL_Surface *surface, int x, int y, int dx, int dy, uint32_t color, unsigned int alpha)
{
   int i,j,idx;
   unsigned *mbuffer = (unsigned *)surface->pixels;

   for(i=x;i<x+dx;i++)
   {
      for(j=y;j<y+dy;j++)
      {
         idx=i+j*retrow;
         if (alpha < 255)
            mbuffer[idx]=blend32(color, mbuffer[idx], alpha);
         else
            mbuffer[idx]=color;
      }
   }
}

#include "font.c"

void Retro_Draw_string16(RSDL_Surface *surface, signed short int x, signed short int y,
    const char *string, unsigned short maxstrlen, unsigned short xscale, unsigned short yscale, unsigned short fg, unsigned short bg)
{
    int k, strlen;
    unsigned char *linesurf;

    int col, bit;
    unsigned char b;

    int xrepeat, yrepeat;
    signed short int ypixel;
    unsigned short *yptr;
    unsigned short *mbuffer = (unsigned short*)surface->pixels;

    #define VIRTUAL_WIDTH surface->w

    if ((surface->clip_rect.w == 0) || (surface->clip_rect.h == 0)) {
        return;
    }

    #define charWidthLocal 8
    #define charHeightLocal 8

    Sint16 left, right, top, bottom;
    Sint16 x1, y1, x2, y2;

    left = surface->clip_rect.x;
    x2 = x + charWidthLocal;
    if (x2 < left) {
        return;
    }
    right = surface->clip_rect.x + surface->clip_rect.w - 1;
    x1 = x;
    if (x1 > right) {
        return;
    }
    top = surface->clip_rect.y;
    y2 = y + charHeightLocal;
    if (y2 < top) {
        return;
    }
    bottom = surface->clip_rect.y + surface->clip_rect.h - 1;
    y1 = y;
    if (y1 > bottom) {
        return;
    }

    if(string == NULL) return;
    for(strlen = 0; strlen < maxstrlen && string[strlen]; strlen++) {}

    int surfw = strlen * 7 * xscale;
    int surfh = 8 * yscale;

    linesurf = (unsigned char *)malloc(sizeof(unsigned short)*surfw*surfh);
    yptr = (unsigned short *)&linesurf[0];

    // Skip the last row
    surfh--;

    for(ypixel = 0; ypixel < 8; ypixel++) {
        for(col = 0; col < strlen; col++) {
            b = font_array[(string[col]^0x80)*8 + ypixel];
            for(bit = 0; bit < 7; bit++, yptr++) {
                *yptr = (b & (1<<(7-bit))) ? fg : bg;
                for(xrepeat = 1; xrepeat < xscale; xrepeat++, yptr++)
                    yptr[1] = *yptr;
            }
        }

        for(yrepeat = 1; yrepeat < yscale; yrepeat++)
            for(xrepeat = 0; xrepeat < surfw; xrepeat++, yptr++)
                *yptr = yptr[-surfw];
    }

    yptr = (unsigned short *)&linesurf[0];

    for(yrepeat = y; yrepeat < y + surfh; yrepeat++)
        for(xrepeat = x; xrepeat < x + surfw; xrepeat++, yptr++)
            if(*yptr != 0 && (xrepeat + yrepeat*VIRTUAL_WIDTH) < surface->w*surface->h)
                mbuffer[xrepeat+yrepeat*VIRTUAL_WIDTH] = *yptr;

    free(linesurf);
}

void Retro_Draw_string32(RSDL_Surface *surface, signed short int x, signed short int y,
    const char *string, unsigned short maxstrlen, unsigned short xscale, unsigned short yscale, unsigned fg, unsigned bg)
{
    int k, strlen;
    unsigned char *linesurf;

    int col, bit;
    unsigned char b;

    int xrepeat, yrepeat;
    signed int ypixel;
    unsigned *yptr;
    unsigned *mbuffer = (unsigned *)surface->pixels;

    #define VIRTUAL_WIDTH surface->w

    if ((surface->clip_rect.w==0) || (surface->clip_rect.h==0)) {
        return;
    }

    #define charWidthLocal 8
    #define charHeightLocal 8

    Sint16 left, right, top, bottom;
    Sint16 x1, y1, x2, y2;

    left = surface->clip_rect.x;
    x2 = x + charWidthLocal;
    if (x2 < left) {
        return;
    }
    right = surface->clip_rect.x + surface->clip_rect.w - 1;
    x1 = x;
    if (x1 > right) {
        return;
    }
    top = surface->clip_rect.y;
    y2 = y + charHeightLocal;
    if (y2 < top) {
        return;
    }
    bottom = surface->clip_rect.y + surface->clip_rect.h - 1;
    y1 = y;
    if (y1 > bottom) {
        return;
    }

    if(string == NULL) return;
    for(strlen = 0; strlen < maxstrlen && string[strlen]; strlen++) {}

    int surfw = strlen * 7 * xscale;
    int surfh = 8 * yscale;

    linesurf = (unsigned char *)malloc(sizeof(unsigned)*surfw*surfh);
    yptr = (unsigned *)&linesurf[0];

    // Skip the last row
    surfh--;

    for(ypixel = 0; ypixel < 8; ypixel++) {
        for(col = 0; col < strlen; col++) {
            b = font_array[(string[col]^0x80)*8 + ypixel];
            for(bit = 0; bit < 7; bit++, yptr++) {
                *yptr = (b & (1<<(7-bit))) ? fg : bg;
                for(xrepeat = 1; xrepeat < xscale; xrepeat++, yptr++)
                    yptr[1] = *yptr;
            }
        }

        for(yrepeat = 1; yrepeat < yscale; yrepeat++)
            for(xrepeat = 0; xrepeat < surfw; xrepeat++, yptr++)
                *yptr = yptr[-surfw];
    }

    yptr = (unsigned *)&linesurf[0];

    for(yrepeat = y; yrepeat < y + surfh; yrepeat++)
        for(xrepeat = x; xrepeat < x + surfw; xrepeat++, yptr++)
            if(*yptr != 0 && (xrepeat + yrepeat*VIRTUAL_WIDTH) < surface->w*surface->h)
                mbuffer[xrepeat+yrepeat*VIRTUAL_WIDTH] = *yptr;

    free(linesurf);
}

void Retro_Draw_char16(RSDL_Surface *surface, signed short int x, signed short int y,
    char string, unsigned short xscale, unsigned short yscale, unsigned short fg, unsigned short bg)
{
    int k, strlen;
    unsigned char *linesurf;
    int col, bit;
    unsigned char b;

    int xrepeat, yrepeat;

    signed short int ypixel;
    unsigned short *yptr;
    unsigned short *mbuffer = (unsigned short*)surface->pixels;

    #define VIRTUAL_WIDTH surface->w

    if ((surface->clip_rect.w==0) || (surface->clip_rect.h==0)) {
        return;
    }

    #define charWidthLocal2 7*xscale
    #define charHeightLocal2 8*yscale

    Sint16 left, right, top, bottom;
    Sint16 x1, y1, x2, y2;

    left = surface->clip_rect.x;
    x2 = x + charWidthLocal2;
    if (x2 < left) {
        return;
    }
    right = surface->clip_rect.x + surface->clip_rect.w - 1;
    x1 = x;
    if (x1 > right) {
        return;
    }
    top = surface->clip_rect.y;
    y2 = y + charHeightLocal2;
    if (y2 < top) {
        return;
    }
    bottom = surface->clip_rect.y + surface->clip_rect.h - 1;
    y1 = y;
    if (y1 > bottom) {
        return;
    }

    strlen = 1;

    int surfw = strlen * 7 * xscale;
    int surfh = 8 * yscale;

    linesurf = (unsigned char *)malloc(sizeof(unsigned short)*surfw*surfh);
    yptr = (unsigned short *)&linesurf[0];

    for(ypixel = 0; ypixel < 8; ypixel++) {
        b = font_array[(string^0x80)*8 + ypixel];
        for(bit = 0; bit < 7; bit++, yptr++) {
            *yptr = (b & (1<<(7-bit))) ? fg : bg;
            for(xrepeat = 1; xrepeat < xscale; xrepeat++, yptr++)
                yptr[1] = *yptr;
        }

        for(yrepeat = 1; yrepeat < yscale; yrepeat++)
            for(xrepeat = 0; xrepeat < surfw; xrepeat++, yptr++)
                *yptr = yptr[-surfw];
    }

    yptr = (unsigned short*)&linesurf[0];

    for(yrepeat = y; yrepeat < y + surfh; yrepeat++)
        for(xrepeat = x; xrepeat < x + surfw; xrepeat++, yptr++)
            if(*yptr != 0 && (xrepeat + yrepeat*VIRTUAL_WIDTH) < surface->w*surface->h)
                mbuffer[xrepeat+yrepeat*VIRTUAL_WIDTH] = *yptr;

    free(linesurf);
}

void Retro_Draw_char32(RSDL_Surface *surface, signed short int x, signed short int y,
    char string, unsigned short xscale, unsigned short yscale, unsigned fg, unsigned bg)
{
    int k, strlen;
    unsigned char *linesurf;
    int col, bit;
    unsigned char b;

    int xrepeat, yrepeat;

    signed int ypixel;
    unsigned *yptr;
    unsigned *mbuffer = (unsigned*)surface->pixels;

    #define VIRTUAL_WIDTH surface->w

    if ((surface->clip_rect.w==0) || (surface->clip_rect.h==0)) {
        return;
    }

    #define charWidthLocal2 7*xscale
    #define charHeightLocal2 8*yscale

    Sint16 left, right, top, bottom;
    Sint16 x1, y1, x2, y2;

    left = surface->clip_rect.x;
    x2 = x + charWidthLocal2;
    if (x2 < left) {
        return;
    }
    right = surface->clip_rect.x + surface->clip_rect.w - 1;
    x1 = x;
    if (x1 > right) {
        return;
    }
    top = surface->clip_rect.y;
    y2 = y + charHeightLocal2;
    if (y2 < top) {
        return;
    }
    bottom = surface->clip_rect.y + surface->clip_rect.h - 1;
    y1 = y;
    if (y1 > bottom) {
        return;
    }

    strlen = 1;

    int surfw = strlen * 7 * xscale;
    int surfh = 8 * yscale;

    linesurf = (unsigned char *)malloc(sizeof(unsigned)*surfw*surfh);
    yptr = (unsigned *)&linesurf[0];

    for(ypixel = 0; ypixel < 8; ypixel++) {
        b = font_array[(string^0x80)*8 + ypixel];
        for(bit = 0; bit < 7; bit++, yptr++) {
            *yptr = (b & (1<<(7-bit))) ? fg : bg;
            for(xrepeat = 1; xrepeat < xscale; xrepeat++, yptr++)
                yptr[1] = *yptr;
        }

        for(yrepeat = 1; yrepeat < yscale; yrepeat++)
            for(xrepeat = 0; xrepeat < surfw; xrepeat++, yptr++)
                *yptr = yptr[-surfw];
    }

    yptr = (unsigned *)&linesurf[0];

    for(yrepeat = y; yrepeat < y + surfh; yrepeat++)
        for(xrepeat = x; xrepeat < x + surfw; xrepeat++, yptr++)
            if(*yptr != 0 && (xrepeat + yrepeat*VIRTUAL_WIDTH) < surface->w*surface->h)
                mbuffer[xrepeat+yrepeat*VIRTUAL_WIDTH] = *yptr;

    free(linesurf);
}

/*
 * Util.h -- a merge from Uwe's ScalVico and Klaus' h263
 *
 * 15.12.97, jw.
 */
#ifndef _UTIL_
#define _UTIL_

#include <malloc.h>
#include "error.h"
#include <stdlib.h>
#include <stdio.h>

/***********************/
/* one-argument macros */
/***********************/
#ifdef ABS
#undef ABS
#endif
#define ABS(a)		(((a)<0) ? -(a) : (a))

#ifdef MAX
#undef MAX
#endif
#define  MAX(a, b)      ( (a) > (b) ? (a) : (b) )
#ifdef MIN
#undef MIN
#endif
#define  MIN(a, b)      ( (a) < (b) ? (a) : (b) )

/* round a to nearest integer towards 0 */
#define FLOOR(a)		((a)>0 ? (int)(a) : -(int)(-a))

/* round a to nearest integer away from 0 */
#define CEILING(a) \
((a)==(int)(a) ? (a) : (a)>0 ? 1+(int)(a) : -(1+(int)(-a)))

/* round a to nearest int */

#ifndef ROUND
#define ROUND(a)	((a)>0 ? (int)((a)+0.5) : -(int)(0.5-(a)))		
#endif

/* take sign of a, either -1, 0, or 1 */
#define ZSGN(a)		(((a)<0) ? -1 : (a)>0 ? 1 : 0)	

/* take binary sign of a, either -1, or 1 if >= 0 */
#define SGN(a)		(((a)<0) ? -1 : 1)

/* square a */
#define SQR(a)		((a)*(a))	

/* logarithmus dualis */
#define LN_2            0.69314718
#define LD(a)           (log(a)/LN_2)

/***********************/
/* two-argument macros */
/***********************/

/* swap a and b (see Gem by Wyvill) */
#define SWAP(a,b)	{ a^=b; b^=a; a^=b; }

/* linear interpolation from l (when a=0) to h (when a=1)*/
/* (equal to (a*h)+((1-a)*l) */
#define LERP(a,l,h)	((l)+(((h)-(l))*(a)))

/* clamp the input to the specified range */
#define CLAMP(v,l,h)   ((v)<(l) ? (l) : ((v) > (h) ? (h) : v))
#define CLIP(v,l,h)    ((v)<(l) ? (l) : ((v) > (h) ? (h) : v))
     /*#define CLIPBYTE(v)    ((v)<(0) ? (0) : ((v) > (255) ? (255) : v))*/



#ifdef DEFINE_GLOBALS
#define GLOBAL
#define INIT(x) = x
#else
#define GLOBAL extern
#define INIT(x)
#endif


#ifndef NEW
#define NEW(x) (x *) malloc (sizeof(x))
#endif
#define NEWELEMS(x,n) (x *) malloc ((n)*sizeof(x))

#define ERRORS(file, line, text) {fprintf(stderr, file "Line %d; " text, \
					 line);exit(1);}

#define EXIT_WITH_ERR(message) ERRORS(__FILE__, __LINE__, message)

#ifdef ASSERT
#undef ASSERT
#endif
#define ASSERT(x)               if (!(x)) {EXIT_WITH_ERR("ASSERT failt\n");}

#define MAX_NEG_CROP 384
#define NUM_CROP_ENTRIES (256+2*MAX_NEG_CROP)

/* #define ROUNDCROP(val) (cropTbl[MAX_NEG_CROP+(int)(val)])*/
#define CROPBYTE(val)      (clp[(val)])
#define ROUNDCROPBYTE(val) (rclp[(int)(2.0*(val))])

#define ICROPBYTE(val)     (iclp[(val)])
#define ROUNDICROPBYTE(val) (iclp[(int)(val)])


/* if PROTOCOL is defined a lot of information can be written to the protocol
   file if the commandline argument is also chosen */
#define PROTOCOL
#ifdef PROTOCOL
GLOBAL int  protFormat_g;
GLOBAL FILE *fp_prot_g;
#endif

/* if PROTOCOL is defined a lot of information can be written to the protocol
   file if the commandline argument is also chosen */
#undef COUNT_BITS
#ifdef COUNT_BITS
struct bit_count_struct {
  int totalFrames;
  int totalBits;
  int headerBits;
  int motionBits;
  int lumBits;
  int lum1Bits;
  int lum2Bits;
  int lum3Bits;
  int chromBits;
};

GLOBAL struct bit_count_struct *bitCount;
GLOBAL struct bit_count_struct *bitCountP;
#endif

/* Quantization tables for H.263 */
GLOBAL unsigned char quantInit INIT(0);
GLOBAL signed char quant_table_0[4081];
GLOBAL signed char *quant_table INIT(quant_table_0 + 2040);
GLOBAL signed short dequant_table_0[255];
GLOBAL signed short *dequant_table INIT(dequant_table_0 + 127);


/* Clipping tables */
GLOBAL const unsigned char clip_pos[1024]
#ifdef DEFINE_GLOBALS
 = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,
  19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,
  44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,
  69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,
  94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,
  114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,
  133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,
  152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,
  171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,
  190,191,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,
  209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,224,225,226,227,
  228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,245,246,
  247,248,249,250,251,252,253,254,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255}
#endif
;
GLOBAL const unsigned char *clp INIT(clip_pos + 384);

GLOBAL const unsigned char rclip_pos[2048]
#ifdef DEFINE_GLOBALS
 = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,2,2,3,3,4,4,
  5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13,14,14,15,15,16,16,17,17,18,18,
  19,19,20,20,21,21,22,22,23,23,24,24,25,25,26,26,27,27,28,28,29,29,30,30,
  31,31,32,32,33,33,34,34,35,35,36,36,37,37,38,38,39,39,40,40,41,41,42,42,
  43,43,44,44,45,45,46,46,47,47,48,48,49,49,50,50,51,51,52,52,53,53,54,54,
  55,55,56,56,57,57,58,58,59,59,60,60,61,61,62,62,63,63,64,64,65,65,66,66,
  67,67,68,68,69,69,70,70,71,71,72,72,73,73,74,74,75,75,76,76,77,77,78,78,
  79,79,80,80,81,81,82,82,83,83,84,84,85,85,86,86,87,87,88,88,89,89,90,90,
  91,91,92,92,93,93,94,94,95,95,96,96,97,97,98,98,99,99,100,100,101,101,
  102,102,103,103,104,104,105,105,106,106,107,107,108,108,109,109,110,110,
  111,111,112,112,113,113,114,114,115,115,116,116,117,117,118,118,119,119,
  120,120,121,121,122,122,123,123,124,124,125,125,126,126,127,127,128,128,
  129,129,130,130,131,131,132,132,133,133,134,134,135,135,136,136,137,137,
  138,138,139,139,140,140,141,141,142,142,143,143,144,144,145,145,146,146,
  147,147,148,148,149,149,150,150,151,151,152,152,153,153,154,154,155,155,
  156,156,157,157,158,158,159,159,160,160,161,161,162,162,163,163,164,164,
  165,165,166,166,167,167,168,168,169,169,170,170,171,171,172,172,173,173,
  174,174,175,175,176,176,177,177,178,178,179,179,180,180,181,181,182,182,
  183,183,184,184,185,185,186,186,187,187,188,188,189,189,190,190,191,191,
  192,192,193,193,194,194,195,195,196,196,197,197,198,198,199,199,200,200,
  201,201,202,202,203,203,204,204,205,205,206,206,207,207,208,208,209,209,
  210,210,211,211,212,212,213,213,214,214,215,215,216,216,217,217,218,218,
  219,219,220,220,221,221,222,222,223,223,224,224,225,225,226,226,227,227,
  228,228,229,229,230,230,231,231,232,232,233,233,234,234,235,235,236,236,
  237,237,238,238,239,239,240,240,241,241,242,242,243,243,244,244,245,245,
  246,246,247,247,248,248,249,249,250,250,251,251,252,252,253,253,254,254,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  255,255,255,255,255,255,255,255,255,255,255,255,255,255,255}
#endif
;
GLOBAL const unsigned char *rclp INIT(rclip_pos + 768);

GLOBAL const short iclip_pos[1024]
#ifdef DEFINE_GLOBALS
 = {
   -256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,
   -256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,
   -256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,
   -256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,
   -256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,
   -256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,
   -256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,
   -256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,
   -256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,
   -256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,
   -256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,
   -256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,
   -256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,
   -256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,
   -256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,
   -256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,
   -256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,
   -256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,-256,
   -256,-256,-256,-256,-256,-255,-254,-253,-252,-251,-250,-249,-248,-247,
   -246,-245,-244,-243,-242,-241,-240,-239,-238,-237,-236,-235,-234,-233,
   -232,-231,-230,-229,-228,-227,-226,-225,-224,-223,-222,-221,-220,-219,
   -218,-217,-216,-215,-214,-213,-212,-211,-210,-209,-208,-207,-206,-205,
   -204,-203,-202,-201,-200,-199,-198,-197,-196,-195,-194,-193,-192,-191,
   -190,-189,-188,-187,-186,-185,-184,-183,-182,-181,-180,-179,-178,-177,
   -176,-175,-174,-173,-172,-171,-170,-169,-168,-167,-166,-165,-164,-163,
   -162,-161,-160,-159,-158,-157,-156,-155,-154,-153,-152,-151,-150,-149,
   -148,-147,-146,-145,-144,-143,-142,-141,-140,-139,-138,-137,-136,-135,
   -134,-133,-132,-131,-130,-129,-128,-127,-126,-125,-124,-123,-122,-121,
   -120,-119,-118,-117,-116,-115,-114,-113,-112,-111,-110,-109,-108,-107,
   -106,-105,-104,-103,-102,-101,-100,-99,-98,-97,-96,-95,-94,-93,-92,-91,
   -90,-89,-88,-87,-86,-85,-84,-83,-82,-81,-80,-79,-78,-77,-76,-75,-74,-73,
   -72,-71,-70,-69,-68,-67,-66,-65,-64,-63,-62,-61,-60,-59,-58,-57,-56,-55,
   -54,-53,-52,-51,-50,-49,-48,-47,-46,-45,-44,-43,-42,-41,-40,-39,-38,-37,
   -36,-35,-34,-33,-32,-31,-30,-29,-28,-27,-26,-25,-24,-23,-22,-21,-20,-19,
   -18,-17,-16,-15,-14,-13,-12,-11,-10,-9,-8,-7,-6,-5,-4,-3,-2,-1,0,1,2,3,
   4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,
   30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,
   54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,
   78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,
   101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,
   119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,
   137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,
   155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,
   173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,
   191,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,
   209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,224,225,226,
   227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,
   245,246,247,248,249,250,251,252,253,254,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255}
#endif
;
GLOBAL const short *iclp INIT(iclip_pos + 512);


#if 0
GLOBAL const short mvd_clip_pos[127]
#ifdef DEFINE_GLOBALS
 = {
   1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,
   28,29,30,31,
   -32,-31,-30,-29,-28,-27,-26,-25,-24,-23,-22,-21,-20,-19,-18,-17,-16,-15,
   -14,-13,-12,-11,-10,-9,-8,-7,-6,-5,-4,-3,-2,-1,0,
   1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,
   28,29,30,31,
   -32,-31,-30,-29,-28,-27,-26,-25,-24,-23,-22,-21,-20,-19,-18,-17,-16,-15,
   -14,-13,-12,-11,-10,-9,-8,-7,-6,-5,-4,-3,-2,-1}
#endif
;
GLOBAL const short *mvd_clp INIT(mvd_clip_pos + 63);
#endif


GLOBAL short spiral[961][2]
#ifdef DEFINE_GLOBALS
 = {
{0  	,0  },
{-1	,-1 }, 
{0	,-1 }, 
{1	,-1 }, 
{1	,0  }, 
{1	,1  }, 
{0	,1  }, 
{-1	,1  }, 
{-1	,0  }, 
{-2	,-2 }, 
{-1	,-2 }, 
{0	,-2 }, 
{1	,-2 }, 
{2	,-2 }, 
{2	,-1 }, 
{2	,0  }, 
{2	,1  }, 
{2	,2  }, 
{1	,2  }, 
{0	,2  }, 
{-1	,2  }, 
{-2	,2  }, 
{-2	,1  }, 
{-2	,0  }, 
{-2	,-1 }, 
{-3	,-3 }, 
{-2	,-3 }, 
{-1	,-3 }, 
{0	,-3 }, 
{1	,-3 }, 
{2	,-3 }, 
{3	,-3 }, 
{3	,-2 }, 
{3	,-1 }, 
{3	,0  }, 
{3	,1  }, 
{3	,2  }, 
{3	,3  }, 
{2	,3  }, 
{1	,3  }, 
{0	,3  }, 
{-1	,3  }, 
{-2	,3  }, 
{-3	,3  }, 
{-3	,2  }, 
{-3	,1  }, 
{-3	,0  }, 
{-3	,-1 }, 
{-3	,-2 }, 
{-4	,-4 }, 
{-3	,-4 }, 
{-2	,-4 }, 
{-1	,-4 }, 
{0	,-4 }, 
{1	,-4 }, 
{2	,-4 }, 
{3	,-4 }, 
{4	,-4 }, 
{4	,-3 }, 
{4	,-2 }, 
{4	,-1 }, 
{4	,0  }, 
{4	,1  }, 
{4	,2  }, 
{4	,3  }, 
{4	,4  }, 
{3	,4  }, 
{2	,4  }, 
{1	,4  }, 
{0	,4  }, 
{-1	,4  }, 
{-2	,4  }, 
{-3	,4  }, 
{-4	,4  }, 
{-4	,3  }, 
{-4	,2  }, 
{-4	,1  }, 
{-4	,0  }, 
{-4	,-1 }, 
{-4	,-2 }, 
{-4	,-3 }, 
{-5	,-5 }, 
{-4	,-5 }, 
{-3	,-5 }, 
{-2	,-5 }, 
{-1	,-5 }, 
{0	,-5 }, 
{1	,-5 }, 
{2	,-5 }, 
{3	,-5 }, 
{4	,-5 }, 
{5	,-5 }, 
{5	,-4 }, 
{5	,-3 }, 
{5	,-2 }, 
{5	,-1 }, 
{5	,0  }, 
{5	,1  }, 
{5	,2  }, 
{5	,3  }, 
{5	,4  }, 
{5	,5  }, 
{4	,5  }, 
{3	,5  }, 
{2	,5  }, 
{1	,5  }, 
{0	,5  }, 
{-1	,5  }, 
{-2	,5  }, 
{-3	,5  }, 
{-4	,5  }, 
{-5	,5  }, 
{-5	,4  }, 
{-5	,3  }, 
{-5	,2  }, 
{-5	,1  }, 
{-5	,0  }, 
{-5	,-1 }, 
{-5	,-2 }, 
{-5	,-3 }, 
{-5	,-4 }, 
{-6	,-6 }, 
{-5	,-6 }, 
{-4	,-6 }, 
{-3	,-6 }, 
{-2	,-6 }, 
{-1	,-6 }, 
{0	,-6 }, 
{1	,-6 }, 
{2	,-6 }, 
{3	,-6 }, 
{4	,-6 }, 
{5	,-6 }, 
{6	,-6 }, 
{6	,-5 }, 
{6	,-4 }, 
{6	,-3 }, 
{6	,-2 }, 
{6	,-1 }, 
{6	,0  }, 
{6	,1  }, 
{6	,2  }, 
{6	,3  }, 
{6	,4  }, 
{6	,5  }, 
{6	,6  }, 
{5	,6  }, 
{4	,6  }, 
{3	,6  }, 
{2	,6  }, 
{1	,6  }, 
{0	,6  }, 
{-1	,6  }, 
{-2	,6  }, 
{-3	,6  }, 
{-4	,6  }, 
{-5	,6  }, 
{-6	,6  }, 
{-6	,5  }, 
{-6	,4  }, 
{-6	,3  }, 
{-6	,2  }, 
{-6	,1  }, 
{-6	,0  }, 
{-6	,-1 }, 
{-6	,-2 }, 
{-6	,-3 }, 
{-6	,-4 }, 
{-6	,-5 }, 
{-7	,-7 }, 
{-6	,-7 }, 
{-5	,-7 }, 
{-4	,-7 }, 
{-3	,-7 }, 
{-2	,-7 }, 
{-1	,-7 }, 
{0	,-7 }, 
{1	,-7 }, 
{2	,-7 }, 
{3	,-7 }, 
{4	,-7 }, 
{5	,-7 }, 
{6	,-7 }, 
{7	,-7 }, 
{7	,-6 }, 
{7	,-5 }, 
{7	,-4 }, 
{7	,-3 }, 
{7	,-2 }, 
{7	,-1 }, 
{7	,0  }, 
{7	,1  }, 
{7	,2  }, 
{7	,3  }, 
{7	,4  }, 
{7	,5  }, 
{7	,6  }, 
{7	,7  }, 
{6	,7  }, 
{5	,7  }, 
{4	,7  }, 
{3	,7  }, 
{2	,7  }, 
{1	,7  }, 
{0	,7  }, 
{-1	,7  }, 
{-2	,7  }, 
{-3	,7  }, 
{-4	,7  }, 
{-5	,7  }, 
{-6	,7  }, 
{-7	,7  }, 
{-7	,6  }, 
{-7	,5  }, 
{-7	,4  }, 
{-7	,3  }, 
{-7	,2  }, 
{-7	,1  }, 
{-7	,0  }, 
{-7	,-1 }, 
{-7	,-2 }, 
{-7	,-3 }, 
{-7	,-4 }, 
{-7	,-5 }, 
{-7	,-6 }, 
{-8	,-8 }, 
{-7	,-8 }, 
{-6	,-8 }, 
{-5	,-8 }, 
{-4	,-8 }, 
{-3	,-8 }, 
{-2	,-8 }, 
{-1	,-8 }, 
{0	,-8 }, 
{1	,-8 }, 
{2	,-8 }, 
{3	,-8 }, 
{4	,-8 }, 
{5	,-8 }, 
{6	,-8 }, 
{7	,-8 }, 
{8	,-8 }, 
{8	,-7 }, 
{8	,-6 }, 
{8	,-5 }, 
{8	,-4 }, 
{8	,-3 }, 
{8	,-2 }, 
{8	,-1 }, 
{8	,0  }, 
{8	,1  }, 
{8	,2  }, 
{8	,3  }, 
{8	,4  }, 
{8	,5  }, 
{8	,6  }, 
{8	,7  }, 
{8	,8  }, 
{7	,8  }, 
{6	,8  }, 
{5	,8  }, 
{4	,8  }, 
{3	,8  }, 
{2	,8  }, 
{1	,8  }, 
{0	,8  }, 
{-1	,8  }, 
{-2	,8  }, 
{-3	,8  }, 
{-4	,8  }, 
{-5	,8  }, 
{-6	,8  }, 
{-7	,8  }, 
{-8	,8  }, 
{-8	,7  }, 
{-8	,6  }, 
{-8	,5  }, 
{-8	,4  }, 
{-8	,3  }, 
{-8	,2  }, 
{-8	,1  }, 
{-8	,0  }, 
{-8	,-1 }, 
{-8	,-2 }, 
{-8	,-3 }, 
{-8	,-4 }, 
{-8	,-5 }, 
{-8	,-6 }, 
{-8	,-7 }, 
{-9	,-9 }, 
{-8	,-9 }, 
{-7	,-9 }, 
{-6	,-9 }, 
{-5	,-9 }, 
{-4	,-9 }, 
{-3	,-9 }, 
{-2	,-9 }, 
{-1	,-9 }, 
{0	,-9 }, 
{1	,-9 }, 
{2	,-9 }, 
{3	,-9 }, 
{4	,-9 }, 
{5	,-9 }, 
{6	,-9 }, 
{7	,-9 }, 
{8	,-9 }, 
{9	,-9 }, 
{9	,-8 }, 
{9	,-7 }, 
{9	,-6 }, 
{9	,-5 }, 
{9	,-4 }, 
{9	,-3 }, 
{9	,-2 }, 
{9	,-1 }, 
{9	,0  }, 
{9	,1  }, 
{9	,2  }, 
{9	,3  }, 
{9	,4  }, 
{9	,5  }, 
{9	,6  }, 
{9	,7  }, 
{9	,8  }, 
{9	,9  }, 
{8	,9  }, 
{7	,9  }, 
{6	,9  }, 
{5	,9  }, 
{4	,9  }, 
{3	,9  }, 
{2	,9  }, 
{1	,9  }, 
{0	,9  }, 
{-1	,9  }, 
{-2	,9  }, 
{-3	,9  }, 
{-4	,9  }, 
{-5	,9  }, 
{-6	,9  }, 
{-7	,9  }, 
{-8	,9  }, 
{-9	,9  }, 
{-9	,8  }, 
{-9	,7  }, 
{-9	,6  }, 
{-9	,5  }, 
{-9	,4  }, 
{-9	,3  }, 
{-9	,2  }, 
{-9	,1  }, 
{-9	,0  }, 
{-9	,-1 }, 
{-9	,-2 }, 
{-9	,-3 }, 
{-9	,-4 }, 
{-9	,-5 }, 
{-9	,-6 }, 
{-9	,-7 }, 
{-9	,-8 }, 
{-10	,-10}, 
{-9	,-10}, 
{-8	,-10}, 
{-7	,-10}, 
{-6	,-10}, 
{-5	,-10}, 
{-4	,-10}, 
{-3	,-10}, 
{-2	,-10}, 
{-1	,-10}, 
{0	,-10}, 
{1	,-10}, 
{2	,-10}, 
{3	,-10}, 
{4	,-10}, 
{5	,-10}, 
{6	,-10}, 
{7	,-10}, 
{8	,-10}, 
{9	,-10}, 
{10	,-10}, 
{10	,-9 }, 
{10	,-8 }, 
{10	,-7 }, 
{10	,-6 }, 
{10	,-5 }, 
{10	,-4 }, 
{10	,-3 }, 
{10	,-2 }, 
{10	,-1 }, 
{10	,0  }, 
{10	,1  }, 
{10	,2  }, 
{10	,3  }, 
{10	,4  }, 
{10	,5  }, 
{10	,6  }, 
{10	,7  }, 
{10	,8  }, 
{10	,9  }, 
{10	,10 }, 
{9	,10 }, 
{8	,10 }, 
{7	,10 }, 
{6	,10 }, 
{5	,10 }, 
{4	,10 }, 
{3	,10 }, 
{2	,10 }, 
{1	,10 }, 
{0	,10 }, 
{-1	,10 }, 
{-2	,10 }, 
{-3	,10 }, 
{-4	,10 }, 
{-5	,10 }, 
{-6	,10 }, 
{-7	,10 }, 
{-8	,10 }, 
{-9	,10 }, 
{-10	,10 }, 
{-10	,9  }, 
{-10	,8  }, 
{-10	,7  }, 
{-10	,6  }, 
{-10	,5  }, 
{-10	,4  }, 
{-10	,3  }, 
{-10	,2  }, 
{-10	,1  }, 
{-10	,0  }, 
{-10	,-1 }, 
{-10	,-2 }, 
{-10	,-3 }, 
{-10	,-4 }, 
{-10	,-5 }, 
{-10	,-6 }, 
{-10	,-7 }, 
{-10	,-8 }, 
{-10	,-9 }, 
{-11	,-11}, 
{-10	,-11}, 
{-9	,-11}, 
{-8	,-11}, 
{-7	,-11}, 
{-6	,-11}, 
{-5	,-11}, 
{-4	,-11}, 
{-3	,-11}, 
{-2	,-11}, 
{-1	,-11}, 
{0	,-11}, 
{1	,-11}, 
{2	,-11}, 
{3	,-11}, 
{4	,-11}, 
{5	,-11}, 
{6	,-11}, 
{7	,-11}, 
{8	,-11}, 
{9	,-11}, 
{10	,-11}, 
{11	,-11}, 
{11	,-10}, 
{11	,-9 }, 
{11	,-8 }, 
{11	,-7 }, 
{11	,-6 }, 
{11	,-5 }, 
{11	,-4 }, 
{11	,-3 }, 
{11	,-2 }, 
{11	,-1 }, 
{11	,0  }, 
{11	,1  }, 
{11	,2  }, 
{11	,3  }, 
{11	,4  }, 
{11	,5  }, 
{11	,6  }, 
{11	,7  }, 
{11	,8  }, 
{11	,9  }, 
{11	,10 }, 
{11	,11 }, 
{10	,11 }, 
{9	,11 }, 
{8	,11 }, 
{7	,11 }, 
{6	,11 }, 
{5	,11 }, 
{4	,11 }, 
{3	,11 }, 
{2	,11 }, 
{1	,11 }, 
{0	,11 }, 
{-1	,11 }, 
{-2	,11 }, 
{-3	,11 }, 
{-4	,11 }, 
{-5	,11 }, 
{-6	,11 }, 
{-7	,11 }, 
{-8	,11 }, 
{-9	,11 }, 
{-10	,11 }, 
{-11	,11 }, 
{-11	,10 }, 
{-11	,9  }, 
{-11	,8  }, 
{-11	,7  }, 
{-11	,6  }, 
{-11	,5  }, 
{-11	,4  }, 
{-11	,3  }, 
{-11	,2  }, 
{-11	,1  }, 
{-11	,0  }, 
{-11	,-1 }, 
{-11	,-2 }, 
{-11	,-3 }, 
{-11	,-4 }, 
{-11	,-5 }, 
{-11	,-6 }, 
{-11	,-7 }, 
{-11	,-8 }, 
{-11	,-9 }, 
{-11	,-10}, 
{-12	,-12}, 
{-11	,-12}, 
{-10	,-12}, 
{-9	,-12}, 
{-8	,-12}, 
{-7	,-12}, 
{-6	,-12}, 
{-5	,-12}, 
{-4	,-12}, 
{-3	,-12}, 
{-2	,-12}, 
{-1	,-12}, 
{0	,-12}, 
{1	,-12}, 
{2	,-12}, 
{3	,-12}, 
{4	,-12}, 
{5	,-12}, 
{6	,-12}, 
{7	,-12}, 
{8	,-12}, 
{9	,-12}, 
{10	,-12}, 
{11	,-12}, 
{12	,-12}, 
{12	,-11}, 
{12	,-10}, 
{12	,-9 }, 
{12	,-8 }, 
{12	,-7 }, 
{12	,-6 }, 
{12	,-5 }, 
{12	,-4 }, 
{12	,-3 }, 
{12	,-2 }, 
{12	,-1 }, 
{12	,0  }, 
{12	,1  }, 
{12	,2  }, 
{12	,3  }, 
{12	,4  }, 
{12	,5  }, 
{12	,6  }, 
{12	,7  }, 
{12	,8  }, 
{12	,9  }, 
{12	,10 }, 
{12	,11 }, 
{12	,12 }, 
{11	,12 }, 
{10	,12 }, 
{9	,12 }, 
{8	,12 }, 
{7	,12 }, 
{6	,12 }, 
{5	,12 }, 
{4	,12 }, 
{3	,12 }, 
{2	,12 }, 
{1	,12 }, 
{0	,12 }, 
{-1	,12 }, 
{-2	,12 }, 
{-3	,12 }, 
{-4	,12 }, 
{-5	,12 }, 
{-6	,12 }, 
{-7	,12 }, 
{-8	,12 }, 
{-9	,12 }, 
{-10	,12 }, 
{-11	,12 }, 
{-12	,12 }, 
{-12	,11 }, 
{-12	,10 }, 
{-12	,9  }, 
{-12	,8  }, 
{-12	,7  }, 
{-12	,6  }, 
{-12	,5  }, 
{-12	,4  }, 
{-12	,3  }, 
{-12	,2  }, 
{-12	,1  }, 
{-12	,0  }, 
{-12	,-1 }, 
{-12	,-2 }, 
{-12	,-3 }, 
{-12	,-4 }, 
{-12	,-5 }, 
{-12	,-6 }, 
{-12	,-7 }, 
{-12	,-8 }, 
{-12	,-9 }, 
{-12	,-10}, 
{-12	,-11}, 
{-13	,-13}, 
{-12	,-13}, 
{-11	,-13}, 
{-10	,-13}, 
{-9	,-13}, 
{-8	,-13}, 
{-7	,-13}, 
{-6	,-13}, 
{-5	,-13}, 
{-4	,-13}, 
{-3	,-13}, 
{-2	,-13}, 
{-1	,-13}, 
{0	,-13}, 
{1	,-13}, 
{2	,-13}, 
{3	,-13}, 
{4	,-13}, 
{5	,-13}, 
{6	,-13}, 
{7	,-13}, 
{8	,-13}, 
{9	,-13}, 
{10	,-13}, 
{11	,-13}, 
{12	,-13}, 
{13	,-13}, 
{13	,-12}, 
{13	,-11}, 
{13	,-10}, 
{13	,-9 }, 
{13	,-8 }, 
{13	,-7 }, 
{13	,-6 }, 
{13	,-5 }, 
{13	,-4 }, 
{13	,-3 }, 
{13	,-2 }, 
{13	,-1 }, 
{13	,0  }, 
{13	,1  }, 
{13	,2  }, 
{13	,3  }, 
{13	,4  }, 
{13	,5  }, 
{13	,6  }, 
{13	,7  }, 
{13	,8  }, 
{13	,9  }, 
{13	,10 }, 
{13	,11 }, 
{13	,12 }, 
{13	,13 }, 
{12	,13 }, 
{11	,13 }, 
{10	,13 }, 
{9	,13 }, 
{8	,13 }, 
{7	,13 }, 
{6	,13 }, 
{5	,13 }, 
{4	,13 }, 
{3	,13 }, 
{2	,13 }, 
{1	,13 }, 
{0	,13 }, 
{-1	,13 }, 
{-2	,13 }, 
{-3	,13 }, 
{-4	,13 }, 
{-5	,13 }, 
{-6	,13 }, 
{-7	,13 }, 
{-8	,13 }, 
{-9	,13 }, 
{-10	,13 }, 
{-11	,13 }, 
{-12	,13 }, 
{-13	,13 }, 
{-13	,12 }, 
{-13	,11 }, 
{-13	,10 }, 
{-13	,9  }, 
{-13	,8  }, 
{-13	,7  }, 
{-13	,6  }, 
{-13	,5  }, 
{-13	,4  }, 
{-13	,3  }, 
{-13	,2  }, 
{-13	,1  }, 
{-13	,0  }, 
{-13	,-1 }, 
{-13	,-2 }, 
{-13	,-3 }, 
{-13	,-4 }, 
{-13	,-5 }, 
{-13	,-6 }, 
{-13	,-7 }, 
{-13	,-8 }, 
{-13	,-9 }, 
{-13	,-10}, 
{-13	,-11}, 
{-13	,-12}, 
{-14	,-14}, 
{-13	,-14}, 
{-12	,-14}, 
{-11	,-14}, 
{-10	,-14}, 
{-9	,-14}, 
{-8	,-14}, 
{-7	,-14}, 
{-6	,-14}, 
{-5	,-14}, 
{-4	,-14}, 
{-3	,-14}, 
{-2	,-14}, 
{-1	,-14}, 
{0	,-14}, 
{1	,-14}, 
{2	,-14}, 
{3	,-14}, 
{4	,-14}, 
{5	,-14}, 
{6	,-14}, 
{7	,-14}, 
{8	,-14}, 
{9	,-14}, 
{10	,-14}, 
{11	,-14}, 
{12	,-14}, 
{13	,-14}, 
{14	,-14}, 
{14	,-13}, 
{14	,-12}, 
{14	,-11}, 
{14	,-10}, 
{14	,-9 }, 
{14	,-8 }, 
{14	,-7 }, 
{14	,-6 }, 
{14	,-5 }, 
{14	,-4 }, 
{14	,-3 }, 
{14	,-2 }, 
{14	,-1 }, 
{14	,0  }, 
{14	,1  }, 
{14	,2  }, 
{14	,3  }, 
{14	,4  }, 
{14	,5  }, 
{14	,6  }, 
{14	,7  }, 
{14	,8  }, 
{14	,9  }, 
{14	,10 }, 
{14	,11 }, 
{14	,12 }, 
{14	,13 }, 
{14	,14 }, 
{13	,14 }, 
{12	,14 }, 
{11	,14 }, 
{10	,14 }, 
{9	,14 }, 
{8	,14 }, 
{7	,14 }, 
{6	,14 }, 
{5	,14 }, 
{4	,14 }, 
{3	,14 }, 
{2	,14 }, 
{1	,14 }, 
{0	,14 }, 
{-1	,14 }, 
{-2	,14 }, 
{-3	,14 }, 
{-4	,14 }, 
{-5	,14 }, 
{-6	,14 }, 
{-7	,14 }, 
{-8	,14 }, 
{-9	,14 }, 
{-10	,14 }, 
{-11	,14 }, 
{-12	,14 }, 
{-13	,14 }, 
{-14	,14 }, 
{-14	,13 }, 
{-14	,12 }, 
{-14	,11 }, 
{-14	,10 }, 
{-14	,9  }, 
{-14	,8  }, 
{-14	,7  }, 
{-14	,6  }, 
{-14	,5  }, 
{-14	,4  }, 
{-14	,3  }, 
{-14	,2  }, 
{-14	,1  }, 
{-14	,0  }, 
{-14	,-1 }, 
{-14	,-2 }, 
{-14	,-3 }, 
{-14	,-4 }, 
{-14	,-5 }, 
{-14	,-6 }, 
{-14	,-7 }, 
{-14	,-8 }, 
{-14	,-9 }, 
{-14	,-10}, 
{-14	,-11}, 
{-14	,-12}, 
{-14	,-13}, 
{-15	,-15}, 
{-14	,-15}, 
{-13	,-15}, 
{-12	,-15}, 
{-11	,-15}, 
{-10	,-15}, 
{-9	,-15}, 
{-8	,-15}, 
{-7	,-15}, 
{-6	,-15}, 
{-5	,-15}, 
{-4	,-15}, 
{-3	,-15}, 
{-2	,-15}, 
{-1	,-15}, 
{0	,-15}, 
{1	,-15}, 
{2	,-15}, 
{3	,-15}, 
{4	,-15}, 
{5	,-15}, 
{6	,-15}, 
{7	,-15}, 
{8	,-15}, 
{9	,-15}, 
{10	,-15}, 
{11	,-15}, 
{12	,-15}, 
{13	,-15}, 
{14	,-15}, 
{15	,-15}, 
{15	,-14}, 
{15	,-13}, 
{15	,-12}, 
{15	,-11}, 
{15	,-10}, 
{15	,-9 }, 
{15	,-8 }, 
{15	,-7 }, 
{15	,-6 }, 
{15	,-5 }, 
{15	,-4 }, 
{15	,-3 }, 
{15	,-2 }, 
{15	,-1 }, 
{15	,0  }, 
{15	,1  }, 
{15	,2  }, 
{15	,3  }, 
{15	,4  }, 
{15	,5  }, 
{15	,6  }, 
{15	,7  }, 
{15	,8  }, 
{15	,9  }, 
{15	,10 }, 
{15	,11 }, 
{15	,12 }, 
{15	,13 }, 
{15	,14 }, 
{15	,15 }, 
{14	,15 }, 
{13	,15 }, 
{12	,15 }, 
{11	,15 }, 
{10	,15 }, 
{9	,15 }, 
{8	,15 }, 
{7	,15 }, 
{6	,15 }, 
{5	,15 }, 
{4	,15 }, 
{3	,15 }, 
{2	,15 }, 
{1	,15 }, 
{0	,15 }, 
{-1	,15 }, 
{-2	,15 }, 
{-3	,15 }, 
{-4	,15 }, 
{-5	,15 }, 
{-6	,15 }, 
{-7	,15 }, 
{-8	,15 }, 
{-9	,15 }, 
{-10	,15 }, 
{-11	,15 }, 
{-12	,15 }, 
{-13	,15 }, 
{-14	,15 }, 
{-15	,15 }, 
{-15	,14 }, 
{-15	,13 }, 
{-15	,12 }, 
{-15	,11 }, 
{-15	,10 }, 
{-15	,9  }, 
{-15	,8  }, 
{-15	,7  }, 
{-15	,6  }, 
{-15	,5  }, 
{-15	,4  }, 
{-15	,3  }, 
{-15	,2  }, 
{-15	,1  }, 
{-15	,0  }, 
{-15	,-1 }, 
{-15	,-2 }, 
{-15	,-3 }, 
{-15	,-4 }, 
{-15	,-5 }, 
{-15	,-6 }, 
{-15	,-7 }, 
{-15	,-8 }, 
{-15	,-9 }, 
{-15	,-10}, 
{-15	,-11}, 
{-15	,-12}, 
{-15	,-13}, 
{-15	,-14}}
#endif
;


#endif /* _UTIL_ */

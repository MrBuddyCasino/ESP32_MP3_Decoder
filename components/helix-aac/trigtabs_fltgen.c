/* ***** BEGIN LICENSE BLOCK *****  
 * Source last modified: $Id: trigtabs_fltgen.c,v 1.1 2005/06/27 21:06:00 gwright Exp $ 
 *   
 * Portions Copyright (c) 1995-2005 RealNetworks, Inc. All Rights Reserved.  
 *       
 * The contents of this file, and the files included with this file, 
 * are subject to the current version of the RealNetworks Public 
 * Source License (the "RPSL") available at 
 * http://www.helixcommunity.org/content/rpsl unless you have licensed 
 * the file under the current version of the RealNetworks Community 
 * Source License (the "RCSL") available at 
 * http://www.helixcommunity.org/content/rcsl, in which case the RCSL 
 * will apply. You may also obtain the license terms directly from 
 * RealNetworks.  You may not use this file except in compliance with 
 * the RPSL or, if you have a valid RCSL with RealNetworks applicable 
 * to this file, the RCSL.  Please see the applicable RPSL or RCSL for 
 * the rights, obligations and limitations governing use of the 
 * contents of the file. 
 *   
 * This file is part of the Helix DNA Technology. RealNetworks is the 
 * developer of the Original Code and owns the copyrights in the 
 * portions it created. 
 *   
 * This file, and the files included with this file, is distributed 
 * and made available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY 
 * KIND, EITHER EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS 
 * ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES 
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET 
 * ENJOYMENT OR NON-INFRINGEMENT. 
 *  
 * Technology Compatibility Kit Test Suite(s) Location:  
 *    http://www.helixcommunity.org/content/tck  
 *  
 * Contributor(s):  
 *   
 * ***** END LICENSE BLOCK ***** */  

/**************************************************************************************
 * Fixed-point HE-AAC decoder
 * Jon Recker (jrecker@real.com), Ken Cooke (kenc@real.com)
 * June 2005
 *
 * trigtabs_fltgen.c - low-ROM alternative to trigtabs.c
 *                     generates large trig tables at runtime using floating-point
 *                     math library
 *                     MUST VERIFY that runtime-generated tables are bit-exact matches
 *                     with ROM tables in trigtabs.c
 **************************************************************************************/
#ifdef HELIX_CONFIG_AAC_GENERATE_TRIGTABS_FLOAT

#include <math.h>
#include "coder.h"

/* read-only tables */
const int cos4sin4tabOffset[NUM_IMDCT_SIZES] = {0, 128};
const int sinWindowOffset[NUM_IMDCT_SIZES] = {0, 128};
const int kbdWindowOffset[NUM_IMDCT_SIZES] = {0, 128};
const int bitrevtabOffset[NUM_IMDCT_SIZES] = {0, 17};
const unsigned char bitrevtab[17 + 129] = {
/* nfft = 64 */
0x01, 0x08, 0x02, 0x04, 0x03, 0x0c, 0x05, 0x0a, 0x07, 0x0e, 0x0b, 0x0d, 0x00, 0x06, 0x09, 0x0f,
0x00,

/* nfft = 512 */
0x01, 0x40, 0x02, 0x20, 0x03, 0x60, 0x04, 0x10, 0x05, 0x50, 0x06, 0x30, 0x07, 0x70, 0x09, 0x48,
0x0a, 0x28, 0x0b, 0x68, 0x0c, 0x18, 0x0d, 0x58, 0x0e, 0x38, 0x0f, 0x78, 0x11, 0x44, 0x12, 0x24,
0x13, 0x64, 0x15, 0x54, 0x16, 0x34, 0x17, 0x74, 0x19, 0x4c, 0x1a, 0x2c, 0x1b, 0x6c, 0x1d, 0x5c,
0x1e, 0x3c, 0x1f, 0x7c, 0x21, 0x42, 0x23, 0x62, 0x25, 0x52, 0x26, 0x32, 0x27, 0x72, 0x29, 0x4a,
0x2b, 0x6a, 0x2d, 0x5a, 0x2e, 0x3a, 0x2f, 0x7a, 0x31, 0x46, 0x33, 0x66, 0x35, 0x56, 0x37, 0x76,
0x39, 0x4e, 0x3b, 0x6e, 0x3d, 0x5e, 0x3f, 0x7e, 0x43, 0x61, 0x45, 0x51, 0x47, 0x71, 0x4b, 0x69,
0x4d, 0x59, 0x4f, 0x79, 0x53, 0x65, 0x57, 0x75, 0x5b, 0x6d, 0x5f, 0x7d, 0x67, 0x73, 0x6f, 0x7b,
0x00, 0x08, 0x14, 0x1c, 0x22, 0x2a, 0x36, 0x3e, 0x41, 0x49, 0x55, 0x5d, 0x63, 0x6b, 0x77, 0x7f,
0x00,

};

/* tables generated at runtime */
int cos4sin4tab[128 + 1024];
int cos1sin1tab[514];
int sinWindow[128 + 1024];
int kbdWindow[128 + 1024];
int twidTabEven[4*6 + 16*6 + 64*6];
int twidTabOdd[8*6 + 32*6 + 128*6];

#define M_PI		3.14159265358979323846
#define M2_30		1073741824.0
#define M2_31		2147483648.0

#define MAX_DBL		 2147483647.0
#define MIN_DBL		-2147483648.0

static int NormAndRound(double x, double q, double n)
{
    if(x >= 0.0)
		x = (x*q*n + 0.5);
    else
        x = (x*q*n - 0.5);

	/* clip */
	if (x > MAX_DBL)
		x = MAX_DBL;
	if (x < MIN_DBL)
		x = MIN_DBL;

	return (int)x;
}

static void Init_cos4sin4tab(int *tPtr, int nmdct)
{
	int i;
	double angle1, angle2, invM, x1, x2, x3, x4;

    invM = -1.0 / (double)nmdct;
    for (i = 0; i < nmdct/4; i++) {
        angle1 = (i + 0.25) * M_PI / nmdct;
        angle2 = (nmdct/2 - 1 - i + 0.25) * M_PI / nmdct;

        x1 = invM * (cos(angle1) + sin(angle1));
        x2 = invM *  sin(angle1);
        x3 = invM * (cos(angle2) + sin(angle2));
        x4 = invM *  sin(angle2);
        
		tPtr[0] = NormAndRound(x1, M2_30, nmdct);
		tPtr[1] = NormAndRound(x2, M2_30, nmdct);
		tPtr[2] = NormAndRound(x3, M2_30, nmdct);
		tPtr[3] = NormAndRound(x4, M2_30, nmdct);
		tPtr += 4;
    }    
}

static void Init_cos1sin1tab(int *tPtr)
{
	int i;
	double angle, x1, x2;

    for (i = 0; i <= (512/2); i++) {    
        angle = i * M_PI / 1024;
        x1 = (cos(angle) + sin(angle));
        x2 =  sin(angle);

		tPtr[0] = NormAndRound(x1, M2_30, 1);
		tPtr[1] = NormAndRound(x2, M2_30, 1);
		tPtr += 2;
    }
}

static void Init_sinWindow(int *tPtr, int nmdct)
{
	int i;
	double angle1, angle2, x1, x2;

    for (i = 0; i < nmdct/2; i++) {
        angle1 = (i + 0.5) * M_PI / (2.0 * nmdct);
        angle2 = (nmdct - 1 - i + 0.5) * M_PI / (2.0 * nmdct);
        x1 = sin(angle1);
        x2 = sin(angle2);

		tPtr[0] = NormAndRound(x1, M2_31, 1);
		tPtr[1] = NormAndRound(x2, M2_31, 1);
		tPtr += 2;
    }
}


#define KBD_THRESH	1e-12

static double CalcI0(double x)
{
	int k;
	double i0, iTmp, iLast, x2, xPow, kFact;

	x2 = x / 2.0;
	i0 = 0.0;
	k = 0;
	kFact = 1;
	xPow = 1;
	do {
		iLast = i0;
		iTmp = xPow / kFact;
		i0 += (iTmp*iTmp);
		k++;
		kFact *= k;
		xPow *= x2;
	} while (fabs(i0 - iLast) > KBD_THRESH);

	return i0;
}

static double CalcW(double nRef, double n, double a)
{
	double i0Base, i0Curr, nTemp;

	i0Base = CalcI0(M_PI * a);

	nTemp = (n - nRef/4) / (nRef/4);
	i0Curr = CalcI0( M_PI * a * sqrt(1.0 - nTemp*nTemp) );

	return i0Curr / i0Base;
}

static void Init_kbdWindow(int *tPtr, int nmdct)
{
	int n, nRef;
	double a, wBase, wCurr, x1;

	nRef = nmdct * 2;

	/* kbd window */
	if (nmdct == 128)
		a = 6.0;
	else
		a = 4.0;

	wBase = 0;
	for (n = 0; n <= nRef/2; n++)
		wBase += CalcW(nRef, n, a);

	/* left */
	wCurr = 0;
	for (n = 0; n < nmdct/2; n++) {
		wCurr += CalcW(nRef, n, a);
		x1 = sqrt(wCurr / wBase);
		tPtr[0] = NormAndRound(x1, M2_31, 1);
		tPtr += 2;
	}
	tPtr--;

	/* right */
	for (n = nmdct/2; n < nmdct; n++) {
		wCurr += CalcW(nRef, n, a);
		x1 = sqrt(wCurr / wBase);
		tPtr[0] = NormAndRound(x1, M2_31, 1);
		tPtr -= 2;
	}

	/* symmetry:
	 *  kbd_right(n) = kbd_ldef(N_REF - 1 - n), n = [N_REF/2, N_REF - 1] 
	 *
	 * 	wCurr = 0;
	 * 	for (n = N_REF-1; n >= N_REF/2; n--) {
	 * 		wCurr += CalcW(N_REF-n-1, a);
	 * 		kbdWindowRef[n] = sqrt(wCurr / wBase);
	 * 	}
	 * 
	 */
	return;
}

static void Init_twidTabs(int *tPtrEven, int *tPtrOdd, int nfft)
{
	int j, k;
	double wr1, wi1, wr2, wi2, wr3, wi3;

    for (k = 4; k <= nfft/4; k <<= 1) {
        for (j = 0; j < k; j++) {
            wr1 = cos(1.0 * M_PI * j / (2*k));
            wi1 = sin(1.0 * M_PI * j / (2*k));
            wr1 = (wr1 + wi1);
            wi1 = -wi1;
 
            wr2 = cos(2.0 * M_PI * j / (2*k));
            wi2 = sin(2.0 * M_PI * j / (2*k));
            wr2 = (wr2 + wi2);
            wi2 = -wi2;
 
            wr3 = cos(3.0 * M_PI * j / (2*k));
            wi3 = sin(3.0 * M_PI * j / (2*k));
            wr3 = (wr3 + wi3);
            wi3 = -wi3;
 
            if (k & 0xaaaaaaaa) {
                tPtrOdd[0] = NormAndRound(wr2, M2_30, 1);
                tPtrOdd[1] = NormAndRound(wi2, M2_30, 1);
                tPtrOdd[2] = NormAndRound(wr1, M2_30, 1);
                tPtrOdd[3] = NormAndRound(wi1, M2_30, 1);
                tPtrOdd[4] = NormAndRound(wr3, M2_30, 1);
                tPtrOdd[5] = NormAndRound(wi3, M2_30, 1);
				tPtrOdd += 6;
            } else {
                tPtrEven[0] = NormAndRound(wr2, M2_30, 1);
                tPtrEven[1] = NormAndRound(wi2, M2_30, 1);
                tPtrEven[2] = NormAndRound(wr1, M2_30, 1);
                tPtrEven[3] = NormAndRound(wi1, M2_30, 1);
                tPtrEven[4] = NormAndRound(wr3, M2_30, 1);
                tPtrEven[5] = NormAndRound(wi3, M2_30, 1);
				tPtrEven += 6;
            }
        }
    }
}

/**************************************************************************************
 * Function:    AACInitTrigtabsFloat
 *
 * Description: generate AAC decoder tables using floating-point math library 
 *
 * Inputs:      none
 *
 * Outputs:     initialized tables
 *
 * Return:      0 on success
 *
 * Notes:       this function should ONLY be called when double-precision 
 *              floating-point math is supported
 *              the generated tables must be bit-exact matches with read-only
 *              tables stored in trigtabs.c
 *              this initializes global tables in RAM, and is NOT thread-safe,
 *              so the caller must ensure that this function is not called
 *              from multiple threads
 *              this should be called exactly once, before the entrypoint function
 *              for the application or DLL is called
 **************************************************************************************/
int AACInitTrigtabsFloat(void)
{
	/* cos4sin4tab */
	Init_cos4sin4tab(cos4sin4tab + cos4sin4tabOffset[0], 128);
	Init_cos4sin4tab(cos4sin4tab + cos4sin4tabOffset[1], 1024);

	/* cos1sin1tab */
	Init_cos1sin1tab(cos1sin1tab);

	/* sinWindow */
	Init_sinWindow(sinWindow + sinWindowOffset[0], 128);
	Init_sinWindow(sinWindow + sinWindowOffset[1], 1024);

	/* kbdWindow */
	Init_kbdWindow(kbdWindow + kbdWindowOffset[0], 128);
	Init_kbdWindow(kbdWindow + kbdWindowOffset[1], 1024);

	/* twidTabEven, twidTabOdd */
	Init_twidTabs(twidTabEven, twidTabOdd, 512);

	return 0;
}

/**************************************************************************************
 * Function:    AACFreeTrigtabsFloat
 *
 * Description: free any memory allocated by AACInitTrigtabsFloat()
 *
 * Inputs:      none
 *
 * Outputs:     none
 *
 * Return:      none
 **************************************************************************************/
void AACFreeTrigtabsFloat(void)
{	
	return;
}

#endif	/* HELIX_CONFIG_AAC_GENERATE_TRIGTABS_FLOAT */

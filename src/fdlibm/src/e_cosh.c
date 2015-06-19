
/* @(#)e_cosh.c 1.3 95/01/18 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunSoft, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 */

/* FDLIBM___ieee754_cosh(x)
 * Method : 
 * mathematically FDLIBM_cosh(x) if defined to be (FDLIBM_exp(x)+FDLIBM_exp(-x))/2
 *	1. Replace x by |x| (FDLIBM_cosh(x) = FDLIBM_cosh(-x)). 
 *	2. 
 *		                                        [ FDLIBM_exp(x) - 1 ]^2 
 *	    0        <= x <= ln2/2  :  FDLIBM_cosh(x) := 1 + -------------------
 *			       			           2*FDLIBM_exp(x)
 *
 *		                                  FDLIBM_exp(x) +  1/FDLIBM_exp(x)
 *	    ln2/2    <= x <= 22     :  FDLIBM_cosh(x) := -------------------
 *			       			          2
 *	    22       <= x <= lnovft :  FDLIBM_cosh(x) := FDLIBM_exp(x)/2 
 *	    lnovft   <= x <= ln2ovft:  FDLIBM_cosh(x) := FDLIBM_exp(x/2)/2 * FDLIBM_exp(x/2)
 *	    ln2ovft  <  x	    :  FDLIBM_cosh(x) := huge*huge (overflow)
 *
 * Special cases:
 *	FDLIBM_cosh(x) is |x| if x is +INF, -INF, or NaN.
 *	only FDLIBM_cosh(0)=1 is exact for finite x.
 */

#include "fdlibm.h"

#ifdef __STDC__
static const double one = 1.0, half=0.5, huge = 1.0e300;
#else
static double one = 1.0, half=0.5, huge = 1.0e300;
#endif

#ifdef __STDC__
	double FDLIBM___ieee754_cosh(double x)
#else
	double FDLIBM___ieee754_cosh(x)
	double x;
#endif
{	
	double t,w;
	int ix;
	unsigned lx;

    /* High word of |x|. */
	ix = __HI(x);
	ix &= 0x7fffffff;

    /* x is INF or NaN */
	if(ix>=0x7ff00000) return x*x;	

    /* |x| in [0,0.5*ln2], return 1+FDLIBM_expm1(|x|)^2/(2*FDLIBM_exp(|x|)) */
	if(ix<0x3fd62e43) {
	    t = FDLIBM_expm1(FDLIBM_fabs(x));
	    w = one+t;
	    if (ix<0x3c800000) return w;	/* FDLIBM_cosh(tiny) = 1 */
	    return one+(t*t)/(w+w);
	}

    /* |x| in [0.5*ln2,22], return (FDLIBM_exp(|x|)+1/FDLIBM_exp(|x|)/2; */
	if (ix < 0x40360000) {
		t = FDLIBM___ieee754_exp(FDLIBM_fabs(x));
		return half*t+half/t;
	}

    /* |x| in [22, FDLIBM_log(maxdouble)] return half*FDLIBM_exp(|x|) */
	if (ix < 0x40862E42)  return half*FDLIBM___ieee754_exp(FDLIBM_fabs(x));

    /* |x| in [FDLIBM_log(maxdouble), overflowthresold] */
	lx = *( (((*(unsigned*)&one)>>29)) + (unsigned*)&x);
	if (ix<0x408633CE || 
	      (ix==0x408633ce)&&(lx<=(unsigned)0x8fb9f87d)) {
	    w = FDLIBM___ieee754_exp(half*FDLIBM_fabs(x));
	    t = half*w;
	    return t*w;
	}

    /* |x| > overflowthresold, FDLIBM_cosh(x) overflow */
	return huge*huge;
}

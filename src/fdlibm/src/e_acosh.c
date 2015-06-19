
/* @(#)e_acosh.c 1.3 95/01/18 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunSoft, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 *
 */

/* FDLIBM___ieee754_acosh(x)
 * Method :
 *	Based on 
 *		FDLIBM_acosh(x) = log [ x + FDLIBM_sqrt(x*x-1) ]
 *	we have
 *		FDLIBM_acosh(x) := FDLIBM_log(x)+ln2,	if x is large; else
 *		FDLIBM_acosh(x) := FDLIBM_log(2x-1/(FDLIBM_sqrt(x*x-1)+x)) if x>2; else
 *		FDLIBM_acosh(x) := FDLIBM_log1p(t+FDLIBM_sqrt(2.0*t+t*t)); where t=x-1.
 *
 * Special cases:
 *	FDLIBM_acosh(x) is NaN with signal if x<1.
 *	FDLIBM_acosh(NaN) is NaN without signal.
 */

#include "fdlibm.h"

#ifdef __STDC__
static const double 
#else
static double 
#endif
one	= 1.0,
ln2	= 6.93147180559945286227e-01;  /* 0x3FE62E42, 0xFEFA39EF */

#ifdef __STDC__
	double FDLIBM___ieee754_acosh(double x)
#else
	double FDLIBM___ieee754_acosh(x)
	double x;
#endif
{	
	double t;
	int hx;
	hx = __HI(x);
	if(hx<0x3ff00000) {		/* x < 1 */
	    return (x-x)/(x-x);
	} else if(hx >=0x41b00000) {	/* x > 2**28 */
	    if(hx >=0x7ff00000) {	/* x is inf of NaN */
	        return x+x;
	    } else 
		return FDLIBM___ieee754_log(x)+ln2;	/* FDLIBM_acosh(huge)=FDLIBM_log(2x) */
	} else if(((hx-0x3ff00000)|__LO(x))==0) {
	    return 0.0;			/* FDLIBM_acosh(1) = 0 */
	} else if (hx > 0x40000000) {	/* 2**28 > x > 2 */
	    t=x*x;
	    return FDLIBM___ieee754_log(2.0*x-one/(x+FDLIBM_sqrt(t-one)));
	} else {			/* 1<x<2 */
	    t = x-one;
	    return FDLIBM_log1p(t+FDLIBM_sqrt(2.0*t+t*t));
	}
}



/* @(#)w_pow.c 1.3 95/01/18 */
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

/* 
 * wrapper FDLIBM_pow(x,y) return x**y
 */

#include "fdlibm.h"


#ifdef __STDC__
	double FDLIBM_pow(double x, double y)	/* wrapper pow */
#else
	double FDLIBM_pow(x,y)			/* wrapper pow */
	double x,y;
#endif
{
#ifdef _IEEE_LIBM
	return  FDLIBM___ieee754_pow(x,y);
#else
	double z;
	z=FDLIBM___ieee754_pow(x,y);
	if(_LIB_VERSION == _IEEE_|| FDLIBM_isnan(y)) return z;
	if(FDLIBM_isnan(x)) {
	    if(y==0.0) 
	        return FDLIBM___kernel_standard(x,y,42); /* FDLIBM_pow(NaN,0.0) */
	    else 
		return z;
	}
	if(x==0.0){ 
	    if(y==0.0)
	        return FDLIBM___kernel_standard(x,y,20); /* FDLIBM_pow(0.0,0.0) */
	    if(FDLIBM_finite(y)&&y<0.0)
	        return FDLIBM___kernel_standard(x,y,23); /* FDLIBM_pow(0.0,negative) */
	    return z;
	}
	if(!FDLIBM_finite(z)) {
	    if(FDLIBM_finite(x)&&FDLIBM_finite(y)) {
	        if(FDLIBM_isnan(z))
	            return FDLIBM___kernel_standard(x,y,24); /* pow neg**non-int */
	        else 
	            return FDLIBM___kernel_standard(x,y,21); /* pow overflow */
	    }
	} 
	if(z==0.0&&FDLIBM_finite(x)&&FDLIBM_finite(y))
	    return FDLIBM___kernel_standard(x,y,22); /* pow underflow */
	return z;
#endif
}

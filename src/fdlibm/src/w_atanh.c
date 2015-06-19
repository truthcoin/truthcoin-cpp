
/* @(#)w_atanh.c 1.3 95/01/18 */
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
 * wrapper FDLIBM_atanh(x)
 */

#include "fdlibm.h"


#ifdef __STDC__
	double FDLIBM_atanh(double x)		/* wrapper atanh */
#else
	double FDLIBM_atanh(x)			/* wrapper atanh */
	double x;
#endif
{
#ifdef _IEEE_LIBM
	return FDLIBM___ieee754_atanh(x);
#else
	double z,y;
	z = FDLIBM___ieee754_atanh(x);
	if(_LIB_VERSION == _IEEE_ || FDLIBM_isnan(x)) return z;
	y = FDLIBM_fabs(x);
	if(y>=1.0) {
	    if(y>1.0)
	        return FDLIBM___kernel_standard(x,x,30); /* FDLIBM_atanh(|x|>1) */
	    else 
	        return FDLIBM___kernel_standard(x,x,31); /* FDLIBM_atanh(|x|==1) */
	} else
	    return z;
#endif
}

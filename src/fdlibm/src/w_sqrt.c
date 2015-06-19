
/* @(#)w_sqrt.c 1.3 95/01/18 */
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
 * wrapper FDLIBM_sqrt(x)
 */

#include "fdlibm.h"

#ifdef __STDC__
	double FDLIBM_sqrt(double x)		/* wrapper sqrt */
#else
	double FDLIBM_sqrt(x)			/* wrapper sqrt */
	double x;
#endif
{
#ifdef _IEEE_LIBM
	return FDLIBM___ieee754_sqrt(x);
#else
	double z;
	z = FDLIBM___ieee754_sqrt(x);
	if(_LIB_VERSION == _IEEE_ || FDLIBM_isnan(x)) return z;
	if(x<0.0) {
	    return FDLIBM___kernel_standard(x,x,26); /* FDLIBM_sqrt(negative) */
	} else
	    return z;
#endif
}

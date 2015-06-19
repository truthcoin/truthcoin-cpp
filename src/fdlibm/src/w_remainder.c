
/* @(#)w_remainder.c 1.3 95/01/18 */
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
 * wrapper FDLIBM_remainder(x,p)
 */

#include "fdlibm.h"

#ifdef __STDC__
	double FDLIBM_remainder(double x, double y)	/* wrapper remainder */
#else
	double FDLIBM_remainder(x,y)			/* wrapper remainder */
	double x,y;
#endif
{
#ifdef _IEEE_LIBM
	return FDLIBM___ieee754_remainder(x,y);
#else
	double z;
	z = FDLIBM___ieee754_remainder(x,y);
	if(_LIB_VERSION == _IEEE_ || FDLIBM_isnan(y)) return z;
	if(y==0.0) 
	    return FDLIBM___kernel_standard(x,y,28); /* FDLIBM_remainder(x,0) */
	else
	    return z;
#endif
}

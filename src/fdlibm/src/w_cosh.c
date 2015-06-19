
/* @(#)w_cosh.c 1.3 95/01/18 */
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
 * wrapper FDLIBM_cosh(x)
 */

#include "fdlibm.h"

#ifdef __STDC__
	double FDLIBM_cosh(double x)		/* wrapper cosh */
#else
	double FDLIBM_cosh(x)			/* wrapper cosh */
	double x;
#endif
{
#ifdef _IEEE_LIBM
	return FDLIBM___ieee754_cosh(x);
#else
	double z;
	z = FDLIBM___ieee754_cosh(x);
	if(_LIB_VERSION == _IEEE_ || FDLIBM_isnan(x)) return z;
	if(FDLIBM_fabs(x)>7.10475860073943863426e+02) {	
	        return FDLIBM___kernel_standard(x,x,5); /* cosh overflow */
	} else
	    return z;
#endif
}

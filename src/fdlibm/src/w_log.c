
/* @(#)w_log.c 1.3 95/01/18 */
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
 * wrapper FDLIBM_log(x)
 */

#include "fdlibm.h"


#ifdef __STDC__
	double FDLIBM_log(double x)		/* wrapper log */
#else
	double FDLIBM_log(x)			/* wrapper log */
	double x;
#endif
{
#ifdef _IEEE_LIBM
	return FDLIBM___ieee754_log(x);
#else
	double z;
	z = FDLIBM___ieee754_log(x);
	if(_LIB_VERSION == _IEEE_ || FDLIBM_isnan(x) || x > 0.0) return z;
	if(x==0.0)
	    return FDLIBM___kernel_standard(x,x,16); /* FDLIBM_log(0) */
	else 
	    return FDLIBM___kernel_standard(x,x,17); /* FDLIBM_log(x<0) */
#endif
}


/* @(#)w_exp.c 1.4 04/04/22 */
/*
 * ====================================================
 * Copyright (C) 2004 by Sun Microsystems, Inc. All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 */

/* 
 * wrapper FDLIBM_exp(x)
 */

#include "fdlibm.h"

#ifdef __STDC__
static const double
#else
static double
#endif
o_threshold=  7.09782712893383973096e+02,  /* 0x40862E42, 0xFEFA39EF */
u_threshold= -7.45133219101941108420e+02;  /* 0xc0874910, 0xD52D3051 */

#ifdef __STDC__
	double FDLIBM_exp(double x)		/* wrapper exp */
#else
	double FDLIBM_exp(x)			/* wrapper exp */
	double x;
#endif
{
#ifdef _IEEE_LIBM
	return FDLIBM___ieee754_exp(x);
#else
	double z;
	z = FDLIBM___ieee754_exp(x);
	if(_LIB_VERSION == _IEEE_) return z;
	if(FDLIBM_finite(x)) {
	    if(x>o_threshold)
	        return FDLIBM___kernel_standard(x,x,6); /* exp overflow */
	    else if(x<u_threshold)
	        return FDLIBM___kernel_standard(x,x,7); /* exp underflow */
	} 
	return z;
#endif
}

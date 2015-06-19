
/* @(#)w_fmod.c 1.3 95/01/18 */
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
 * wrapper FDLIBM_fmod(x,y)
 */

#include "fdlibm.h"


#ifdef __STDC__
	double FDLIBM_fmod(double x, double y)	/* wrapper fmod */
#else
	double FDLIBM_fmod(x,y)		/* wrapper fmod */
	double x,y;
#endif
{
#ifdef _IEEE_LIBM
	return FDLIBM___ieee754_fmod(x,y);
#else
	double z;
	z = FDLIBM___ieee754_fmod(x,y);
	if(_LIB_VERSION == _IEEE_ ||FDLIBM_isnan(y)||FDLIBM_isnan(x)) return z;
	if(y==0.0) {
	        return FDLIBM___kernel_standard(x,y,27); /* FDLIBM_fmod(x,0) */
	} else
	    return z;
#endif
}


/* @(#)w_j1.c 1.3 95/01/18 */
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
 * wrapper of j1,y1 
 */

#include "fdlibm.h"

#ifdef __STDC__
	double FDLIBM_j1(double x)		/* wrapper j1 */
#else
	double FDLIBM_j1(x)			/* wrapper j1 */
	double x;
#endif
{
#ifdef _IEEE_LIBM
	return FDLIBM___ieee754_j1(x);
#else
	double z;
	z = FDLIBM___ieee754_j1(x);
	if(_LIB_VERSION == _IEEE_ || FDLIBM_isnan(x) ) return z;
	if(FDLIBM_fabs(x)>X_TLOSS) {
	        return FDLIBM___kernel_standard(x,x,36); /* FDLIBM_j1(|x|>X_TLOSS) */
	} else
	    return z;
#endif
}

#ifdef __STDC__
	double FDLIBM_y1(double x)		/* wrapper y1 */
#else
	double FDLIBM_y1(x)			/* wrapper y1 */
	double x;
#endif
{
#ifdef _IEEE_LIBM
	return FDLIBM___ieee754_y1(x);
#else
	double z;
	z = FDLIBM___ieee754_y1(x);
	if(_LIB_VERSION == _IEEE_ || FDLIBM_isnan(x) ) return z;
        if(x <= 0.0){
                if(x==0.0)
                    /* d= -one/(x-x); */
                    return FDLIBM___kernel_standard(x,x,10);
                else
                    /* d = zero/(x-x); */
                    return FDLIBM___kernel_standard(x,x,11);
        }
	if(x>X_TLOSS) {
	        return FDLIBM___kernel_standard(x,x,37); /* FDLIBM_y1(x>X_TLOSS) */
	} else
	    return z;
#endif
}


/* @(#)w_gamma.c 1.3 95/01/18 */
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

/* double FDLIBM_gamma(double x)
 * Return the logarithm of the Gamma function of x.
 *
 * Method: call gamma_r
 */

#include "fdlibm.h"

extern int signgam;

#ifdef __STDC__
	double FDLIBM_gamma(double x)
#else
	double FDLIBM_gamma(x)
	double x;
#endif
{
#ifdef _IEEE_LIBM
	return FDLIBM___ieee754_gamma_r(x,&signgam);
#else
        double y;
        y = FDLIBM___ieee754_gamma_r(x,&signgam);
        if(_LIB_VERSION == _IEEE_) return y;
        if(!FDLIBM_finite(y)&&FDLIBM_finite(x)) {
            if(FDLIBM_floor(x)==x&&x<=0.0)
                return FDLIBM___kernel_standard(x,x,41); /* gamma pole */
            else
                return FDLIBM___kernel_standard(x,x,40); /* gamma overflow */
        } else
            return y;
#endif
}             

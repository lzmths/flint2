/*=============================================================================

    This file is part of FLINT.

    FLINT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    FLINT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FLINT; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

=============================================================================*/
/******************************************************************************

    Copyright (C) 2010, 2011 Sebastian Pancratz

******************************************************************************/

#include <stdlib.h>
#include <gmp.h>
#include "flint.h"
#include "fmpz.h"
#include "fmpz_vec.h"
#include "fmpz_poly.h"
#include "fmpz_mod_poly.h"

void fmpz_mod_poly_inv_series_newton_f(fmpz_t f, fmpz_mod_poly_t Qinv, 
    const fmpz_mod_poly_t Q, slong n)
{
    const fmpz *p = &(Q->p);
    fmpz_t cinv;
    fmpz *Qcopy;
    int Qalloc;

    if (Q->length >= n)
    {
        Qcopy = Q->coeffs;
        Qalloc = 0;
    }
    else
    {
        slong i;
        Qcopy = (fmpz *) flint_malloc(n * sizeof(fmpz));
        for (i = 0; i < Q->length; i++)
            Qcopy[i] = Q->coeffs[i];
        flint_mpn_zero((mp_ptr) Qcopy + i, n - i);
        Qalloc = 1;
    }

    fmpz_init(cinv);
    fmpz_gcdinv(f, cinv, Q->coeffs, p);

    if (!fmpz_is_one(f))
       goto cleanup;
    
    if (Qinv != Q)
    {
        fmpz_mod_poly_fit_length(Qinv, n);
        _fmpz_mod_poly_inv_series_newton(Qinv->coeffs, Qcopy, n, cinv, p);
    }
    else
    {
        fmpz *t = _fmpz_vec_init(n);

        _fmpz_mod_poly_inv_series_newton(t, Qcopy, n, cinv, p);

        _fmpz_vec_clear(Qinv->coeffs, Qinv->alloc);
        Qinv->coeffs = t;
        Qinv->alloc  = n;
        Qinv->length = n;
    }
    _fmpz_mod_poly_set_length(Qinv, n);
    _fmpz_mod_poly_normalise(Qinv);

cleanup:

    if (Qalloc)
        flint_free(Qcopy);
    fmpz_clear(cinv);
}


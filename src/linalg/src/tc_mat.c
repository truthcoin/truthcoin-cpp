/*
 * Copyright (c) 2015 The Truthcoin Core developers
 * Distributed under the MIT software license, see the accompanying
 * file COPYING or http://www.opensource.org/licenses/mit-license.php.
 */

#include <malloc.h>
#include <math.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tc_mat.h"

/****************************************************************************
 * tc_mat                                                                   *
 ****************************************************************************/

struct tc_mat *
tc_mat_ctr(uint32_t nr_, uint32_t nc_) 
{
    struct tc_mat *A = (struct tc_mat *) malloc(sizeof(struct tc_mat));
    memset(A, 0, sizeof(struct tc_mat));
    tc_mat_resize(A, nr_, nc_);
    return A;
}

void 
tc_mat_dtr(struct tc_mat *A)
{
    if (!A)
        return;
    tc_mat_clear(A);
    free(A);
}

void
tc_mat_clear(struct tc_mat *A)
{
    if (!A)
        return;
    if (A->a) {
        for(uint32_t r=0; r < A->nr; r++)
            if (A->a[r])
                free(A->a[r]);
        free(A->a);
    }
    A->a = NULL;
    A->nr = 0;
    A->nc = 0;
}

void
tc_mat_print(const struct tc_mat *A)
{
    if (!A)
        return;
    for(uint32_t i=0; i < A->nr; i++) {
        for(uint32_t j=0; j < A->nc; j++)
            printf(" %13.10f", A->a[i][j]);
        printf("\n");
    }
}

double
tc_mat_norm(const struct tc_mat *A)
{
    if (!A)
        return 0.0;
    double sum = 0.0;
    for(uint32_t i=0; i < A->nr; i++)
        for(uint32_t j=0; j < A->nc; j++)
            sum += A->a[i][j] * A->a[i][j];
    return sum;
}

void
tc_mat_resize(struct tc_mat *A, uint32_t nr_, uint32_t nc_)
{
    if (!A)
        return;
    if ((A->nr == nr_) && (A->nc == nc_))
        return;
    tc_mat_clear(A);
    A->nr = nr_;
    A->nc = nc_;
    A->a = (double **) malloc(sizeof(double *) * nr_);
    for(uint32_t r=0; r < nr_; r++)
        A->a[r] = (double *) malloc(sizeof(double) * nc_);
}

void
tc_mat_copy(struct tc_mat *B, const struct tc_mat *A)
{
    if (!A || !B)
        return;
    if (A == B)
        return;
    if ((B->nr != A->nr) || (B->nc != A->nc))
        tc_mat_resize(B, A->nr, A->nc);
    for(uint32_t i=0; i < A->nr; i++)
        for(uint32_t j=0; j < A->nc; j++)
            B->a[i][j] = A->a[i][j];
}

void
tc_mat_identity(struct tc_mat *A)
{
    if (!A)
        return;
    for(uint32_t i=0; i < A->nr; i++)
        for(uint32_t j=0; j < A->nc; j++)
            A->a[i][j] = (i==j)? 1.0: 0.0;
}

void
tc_mat_transpose(struct tc_mat *B, const struct tc_mat *A)
{
    if (!A || !B)
        return;
    if (A == B) {
        struct tc_mat *b = tc_mat_ctr(A->nc, A->nr);
        for(uint32_t i=0; i < A->nr; i++) 
            for(uint32_t j=0; j < A->nc; j++) 
                b->a[j][i] = A->a[i][j];
        tc_mat_copy(B, b);
        tc_mat_dtr(b);
        return;
    }
    if ((B->nr != A->nc) || (B->nc != A->nr))
        tc_mat_resize(B, A->nc, A->nr);
    for(uint32_t i=0; i < A->nr; i++) 
        for(uint32_t j=0; j < A->nc; j++) 
            B->a[j][i] = A->a[i][j];
}

int
tc_mat_add(struct tc_mat *C, const struct tc_mat *A, const struct tc_mat *B)
{
    if (!C || !A || !B || (A->nr != B->nr) || (A->nc != B->nc))
        return -1;
    if ((C->nr != A->nr) || (C->nc != A->nc))
        tc_mat_resize(C, A->nr, B->nc);
    for(uint32_t i=0; i < C->nr; i++)
        for(uint32_t j=0; j < C->nc; j++)
            C->a[i][j] = A->a[i][j] + B->a[i][j];
    return 0;
}

int
tc_mat_sub(struct tc_mat *C, const struct tc_mat *A, const struct tc_mat *B)
{
    if (!C || !A || !B || (A->nr != B->nr) || (A->nc != B->nc))
        return -1;
    if ((C->nr != A->nr) || (C->nc != A->nc))
        tc_mat_resize(C, A->nr, B->nc);
    for(uint32_t i=0; i < C->nr; i++)
        for(uint32_t j=0; j < C->nc; j++)
            C->a[i][j] = A->a[i][j] - B->a[i][j];
    return 0;
}

int
tc_mat_mult(struct tc_mat *C, const struct tc_mat *A, const struct tc_mat *B)
{
    if (!C || !A || !B || (A->nc != B->nr))
        return -1;
    if ((C == A) || (C == B)) {
        struct tc_mat *c = tc_mat_ctr(A->nr, B->nr);
        tc_mat_mult(c, A, B);
        tc_mat_copy(C, c);
        tc_mat_dtr(c);
        return 0;
    }
    if ((C->nc != B->nc) || (C->nr != A->nr))
        tc_mat_resize(C, A->nr, B->nc);
    for(uint32_t i=0; i < C->nr; i++) {
        for(uint32_t j=0; j < C->nc; j++) {
            C->a[i][j] = 0.0;
            for(uint32_t k=0; k < A->nc; k++)
                C->a[i][j] += A->a[i][k] * B->a[k][j];
        }
    }
    return 0;
}

int
tc_mat_mult_scalar(struct tc_mat *C, double a, const struct tc_mat *B)
{
    if (!C || !B)
        return -1;
    if ((C->nr != B->nr) || (C->nc != B->nc))
        tc_mat_resize(C, B->nr, B->nc);
    for(uint32_t i=0; i < C->nr; i++)
        for(uint32_t j=0; j < C->nc; j++)
            C->a[i][j] = a * B->a[i][j];
    return 0;
}

/* Householder Reduction to Bidiagonal Form
 * Input m x n matrix A 
 * Output: m x n matrix  U, products of Householder matrices 
 *         m x n matrix  B, bidiagonal
 *         m x n matrix  V, products of Householder matrices
 *   such that A = U B V^T
 *
 * A Householder matrix P is a reflection transformation of the form
 * I - tau u u^T where tau = 2 / (u^T u). P is orthonormal since 
 *        P^T P = (I - tau u u^T)^T (I - tau u u^T)
 *              = (I - tau u u^T)   (I - tau u u^T)
 *              =  I - 2 tau u u^T + tau^2 u (u^T u) u^T
 *              =  I - 2 tau u u^T + tau^2 u (2 / tau) u^T
 *              =  I                                 
 * Products of orthonomal matrices are orthonormal so U and V will be 
 * orthonormal.
 *
 * B is constructed via the algorithm
 *   1.  B <-- A
 *   2.  U <-- I  m x n
 *   3.  V <-- I  n x n
 *   4.  For k = 1, ..., n
 *       a. construct a Householder matrix P such that 
 *             i. P is the identity on the first k-1 components, and
 *            ii. the k-th column of P B is zero below the diagonal.
 *       b.  B <--- P B
 *       c.  U <--- U P
 *       d.  If k < n - 2, increment k and repeat steps a-c for the
 *              operation from the right (the row of B P will be zero
 *              to the right of the superdiagonal).
 *   
 *  Step 4a(i) is equivalent to setting the first k-1 components of u 
 *  to be zero. Let v be the k-th column of P B. Since
 *      P v = (I - tau u u^T) v
 *          = (I - tau u u^T) v
 *          = v - tau (u^T v) u   (Eq. 1)
 *  we have that step 4a(ii) is equivalent to setting u to be an appropriate
 *  scalar multiple of v for all components above k.  The previous steps 
 *  would have zeroed out the first k-2 components of v. Let 
 *     b^2 = Sum_{i>=k} (v_i)^2
 *  and choose the sign of b to ensure s = v_k - b is nonzero. Set u = v/s 
 *  on all components larger than k and set u_k = 1. Then
 *    tau = 2 / (u^T u) 
 *        = 2 / (1 + (b^2 - (v_k)^2) / s^2 ) 
 *        = (2 s) / (s + (b^2 - (v_k)^2) / s)
 *        = (2 s) / (s - (b + v_k))
 *        = (2 s) / ((v_k - b) - (b + v_k))
 *        = (2 s) / (- 2 b)
 *        =  -s / b
 *  This leads to the zeroing out of the lower components of P v since
 *    u^T v = v_k + (b^2 - (v_k)^2) / s
 *          = v_k - (b   +  v_k)
 *          = b
 *  meaning that Eq 1 is simply 
 *      P v = v - (-s/b) (b) u 
 *          = v + s u
 *  and so P v is zero on all components larger than k.
 *  
 *  Side note on efficiency 1 (not implemented):
 *    Step c produces U and V as products of Housholder matrices. The actual 
 *    computation of these products is not the most computationally efficient 
 *    method. A Housholder matrix acts on a vector v as
 *          P v = (I - tau u u^T) v 
 *              =  v - tau u (v^T u)^T  (Eq. 2)
 *    Thus if we've recorded all u's and tau's, we can find U, the product of 
 *    all the P's, by sequentially applying Eq 2 to the components e1, e2, etc.
 *
 *  Side note on efficiency 2 (not implemented):
 *    Since each iteration of B leads to known places where its terms have zeros,
 *    all matrix multiplications involving B can be restricted to the nonzero
 *    terms.
 * 
 *  Side note on efficiency 3 (not implemented):
 *    Since the u's have more zeros in each iteration, the collection of u's
 *    can be stored in the original matrix A (and hence B) so long as the 
 *    Side note on efficiency #2 was implemented.
 */  
int
tc_mat_bidiag_decomp(
    const struct tc_mat *A,
    struct tc_mat *U,
    struct tc_mat *B,
    struct tc_mat *V)
{
    if (!A || !U || !B || !V)
        return -1;
    if ((U->nr != A->nr) || (U->nc != A->nr))
        tc_mat_resize(U, A->nr, A->nr);
    if ((V->nr != A->nc) || (V->nc != A->nc))
        tc_mat_resize(V, A->nc, A->nc);
    if ((B->nr != A->nr) || (B->nc != A->nc))
        tc_mat_resize(B, A->nr, A->nc);
    tc_mat_copy(B, A);
    tc_mat_identity(U);
    tc_mat_identity(V);
    struct tc_mat *Ic = tc_mat_ctr(A->nc, A->nc);
    tc_mat_identity(Ic);
    struct tc_mat *Ir = tc_mat_ctr(A->nr, A->nr);
    tc_mat_identity(Ir);
    struct tc_mat *u = tc_mat_ctr(0, 0);
    struct tc_mat *uT = tc_mat_ctr(0, 0);
    struct tc_mat *u_uT = tc_mat_ctr(0, 0);
    struct tc_mat *P = tc_mat_ctr(0, 0);
    for(uint32_t k=0; (k < A->nc) && (k+1 < A->nr); k++) {
        /* from the left */
        double b_k = B->a[k][k];
        double b = 0.0;
        for(uint32_t i=k; i < A->nr; i++)
            b += B->a[i][k] * B->a[i][k];
        if (b != 0.0) { /* if b = 0, there's nothing to do */
            b = ((b_k > 0)? -1.0: 1.0) * sqrt(b);
            double s = b_k - b;
            double tau = - s / b;
            tc_mat_resize(u, A->nr, 1);
            for(uint32_t i=0; i < A->nr; i++)
                u->a[i][0] = (i > k)? B->a[i][k]/s: ((i==k)? 1.0: 0.0);
            /*  P = I - tau u u^T */
            tc_mat_transpose(uT, u);
            tc_mat_mult(u_uT, u, uT);
            tc_mat_mult_scalar(P, -tau, u_uT);
            tc_mat_add(P, Ir, P);
            /*  U B = (U P) (P B) */
            tc_mat_mult(B, P, B);
            tc_mat_mult(U, U, P);
        }
        if (k + 2 < A->nc) {
            /* from the right */
            double b_k1 = B->a[k][k+1];
            double bb = 0.0;
            for(uint32_t j=k+1; j < A->nc; j++)
                bb += B->a[k][j] * B->a[k][j];
            bb = ((b_k1 > 0)? -1.0: 1.0) * sqrt(bb);
            if (bb != 0.0) { /* if bb = 0, there's nothing to do */
                double s = b_k1 - bb;
                double tau = - s / bb;
                tc_mat_resize(u, A->nc, 1);
                for(uint32_t i=0; i < A->nc; i++)
                    u->a[i][0] = (i > k+1)? B->a[k][i]/s: ((i==k+1)? 1.0: 0.0);
                /*  P = I - tau u u^T */
                tc_mat_transpose(uT, u);
                tc_mat_mult(u_uT, u, uT);
                tc_mat_mult_scalar(P, -tau, u_uT);
                tc_mat_add(P, Ic, P);
                /*  B V = (B P) (P V) */
                tc_mat_mult(B, B, P);
                tc_mat_mult(V, P, V);
            }
        }
    }
    tc_mat_dtr(P);
    tc_mat_dtr(u);
    tc_mat_dtr(uT);
    tc_mat_dtr(u_uT);
    tc_mat_dtr(Ic);
    tc_mat_dtr(Ir);
    return 0;
}

/* tc_mat_eigenvalues
 * only implmented for 2x2 matrices
 * Input: 2 x 2 matrix A 
 * Output: 2 x 1 matrix E of eigenvalues
 * 
 * If we set A = [ a b ] 
 *               [ c d ]
 * Then
 *  0 = det(A - x I)
 *    = (a - x)(d - x) - bc
 *    = x^2 - (a + d) x + (ad - bc) 
 */
int
tc_mat_eigenvalues(struct tc_mat *E, const struct tc_mat *A)
{
    if (!A || !E)
        return -1;
    if ((A->nr != 2) || (A->nc != 2))
        return -1;
    if (E->nr != A->nr) 
        tc_mat_resize(E, A->nr, 1);
    double a = A->a[0][0];
    double b = A->a[1][0];
    double c = A->a[0][1];
    double d = A->a[1][1];
    double ad2 = (a + d)/2.0;
    double det = ad2*ad2 - (a*d - b*c);
    double s = (det <= 0.0)? 0.0: sqrt(det);
    E->a[0][0] = ad2 + s;
    E->a[1][0] = ad2 - s;
    return 0;
}


/* Wilkinson Shift                 
 * Input: m x 2 matrix A 
 * Output: the Wilkinson shift
 * 
 * ei= trailing 2x2 matrix of D^T D */
double
tc_mat_wilkinson_shift(const struct tc_mat *A)
{
    if (!A)
        return 0.0;
    if (A->nc != 2)
        return 0.0;
    struct tc_mat *AT = tc_mat_ctr(2, A->nr);
    tc_mat_transpose(AT, A);
    struct tc_mat *AT_A = tc_mat_ctr(2, 2);
    tc_mat_mult(AT_A, AT, A);
    struct tc_mat *E = tc_mat_ctr(2, 1);
    tc_mat_eigenvalues(E, AT_A);
    double l0 = E->a[0][0];
    double l1 = E->a[1][0];
    double t22 = AT_A->a[1][1];
    tc_mat_dtr(AT);
    tc_mat_dtr(AT_A);
    tc_mat_dtr(E);
    return (fabs(l0 - t22) < fabs(l1 - t22))? l0: l1;
}

int
tc_mat_svd(
    const struct tc_mat *A,
    struct tc_mat *U,
    struct tc_mat *D,
    struct tc_mat *V)
{
    if (!U || !D || !V || !A)
        return -1;
    if ((U->nr != A->nr) || (U->nc != A->nc))
        tc_mat_resize(U, A->nr, A->nc);
    if ((V->nr != A->nc) || (V->nc != A->nc))
        tc_mat_resize(V, A->nc, A->nc);
    if ((D->nr != A->nc) || (D->nc != A->nc))
        tc_mat_resize(D, A->nc, A->nc);
    if (A->nr < A->nc)
        return -2;
    /* case nc = 1 */
    if (A->nc == 1) {
        V->a[0][0] = 1.0;
        double normsq = 0.0;
        for(uint32_t i=0; i < A->nr; i++)
            normsq += A->a[i][0] * A->a[i][0];
        D->a[0][0] = sqrt(normsq);
        if (D->a[0][0] == 0.0)
            tc_mat_copy(U, A);
        else {
            double inv = 1.0 / D->a[0][0];
            for(uint32_t i=0; i < A->nr; i++)
                U->a[i][0] = A->a[i][0] * inv;
        }
        return 0;
    }
    /* Step1: A = U D V where D is bi-diagonal 
     *            A is nr x nc
     *            U is nr x nr
     *            D is nr x nc
     *            V is nc x nc
     * 
     *    If A was not a square matrix, change U and D
     *            U  nr x nc
     *            D  nc x nc
     */
    tc_mat_bidiag_decomp(A, U, D, V);
    if (D->nr != D->nc) {
        struct tc_mat *U0 = tc_mat_ctr(A->nr, A->nc);
        for(uint32_t i=0; i < A->nr; i++)
            for(uint32_t j=0; j < A->nc; j++)
                U0->a[i][j] = U->a[i][j];
        tc_mat_resize(U, A->nr, A->nc);
        tc_mat_copy(U, U0);
        tc_mat_dtr(U0);

        struct tc_mat *D0 = tc_mat_ctr(A->nc, A->nc);
        for(uint32_t i=0; i < A->nc; i++)
            for(uint32_t j=0; j < A->nc; j++)
                D0->a[i][j] = D->a[i][j];
        tc_mat_resize(D, A->nc, A->nc);
        tc_mat_copy(D, D0);
        tc_mat_dtr(D0);
    }
    /* iterate until either max_iterations has been hit
     * or the largest superdiagonal entry is below the
     * threshold. Set threshold = 1-e8 * largest element.
     * Any term less than 0.1 threshold will be considered
     * to be zero.
     */
    const uint32_t max_iterations = 100 * D->nc;
    double threshold = D->a[0][0];
    for(uint32_t i=0; i < D->nc; i++)
        if (fabs(D->a[i][i]) > threshold)
            threshold = fabs(D->a[i][i]);
    for(uint32_t i=0; i+1 < D->nc; i++)
        if (fabs(D->a[i][i+1]) > threshold)
            threshold = fabs(D->a[i][i+1]);
    threshold *= 1e-12;
    const double zero_threshold = 0.1 * threshold;
    /* Always reindex the components so that 
     * D so that has the form 
     *     [D1, 0] where D1 is diagonal and 
     *     [ 0,D2]       D2 is bidiagonal   
     * Let i0 be the starting index of D2     
     */
    uint32_t i0 = 0;
    for(uint32_t I=0; I < max_iterations; I++)
    {
        /* For any zeros on the diagonal, apply a series of 
         * Givens rotations to also zero out its off-diagonal
         * term.  Then move this component into D1.
         */
        for(uint32_t i1=i0; i1 < D->nr; i1++) {
            if (fabs(D->a[i1][i1]) > zero_threshold)
                continue;
            if ((i1+1 == D->nr) && fabs(D->a[i1-1][i1]) > zero_threshold) 
                continue;
            if ((i1+1 < D->nr) && (fabs(D->a[i1][i1+1]) > zero_threshold)) {
                for(uint32_t i=i1; i+1 < D->nr; i++) {
                    /* U D = (U G) (G^T D)  */
                    double alpha = D->a[i1][i+1];
                    if (fabs(alpha) < zero_threshold)
                        break;
                    double beta = D->a[i+1][i+1];
                    double gamma = sqrt(alpha*alpha + beta*beta);
                    double c = beta / gamma;
                    double s = alpha / gamma;
                    for(uint32_t j=0; j < D->nr; j++) {
                        double a = D->a[i1][j];
                        double b = D->a[i+1][j];
                        D->a[i1][j] = a*c - b*s;
                        D->a[i+1][j] = a*s + b*c;
                    }
                    for(uint32_t j=0; j < U->nr; j++) {
                        double a = U->a[j][i1];
                        double b = U->a[j][i+1];
                        U->a[j][i1] = a*c - b*s;
                        U->a[j][i+1] = a*s + b*c;
                    }
                }
                i1++;
            }
            /* move (i0,i1-1) down, i1 -> i0  */
            for(uint32_t j=0; j < D->nr; j++) {
                double tmp = V->a[i1][j];
                for(uint32_t k=i1; k > i0; k--)
                    V->a[k][j] = V->a[k-1][j];
                V->a[i0][j] = tmp;
            }
            for(uint32_t j=0; j < D->nr; j++) {
                double tmp = U->a[j][i1];
                for(uint32_t k=i1; k > i0; k--)
                    U->a[j][k] = U->a[j][k-1];
                U->a[j][i0] = tmp;
            }
            double tmp = D->a[i1][i1];
            double tmp1 = D->a[i1][i1+1];
            for(uint32_t k=i1; k > i0; k--) {
                D->a[k][k] = D->a[k-1][k-1];
                if (k+1 < D->nc)
                    D->a[k][k+1] = D->a[k-1][k];
            }
            D->a[i0][i0] = tmp;
            D->a[i0][i0+1] = tmp1;
            i0++;
        }
        /* For any zeros on the superdiagonal, move the
         * component to D1.
         */
        for(uint32_t i=i0; i+1 < D->nr; i++) {
            if (fabs(D->a[i][i+1]) >= zero_threshold)
                continue;
            if (i == i0) {
                i0++;
                continue;
            }
            if (i+2 != D->nr)
                continue;
            /* move (i0,i) down, i+1 -> i0    */
            for(uint32_t j=0; j < D->nr; j++) {
                double tmp = V->a[i+1][j];
                for(uint32_t k=i+1; k > i0; k--)
                    V->a[k][j] = V->a[k-1][j];
                V->a[i0][j] = tmp;
            }
            for(uint32_t j=0; j < D->nr; j++) {
                double tmp = U->a[j][i+1];
                for(uint32_t k=i+1; k > i0; k--)
                    U->a[j][k] = U->a[j][k-1];
                U->a[j][i0] = tmp;
            }
            double tmp = D->a[i+1][i+1];
            double tmp1 = D->a[i][i+1];
            for(uint32_t k=i+1; k > i0; k--) {
                D->a[k][k] = D->a[k-1][k-1];
                D->a[k-1][k] = (k-1==i0)? tmp1: D->a[k-2][k-1];
            }
            D->a[i0][i0] = tmp;
            i0++;
            if ((i+2 == D->nr) && (i0 != i))
                i--; /* retry last term */
        }
        if (i0 >= D->nr - 1)
            break;
        /* Find largest element on super diagonal.*/
        /* Break if less than threshold.          */
        double largest_off_diag = 0.0;
        uint32_t n_zeros_on_off_diagonal = 0;
        for(uint32_t i=i0; i+1 < D->nr; i++) {
            if (fabs(D->a[i][i+1]) > largest_off_diag)
                largest_off_diag = fabs(D->a[i][i+1]);
            if (fabs(D->a[i][i+1]) < zero_threshold)
                n_zeros_on_off_diagonal++;
        }
        if (largest_off_diag < threshold)
            break;
        /* Find the next zero on the uper diagonal*/
        /* Treat [i0,i1] as a block.              */
        uint32_t i1 = i0;
        for(  ; i1+1 < D->nr; i1++)
            if (fabs(D->a[i1][i1+1]) < zero_threshold)
                break;
        /* Find Wilkerson shift.                  */
        struct tc_mat *t = tc_mat_ctr(3, 2);
        for(uint32_t i=0; i < 3; i++)
            for(uint32_t j=0; j < 2; j++)
                t->a[i][j] = (i1+i>2)? D->a[i1+i-2][i1+j-1]: 0.0;
        double mu = tc_mat_wilkinson_shift(t);
        tc_mat_dtr(t);
        double alpha = D->a[i0][i0] * D->a[i0][i0] - mu;
        double beta = D->a[i0][i0] * D->a[i0][i0+1];
        /* Apply Givens rotations G from i0 to the bottom,
         * chasing the nonzero element until off the matrix                     
         */
        for(uint32_t i=i0; i < i1; i++) {
            /* D V = (D G) (G^T V)  */
            double gamma = sqrt(alpha*alpha + beta*beta);
            double c = alpha / gamma;
            double s = -beta / gamma;
            for(uint32_t j=0; j < D->nr; j++) {
                double a = D->a[j][i+0];
                double b = D->a[j][i+1];
                D->a[j][i+0] = a*c - b*s;
                D->a[j][i+1] = a*s + b*c;
            }
            for(uint32_t j=0; j < D->nc; j++) {
                double a = V->a[i+0][j];
                double b = V->a[i+1][j];
                V->a[i+0][j] = a*c - b*s;
                V->a[i+1][j] = a*s + b*c;
            }
            /* U D = (U G) (G^T D) */ 
            alpha = D->a[i+0][i];
            beta = D->a[i+1][i];
            gamma = sqrt(alpha*alpha + beta*beta);
            if (fabs(gamma) > 0.0) {
                c = alpha / gamma;
                s = -beta / gamma;
                for(uint32_t j=0; j < D->nc; j++) {
                    double a = D->a[i+0][j];
                    double b = D->a[i+1][j];
                    D->a[i+0][j] = a*c - b*s;
                    D->a[i+1][j] = a*s + b*c;
                }
                for(uint32_t j=0; j < U->nr; j++) {
                    double a = U->a[j][i+0];
                    double b = U->a[j][i+1];
                    U->a[j][i+0] = a*c - b*s;
                    U->a[j][i+1] = a*s + b*c;
                }
            }
            if (i + 2 < D->nr) {
                alpha = D->a[i][i+1];
                beta = D->a[i][i+2];
            }
        }
    }
    /* now swap components to order the diagonal terms
     * from largest to smallest 
     */
    for(uint32_t i=0; i+1 < D->nr; i++) {
        double largest = fabs(D->a[i][i]);
        uint32_t largest_i = i;
        for(uint32_t j=i+1; j < D->nr; j++) {
            if (fabs(D->a[j][j]) > largest) {
                largest_i = j;
                largest = fabs(D->a[j][j]);
            }
        }
        if (largest_i != i) {
            for(uint32_t j=0; j < D->nr; j++) {
                double tmp = V->a[i][j];
                V->a[i][j] = V->a[largest_i][j];
                V->a[largest_i][j] = tmp;
                tmp = U->a[j][i];
                U->a[j][i] = U->a[j][largest_i];
                U->a[j][largest_i] = tmp;
            }
            double tmp = D->a[i][i];
            D->a[i][i] = D->a[largest_i][largest_i];
            D->a[largest_i][largest_i] = tmp;
        }
        if (D->a[i][i] < 0) { 
            D->a[i][i] = -D->a[i][i];
            for(uint32_t j=0; j < D->nr; j++)
                V->a[i][j] = -V->a[i][j];
        }
    }
    /* just to be sure, zero out all off-diagonal terms of D */
    for(uint32_t i=0; i < D->nr; i++) 
        for(uint32_t j=0; j < D->nc; j++) 
            if (i != j)
                D->a[i][j] = 0.0;
    /* transpose V */
    tc_mat_transpose(V, V);
    return 0;
}

/****************************************************************************
 * tc_wgt                                                                   *
 ****************************************************************************/

/* tc_wgt_normalize
 * Input: wgt N x M matrix
 * Output: wgt N x M matrix
 *
 * the entries are |a_ij| / Sum |a_ij|
 */
void
tc_wgt_normalize(struct tc_mat *wgt)
{
    if (!wgt)
        return;
    double sum = 0.0;
    for(uint32_t i=0; i < wgt->nr; i++) {
        for(uint32_t j=0; j < wgt->nc; j++) {
            if (wgt->a[i][j] < 0.0)
               wgt->a[i][j] = -wgt->a[i][j];
            sum += wgt->a[i][j];
        }
    }
    if (sum == 0.0)
        return;
    for(uint32_t i=0; i < wgt->nr; i++)
        for(uint32_t j=0; j < wgt->nc; j++)
            wgt->a[i][j] = wgt->a[i][j] / sum;
}

/* tc_wgt_mean: weighted mean on the j-th column of A
 * Input: wgt N x 1 matrix
 * Input: A N x M matrix
 * Return: weighed mean of the j-th column of A, skipping NA values
 */
static double
tc_wgt_mean(const struct tc_mat *wgt, const struct tc_mat *A, uint32_t j,
    double NA)
{
    if (!wgt || !A || !A->nr || !A->nc
            || (wgt->nr != A->nr) || (j >= A->nc))
        return 0.0;
    double sum = 0.0;
    double sum_wgts = 0.0;
    for(uint32_t i=0; i < A->nr; i++) {
        double w = wgt->a[i][0];
        if (w <= 0.0)
            continue;
        if (A->a[i][j] == NA)
            continue;
        sum += w * A->a[i][j];
        sum_wgts += w;
    }
    return (sum_wgts > 0.0)? sum / sum_wgts: 0.0;
}

/* tc_wgt_median
 * Input: wgt N x 1 matrix
 * Input: A N x M matrix
 * Return: weighed median of the j-th column of A, skipping NA values
 * The weighted median is where the sum of the weights for the values below
 * is equal to the sum of the weights above, and if there is no exact midpoint
 * a weighted average of the two closest values.
 */
struct vecdouble { double wgt; double value; };
int vecdouble_cmp(const void *a, const void *b) {
    if (!a || !b) return 0;
    const struct vecdouble *aptr = (const struct vecdouble *) a;
    const struct vecdouble *bptr = (const struct vecdouble *) b;
    return (aptr->value < bptr->value)? -1: 1;
}
static double
tc_wgt_median(const struct tc_mat *wgt, const struct tc_mat *A, uint32_t j,
    double NA)
{
    if (!wgt || !A || !A->nr || !A->nc
            || (wgt->nr != A->nr) || (j >= A->nc))
        return 0.0;

    struct vecdouble *v = (struct vecdouble *)
        malloc(sizeof(struct vecdouble) * A->nr);
    uint32_t nwgts = 0;
    double sum_wgts = 0.0;
    for(uint32_t i=0; i < A->nr; i++) {
        if (A->a[i][j] == NA)
           continue; /* skip NA values */
        v[nwgts].value = A->a[i][j];
        v[nwgts].wgt = wgt->a[i][0];
        sum_wgts += wgt->a[i][0];
        nwgts++;
    }
	double mid_wgts = sum_wgts / 2.0;
    if (nwgts == 0) 
        return 0.0;

    /* iterate throught the sorted values until mid_wgts is passed */
    qsort(v, nwgts, sizeof(struct vecdouble), vecdouble_cmp);
    double median = v[0].value;
    double sum = v[0].wgt;
    for(uint32_t i=1; (i < nwgts) && (sum < mid_wgts); i++) {
        median = v[i].value;
        sum += v[i].wgt;
    }
    free(v);
    return median;
}

/* tc_wgt_prin_comp
 * wCVM = weighted covariance matrix of M
 * U D V^T = singular value decomposition of wCVM
 * loadings = first column of U
 * scores = (M-colavgs(M)) * loadings
 */
int
tc_wgt_prin_comp(
    const struct tc_mat *wgt /* Reputation Vector */,
    const struct tc_mat *M /* Vote Matrix*/,
    struct tc_mat *loadings,
    struct tc_mat *scores)
{
    if (!wgt || !M || !loadings || !scores)
        return -1;
    if ((M->nr <= 1) || (wgt->nr != M->nr))
        return -1;

    int rc = 0;

    /* X = M minus its column weighted averages */
    struct tc_mat *x_mat = tc_mat_ctr(M->nr, M->nc);
    double **x = (double **) x_mat->a;
    for(uint32_t j=0; j < M->nc; j++) {
        double avg = 0.0;
        for(uint32_t i=0; i < M->nr; i++)
            avg += wgt->a[i][0] * M->a[i][j];
        for(uint32_t i=0; i < M->nr; i++)
            x[i][j] = M->a[i][j] - avg;
    }
    /* wCMV = weighted covariance matrix of M */
    double wgts2 = 0.0;
    for(uint32_t i=0; i < M->nr; i++)
        wgts2 += wgt->a[i][0] * wgt->a[i][0];
    double factor = 1.0/(1.0 - wgts2);
    struct tc_mat *wCVM = tc_mat_ctr(M->nc, M->nc);
    for(uint32_t i=0; i < M->nc; i++) {
        for(uint32_t j=0; j <= i; j++) {
            double sum = 0.0;
            for(uint32_t k=0; k < M->nr; k++)
                sum += wgt->a[k][0] * x[k][i] * x[k][j];
            wCVM->a[i][j] =
            wCVM->a[j][i] = factor * sum;
        }
    }
    /* SVD of wCVM */
    struct tc_mat *U = tc_mat_ctr(0, 0);
    struct tc_mat *D = tc_mat_ctr(0, 0);
    struct tc_mat *V = tc_mat_ctr(0, 0);
    rc = tc_mat_svd(wCVM, U, D, V);
    if (!rc) {
        tc_mat_resize(loadings, M->nc, 1);
        for(uint32_t i=0; i < M->nc; i++)
            loadings->a[i][0] = U->a[i][0];
        tc_mat_mult(scores, x_mat, loadings);
    }

    tc_mat_dtr(wCVM);
    tc_mat_dtr(U);
    tc_mat_dtr(D);
    tc_mat_dtr(V);
    tc_mat_dtr(x_mat);
    return rc;
}

/****************************************************************************
 * tc_vote                                                                  *
 ****************************************************************************/

struct tc_vote *
tc_vote_ctr(uint32_t nr, uint32_t nc)
{
	struct tc_vote *ptr = (struct tc_vote *) malloc(sizeof(struct tc_vote));
    ptr->nr = nr;
    ptr->nc = nc;
	ptr->M = tc_mat_ctr(nr, nc);
    for(uint32_t i=0; i < TC_VOTE_NCOLS; i++)
        ptr->cvecs[i] = tc_mat_ctr(1, nc);
    for(uint32_t i=0; i < TC_VOTE_NROWS; i++)
        ptr->rvecs[i] = tc_mat_ctr(nr, 1);
	return ptr;
}

void
tc_vote_dtr(struct tc_vote *ptr)
{
    if (!ptr)
        return;
    tc_mat_dtr(ptr->M);
    for(uint32_t i=0; i < TC_VOTE_NCOLS; i++)
        tc_mat_dtr(ptr->cvecs[i]);
    for(uint32_t i=0; i < TC_VOTE_NROWS; i++)
        tc_mat_dtr(ptr->rvecs[i]);
    free(ptr);
}

int
tc_vote_notvalid(const struct tc_vote *ptr)
{
    if (!ptr)
        return -1;
    if (!ptr->M)
        return -1;
    for(uint32_t i=0; i < TC_VOTE_NCOLS; i++)
        if ((!ptr->cvecs[i])
            || (ptr->cvecs[i]->nr != 1)
            || (ptr->cvecs[i]->nc != ptr->M->nc))
                return -1;
    for(uint32_t i=0; i < TC_VOTE_NROWS; i++)
        if ((!ptr->rvecs[i])
            || (ptr->rvecs[i]->nr != ptr->M->nr)
            || (ptr->rvecs[i]->nc != 1))
                return -1;
    return 0;
}

static int
tc_vote_print_M(const struct tc_vote *ptr)
{
    const double **M = (const double **) ptr->M->a;
    for(uint32_t i=0; i < ptr->nr; i++) {
        for(uint32_t j=0; j < ptr->nc; j++)
            if (M[i][j] != ptr->NA)
                printf(" %12.8f", M[i][j]);
            else
                printf(" %12s", "NA");
        printf("\n");
    }

    return 0;
}

static int
tc_vote_print_cols(const struct tc_vote *ptr)
{
    const char *hdrs[TC_VOTE_NCOLS] = {"Is Binary", "1st Loading",
        "Decision Raw", "Consensus", "Certainty", "NACol",
        "Partic Col", "AuthorBonus", "Decision Fin" };

    for(uint32_t i=0; i < TC_VOTE_NCOLS; i++) {
        printf(" %12s", hdrs[i]);  
        for(uint32_t j=0; j < ptr->nc; j++)
            if (ptr->cvecs[i]->a[0][j] != ptr->NA)
                printf(" %12.8f", ptr->cvecs[i]->a[0][j]);
            else
                printf(" %12s", "NA");
        printf("\n");
    }

    return 0;
}

static int
tc_vote_print_rows(const struct tc_vote *ptr)
{
    const char *hdrs[TC_VOTE_NROWS] = { "OldRep", "ThisRep", "SmoothRep",
        "NARow", "Partic Row", "RelativePart", "RowBonus" }; 

    for(uint32_t j=0; j < TC_VOTE_NROWS; j++)
        printf(" %12s", hdrs[j]);  
    printf("\n");

    for(uint32_t i=0; i < ptr->nr; i++) {
        for(uint32_t j=0; j < TC_VOTE_NROWS; j++) 
            if (ptr->rvecs[j]->a[i][0] != ptr->NA)
                printf(" %12.8f", ptr->rvecs[j]->a[i][0]);
            else
                printf(" %12s", "NA");
        printf("\n");
    }

    return 0;
}

int
tc_vote_print(const struct tc_vote *ptr)
{
    if (!ptr || tc_vote_notvalid(ptr))
        return -1;
    tc_vote_print_M(ptr);
    printf("\n");
    tc_vote_print_rows(ptr);
    printf("\n");
    tc_vote_print_cols(ptr);
    printf("\n");
    return 0;
}

int
tc_vote_proc(struct tc_vote *vote)
{
    if (!vote || tc_vote_notvalid(vote))
        return -1;

    struct tc_mat *M = vote->M;
    struct tc_mat *wgt = vote->rvecs[TC_VOTE_OLD_REP];
    struct tc_mat *twgt = vote->rvecs[TC_VOTE_THIS_REP];
    struct tc_mat *nwgt = vote->rvecs[TC_VOTE_SMOOTHED_REP];
    struct tc_mat *isbin = vote->cvecs[TC_VOTE_IS_BINARY];
    struct tc_mat *firstloading = vote->cvecs[TC_VOTE_FIRST_LOADING];

    double wgtavg = 0.0;
    for(uint32_t i=0; i < wgt->nr; i++)
        wgtavg += wgt->a[i][0];
    wgtavg /= wgt->nr;
    if (wgtavg == 0.0)
        return -1;

    /* fM: M with NAs filled in with the preliminary outcomes */
    struct tc_mat *fM = tc_mat_ctr(M->nr, M->nc);
    tc_mat_copy(fM, M);
    for(uint32_t j=0; j < M->nc; j++) {
        double prelim_outcome = (isbin->a[0][j] > 0.0)?
            tc_wgt_mean(wgt, M, j, vote->NA):
            tc_wgt_median(wgt, M, j, vote->NA);
        for(uint32_t i=0; i < M->nr; i++)
            if (fM->a[i][j] == vote->NA)
                fM->a[i][j] = prelim_outcome;
    }

    /* loadings: 
     * scores: 
     */
    struct tc_mat *scores = tc_mat_ctr(0, 0);
    int rc = tc_wgt_prin_comp(wgt, fM, firstloading, scores);
    if (rc < 0) {
        tc_mat_dtr(scores);
        tc_mat_dtr(fM);
        return rc;
    }
    tc_mat_transpose(firstloading, firstloading);
    
    /* wgtT: transpose of wgt */
    struct tc_mat *wgtT = tc_mat_ctr(0, 0);
    tc_mat_transpose(wgtT, wgt);

    /* wgtT_M: wgt^T * fM */
    struct tc_mat *wgtT_fM = tc_mat_ctr(0, 0);
    tc_mat_mult(wgtT_fM, wgtT, fM);

    /* scores1: scores adjusted by adding min{scores} */
    double min_score = scores->a[0][0];
    for(uint32_t i=1; i < scores->nr; i++)
        if (min_score > scores->a[i][0])
            min_score = scores->a[i][0];
    if (min_score < 0.0)
        min_score = -min_score;
    struct tc_mat *scores1 = tc_mat_ctr(0, 0);
    tc_mat_copy(scores1, scores);
    for(uint32_t i=0; i < scores1->nr; i++)
        scores1->a[i][0] += min_score;
    struct tc_mat *scores1T = tc_mat_ctr(0, 0);
    tc_mat_transpose(scores1T, scores1);
    struct tc_mat *v1 = tc_mat_ctr(0, 0);
    tc_mat_mult(v1, scores1T, fM);
    tc_wgt_normalize(v1);
    tc_mat_sub(v1, v1, wgtT_fM);

    /* scores2: scores adjusted by subtracting max{scores} */
    double max_score = scores->a[0][0];
    for(uint32_t i=1; i < scores->nr; i++)
        if (max_score < scores->a[i][0])
            max_score = scores->a[i][0];
    struct tc_mat *scores2 = tc_mat_ctr(0, 0);
    tc_mat_copy(scores2, scores);
    for(uint32_t i=0; i < scores2->nr; i++)
        scores2->a[i][0] -= max_score;
    struct tc_mat *scores2T = tc_mat_ctr(0, 0);
    tc_mat_transpose(scores2T, scores2);
    struct tc_mat *v2 = tc_mat_ctr(0, 0);
    tc_mat_mult(v2, scores2T, fM);
    tc_wgt_normalize(v2);
    tc_mat_sub(v2, v2, wgtT_fM);

    /* twgt: reputation vector of this round of votes  */
    /* twgt = (||v1|| < ||v2||)? score1: score2          */
    /* terms are mult by wgt_i / wgtavg, then normalized */
    if (tc_mat_norm(v1) <= tc_mat_norm(v2))
        tc_mat_copy(twgt, scores1);
    else
        tc_mat_copy(twgt, scores2); 
    for(uint32_t i=0; i < wgt->nr; i++)
        twgt->a[i][0] *= wgt->a[i][0] / wgtavg;
    tc_wgt_normalize(twgt);

    /* smoothedrep: smoothed with previous oldrep   */
    /* smoothedrep: (1-alpha) oldrep + alpha * smoothedrep */
    for(uint32_t i=0; i < wgt->nr; i++)
        nwgt->a[i][0] = (1.0 - vote->alpha) * wgt->a[i][0] 
                         + vote->alpha * twgt->a[i][0];

    /* outcome (raw) */
    struct tc_mat *decraw = vote->cvecs[TC_VOTE_DECISIONS_RAW];
    for(uint32_t j=0; j < fM->nc; j++) {
        decraw->a[0][j] = (isbin->a[0][j] > 0.0)?
            tc_wgt_mean(nwgt, fM, j, vote->NA):
            tc_wgt_median(nwgt, fM, j, vote->NA);
    }

    /* outcome (final) */
    struct tc_mat *decfin = vote->cvecs[TC_VOTE_DECISIONS_FINAL];
    for(uint32_t j=0; j < M->nc; j++) {
        if (isbin->a[0][j] > 0.0) {
            if (decraw->a[0][j] > 0.50 + 0.50*vote->tol)
                decfin->a[0][j] = 1.0;
            else
            if (decraw->a[0][j] < 0.50 - 0.50*vote->tol)
                decfin->a[0][j] = 0.0;
            else
                decfin->a[0][j] = 0.50;
        }
        else
            decfin->a[0][j] = decraw->a[0][j];
    }

    /* row stats */
    struct tc_mat *narow = vote->rvecs[TC_VOTE_NA_ROW];
    struct tc_mat *partrow = vote->rvecs[TC_VOTE_PARTIC_ROW];
    for(uint32_t i=0; i < M->nr; i++) {
        narow->a[i][0] = 0;
        for(uint32_t j=0; j < M->nc; j++)
            if (M->a[i][j] == vote->NA)
                narow->a[i][0] += 1.0;
        partrow->a[i][0] = 1.0 - narow->a[i][0] /  M->nc;
    }

    /* col stats */
    struct tc_mat *nacol = vote->cvecs[TC_VOTE_NA_COL];
    struct tc_mat *partcol = vote->cvecs[TC_VOTE_PARTIC_COL];
    for(uint32_t j=0; j < M->nc; j++) {
        double value = 0.0;
        nacol->a[0][j] = 0;
        for(uint32_t i=0; i < M->nr; i++)
            if (M->a[i][j] == vote->NA) {
                nacol->a[0][j] += 1.0;
                value += nwgt->a[i][0];
            }
        partcol->a[0][j] = 1.0 - value;
    }

    /* fracNA */
    double x = 0.0;
    for(uint32_t j=0; j < M->nc; j++)
        x += partcol->a[0][j];
    double fracNA = 1.0 - x / M->nc;

    /* row bonus */
    struct tc_mat *partic_rel = vote->rvecs[TC_VOTE_PARTIC_REL];
    tc_mat_copy(partic_rel, partrow);
    tc_wgt_normalize(partic_rel);
    struct tc_mat *rowbonus = vote->rvecs[TC_VOTE_ROW_BONUS];
    for(uint32_t i=0; i < M->nr; i++)
        rowbonus->a[i][0] = fracNA * partic_rel->a[i][0] + (1.0 - fracNA) * nwgt->a[i][0];

    /* certainty */
    struct tc_mat *certainty = vote->cvecs[TC_VOTE_CERTAINTY];
    for(uint32_t j=0; j < M->nc; j++) {
        double sum = 0.0;
        for(uint32_t i=0; i < M->nr; i++)
            if (fabs(fM->a[i][j] - decfin->a[0][j]) < 1e-5)
                sum += nwgt->a[i][0];
        certainty->a[0][j] = sum;
    }

    /* col bonus */
    struct tc_mat *conreward = vote->cvecs[TC_VOTE_CONSENSUS_REW];
    struct tc_mat *partic_rel_col = tc_mat_ctr(0, 0);
    tc_mat_copy(partic_rel_col, partcol);
    tc_wgt_normalize(partic_rel_col);
    tc_mat_copy(conreward, certainty);
    tc_wgt_normalize(conreward);
    struct tc_mat *colbonus = vote->cvecs[TC_VOTE_AUTHOR_BONUS];
    for(uint32_t j=0; j < M->nc; j++)
        colbonus->a[0][j] = fracNA * partic_rel_col->a[0][j] + (1.0 - fracNA) * conreward->a[0][j];
    tc_mat_dtr(partic_rel_col);

    tc_mat_dtr(v2);
    tc_mat_dtr(scores2T);
    tc_mat_dtr(scores2);
    tc_mat_dtr(v1);
    tc_mat_dtr(scores1T);
    tc_mat_dtr(scores1);
    tc_mat_dtr(wgtT_fM);
    tc_mat_dtr(wgtT);
    tc_mat_dtr(scores);
    tc_mat_dtr(fM);

    return rc;
}


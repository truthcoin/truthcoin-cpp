/* 
 * Copyright (c) 2015 The Truthcoin Core developers
 * Distributed under the MIT software license, see the accompanying
 * file COPYING or http://www.opensource.org/licenses/mit-license.php.
 */

#ifndef TRUTHCOIN_LINALG_MAT_H
#define TRUTHCOIN_LINALG_MAT_H

#include <stdint.h>

struct tc_mat {
    double **a;
    uint32_t nr, nc;
};
struct tc_mat *tc_mat_ctr(uint32_t nr_, uint32_t nc_);
void tc_mat_dtr(struct tc_mat *);
void tc_mat_clear(struct tc_mat *);
void tc_mat_print(const struct tc_mat *);
double tc_mat_norm(const struct tc_mat *);
void tc_mat_resize(struct tc_mat *, uint32_t nr_, uint32_t nc_);
void tc_mat_copy(struct tc_mat *, const struct tc_mat *);
void tc_mat_identity(struct tc_mat *);
void tc_mat_transpose(struct tc_mat *, const struct tc_mat *);
int tc_mat_add(struct tc_mat *, const struct tc_mat *A, const struct tc_mat *B);
int tc_mat_sub(struct tc_mat *, const struct tc_mat *A, const struct tc_mat *B);
int tc_mat_mult(struct tc_mat *, const struct tc_mat *A, const struct tc_mat *B);
int tc_mat_mult_scalar(struct tc_mat *, double a, const struct tc_mat *B);
int tc_mat_bidiag_decomp(const struct tc_mat *A, struct tc_mat *U, struct tc_mat *B, struct tc_mat *V);
int tc_mat_svd(const struct tc_mat *A, struct tc_mat *U, struct tc_mat *D, struct tc_mat *V);

void tc_wgt_normalize(struct tc_mat *wgt);
int tc_wgt_prin_comp(const struct tc_mat *wgt, const struct tc_mat *M,
    struct tc_mat *loadings, struct tc_mat *scores);

#define TC_VOTE_NCOLS           9
#define TC_VOTE_NROWS           7

#define TC_VOTE_OLD_REP         0
#define TC_VOTE_THIS_REP        1
#define TC_VOTE_SMOOTHED_REP    2
#define TC_VOTE_NA_ROW          3
#define TC_VOTE_PARTIC_ROW      4
#define TC_VOTE_PARTIC_REL      5
#define TC_VOTE_ROW_BONUS       6

#define TC_VOTE_IS_BINARY       0
#define TC_VOTE_FIRST_LOADING   1
#define TC_VOTE_DECISIONS_RAW   2
#define TC_VOTE_CONSENSUS_REW   3
#define TC_VOTE_CERTAINTY       4
#define TC_VOTE_NA_COL          5
#define TC_VOTE_PARTIC_COL      6
#define TC_VOTE_AUTHOR_BONUS    7
#define TC_VOTE_DECISIONS_FINAL 8

struct tc_vote {
    struct tc_mat *M; /* Vote Matrix */
    struct tc_mat *cvecs[TC_VOTE_NCOLS]; /* column (Decision) vectors */
    struct tc_mat *rvecs[TC_VOTE_NROWS]; /* row (Voter) vectors */
    double NA;
    double alpha;
    double tol;
    uint32_t nr, nc;
};
struct tc_vote *tc_vote_ctr(uint32_t nr, uint32_t nc);
void tc_vote_dtr(struct tc_vote *);
int tc_vote_notvalid(const struct tc_vote *);
int tc_vote_print(const struct tc_vote *);
int tc_vote_proc(struct tc_vote *);


#endif /* TRUTHCOIN_LINALG_MAT_H */


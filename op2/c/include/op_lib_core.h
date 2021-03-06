/*
 * Open source copyright declaration based on BSD open source template:
 * http://www.opensource.org/licenses/bsd-license.php
 *
 * This file is part of the OP2 distribution.
 *
 * Copyright (c) 2011, Mike Giles and others. Please see the AUTHORS file in
 * the main source directory for a full list of copyright holders.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * The name of Mike Giles may not be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Mike Giles ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Mike Giles BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __OP_LIB_CORE_H
#define __OP_LIB_CORE_H

/*
 * This header file declares all types and functions required by *any* OP2
 * implementation, i.e. independently of the back-end and the target
 * languages.  It is typically used by language or back-end specific top level
 * OP2 libraries
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <math.h>
#include <stdarg.h>

/*
 * essential typedefs
 */

typedef unsigned int uint;
typedef long long ll;
typedef unsigned long long ull;

/*
 * OP2 global state variables
 */

/*
 * OP diagnostics level
   0            none
   1 or above   error-checking
   2 or above   info on plan construction
   3 or above   report execution of parallel loops
   4 or above   report use of old plans
   7 or above   report positive checks in op_plan_check
*/

extern int OP_diags;

extern int OP_cache_line_size;

/*
 * enum list for op_par_loop
 */

typedef enum { OP_READ, OP_WRITE, OP_RW, OP_INC, OP_MIN, OP_MAX } op_access;

typedef enum { OP_ARG_GBL, OP_ARG_DAT, OP_ARG_MAT } op_arg_type;

/*
 * symbolic indices
 */

/* index for direct mapping */
#define OP_NONE -1
/* all indices selector */
#define OP_ALL -2
/* op_i(int) translation */
#define OP_I_OFFSET -1024
#define op_i(idx) (OP_I_OFFSET - (idx))

/*
 * structures
 */

typedef struct
{
  int         index;        /* index */
  int         size;         /* number of elements in set */
  char const *name;         /* name of set */
// for MPI support
  int         exec_size;    /* number of additional imported elements to be executed */
  int         nonexec_size; /* number of additional imported elements that are not executed */
} op_set_core;

typedef op_set_core * op_set;

typedef struct
{
  int         index;  /* index */
  op_set      from,   /* set pointed from */
              to;     /* set pointed to */
  int         dim,    /* dimension of pointer */
             *map;    /* array defining pointer */
  char const *name;   /* name of pointer */
} op_map_core;

typedef op_map_core * op_map;

typedef struct
{
  int         index;  /* index */
  op_set      set;    /* set on which data is defined */
  int         dim,    /* dimension of data */
              size;   /* size of each element in dataset */
  char       *data,   /* data on host */
             *data_d; /* data on device (GPU) */
  char const *type,   /* datatype */
             *name;   /* name of dataset */
  char*      buffer_d; /* buffer for MPI halo sends on the devidce */
} op_dat_core;

typedef op_dat_core * op_dat;

typedef struct {
  op_map rowmap,
         colmap;
  size_t nrows;       /* number of rows */
  size_t ncols;       /* number of columns */
  int    *nnz;        /* vector of number of nonzeros per row */
  int    total_nz;    /* total nonzeros in the pattern sum(nnz) */
  int    *rowptr;     /* csr row pointer (accumulation of nnz) */
  int    *colidx;     /* csr column indices for each row */
  size_t max_nonzeros;/* maximum number of nonzeros per row */
  char const *name;   /* name of dataset */
} op_sparsity_core;

typedef op_sparsity_core * op_sparsity;

typedef struct
{
  int         index;  /* index */
  op_set      rowset, /* set on which row data is defined */
              colset; /* set on which col data is defined */
  int         dim,    /* dimension of data */
              size;   /* size of each element in dataset */
  void*       mat;    /* handle to matrix object from 3rd party library */
  char const *type,   /* datatype */
             *name;   /* name of matrix */
  op_sparsity sparsity;         /* sparsity structure */
  char       *data;   /* matrix entries (device) */
} op_mat_core;

typedef op_mat_core * op_mat;

typedef struct
{
  op_set set;                   /* set associated with the iteration space */
  int    ndims;                 /* number of extra iteration dimensions */
  int   *dims;                  /* upper extent of the extra iteration dimensions */
} op_itspace_core;

typedef op_itspace_core * op_itspace;

typedef struct
{
  int         index;  /* index */
  op_dat      dat;    /* dataset (not used for matrix) */
  op_mat      mat;    /* matrix (not used for dat) */
  op_map      map,    /* indirect mapping */
              map2;   /* 2nd mapping only used for matrices*/
  int         dim,    /* dimension of data */
              idx,    /* index into the mapping */
              idx2,   /* 2nd index only used for matrices */
              size;   /* size (for sequential execution) */
  char       *data,   /* data on host */
             *data_d; /* data on device (for CUDA execution) */
  char const *type;   /* datatype */
  op_access   acc;
  op_arg_type argtype;
} op_arg;


typedef struct
{
  char const *name;     /* name of kernel function */
  int         count;    /* number of times called */
  float       time;     /* total execution time */
  float       transfer; /* bytes of data transfer (used) */
  float       transfer2;/* bytes of data transfer (total) */
} op_kernel;


/*
 * min / max definitions
 */

#ifndef MIN
#define MIN(a,b) ((a<b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a,b) ((a>b) ? (a) : (b))
#endif

/*
 * alignment macro based on example on page 50 of CUDA Programming Guide version 3.0
 * rounds up to nearest multiple of 16 bytes
 */

#define ROUND_UP(bytes) (((bytes) + 15) & ~15)

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Core lib function prototypes
 */

void op_init_core ( int, char **, int );

void op_exit_core ( void );

op_set op_decl_set_core ( int, char const * );

op_map op_decl_map_core ( op_set, op_set, int, int *, char const * );

op_dat op_decl_dat_core ( op_set, int, char const *, int, char *, char const * );

op_mat op_decl_mat_core ( op_set rowset, op_set colset, int dim, char const * type, int size, char const * name );

op_sparsity op_decl_sparsity_core ( op_map rowmap, op_map colmap, char const * name );

void op_build_sparsity_pattern ( op_map rowmap, op_map colmap, op_sparsity sparsity );

void op_decl_const_core ( int dim, char const * type, int typeSize, char * data, char const * name );

void op_err_print ( const char * error_string, int m, const char * name );

void op_arg_check ( op_set, int, op_arg, int *, char const * );

op_arg op_arg_dat_core ( op_dat dat, int idx, op_map map, int dim, const char * typ, op_access acc );

op_arg op_arg_mat_core ( op_mat mat, int rowidx, op_map rowmap, int colidx, op_map colmap, int dim, const char * typ, op_access acc );

op_arg op_arg_gbl_core ( char *, int, const char *, op_access );

void op_diagnostic_output ( void );

void op_timing_output ( void );

void op_timing_output_2_file ( const char * );

void op_timing_realloc ( int );

void op_timers_core( double *cpu, double *et );

void op_dump_dat ( op_dat data );

#ifdef __cplusplus
}
#endif

#endif /* __OP_LIB_CORE_H */


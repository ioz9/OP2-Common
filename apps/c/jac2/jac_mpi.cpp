/*
  Open source copyright declaration based on BSD open source template:
  http://www.opensource.org/licenses/bsd-license.php

* Copyright (c) 2009-2011, Mike Giles
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

//
// test program for new OPlus2 development
//

//
// standard headers
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>


//
// mpi header file - included by user for user level mpi
//
#include <mpi.h>


// global constants

double alpha;


//
// OP header file
//

#include "op_lib_mpi.h"

//
// kernel routines for parallel loops
//

#include "res.h"
#include "update.h"

//                                                   
// op_par_loop declarations                          
//                                                   

#include "op_mpi_seq.h"


//
//user declared functions
//
int compute_local_size (int global_size, int mpi_comm_size, int mpi_rank )
{
  	  int local_size = global_size/mpi_comm_size;
  	  int remainder = (int)fmod(global_size,mpi_comm_size);
  
  	  if (mpi_rank < remainder)
  	  {
  	  	  local_size = local_size + 1;
  	  
  	  }
  	  return local_size;
}

void scatter_double_array(double* g_array, double* l_array, int comm_size, int g_size, 
	int l_size, int elem_size)
{
  	  int* sendcnts = (int *) malloc(comm_size*sizeof(int));
  	  int* displs = (int *) malloc(comm_size*sizeof(int));
  	  int disp = 0;
  
  	  for(int i = 0; i<comm_size; i++)
  	  {
  	  	  sendcnts[i] =   elem_size*compute_local_size (g_size, comm_size, i);	
  	  }
  	  for(int i = 0; i<comm_size; i++)
  	  {	
  	  	  displs[i] =   disp;
  	  	  disp = disp + sendcnts[i];
  	  }
  
  	  MPI_Scatterv(g_array, sendcnts, displs, MPI_DOUBLE, l_array, 
  	  	  l_size*elem_size, MPI_DOUBLE, MPI_ROOT,  MPI_COMM_WORLD ); 
  	  
  	  free(sendcnts);
  	  free(displs);
}

void scatter_int_array(int* g_array, int* l_array, int comm_size, int g_size, 
	int l_size, int elem_size)
{
  	  int* sendcnts = (int *) malloc(comm_size*sizeof(int));
  	  int* displs = (int *) malloc(comm_size*sizeof(int));
  	  int disp = 0;
  
  	  for(int i = 0; i<comm_size; i++)
  	  {
  	  	  sendcnts[i] =   elem_size*compute_local_size (g_size, comm_size, i);	
  	  }
  	  for(int i = 0; i<comm_size; i++)
  	  {	
  	  	  displs[i] =   disp;
  	  	  disp = disp + sendcnts[i];
  	  }
  
  	  MPI_Scatterv(g_array, sendcnts, displs, MPI_INT, l_array, 
  	  	  l_size*elem_size, MPI_INT, MPI_ROOT,  MPI_COMM_WORLD ); 
  	  
  	  free(sendcnts);
  	  free(displs);
}

// define problem size

#define NN       6
#define NITER    2


// main program

int main(int argc, char **argv){

  int my_rank;
  int comm_size;
  
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
  
  //timer
  double cpu_t1, cpu_t2, wall_t1, wall_t2;                                        
  double time;
  double max_time;

  
  int   nnode, nedge, n, e;
  double dx;

  nnode = (NN-1)*(NN-1);
  nedge = (NN-1)*(NN-1) + 4*(NN-1)*(NN-2);
  dx    = 1.0f / ((double) NN);

  int    *pp = (int *)malloc(sizeof(int)*2*nedge);
  int    *p1 = (int *)malloc(sizeof(int)*nedge);
  int    *p2 = (int *)malloc(sizeof(int)*nedge);

  double  *xe = (double *)malloc(sizeof(double)*2*nedge);
  double  *xn = (double *)malloc(sizeof(double)*2*nnode);

  double *A  = (double *)malloc(sizeof(double)*3*nedge);
  double  *r  = (double *)malloc(sizeof(double)*2*nnode);
  double  *u  = (double *)malloc(sizeof(double)*2*nnode);
  double  *du = (double *)malloc(sizeof(double)*3*nnode);

  // create matrix and r.h.s., and set coordinates needed for renumbering / partitioning

  e = 0;

  for (int i=1; i<NN; i++) {
    for (int j=1; j<NN; j++) {
      n         = i-1 + (j-1)*(NN-1);
      r[2*n]      = 0.0f;
      u[2*n]      = 0.0f;
      du[3*n]     = 0.0f;
      xn[2*n  ] = i*dx;
      xn[2*n+1] = j*dx;

      p1[e]     = n;
      p2[e]     = n;
      pp[2*e]   = p1[e];
      pp[2*e+1] = p2[e];
      A[3*e]      = -1.0f;
      xe[2*e  ] = i*dx;
      xe[2*e+1] = j*dx;
      e++;

      for (int pass=0; pass<4; pass++) {
        int i2 = i;
        int j2 = j;
        if (pass==0) i2 += -1;
        if (pass==1) i2 +=  1;
        if (pass==2) j2 += -1;
        if (pass==3) j2 +=  1;

        if ( (i2==0) || (i2==NN) || (j2==0) || (j2==NN) ) {
          r[2*n] += 0.25f;
	}
        else {
          p1[e]     = n;
          p2[e]     = i2-1 + (j2-1)*(NN-1);
          pp[2*e]   = p1[e];
          pp[2*e+1] = p2[e];
          A[3*e]      = 0.25f;
          xe[2*e  ] = i*dx;
          xe[2*e+1] = j*dx;
          e++;
        }
      }
    }
  }

  // OP initialisation

  op_init(argc,argv,5);

  // declare sets, pointers, and datasets

  op_set nodes = op_decl_set(nnode, "nodes");
  op_set edges = op_decl_set(nedge, "edges");

  op_map ppedge = op_decl_map(edges,nodes,2,pp, "ppedge");

  op_dat p_A  = op_decl_dat(edges,3,"double",A,  "p_A" );
  op_dat p_r  = op_decl_dat(nodes,2,"double", r,  "p_r" );
  op_dat p_u  = op_decl_dat(nodes,2,"double", u,  "p_u" );
  op_dat p_du = op_decl_dat(nodes,3,"double", du, "p_du");

  alpha = 2.0f;
  op_decl_const(1,"double",&alpha);
  alpha = 1.0f;
  op_decl_const(1,"double",&alpha);

  op_diagnostic_output();

  // main iteration loop

  double u_sum, u_max, beta = 1.0f;

  for (int iter=0; iter<NITER; iter++) {
    op_par_loop(res,"res", edges,
                op_arg_dat(p_A, -1,OP_ID,  3,"double",OP_READ),
                op_arg_dat(p_u,  1,ppedge, 2,"double", OP_READ),
                op_arg_dat(p_du, 0,ppedge, 3,"double", OP_INC ),
                op_arg_gbl(&beta,1,"double",OP_READ));

    u_sum = 0.0f;
    u_max = 0.0f;
    op_par_loop(update,"update", nodes,
                op_arg_dat(p_r,  -1,OP_ID, 2,"double",OP_READ),
                op_arg_dat(p_du, -1,OP_ID, 3,"double",OP_RW  ),
                op_arg_dat(p_u,  -1,OP_ID, 2,"double",OP_INC ),
                op_arg_gbl(&u_sum,1,"double",OP_INC),
                op_arg_gbl(&u_max,1,"double",OP_MAX));
    printf("\n u max/rms = %f %f \n\n",u_max, sqrt(u_sum/nnode));
  }

  // print out results

  printf("\n  Results after %d iterations:\n\n",NITER);

  op_fetch_data(p_u);
  /*
  op_fetch_data(p_du);
  op_fetch_data(p_r);
  */

  for (int pass=0; pass<1; pass++) {
    /*
    if(pass==0)      printf("\narray u\n");
    else if(pass==1) printf("\narray du\n");
    else if(pass==2) printf("\narray r\n");
    */

    for (int j=NN-1; j>0; j--) {
      for (int i=1; i<NN; i++) {
        if (pass==0)
	  printf(" %7.4f",u[2*(i-1 + (j-1)*(NN-1))]);
        else if (pass==1)
          printf(" %7.4f",du[i-1 + (j-1)*(NN-1)]);
        else if (pass==2)
          printf(" %7.4f",r[2*(i-1 + (j-1)*(NN-1))]);
      }
      printf("\n");
    }
    printf("\n");
  }

  op_timing_output();

  op_exit();

  // free allocated arrays

  free(pp);
  free(A);
  free(r);
  free(u);
  free(du);
}
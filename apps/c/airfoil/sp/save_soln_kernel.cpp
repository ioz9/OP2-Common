// 
// auto-generated by op2.m on 05-Jan-2012 10:50:49 
//

// user function                                              
                                                              
#include "save_soln.h"                                        
                                                              
                                                              
// x86 kernel function                                        
                                                              
void op_x86_save_soln(                                        
  float *arg0,                                                
  float *arg1,                                                
  int   start,                                                
  int   finish ) {                                            
                                                              
                                                              
  // process set elements                                     
                                                              
  for (int n=start; n<finish; n++) {                          
                                                              
    // user-supplied kernel call                              
                                                              
    save_soln(  arg0+n*4, arg1+n*4 );                         
  }                                                           
}                                                             
                                                              
                                                              
// host stub function                                         
                                                              
void op_par_loop_save_soln(char const *name, op_set set,      
  op_arg arg0,                                                
  op_arg arg1 ){                                              
                                                              
                                                              
  if (OP_diags>2) {                                           
    printf(" kernel routine w/o indirection:  save_soln \n"); 
  }                                                           
                                                              
  // initialise timers                                        
                                                              
  double cpu_t1, cpu_t2, wall_t1, wall_t2;                    
  op_timers_core(&cpu_t1, &wall_t1);                               
                                                              
  // set number of threads                                    
                                                              
#ifdef _OPENMP                                                
  int nthreads = omp_get_max_threads( );                      
#else                                                         
  int nthreads = 1;                                           
#endif                                                        
                                                              
  // execute plan                                             
                                                              
#pragma omp parallel for                                      
  for (int thr=0; thr<nthreads; thr++) {                      
    int start  = (set->size* thr   )/nthreads;                
    int finish = (set->size*(thr+1))/nthreads;                
    op_x86_save_soln( (float *) arg0.data,                    
                      (float *) arg1.data,                    
                      start, finish );                        
  }                                                           
                                                              
  // combine reduction data                                   
                                                              
  // update kernel record                                     
                                                              
  op_timers_core(&cpu_t2, &wall_t2);                               
  op_timing_realloc(0);                                       
  OP_kernels[0].name      = name;                             
  OP_kernels[0].count    += 1;                                
  OP_kernels[0].time     += wall_t2 - wall_t1;                
  OP_kernels[0].transfer += (float)set->size * arg0.size;     
  OP_kernels[0].transfer += (float)set->size * arg1.size;     
}                                                             
                                                              

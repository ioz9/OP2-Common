// 
// auto-generated by op2.m on 20-Dec-2011 15:33:56 
//

// header                        
                                 
#include "op_lib_cpp.h"          
#include "op_openmp_rt_support.h"
                                 
// global constants              
                                 
extern float gam;                
extern float gm1;                
extern float cfl;                
extern float eps;                
extern float mach;               
extern float alpha;              
extern float qinf[4];            
                                 
// user kernel files             
                                 
#include "save_soln_kernel.cpp"  
#include "adt_calc_kernel.cpp"   
#include "res_calc_kernel.cpp"   
#include "bres_calc_kernel.cpp"  
#include "update_kernel.cpp"     
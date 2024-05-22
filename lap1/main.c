//
//  main.c
//  lap1
//
//  Created by toby on 29.04.24.
//

#include <stdio.h>
#include <OpenCL/opencl.h>
#include <Accelerate/Accelerate.h>

#include "msh.h"
#include "ocl.h"
#include "slv.h"
#include "io.h"


//fem assembly test
int main(int argc, const char * argv[])
{
    printf("hello\n");
    
    //msh
    struct msh_obj msh;
    msh_init(&msh);
    
    //ocl
    struct ocl_obj ocl;
    ocl_init(&msh, &ocl);
    
    //init
    ocl.err = clEnqueueNDRangeKernel(ocl.command_queue, ocl.vtx_init, 3, NULL, msh.nv, NULL, 0, NULL, NULL);
    ocl.err = clEnqueueNDRangeKernel(ocl.command_queue, ocl.vtx_assm, 3, NULL, msh.nv, NULL, 0, NULL, NULL);
    ocl.err = clEnqueueNDRangeKernel(ocl.command_queue, ocl.vtx_bc01, 3, NULL, msh.nv, NULL, 0, NULL, NULL);
    
    //read vec
    ocl.err = clEnqueueReadBuffer(ocl.command_queue,    ocl.xx.dev,     CL_TRUE, 0, msh.nv_tot*sizeof(struct flt3), ocl.xx.hst,     0, NULL, NULL);
    ocl.err = clEnqueueReadBuffer(ocl.command_queue,    ocl.uu.dev,     CL_TRUE, 0, msh.nv_tot*sizeof(float),       ocl.uu.hst,     0, NULL, NULL);
    ocl.err = clEnqueueReadBuffer(ocl.command_queue,    ocl.ff.dev,     CL_TRUE, 0, msh.nv_tot*sizeof(float),       ocl.ff.hst,     0, NULL, NULL);
    ocl.err = clEnqueueReadBuffer(ocl.command_queue,    ocl.aa.dev,     CL_TRUE, 0, msh.nv_tot*sizeof(float),       ocl.aa.hst,     0, NULL, NULL);

    //read mtx
    ocl.err = clEnqueueReadBuffer(ocl.command_queue,    ocl.ii.dev,     CL_TRUE, 0, 27*msh.nv_tot*sizeof(int),      ocl.ii.hst,     0, NULL, NULL);
    ocl.err = clEnqueueReadBuffer(ocl.command_queue,    ocl.jj.dev,     CL_TRUE, 0, 27*msh.nv_tot*sizeof(int),      ocl.jj.hst,     0, NULL, NULL);
    ocl.err = clEnqueueReadBuffer(ocl.command_queue,    ocl.A_vv.dev,   CL_TRUE, 0, 27*msh.nv_tot*sizeof(float),    ocl.A_vv.hst,   0, NULL, NULL);
    ocl.err = clEnqueueReadBuffer(ocl.command_queue,    ocl.M_vv.dev,   CL_TRUE, 0, 27*msh.nv_tot*sizeof(float),    ocl.M_vv.hst,   0, NULL, NULL);
    
    //solve
    slv_mtx(&msh, &ocl);
    
    //write
    wrt_vtk(&msh, &ocl);
    
//    //txt
//    wrt_vec(&msh, &ocl);
//    wrt_mtx(&msh, &ocl);
    
    //raw vec
    wrt_raw(ocl.xx.hst,     msh.nv_tot,     sizeof(struct flt3), "xx");
    wrt_raw(ocl.uu.hst,     msh.nv_tot,     sizeof(float), "uu");
    wrt_raw(ocl.ff.hst,     msh.nv_tot,     sizeof(float), "ff");
    wrt_raw(ocl.aa.hst,     msh.nv_tot,     sizeof(float), "aa");
    
    //raw mtx
    wrt_raw(ocl.ii.hst,     27*msh.nv_tot,  sizeof(int),   "ii");
    wrt_raw(ocl.jj.hst,     27*msh.nv_tot,  sizeof(int),   "jj");
    
    wrt_raw(ocl.A_vv.hst,   27*msh.nv_tot,  sizeof(float), "A_vv");
    wrt_raw(ocl.M_vv.hst,   27*msh.nv_tot,  sizeof(float), "M_vv");
    
    //clean
    ocl_final(&msh, &ocl);
    
    printf("done\n");
    
    return 0;
}

//
//  main.c
//  lap1
//
//  Created by toby on 29.04.24.
//

#include <stdio.h>
#include <OpenCL/opencl.h>

#include "msh.h"
#include "ocl.h"
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
    
    //read vec
    ocl.err = clEnqueueReadBuffer(ocl.command_queue, ocl.uu.dev, CL_TRUE, 0, msh.nv_tot*sizeof(float), ocl.uu.hst,  0, NULL, NULL);
    ocl.err = clEnqueueReadBuffer(ocl.command_queue, ocl.ff.dev, CL_TRUE, 0, msh.nv_tot*sizeof(float), ocl.ff.hst,  0, NULL, NULL);

    //read mtx
    ocl.err = clEnqueueReadBuffer(ocl.command_queue, ocl.A.ii.dev, CL_TRUE, 0, 27*msh.nv_tot*sizeof(int),   ocl.A.ii.hst,  0, NULL, NULL);
    ocl.err = clEnqueueReadBuffer(ocl.command_queue, ocl.A.jj.dev, CL_TRUE, 0, 27*msh.nv_tot*sizeof(int),   ocl.A.jj.hst,  0, NULL, NULL);
    ocl.err = clEnqueueReadBuffer(ocl.command_queue, ocl.A.vv.dev, CL_TRUE, 0, 27*msh.nv_tot*sizeof(float), ocl.A.vv.hst,  0, NULL, NULL);
    
    //txt
    wrt_vec(&msh, &ocl);
    wrt_mtx(&msh, &ocl);
    
    //raw vec
    wrt_raw(ocl.uu.hst, msh.nv_tot, sizeof(float), "uu");
    wrt_raw(ocl.ff.hst, msh.nv_tot, sizeof(float), "ff");
    
    //raw mtx
    wrt_raw(ocl.A.ii.hst, 27*msh.nv_tot, sizeof(int),   "A_ii");
    wrt_raw(ocl.A.jj.hst, 27*msh.nv_tot, sizeof(int),   "A_jj");
    wrt_raw(ocl.A.vv.hst, 27*msh.nv_tot, sizeof(float), "A_vv");
    
    //clean
    ocl_final(&msh, &ocl);
    
    printf("done\n");
    
    return 0;
}

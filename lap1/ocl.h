//
//  ocl.h
//  lap1
//
//  Created by toby on 29.04.24.
//

#ifndef ocl_h
#define ocl_h



struct buf_int
{
    int*            hst;
    cl_mem          dev;
};

struct buf_flt
{
    float*          hst;
    cl_mem          dev;
};

struct buf_flt4
{
    cl_float4*      hst;
    cl_mem          dev;
};



//object
struct ocl_obj
{
    //environment
    cl_int              err;
    cl_platform_id      platform_id;
    cl_device_id        device_id;
    cl_uint             num_devices;
    cl_uint             num_platforms;
    cl_context          context;
    cl_command_queue    command_queue;
    cl_program          program;
    char                device_str[100];
    cl_event            event;  //for profiling
        
    //memory
    struct buf_flt4     xx;
    struct buf_flt      uu;
    struct buf_flt      ff;
    struct buf_flt      aa;
    
    //coo
    struct buf_int      ii;
    struct buf_int      jj;
    
    //mtx
    struct buf_flt      A_vv;
    struct buf_flt      M_vv;
    
    
    //kernels
    cl_kernel vtx_init;
    cl_kernel vtx_assm;
    cl_kernel vtx_bc01;
};


//init
void ocl_init(struct msh_obj *msh, struct ocl_obj *ocl)
{
    /*
     =============================
     environment
     =============================
     */
    
    ocl->err            = clGetPlatformIDs(1, &ocl->platform_id, &ocl->num_platforms);                                              //platform
    ocl->err            = clGetDeviceIDs(ocl->platform_id, CL_DEVICE_TYPE_GPU, 1, &ocl->device_id, &ocl->num_devices);              //devices
    ocl->context        = clCreateContext(NULL, ocl->num_devices, &ocl->device_id, NULL, NULL, &ocl->err);                          //context
    ocl->command_queue  = clCreateCommandQueue(ocl->context, ocl->device_id, CL_QUEUE_PROFILING_ENABLE, &ocl->err);                 //command queue
    ocl->err            = clGetDeviceInfo(ocl->device_id, CL_DEVICE_NAME, sizeof(ocl->device_str), &ocl->device_str, NULL);         //device info
    
    printf("%s\n", ocl->device_str);
    
    /*
     =============================
     program
     =============================
     */
    
    //src
    FILE* src_file = fopen("./prg.cl", "r");
    if(!src_file)
    {
        fprintf(stderr, "prg.cl not found\n");
        exit(1);
    }

    //length
    fseek(src_file, 0, SEEK_END);
    size_t  prg_len =  ftell(src_file);
    rewind(src_file);
//    printf("len %lu\n",prg_len);

    //source
    char *prg_src = (char*)malloc(prg_len);
    fread(prg_src, sizeof(char), prg_len, src_file);
    fclose(src_file);
//    printf("%s\n",prg_src);

    //create
    ocl->program = clCreateProgramWithSource(ocl->context, 1, (const char**)&prg_src, (const size_t*)&prg_len, &ocl->err);
    printf("prg %d\n",ocl->err);
    
    //clean
    free(prg_src);

    //build
    ocl->err = clBuildProgram(ocl->program, 1, &ocl->device_id, NULL, NULL, NULL);
    printf("bld %d\n",ocl->err);
    
    //unload compiler
    ocl->err = clUnloadPlatformCompiler(ocl->platform_id);
    
    /*
     =============================
     log
     =============================
     */

    //log
    size_t log_size = 0;
    
    //log size
    clGetProgramBuildInfo(ocl->program, ocl->device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

    //allocate
    char *log = (char*)malloc(log_size);

    //log text
    clGetProgramBuildInfo(ocl->program, ocl->device_id, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);

    //print
    printf("%s\n", log);

    //clear
    free(log);
    
    /*
     =============================
     kernels
     =============================
     */

    ocl->vtx_init = clCreateKernel(ocl->program, "vtx_init", &ocl->err);
    ocl->vtx_assm = clCreateKernel(ocl->program, "vtx_assm", &ocl->err);
    ocl->vtx_bc01 = clCreateKernel(ocl->program, "vtx_bc01", &ocl->err);

    /*
     =============================
     memory
     =============================
     */
    
    //CL_MEM_READ_WRITE/CL_MEM_HOST_READ_ONLY/CL_MEM_HOST_NO_ACCESS / CL_MEM_ALLOC_HOST_PTR
    
    //vec
    ocl->xx.hst = malloc(msh->nv_tot*sizeof(cl_float4));
    
    ocl->uu.hst = malloc(msh->nv_tot*sizeof(float));
    ocl->ff.hst = malloc(msh->nv_tot*sizeof(float));
    ocl->aa.hst = malloc(msh->nv_tot*sizeof(float));
    
    //coo
    ocl->ii.hst = malloc(27*msh->nv_tot*sizeof(int));
    ocl->jj.hst = malloc(27*msh->nv_tot*sizeof(int));
    
    //mtx
    ocl->A_vv.hst = malloc(27*msh->nv_tot*sizeof(float));
    ocl->M_vv.hst = malloc(27*msh->nv_tot*sizeof(float));
    
    //vec
    ocl->xx.dev = clCreateBuffer(ocl->context, CL_MEM_HOST_READ_ONLY, msh->nv_tot*sizeof(cl_float4), NULL, &ocl->err);
    
    ocl->uu.dev = clCreateBuffer(ocl->context, CL_MEM_HOST_READ_ONLY, msh->nv_tot*sizeof(float), NULL, &ocl->err);
    ocl->ff.dev = clCreateBuffer(ocl->context, CL_MEM_HOST_READ_ONLY, msh->nv_tot*sizeof(float), NULL, &ocl->err);
    ocl->aa.dev = clCreateBuffer(ocl->context, CL_MEM_HOST_READ_ONLY, msh->nv_tot*sizeof(float), NULL, &ocl->err);
    
    //coo
    ocl->ii.dev = clCreateBuffer(ocl->context, CL_MEM_HOST_READ_ONLY, 27*msh->nv_tot*sizeof(int),   NULL, &ocl->err);
    ocl->jj.dev = clCreateBuffer(ocl->context, CL_MEM_HOST_READ_ONLY, 27*msh->nv_tot*sizeof(int),   NULL, &ocl->err);
    
    //mtx
    ocl->A_vv.dev = clCreateBuffer(ocl->context, CL_MEM_HOST_READ_ONLY, 27*msh->nv_tot*sizeof(float), NULL, &ocl->err);
    ocl->M_vv.dev = clCreateBuffer(ocl->context, CL_MEM_HOST_READ_ONLY, 27*msh->nv_tot*sizeof(float), NULL, &ocl->err);

    /*
     =============================
     arguments
     =============================
     */

    ocl->err = clSetKernelArg(ocl->vtx_init,  0, sizeof(cl_float4), (void*)&msh->dx);
    ocl->err = clSetKernelArg(ocl->vtx_init,  1, sizeof(cl_mem),    (void*)&ocl->xx.dev);
    ocl->err = clSetKernelArg(ocl->vtx_init,  2, sizeof(cl_mem),    (void*)&ocl->uu.dev);
    ocl->err = clSetKernelArg(ocl->vtx_init,  3, sizeof(cl_mem),    (void*)&ocl->ff.dev);
    ocl->err = clSetKernelArg(ocl->vtx_init,  4, sizeof(cl_mem),    (void*)&ocl->aa.dev);
    ocl->err = clSetKernelArg(ocl->vtx_init,  5, sizeof(cl_mem),    (void*)&ocl->ii.dev);
    ocl->err = clSetKernelArg(ocl->vtx_init,  6, sizeof(cl_mem),    (void*)&ocl->jj.dev);
    ocl->err = clSetKernelArg(ocl->vtx_init,  7, sizeof(cl_mem),    (void*)&ocl->A_vv.dev);
    ocl->err = clSetKernelArg(ocl->vtx_init,  8, sizeof(cl_mem),    (void*)&ocl->M_vv.dev);
    
    ocl->err = clSetKernelArg(ocl->vtx_assm,  0, sizeof(cl_float4), (void*)&msh->dx);
    ocl->err = clSetKernelArg(ocl->vtx_assm,  1, sizeof(cl_mem),    (void*)&ocl->uu.dev);
    ocl->err = clSetKernelArg(ocl->vtx_assm,  2, sizeof(cl_mem),    (void*)&ocl->ff.dev);
    ocl->err = clSetKernelArg(ocl->vtx_assm,  3, sizeof(cl_mem),    (void*)&ocl->A_vv.dev);
    ocl->err = clSetKernelArg(ocl->vtx_assm,  4, sizeof(cl_mem),    (void*)&ocl->M_vv.dev);
    
    ocl->err = clSetKernelArg(ocl->vtx_bc01,  0, sizeof(cl_mem),    (void*)&ocl->uu.dev);
    ocl->err = clSetKernelArg(ocl->vtx_bc01,  1, sizeof(cl_mem),    (void*)&ocl->ff.dev);
    ocl->err = clSetKernelArg(ocl->vtx_bc01,  2, sizeof(cl_mem),    (void*)&ocl->A_vv.dev);
    ocl->err = clSetKernelArg(ocl->vtx_bc01,  3, sizeof(cl_mem),    (void*)&ocl->M_vv.dev);

}


//final
void ocl_final(struct msh_obj *msh, struct ocl_obj *ocl)
{
    ocl->err = clFlush(ocl->command_queue);
    ocl->err = clFinish(ocl->command_queue);
    
    //kernels
    ocl->err = clReleaseKernel(ocl->vtx_init);
    ocl->err = clReleaseKernel(ocl->vtx_assm);
    ocl->err = clReleaseKernel(ocl->vtx_bc01);

    //device
    ocl->err = clReleaseMemObject(ocl->uu.dev);
    ocl->err = clReleaseMemObject(ocl->ff.dev);
    ocl->err = clReleaseMemObject(ocl->aa.dev);
    
    ocl->err = clReleaseMemObject(ocl->ii.dev);
    ocl->err = clReleaseMemObject(ocl->jj.dev);
    
    ocl->err = clReleaseMemObject(ocl->A_vv.dev);
    ocl->err = clReleaseMemObject(ocl->M_vv.dev);
    
    //host
    free(ocl->uu.hst);
    free(ocl->ff.hst);
    free(ocl->aa.hst);
    
    free(ocl->ii.hst);
    free(ocl->jj.hst);
    free(ocl->A_vv.hst);
    free(ocl->M_vv.hst);
    
    //context
    ocl->err = clReleaseProgram(ocl->program);
    ocl->err = clReleaseCommandQueue(ocl->command_queue);
    ocl->err = clReleaseContext(ocl->context);
    
    return;
}


#endif /* ocl_h */

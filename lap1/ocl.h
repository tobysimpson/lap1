//
//  ocl.h
//  lap1
//
//  Created by toby on 29.04.24.
//

#ifndef ocl_h
#define ocl_h


#define ROOT_PRG    "/Users/toby/Documents/USI/postdoc/fracture/xcode/lap1/lap1"


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

struct buf_coo
{
    struct buf_int  ii;
    struct buf_int  jj;
    struct buf_flt  vv;
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
    struct buf_flt uu;
    struct buf_flt ff;
    struct buf_coo A;
    
    //kernels
    cl_kernel vtx_init;
    cl_kernel vtx_assm;

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
    
//    printf("__FILE__: %s\n", __FILE__);
    
    //path
    char prg_root[200];
    strncpy(prg_root, __FILE__, strlen(__FILE__) - 6);
//    printf("%s\n", prg_root);
    
    //name
    char prg_name[200];
    sprintf(prg_name,"%s/%s", prg_root, "prg.cl");
    printf("%s\n",prg_name);

    //file
    FILE* src_file = fopen(prg_name, "r");
    if(!src_file)
    {
        fprintf(stderr, "program file not found\n");
        exit(1);
    }

    //length
    fseek(src_file, 0, SEEK_END);
    size_t  prg_len =  ftell(src_file);
    rewind(src_file);

//    printf("%lu\n",prg_len);

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

    /*
     =============================
     memory
     =============================
     */
    
    //CL_MEM_READ_WRITE/CL_MEM_HOST_READ_ONLY/CL_MEM_HOST_NO_ACCESS / CL_MEM_ALLOC_HOST_PTR
    
    //vec
    ocl->uu.hst = malloc(msh->nv_tot*sizeof(float));
    ocl->ff.hst = malloc(msh->nv_tot*sizeof(float));
    
    //mtx
    ocl->A.ii.hst = malloc(27*msh->nv_tot*sizeof(int));
    ocl->A.jj.hst = malloc(27*msh->nv_tot*sizeof(int));
    ocl->A.vv.hst = malloc(27*msh->nv_tot*sizeof(float));
    
    //vec
    ocl->uu.dev = clCreateBuffer(ocl->context, CL_MEM_HOST_READ_ONLY, msh->nv_tot*sizeof(float), NULL, &ocl->err);
    ocl->ff.dev = clCreateBuffer(ocl->context, CL_MEM_HOST_READ_ONLY, msh->nv_tot*sizeof(float), NULL, &ocl->err);
    
    //mtx
    ocl->A.ii.dev = clCreateBuffer(ocl->context, CL_MEM_HOST_READ_ONLY, 27*msh->nv_tot*sizeof(int),   NULL, &ocl->err);
    ocl->A.jj.dev = clCreateBuffer(ocl->context, CL_MEM_HOST_READ_ONLY, 27*msh->nv_tot*sizeof(int),   NULL, &ocl->err);
    ocl->A.vv.dev = clCreateBuffer(ocl->context, CL_MEM_HOST_READ_ONLY, 27*msh->nv_tot*sizeof(float), NULL, &ocl->err);

    /*
     =============================
     arguments
     =============================
     */

    ocl->err = clSetKernelArg(ocl->vtx_init,  0, sizeof(cl_float4), (void*)&msh->dx);
    ocl->err = clSetKernelArg(ocl->vtx_init,  1, sizeof(cl_mem),    (void*)&ocl->uu.dev);
    ocl->err = clSetKernelArg(ocl->vtx_init,  2, sizeof(cl_mem),    (void*)&ocl->ff.dev);
    ocl->err = clSetKernelArg(ocl->vtx_init,  3, sizeof(cl_mem),    (void*)&ocl->A.ii.dev);
    ocl->err = clSetKernelArg(ocl->vtx_init,  4, sizeof(cl_mem),    (void*)&ocl->A.jj.dev);
    ocl->err = clSetKernelArg(ocl->vtx_init,  5, sizeof(cl_mem),    (void*)&ocl->A.vv.dev);
    
    ocl->err = clSetKernelArg(ocl->vtx_assm,  0, sizeof(cl_float4), (void*)&msh->dx);
    ocl->err = clSetKernelArg(ocl->vtx_assm,  1, sizeof(cl_mem),    (void*)&ocl->uu.dev);
    ocl->err = clSetKernelArg(ocl->vtx_assm,  2, sizeof(cl_mem),    (void*)&ocl->ff.dev);
    ocl->err = clSetKernelArg(ocl->vtx_assm,  3, sizeof(cl_mem),    (void*)&ocl->A.vv.dev);

}


//final
void ocl_final(struct msh_obj *msh, struct ocl_obj *ocl)
{
    ocl->err = clFlush(ocl->command_queue);
    ocl->err = clFinish(ocl->command_queue);
    
    //kernels
    ocl->err = clReleaseKernel(ocl->vtx_init);

    //device
    ocl->err = clReleaseMemObject(ocl->uu.dev);
    ocl->err = clReleaseMemObject(ocl->ff.dev);
    
    ocl->err = clReleaseMemObject(ocl->A.ii.dev);
    ocl->err = clReleaseMemObject(ocl->A.jj.dev);
    ocl->err = clReleaseMemObject(ocl->A.vv.dev);
    
    //host
    free(ocl->uu.hst);
    free(ocl->ff.hst);
    
    free(ocl->A.ii.hst);
    free(ocl->A.jj.hst);
    free(ocl->A.vv.hst);
    
    //context
    ocl->err = clReleaseProgram(ocl->program);
    ocl->err = clReleaseCommandQueue(ocl->command_queue);
    ocl->err = clReleaseContext(ocl->context);
    
    return;
}


#endif /* ocl_h */
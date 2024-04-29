//
//  io.h
//  lap1
//
//  Created by toby on 29.04.24.
//

#ifndef io_h
#define io_h


#define ROOT_WRITE  "/Users/toby/Downloads/"

//write
void wrt_raw(void *ptr, size_t n, size_t bytes, char *file_name)
{
//    printf("%s\n",file_name);
    
    //name
    char file_path[250];
    sprintf(file_path, "%s%s.raw", ROOT_WRITE, file_name);

    //open
    FILE* file = fopen(file_path,"wb");
  
    //write
    fwrite(ptr, bytes, n, file);
    
    //close
    fclose(file);
    
    return;
}


//write
void wrt_vec(struct msh_obj *msh, struct ocl_obj *ocl)
{
    //name
    char file_path[250];
    sprintf(file_path, "%s%s.txt", ROOT_WRITE, "vec");

    //open
    FILE* fp = fopen(file_path,"w");
  
    for(int i=0; i<msh->nv_tot; i++)
    {
        fprintf(fp,"%e %e\n", ocl->uu.hst[i], ocl->ff.hst[i]);
    }
    
    //close
    fclose(fp);
    
    return;
}

//write
void wrt_mtx(struct msh_obj *msh, struct ocl_obj *ocl)
{
    //name
    char file_path[250];
    sprintf(file_path, "%s%s.txt", ROOT_WRITE, "mtx");

    //open
    FILE* fp = fopen(file_path,"w");
  
    for(int i=0; i<27*msh->nv_tot; i++)
    {
        fprintf(fp,"%3d %3d %e\n", ocl->A.ii.hst[i], ocl->A.jj.hst[i], ocl->A.vv.hst[i]);
    }
    
    //close
    fclose(fp);
    
    return;
}


#endif /* io_h */

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
        fprintf(fp,"% e % e % e\n", ocl->uu.hst[i], ocl->ff.hst[i], ocl->aa.hst[i]);
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
        fprintf(fp,"%3d %3d %+e %+e\n", ocl->ii.hst[i], ocl->jj.hst[i], ocl->A_vv.hst[i], ocl->M_vv.hst[i]);
    }
    
    //close
    fclose(fp);
    
    return;
}


//write
void wrt_vtk(struct msh_obj *msh, struct ocl_obj *ocl)
{
    FILE* file1;
    char file1_name[250];
    
    //file name
    sprintf(file1_name, "%s%s.%03d.vtk", ROOT_WRITE, "grid1", 0);
    
    //open
    file1 = fopen(file1_name,"w");
    
    //write
    fprintf(file1,"# vtk DataFile Version 3.0\n");
    fprintf(file1,"grid1\n");
    fprintf(file1,"ASCII\n");
    fprintf(file1,"DATASET STRUCTURED_GRID\n");
    fprintf(file1,"DIMENSIONS %zu %zu %zu\n", msh->nv[0], msh->nv[1], msh->nv[2]);
    
    /*
     ===================
     coords
     ===================
     */
    
    fprintf(file1,"\nPOINTS %d float\n", msh->nv_tot);

    for(int i=0; i<msh->nv_tot; i++)
    {
        fprintf(file1, "%e %e %e\n", ocl->xx.hst[i].x, ocl->xx.hst[i].y, ocl->xx.hst[i].z);
    }

    //point data flag
    fprintf(file1,"\nPOINT_DATA %d\n", msh->nv_tot);


//    fprintf(file1,"VECTORS Uu float\n");
//
//    for(int i=0; i<msh->nv_tot; i++)
//    {
//        fprintf(file1, "%e %e %e\n", ocl->hst.U[i].x, ocl->hst.U[i].y, ocl->hst.U[i].z);
//    }
    


    //uu
    fprintf(file1,"SCALARS uu float 1\n");
    fprintf(file1,"LOOKUP_TABLE default\n");
    
    for(int i=0; i<msh->nv_tot; i++)
    {
        fprintf(file1, "%e\n", ocl->uu.hst[i]);
    }
    
    //ff
    fprintf(file1,"SCALARS ff float 1\n");
    fprintf(file1,"LOOKUP_TABLE default\n");
    
    for(int i=0; i<msh->nv_tot; i++)
    {
        fprintf(file1, "%e\n", ocl->ff.hst[i]);
    }

    //aa
    fprintf(file1,"SCALARS aa float 1\n");
    fprintf(file1,"LOOKUP_TABLE default\n");
    
    for(int i=0; i<msh->nv_tot; i++)
    {
        fprintf(file1, "%e\n", ocl->aa.hst[i]);
    }
    
    //ee
    fprintf(file1,"SCALARS ee float 1\n");
    fprintf(file1,"LOOKUP_TABLE default\n");
    
    for(int i=0; i<msh->nv_tot; i++)
    {
        fprintf(file1, "%e\n", ocl->ff.hst[i] - ocl->aa.hst[i]);
    }

    
    //clean up
    fclose(file1);

    return;
}



#endif /* io_h */

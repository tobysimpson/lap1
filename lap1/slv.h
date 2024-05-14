//
//  slv.h
//  lap1
//
//  Created by toby on 08.05.24.
//

#ifndef slv_h
#define slv_h


void fn_print_csr(SparseMatrix_Float A)
{
    float aa[A.structure.rowCount*A.structure.columnCount];
    
    //reset
    for(int i=0; i<A.structure.rowCount*A.structure.columnCount; i++)
    {
        aa[i] = 0;
    }
    
    //read
    for(int col_idx=0; col_idx<A.structure.columnCount; col_idx++)
    {
        for(long row_ptr=A.structure.columnStarts[col_idx]; row_ptr<A.structure.columnStarts[col_idx+1]; row_ptr++)
        {
            int row_idx = A.structure.rowIndices[row_ptr];
            float   val = A.data[row_ptr];
            
            aa[row_idx*A.structure.columnCount+col_idx] = val;
        }
    }
    
    //print
    for(int j=0; j<A.structure.columnCount; j++)
    {
        for(int i=0; i<A.structure.rowCount; i++)
        {
            float a = aa[j*A.structure.columnCount+i];
            
            if(a!=0e0f)
            {
                printf("%s",(a>0e0f)?"+":"-");
//                printf("% 3.2f ",a);
            }
            else
            {
                printf(" ");
//                printf("      ");
            }
        }
        printf("\n");
    }
    printf("\n");
}



//solve
int slv_mtx(struct msh_obj *msh, struct ocl_obj *ocl)
{
    //store trianglew convert to symmetric afterwards to avoid sums
    SparseAttributes_t atts;
//    atts.kind       = SparseOrdinary;
    
    atts.kind       = SparseTriangular;
    atts.triangle   = SparseUpperTriangle;
    
    //size of input array
    long    blk_num     = 27*msh->nv_tot;
    int     num_rows    = msh->nv_tot;
    int     num_cols    = msh->nv_tot;
    uint8_t blk_sz      = 1;
    
    //create
    SparseMatrix_Float A = SparseConvertFromCoordinate(num_rows, num_cols, blk_num, blk_sz, atts, ocl->ii.hst, ocl->jj.hst, ocl->A_vv.hst);  //duplicates sum
    
    //this is key for CG
    A.structure.attributes.kind = SparseSymmetric;
    
//    fn_print_csr(A);
    
    //vecs
    DenseVector_Float u = {msh->nv_tot, ocl->uu.hst};
    DenseVector_Float f = {msh->nv_tot, ocl->ff.hst};
//    DenseVector_Float a = {msh->nv_tot, ocl->aa.hst};

    //solve
    SparseSolve(SparseConjugateGradient(), A, f, u);
//    SparseSolve(SparseGMRES(), A, f, u);
    
//    SparseMultiply(A, a, u);

    //clean
    SparseCleanup(A);

//    //disp
//    for(int i=0; i<msh->nv_tot; i++)
//    {
//        printf("%e %e %e %e\n", f.data[i], u.data[i], ocl->aa.hst[i], (ocl->uu.hst[i] - ocl->aa.hst[i]));
//    }

    return 0;
}


#endif /* slv_h */

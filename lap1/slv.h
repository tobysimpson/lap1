//
//  slv.h
//  lap1
//
//  Created by toby on 08.05.24.
//

#ifndef slv_h
#define slv_h



//solve
int slv_mtx(struct msh_obj *msh, struct ocl_obj *ocl)
{
    //unsymmetric
    SparseAttributes_t atts;
    atts.kind = SparseOrdinary;
    atts.triangle = false;
    atts.transpose  = false;
    
    //size of input array
    long    blk_num     = 27*msh->nv_tot;
    int     num_rows    = msh->nv_tot;
    int     num_cols    = msh->nv_tot;
    uint8_t blk_sz      = 1;
    
    
    //create
    SparseMatrix_Float A = SparseConvertFromCoordinate(num_rows, num_cols, blk_num, blk_sz, atts, ocl->A.ii.hst, ocl->A.jj.hst, ocl->A.vv.hst);  //duplicates sum
    
    //vecs
    DenseVector_Float u;
    DenseVector_Float f;
    
    u.count = msh->nv_tot;
    f.count = msh->nv_tot;
    
    u.data = ocl->uu.hst;
    f.data = ocl->ff.hst;

    /*
     ========================
     solve
     ========================
     */
    
    //GMRES
    SparseGMRESOptions options;
    options.maxIterations =  4*msh->nv_tot;
    options.nvec = 100;
    options.atol = 1e-3f;
    options.rtol = 1e-3f;
    options.variant = SparseVariantGMRES;
    SparseSolve(SparseGMRES(options), A, f, u);
    
//    //CG
//    SparseCGOptions options;
//    options.maxIterations = msh->nv_tot;
//    options.atol = 1e-3f;
//    options.rtol = 1e-3f;
//    SparseSolve(SparseConjugateGradient(options), A, f, u);

//    //LSMR
//    SparseSolve(SparseLSMR(), A, f, u); //minres - symmetric
    
//    //QR
//    SparseOpaqueFactorization_Float QR = SparseFactor(SparseFactorizationQR, A);       //no
//    SparseSolve(QR, f , u);
//    SparseCleanup(QR);
    
    //clean
    SparseCleanup(A);
    
//    //disp
//    for(int i=0; i<msh->nv_tot; i++)
//    {
//        printf("%3d %+e %+e\n", i, u.data[i], f.data[i]);
//    }
//    printf("\n");
    

    return 0;
}


#endif /* slv_h */

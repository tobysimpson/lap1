//
//  slv.h
//  lap1
//
//  Created by toby on 08.05.24.
//

#ifndef slv_h
#define slv_h


void dsp_vec(DenseVector_Float v)
{
    for(int i=0; i<v.count; i++)
    {
        printf("%+e ", v.data[i]);
    }
    printf("\n\n");
    
    return;
}


//solve
int slv_mtx(struct msh_obj *msh, struct ocl_obj *ocl)
{
    //unsymmetric
    SparseAttributes_t atts;
    atts.kind = SparseOrdinary;
    atts.transpose  = false;
    
//    //symmetric (ignores upper) - no
//    SparseAttributes_t atts;
//    atts.kind = SparseSymmetric;
//    atts.triangle = SparseLowerTriangle;
    
    //size of input array
    long blk_num = 27*16*msh->nv_tot;
    int num_rows = 4*msh->nv_tot;
    int num_cols = 4*msh->nv_tot;
    uint8_t blk_sz = 1;

    //create
    SparseMatrix_Float A = SparseConvertFromCoordinate(num_rows, num_cols, blk_num, blk_sz, atts, ocl->A.ii.hst, ocl->A.jj.hst, ocl->A.vv.hst);  //duplicates sum
    
    //vecs
    DenseVector_Float u;
    DenseVector_Float f;
    
    u.count = 4*msh->nv_tot;
    f.count = 4*msh->nv_tot;
    
    u.data = (float*)ocl->uu.hst;
    f.data = (float*)ocl->ff.hst;

    /*
     ========================
     solve
     ========================
     */
    
//    //GMRES
//    SparseGMRESOptions options;
//    options.maxIterations =  4*msh->nv_tot;
//    options.nvec = 100;
//    options.atol = 1e-3f;
//    options.rtol = 1e-3f;
//    options.variant = SparseVariantGMRES;
//    SparseSolve(SparseGMRES(options), A, f, u);
    
    //CG
    SparseCGOptions options;
    options.maxIterations = 4*msh->nv_tot;
    options.atol = 1e-3f;
    options.rtol = 1e-3f;
    SparseSolve(SparseConjugateGradient(options), A, f, u);

//    //LSMR
//    SparseSolve(SparseLSMR(), A, f, u); //minres - symmetric
    
//    //QR
//    SparseOpaqueFactorization_Float QR = SparseFactor(SparseFactorizationQR, A);       //no
//    SparseSolve(QR, f , u);
//    SparseCleanup(QR);
    
    //clean
    SparseCleanup(A);

    return 0;
}


#endif /* slv_h */

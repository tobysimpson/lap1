//
//  msh.h
//  lap1
//
//  Created by toby on 29.04.24.
//

#ifndef msh_h
#define msh_h


//object
struct msh_obj
{
    size_t      ne[3];
    size_t      nv[3];
    
    size_t      ne_tot;
    size_t      nv_tot;
    
    cl_float3   dx;
    cl_float8   prm;
};


//init
void msh_init(struct msh_obj *msh)
{
    //ele
    msh->ne[0] = 2;
    msh->ne[1] = msh->ne[0];
    msh->ne[2] = msh->ne[0];
    
    //vtx
    msh->nv[0] = msh->ne[0] + 1;
    msh->nv[1] = msh->ne[1] + 1;
    msh->nv[2] = msh->ne[2] + 1;

    //tot
    msh->ne_tot = msh->ne[0]*msh->ne[1]*msh->ne[2];
    msh->nv_tot = msh->nv[0]*msh->nv[1]*msh->nv[2];
    
    printf("ne %zu %zu %zu\n", msh->ne[0], msh->ne[1], msh->ne[2]);
    printf("nv %zu %zu %zu\n", msh->nv[0], msh->nv[1], msh->nv[2]);
    
    printf("ne_tot %zu\n", msh->ne_tot);
    printf("nv_tot %zu\n", msh->nv_tot);

    //dx
    msh->dx.x = 1e0/(float)msh->ne[0];
    msh->dx.y = 1e0/(float)msh->ne[1];
    msh->dx.z = 1e0/(float)msh->ne[2];

    printf("dx %+f %+f %+f\n", msh->dx.x, msh->dx.y, msh->dx.z);
    
    //youngs, poisson
    float mat_E = 1e+1f;    //Youngs    //kPa = 1 mg mm^-1 ms^-2
    float mat_v = 0.2f;     //Poisson
    
    //material params
    msh->prm.s0 = (mat_E*mat_v)/((1e0f+mat_v)*(1e0f-2e0f*mat_v));   //lamé      lambda
    msh->prm.s1 = mat_E/(2e0f*(1e0f+mat_v));                        //lamé      mu
    msh->prm.s2 = 1e+0f;                                            //density   rho     mg/mm^3
    msh->prm.s3 = 1e-2f;                                            //gravity   g       mm/ms
    msh->prm.s4 = 0e+0f;
    msh->prm.s5 = 0e+0f;
    msh->prm.s6 = 0e+0f;
    msh->prm.s7 = 0e+0f;

    printf("s0 %f\n", msh->prm.s0);
    printf("s1 %f\n", msh->prm.s1);
    printf("s2 %f\n", msh->prm.s2);
    printf("s3 %f\n", msh->prm.s3);
    printf("s4 %f\n", msh->prm.s4);
    printf("s5 %f\n", msh->prm.s5);
    printf("s6 %f\n", msh->prm.s6);
    printf("s7 %f\n", msh->prm.s7);
    
    return;
}




#endif /* msh_h */

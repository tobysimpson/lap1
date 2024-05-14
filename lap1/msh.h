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
    size_t  ne[3];      //ele
    size_t  nv[3];      //vtx
    
    int     ne_tot;
    int     nv_tot;
    
    float   dx;
};


//init
void msh_init(struct msh_obj *msh)
{
    //msh
    int     ne = 10;
    int     nv = ne+1;
    float   dx = 1e0f/(float)ne;
    
    msh->ne[0] = ne;
    msh->ne[1] = ne;
    msh->ne[2] = ne;
    
    msh->nv[0] = nv;
    msh->nv[1] = nv;
    msh->nv[2] = nv;
    
    msh->ne_tot = (int)(msh->ne[0]*msh->ne[1]*msh->ne[2]);
    msh->nv_tot = (int)(msh->nv[0]*msh->nv[1]*msh->nv[2]);
    
    msh->dx = dx;

    printf("%zu %zu %zu\n",msh->ne[0], msh->ne[1], msh->ne[2]);
    printf("%zu %zu %zu\n",msh->nv[0], msh->nv[1], msh->nv[2]);
    printf("%d\n",msh->ne_tot);
    printf("%d\n",msh->nv_tot);
    printf("%f\n",msh->dx);
}


#endif /* msh_h */

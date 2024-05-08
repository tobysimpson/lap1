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


#endif /* msh_h */

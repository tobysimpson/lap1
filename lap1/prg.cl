//
//  prg.cl
//  lap1
//
//  Created by toby on 29.04.24.
//


/*
 ===================================
 proto
 ===================================
 */

int     fn_idx1(int3 pos, int3 dim);
int     fn_idx3(int3 pos);

int     fn_bnd1(int3 pos, int3 dim);
int     fn_bnd2(int3 pos, int3 dim);

/*
 ===================================
 util
 ===================================
 */

//flat index
int fn_idx1(int3 pos, int3 dim)
{
    return pos.x + dim.x*(pos.y + dim.y*pos.z);
}

//index 3x3x3
int fn_idx3(int3 pos)
{
    return pos.x + 3*pos.y + 9*pos.z;
}

//in-bounds
int fn_bnd1(int3 pos, int3 dim)
{
    return all(pos>=0)*all(pos<dim);
}

//on the boundary
int fn_bnd2(int3 pos, int3 dim)
{
    return (pos.x==0)||(pos.y==0)||(pos.z==0)||(pos.x==dim.x-1)||(pos.y==dim.y-1)||(pos.z==dim.z-1);
}

/*
 ===================================
 kernels
 ===================================
 */

//init
kernel void vtx_init(const  float3  dx,
                     global float   *uu,
                     global float   *ff,
                     global int     *A_ii,
                     global int     *A_jj,
                     global float   *A_vv)
{
    int3 vtx_dim    = {get_global_size(0), get_global_size(1), get_global_size(2)};
    int3 vtx1_pos1  = {get_global_id(0)  , get_global_id(1),   get_global_id(2)};
    int  vtx1_idx1  = fn_idx1(vtx1_pos1, vtx_dim);
    
    printf("%3d %v3d\n", vtx1_idx1, vtx1_pos1);
    
    //vec
    uu[vtx1_idx1] = fn_bnd2(vtx1_pos1, vtx_dim);
    ff[vtx1_idx1] = vtx1_idx1;
    
    //vtx2
    for(int vtx2_k=0; vtx2_k<3; vtx2_k++)
    {
        for(int vtx2_j=0; vtx2_j<3; vtx2_j++)
        {
            for(int vtx2_i=0; vtx2_i<3; vtx2_i++)
            {
                int3 vtx2_pos3 = (int3){vtx2_i,vtx2_j,vtx2_k};
                int3 vtx2_pos1 = vtx1_pos1 + vtx2_pos3 - 1;
                int  vtx2_idx1 = fn_idx1(vtx2_pos1, vtx_dim);
                int  vtx2_bnd1 = fn_bnd1(vtx2_pos1, vtx_dim);
                int  vtx2_idx3 = fn_idx3(vtx2_pos3);
                
                //block
                int blk = 27*vtx1_idx1 + vtx2_idx3;
                
                A_ii[blk] = vtx2_bnd1*vtx1_idx1;
                A_jj[blk] = vtx2_bnd1*vtx2_idx1;
                A_vv[blk] = (vtx1_idx1==vtx2_idx1); //I
            }
        }
    }
    
    


//    //vtx2
//    for(int vtx2_idx3=0; vtx2_idx3<27; vtx2_idx3++)
//    {
//        int3 vtx2_pos1 = vtx1_pos1 + off3[vtx2_idx3] - 1;
//        int  vtx2_idx1 = fn_idx1(vtx2_pos1, vtx_dim);
//        int  vtx2_bnd1 = fn_bnd1(vtx2_pos1, vtx_dim);
//        
//        //blocks (nv*27)
//        int idx1 = 27*vtx1_idx1 + vtx2_idx3;
//        
//        //coo
//        A_ii[idx1]      = vtx2_bnd1*vtx1_idx1;
//        A_jj[idx1]      = vtx2_bnd1*vtx2_idx1;
//        A_vv[idx1].vec  = 0e0f;
//        
//        //Au=I
////        A_vv[idx1].vec.s05a = vtx2_bnd1*(vtx1_idx1==vtx2_idx1); //diag=05af
//        
//    } //vtx2
    
    return;
}


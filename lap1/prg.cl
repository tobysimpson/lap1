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

float   fn_f1(float4 p);
float   fn_u1(float4 p);

int3    fn_pos2(int i);
int3    fn_pos3(int i);

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


//pos 2x2x2
int3 fn_pos2(int i)
{
    return (int3){(i>>0)&1, (i>>1)&1, (i>>2)&1};
}

//pos 3x3x3
int3 fn_pos3(int i)
{
    return (int3){i%3, i/3, i/9};
}

/*
 ===================================
 analytic
 ===================================
 */

//rhs
float fn_f1(float4 p)
{
    return 2e0f*(p.x*p.y*(p.x - 1e0f)*(p.y - 1e0f) + p.x*p.z*(p.x - 1e0f)*(p.z - 1e0f) + p.y*p.z*(p.y - 1e0f)*(p.z - 1e0f));
}

//soln
float fn_u1(float4 p)
{
    return p.x*(1e0f - p.x)*p.y*(1e0f - p.y)*p.z*(1e0f - p.z);
}

/*
 ===================================
 kernels
 ===================================
 */

//init
kernel void vtx_init(const  float3  dx,
                     global write_only float   *uu,
                     global write_only float   *ff,
                     global write_only int     *A_ii,
                     global write_only int     *A_jj,
                     global write_only float   *A_vv)
{
    int3 vtx_dim    = {get_global_size(0), get_global_size(1), get_global_size(2)};
    int3 vtx1_pos1  = {get_global_id(0)  , get_global_id(1),   get_global_id(2)};
    int  vtx1_idx1  = fn_idx1(vtx1_pos1, vtx_dim);
    
//    printf("%3d %v3d\n", vtx1_idx1, vtx1_pos1);
    
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
    
    return;
}


//assemble
kernel void vtx_assm(const  float3  dx,
                     global read_only   float   *uu,
                     global write_only  float   *ff,
                     global write_only  float   *A_vv)
{
    int3 vtx_dim    = {get_global_size(0), get_global_size(1), get_global_size(2)};
    int3 vtx1_pos1  = {get_global_id(0)  , get_global_id(1),   get_global_id(2)};
    int  vtx1_idx1  = fn_idx1(vtx1_pos1, vtx_dim);
    
//    printf("vtx %2d %v3d\n", vtx1_idx1, vtx1_pos1);
    
    if(all(vtx1_pos1==1))
    {
        //        //ele
        //        for(int i=0; i<8; i++)
        //        {
        //            int3 ele_pos2 = fn_pos2(i);
        //
        //            //vtx
        //            for(int j=0; j<8; j++)
        //            {
        //                int3 vtx_pos2 = fn_pos2(j);
        //
        //                int3 vtx_pos3 = ele_pos2 + vtx_pos2;
        ////                printf("ij %d %d %v3d %v3d %v3d\n", i, j, ele_pos2, vtx_pos2, vtx_pos3);
        //            }
        
        for(int i=0; i<27; i++)
        {
            int3 pos3 = fn_pos3(i);
            
            printf("i %2d %v3d\n", i, pos3);
        }
        
    }//if
    return;
}

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

float   fn_f1(float3 p);
float   fn_u1(float3 p);

int     fn_idx1(int3 pos, int3 dim);
int     fn_idx3(int3 pos);

int     fn_bnd1(int3 pos, int3 dim);
int     fn_bnd2(int3 pos, int3 dim);

int3    fn_pos2(int i);

void    bas_eval(float3 p, float ee[8]);
void    bas_grad(float3 p, float3 gg[8], float dx);
float4  bas_itpe(float4 uu2[8], float bas_ee[8]);
float16 bas_itpg(float4 uu2[8], float3 bas_gg[8]);
float16 bas_tens(int dim, float3 g);

/*
 ===================================
 ana
 ===================================
 */

//rhs
float fn_f1(float3 p)
{
    return 2e0f*(p.x*p.y*(p.x - 1e0f)*(p.y - 1e0f) + p.x*p.z*(p.x - 1e0f)*(p.z - 1e0f) + p.y*p.z*(p.y - 1e0f)*(p.z - 1e0f));
}

//soln
float fn_u1(float3 p)
{
    return p.x*(1e0f - p.x)*p.y*(1e0f - p.y)*p.z*(1e0f - p.z);
}

/*
 ===================================
 util
 ===================================
 */

//global index
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

//pos2 2x2x2
int3 fn_pos2(int i)
{
    return (int3){(i>>0)&1, (i>>1)&1, (i>>2)&1};
}

/*
 ===================================
 quadrature [0,1]
 ===================================
 */

//1-point gauss [0,1]
constant float qp1 = 5e-1f;
constant float qw1 = 1e+0f;

//2-point gauss [0,1]
constant float qp2[2] = {0.211324865405187f,0.788675134594813f};
constant float qw2[2] = {5e-1f,5e-1f};

//3-point gauss [0,1]
constant float qp3[3] = {0.112701665379258f,0.500000000000000f,0.887298334620742f};
constant float qw3[3] = {0.277777777777778f,0.444444444444444f,0.277777777777778f};

/*
 ===================================
 basis
 ===================================
 */

//eval at qp
void bas_eval(float3 p, float ee[8])
{
    float x0 = 1e0f - p.x;
    float y0 = 1e0f - p.y;
    float z0 = 1e0f - p.z;
    
    float x1 = p.x;
    float y1 = p.y;
    float z1 = p.z;
    
    ee[0] = x0*y0*z0;
    ee[1] = x1*y0*z0;
    ee[2] = x0*y1*z0;
    ee[3] = x1*y1*z0;
    ee[4] = x0*y0*z1;
    ee[5] = x1*y0*z1;
    ee[6] = x0*y1*z1;
    ee[7] = x1*y1*z1;
    
    return;
}

//grad at qp
void bas_grad(float3 p, float3 gg[8], float dx)
{
    float x0 = 1e0f - p.x;
    float y0 = 1e0f - p.y;
    float z0 = 1e0f - p.z;
    
    float x1 = p.x;
    float y1 = p.y;
    float z1 = p.z;
    
    //{/dx,/dy,/dz}
    gg[0] = (float3){-y0*z0, -x0*z0, -x0*y0}/dx;
    gg[1] = (float3){+y0*z0, -x1*z0, -x1*y0}/dx;
    gg[2] = (float3){-y1*z0, +x0*z0, -x0*y1}/dx;
    gg[3] = (float3){+y1*z0, +x1*z0, -x1*y1}/dx;
    gg[4] = (float3){-y0*z1, -x0*z1, +x0*y0}/dx;
    gg[5] = (float3){+y0*z1, -x1*z1, +x1*y0}/dx;
    gg[6] = (float3){-y1*z1, +x0*z1, +x0*y1}/dx;
    gg[7] = (float3){+y1*z1, +x1*z1, +x1*y1}/dx;
    
    return;
}


/*
 ===================================
 init
 ===================================
 */

//init
kernel void vtx_init(const                  float    dx,
                     global     write_only  float   *uu,
                     global     write_only  float   *ff,
                     global     write_only  int     *A_ii,
                     global     write_only  int     *A_jj,
                     global     write_only  float   *A_vv)
{
    int3 vtx_dim    = {get_global_size(0), get_global_size(1), get_global_size(2)};
    int3 vtx1_pos1  = {get_global_id(0)  , get_global_id(1),   get_global_id(2)};
    int  vtx1_idx1  = fn_idx1(vtx1_pos1, vtx_dim);
    
    //    printf("%3d %v3d\n", vtx1_idx1, vtx1_pos1);
    
    //vec
    uu[vtx1_idx1] = 0e0f; //fn_bnd2(vtx1_pos1, vtx_dim);
    ff[vtx1_idx1] = 0e0f; //vtx1_idx1;
    
    
    //adj
    for(int k=0; k<3; k++)
    {
        for(int j=0; j<3; j++)
        {
            for(int i=0; i<3; i++)
            {
                int3 vtx2_pos3 = (int3){i,j,k};
                int3 vtx2_pos1 = vtx1_pos1 + vtx2_pos3 - 1;
                int  vtx2_idx1 = fn_idx1(vtx2_pos1, vtx_dim);
                int  vtx2_bnd1 = fn_bnd1(vtx2_pos1, vtx_dim);
                int  vtx2_idx3 = fn_idx3(vtx2_pos3);
                
                //block
                int blk = 27*vtx1_idx1 + vtx2_idx3;
                
                A_ii[blk] = vtx2_bnd1*vtx1_idx1;
                A_jj[blk] = vtx2_bnd1*vtx2_idx1;
                A_vv[blk] = 0e0f; //(vtx1_idx1==vtx2_idx1);
            }
        }
    }
    
    return;
}

/*
 ===================================
 assemble
 ===================================
 */

//assemble
kernel void vtx_assm(const                  float    dx,
                     global     read_only   float   *uu,
                     global     write_only  float   *ff,
                     global     write_only  float   *A_vv)
{
    int3 vtx_dim    = {get_global_size(0), get_global_size(1), get_global_size(2)};
    int3 vtx1_pos1  = {get_global_id(0)  , get_global_id(1),   get_global_id(2)};
    int3 ele_dim    = vtx_dim - 1;
    
    int vtx1_idx1   = fn_idx1(vtx1_pos1, vtx_dim);
    int vtx1_idx2   = 8;
    
    float vlm = dx*dx*dx;
    
    //ele1
    for(uint ele_idx2=0; ele_idx2<8; ele_idx2++)
    {
        int3 ele_pos2 = fn_pos2(ele_idx2);
        int3 ele_pos1 = vtx1_pos1 + ele_pos2 - 1;
        int  ele_bnd1 = fn_bnd1(ele_pos1, ele_dim);
        
        //decrement
        vtx1_idx2 -= 1;
        
        //in-bounds
        if(ele_bnd1)
        {
            //qpt (2pt gauss)
            for(int qpt_idx=0; qpt_idx<8; qpt_idx++)
            {
                int3 qpt_pos = fn_pos2(qpt_idx);
                
                //2pt
                float3 qp = (float3){qp2[qpt_pos.x], qp2[qpt_pos.y], qp2[qpt_pos.z]};
                float  qw = qw2[qpt_pos.x]*qw2[qpt_pos.y]*qw2[qpt_pos.z];
                
                //scale
                qw *= vlm;
                
                //basis
                float  bas_ee[8];
                float3 bas_gg[8];
                bas_eval(qp, bas_ee);
                bas_grad(qp, bas_gg, dx);
                
                //qpt global
                float3 x = dx*convert_float3(vtx1_pos1 + qp);
                
                //rhs
                ff[vtx1_idx1] += fn_f1(x)*bas_ee[vtx1_idx2]*qw;
                
                //vtx2
                for(int vtx2_idx2=0; vtx2_idx2<8; vtx2_idx2++)
                {
                    //idx
                    int3 vtx2_pos3 = ele_pos2 + fn_pos2(vtx2_idx2);
                    int  vtx2_idx3 = fn_idx3(vtx2_pos3);
                    
                    //block idx
                    int idx1 = 27*vtx1_idx1 + vtx2_idx3;
                    
                    //scalar poisson
                    A_vv[idx1] += dot(bas_gg[vtx2_idx2], bas_gg[vtx1_idx2])*qw;
                    
                    //                        //dim1
                    //                        for(int dim1=0; dim1<3; dim1++)
                    //                        {
                    //                            //tensor basis
                    //                            float16 du1 = bas_tens(dim1, bas_gg[vtx1_idx2]);
                    //
                    //                            //strain
                    //                            float8 E1 = mec_E(du1);
                    //
                    //                            //dim2
                    //                            for(int dim2=0; dim2<3; dim2++)
                    //                            {
                    //                                //tensor basis
                    //                                float16 du2 = bas_tens(dim2, bas_gg[vtx2_idx2]);
                    //
                    //                                //strain
                    //                                float8 E2 = mec_E(du2);
                    //
                    //                                //stress
                    //                                float8 S2 = mec_S(E2, mat);
                    //
                    //                                //write
                    //                                A_vv[idx1].arr[dim1][dim2] += sym_tip(S2, E1)*qw;
                    //
                    //                            } //dim2
                    //
                    //                        } //dim1
                    
                } //vtx2
                
            } //qpt
            
        } //ele_bnd1
        
    } //ele
    
    return;
}

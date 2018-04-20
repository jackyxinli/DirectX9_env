/**********************************************************/
/*★ MyD3D Object Class   2007/10/15  Ver 1.3   前田 稔 ★*/
/**********************************************************/
#include "MyD3D.h"

GetPicture      F_GetPicture;

//★ Constructor
MyD3D::MyD3D(HWND hwnd)
{   hWnd= hwnd;
    pD3D= NULL;
    pDEV= NULL;
    pBack = NULL;
    pSurface = NULL;
    pTexture = NULL;
    hBmp= NULL;
    hBmpDC= NULL;
    SWidth=SHeight= 0;
    XNum=YNum= 1;
    szFile[0]= '\0';
}

//★ Destructor
MyD3D::~MyD3D()
{
    SAFE_DELDC(hBmpDC);
    SAFE_DELOBJ(hBmp);
    SWidth=SHeight= 0;
    SAFE_RELEASE(pDEV);
    SAFE_RELEASE(pD3D);
}

//★ Debug Message Box
void  MyD3D::DMsg(char *chr, int n)
{   char    str[80];
    wsprintf(str,"%s=%d[%x]\n",chr,n,n);
    MessageBox(NULL,str,"Debug Message Box",MB_OK);
}
void  MyD3D::DMsg(char *chr, float f)
{   char    str[80];
    wsprintf(str,"%s=%8.2f\n",chr,f);
    MessageBox(NULL,str,"Debug Message Box",MB_OK);
}

// Windows Mode の D3DPPWin の設定
HRESULT  MyD3D::SetD3DPPWin()
{   D3DDISPLAYMODE          dmode;

    // デバイス/モード等の初期化
    if (pD3D==NULL)   pD3D = Direct3DCreate9(D3D_SDK_VERSION);
    if (pD3D==NULL)   return E_FAIL;
    //現在のディスプレイモードを得る
    if (FAILED(pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&dmode))) return E_FAIL;

    // D3DDeviceオブジェクトの作成
    ZeroMemory(&D3DPPWin, sizeof(D3DPPWin));
    D3DPPWin.BackBufferFormat           = dmode.Format;
    D3DPPWin.BackBufferCount            = 1;
    D3DPPWin.SwapEffect                 = D3DSWAPEFFECT_DISCARD;
    D3DPPWin.Windowed                   = TRUE;
    D3DPPWin.EnableAutoDepthStencil     = TRUE;
    D3DPPWin.AutoDepthStencilFormat     = D3DFMT_D16;
    D3DPPWin.Flags                      = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
    return S_OK;
}

// Full Screen  の D3DPPFull の設定
HRESULT  MyD3D::SetD3DPPFull(UINT Width, UINT Height)
{   D3DDISPLAYMODE          dmode;

    // デバイス/モード等の初期化
    if (pD3D==NULL)   pD3D = Direct3DCreate9(D3D_SDK_VERSION);
    if (pD3D==NULL)   return E_FAIL;
    //現在のディスプレイモードを得る
    if (FAILED(pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&dmode))) return E_FAIL;

    // D3DDeviceオブジェクトの作成
    ZeroMemory(&D3DPPFull, sizeof(D3DPPFull));
    D3DPPFull.BackBufferFormat          = dmode.Format;
    D3DPPFull.BackBufferCount           = 1;
    D3DPPFull.SwapEffect                = D3DSWAPEFFECT_DISCARD;
    D3DPPFull.Windowed                  = FALSE;
    D3DPPFull.BackBufferWidth           = Width;
    D3DPPFull.BackBufferHeight          = Height;
    D3DPPFull.EnableAutoDepthStencil    = TRUE;
    D3DPPFull.AutoDepthStencilFormat    = D3DFMT_D16;
    D3DPPFull.Flags                     = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
    return S_OK;
}

// デバイスの取得
HRESULT  MyD3D::InitD3D(LPDIRECT3DDEVICE9 *ppDEV)
{   HRESULT     hr;

    SetD3DPPWin();
    hr = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                D3DCREATE_HARDWARE_VERTEXPROCESSING, &D3DPPWin, &pDEV);
    if (FAILED(hr))
    {   hr = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                D3DCREATE_SOFTWARE_VERTEXPROCESSING, &D3DPPWin, &pDEV);
        if (FAILED(hr))
        {   hr = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd,
                D3DCREATE_SOFTWARE_VERTEXPROCESSING, &D3DPPWin, &pDEV);
            if (FAILED(hr))
            {  MessageBox(NULL,"Create Device Error","Surface Error",MB_OK); return E_FAIL;  }
        }
    }
    *ppDEV= pDEV;
    return S_OK;
}

// デバイスを Full Screen で初期化
HRESULT  MyD3D::InitD3DFull(LPDIRECT3DDEVICE9 *ppDEV, UINT Width, UINT Height)
{   HRESULT     hr;

    SetD3DPPFull(Width,Height);
    hr = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                 D3DCREATE_HARDWARE_VERTEXPROCESSING, &D3DPPFull, &pDEV);
    if (FAILED(hr))
    {   hr = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd,
                     D3DCREATE_SOFTWARE_VERTEXPROCESSING, &D3DPPFull, &pDEV);
        if (FAILED(hr))
        {  MessageBox(NULL,"Create Device Error","Surface Error",MB_OK); return E_FAIL;  }
    }
    *ppDEV= pDEV;
    return S_OK;
}

//★ Load X-File
//if ((g_Num=LoadXFile("Bball.x",&g_pMesh,&g_pMat,&g_pTex))==0)
DWORD  MyD3D::LoadXFile(char* name,LPD3DXMESH* pMesh,D3DMATERIAL9** pMat,LPDIRECT3DTEXTURE9** pTex)
{   LPD3DXBUFFER pD3DXBuf;
    DWORD        Num,i;

    if( FAILED( D3DXLoadMeshFromX(name,D3DXMESH_SYSTEMMEM,pDEV,NULL,
                                  &pD3DXBuf,NULL,&Num,pMesh)))
    {   return 0;   }
    // We need to extract the material properties and texture names from the 
    D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXBuf->GetBufferPointer();
    *pMat = new D3DMATERIAL9[Num];
    *pTex = new LPDIRECT3DTEXTURE9[Num];
    for(i=0; i<Num; i++)
    {   // Copy the material
        (*pMat)[i] = d3dxMaterials[i].MatD3D;
//        (*pMat)[i].Ambient= (*pMat)[i].Diffuse;
        (*pMat)[i].Ambient.r= (*pMat)[i].Diffuse.r/2;
        (*pMat)[i].Ambient.g= (*pMat)[i].Diffuse.g/2;
        (*pMat)[i].Ambient.b= (*pMat)[i].Diffuse.b/2;
        // Create the texture
        if (FAILED(D3DXCreateTextureFromFile(pDEV,
                   d3dxMaterials[i].pTextureFilename,&(*pTex)[i])))
        {   (*pTex)[i] = NULL;   }
    }
    // Done with the material buffer
    pD3DXBuf->Release();
    // 法線ベクトルが設定されていないときの処理
    if (!( (*pMesh)->GetFVF() & D3DFVF_NORMAL ))
    {
        ID3DXMesh* pTempMesh;
        (*pMesh)->CloneMeshFVF((*pMesh)->GetOptions(), (*pMesh)->GetFVF() | D3DFVF_NORMAL, pDEV, &pTempMesh);
        D3DXComputeNormals(pTempMesh, NULL);
        SAFE_RELEASE(*pMesh);
        (*pMesh) = pTempMesh;
    }
    return Num;
}

//★ Light 初期化して RGB を設定する
//   SetLight(1.0f, 0.8f, 0.5f);
void  MyD3D::SetLight(float r, float g, float b)
{   //光源の作成
    ZeroMemory(&Light,sizeof(D3DLIGHT9));
    Light.Type = D3DLIGHT_DIRECTIONAL;
    Light.Diffuse.r= r;
    Light.Diffuse.g= g;
    Light.Diffuse.b= b;
    Light.Specular.r= r;
    Light.Specular.g= g;
    Light.Specular.b= b;
    Light.Ambient.r= r/2.0f;
    Light.Ambient.g= g/2.0f;
    Light.Ambient.b= b/2.0f;
    D3DXVec3Normalize((D3DXVECTOR3*)&Light.Direction,&D3DXVECTOR3(1.0f,-1.5f,0.7f));
    pDEV->SetLight(0,&Light);
    pDEV->LightEnable(0,TRUE);
}

//   SetLight(D3DCOLOR_XRGB(200,150,0));
void  MyD3D::SetLight(D3DCOLOR cor)
{   float       r,g,b;
    r= (float)((cor>>16)&0xff)/255.0f;
    g= (float)((cor>>8)&0xff)/255.0f;
    b= (float)(cor&0xff)/255.0f;
    SetLight(r,g,b);
}

void  MyD3D::SetLight()
{   SetLight(1.0f, 1.0f, 1.0f);
}

//★ 標準的 Material の設定
void  MyD3D::SetMaterial()
{
    ZeroMemory(&Material,sizeof(D3DMATERIAL9));
    Material.Diffuse.r= Material.Diffuse.g= Material.Diffuse.b= 1.0f;
    Material.Ambient.r= Material.Ambient.g= Material.Ambient.b= 0.5f;
    Material.Specular.r= Material.Specular.g= Material.Specular.b= 0.1f;
    Material.Emissive.r= Material.Emissive.g= Material.Emissive.b= 0.1f;
    Material.Power= 0;
    pDEV->SetMaterial(&Material);
}

//★ Material の設定
//   SetMaterial(1.0f,0.8f,0.3f);
void  MyD3D::SetMaterial(float r, float g, float b)
{
    ZeroMemory(&Material,sizeof(D3DMATERIAL9));
    Material.Diffuse.r= r;
    Material.Diffuse.g= g;
    Material.Diffuse.b= b;
    Material.Ambient.r= r/2;
    Material.Ambient.g= g/2;
    Material.Ambient.b= b/2;
    Material.Specular.r= r/10;
    Material.Specular.g= g/10;
    Material.Specular.b= b/10;
    Material.Emissive.r= r/10;
    Material.Emissive.g= g/10;
    Material.Emissive.b= b/10;
    Material.Power= 0;
    pDEV->SetMaterial(&Material);
}

//★ 回転計算(rt=度)
void  MyD3D::Rotate(D3DXVECTOR3 *Vect,D3DXVECTOR3 *Cen,float rt,int xyz)
{   float   px,py,pz;

    px= Vect->x-Cen->x;
    py= Vect->y-Cen->y;
    pz= Vect->z-Cen->z;
    switch(xyz)
    {   case 0: Vect->z= (float)(pz*cos(rt/180*3.14)-py*sin(rt/180*3.14)+Cen->y);
                Vect->y= (float)(pz*sin(rt/180*3.14)+py*cos(rt/180*3.14)+Cen->z);
                return;
        case 1: Vect->x= (float)(px*cos(rt/180*3.14)-pz*sin(rt/180*3.14)+Cen->x);
                Vect->z= (float)(px*sin(rt/180*3.14)+pz*cos(rt/180*3.14)+Cen->z);
                return;
        case 2: Vect->y= (float)(py*cos(rt/180*3.14)-px*sin(rt/180*3.14)+Cen->x);
                Vect->x= (float)(py*sin(rt/180*3.14)+px*cos(rt/180*3.14)+Cen->y);
                return;
    }
}

//★ メッシュの頂点バッファにアクセスして中心から横に移動
//   Move_Mesh(g_pMesh2, D3DXVECTOR3(-1,0,0));
void  MyD3D::Move_Mesh(LPD3DXMESH pMesh, D3DXVECTOR3 v)
{   DWORD                   nV,i;
    D3DVERTEX*              pVSrc;
    LPDIRECT3DVERTEXBUFFER9 pSrcVB;

    nV= pMesh->GetNumVertices();
    pMesh->GetVertexBuffer(&pSrcVB);
    pSrcVB->Lock(0,0,(void**)&pVSrc,0);
    for(i=0; i<nV; i++)
    {
        pVSrc->position= pVSrc->position + v;
        pVSrc+= 1;
    }
    pSrcVB->Unlock();
    SAFE_RELEASE(pSrcVB);
}

//★ 二個のMeshの頂点をウエイトを付けて混ぜ合わす
//   BlendMeshes(g_pMesh[0],g_pMesh[3],g_pMesh[2],-fweight);
VOID  MyD3D::BlendMeshes(LPD3DXMESH pDst, LPD3DXMESH pSrc1, LPD3DXMESH pSrc2, FLOAT fWeight)
{
    LPDIRECT3DVERTEXBUFFER9 pDstVB;
    LPDIRECT3DVERTEXBUFFER9 pSrcVB1;
    LPDIRECT3DVERTEXBUFFER9 pSrcVB2;
    D3DVERTEX*              pVDst;
    D3DVERTEX*              pVSrc1;
    D3DVERTEX*              pVSrc2;
    DWORD                   NumVT;
    FLOAT                   finvWeight= 1.0f - fWeight;

    NumVT = pDst->GetNumVertices();
    pDst->GetVertexBuffer(&pDstVB);
    pSrc1->GetVertexBuffer(&pSrcVB1);
    pSrc2->GetVertexBuffer(&pSrcVB2);

    pDstVB->Lock(0,0,(void**)&pVDst,0);
    pSrcVB1->Lock(0,0,(void**)&pVSrc1,0);
    pSrcVB2->Lock(0,0,(void**)&pVSrc2,0);
    for(DWORD i=0; i<NumVT; i++)
    {
        pVDst->position= fWeight * pVSrc1->position + finvWeight * pVSrc2->position;
        pVDst->normal= fWeight * pVSrc1->normal + finvWeight * pVSrc2->normal;
        pVDst++;
        pVSrc1++;
        pVSrc2++;
    }
    pDstVB->Unlock();
    pSrcVB1->Unlock();
    pSrcVB2->Unlock();
    SAFE_RELEASE(pDstVB);
    SAFE_RELEASE(pSrcVB1);
    SAFE_RELEASE(pSrcVB2);
}

//★ Vertex Buffer を取得する
HRESULT MyD3D::GetVtxBuff(LPD3DXMESH pMesh,DWORD *NumVtx,DWORD *NumFaces,
                LPDIRECT3DVERTEXBUFFER9 *pVB,LPDIRECT3DINDEXBUFFER9 *pMIdx)
{
    *NumVtx= pMesh->GetNumVertices();
    *NumFaces= pMesh->GetNumFaces();
    pMesh->GetVertexBuffer(pVB);
    pMesh->GetIndexBuffer(pMIdx);
    if (*NumVtx==0 || *NumFaces==0 || *pVB==NULL || *pMIdx==NULL)   return E_FAIL;
    return S_OK;
}

//★ Surface の Pixcel を Blend する
HRESULT  MyD3D::BlendPix(LPDIRECT3DSURFACE9 D_Sur,LPDIRECT3DSURFACE9 S_Sur1,LPDIRECT3DSURFACE9 S_Sur2, RECT Rect, float Rate)
{   D3DLOCKED_RECT  D_Rect,S_Rect1,S_Rect2;
    int             xp,yp;
    BYTE            r1,g1,b1,r2,g2,b2;
    float           rate2;
	BYTE            *wp;

    if (D_Sur->LockRect(&D_Rect,&Rect,D3DLOCK_DISCARD)!=D3D_OK)
    {   MessageBox(NULL,"LockRect Error","Surface Error",MB_OK);
        return FALSE;
    }
    if (S_Sur1->LockRect(&S_Rect1,&Rect,D3DLOCK_READONLY)!=D3D_OK)
    {   MessageBox(NULL,"LockRect Error","Surface Error",MB_OK);
        return FALSE;
    }
    if (S_Sur2->LockRect(&S_Rect2,&Rect,D3DLOCK_READONLY)!=D3D_OK)
    {   MessageBox(NULL,"LockRect Error","Surface Error",MB_OK);
        return FALSE;
    }
	rate2= 1.0f-Rate;
    // r,g,b をブレンドして転送
    for(yp=Rect.top; yp<Rect.bottom; yp++)
        for(xp=0; xp<D_Rect.Pitch; xp+=4)
        {   wp= (BYTE*)S_Rect1.pBits+(yp-Rect.top)*S_Rect1.Pitch+xp;
			b1= *(wp);
            g1= *(wp+1);
            r1= *(wp+2);
            wp= (BYTE*)S_Rect2.pBits+(yp-Rect.top)*S_Rect2.Pitch+xp;
            b2= *(wp);
            g2= *(wp+1);
            r2= *(wp+2);
            wp= (BYTE*)D_Rect.pBits+(yp-Rect.top)*D_Rect.Pitch+xp;
            *(wp)=   (BYTE)(b1*rate2 + b2*Rate);
            *(wp+1)= (BYTE)(g1*rate2 + g2*Rate);
            *(wp+2)= (BYTE)(r1*rate2 + r2*Rate);
        }
    D_Sur->UnlockRect();
    S_Sur1->UnlockRect();
    S_Sur2->UnlockRect();
    return TRUE;
}

//★ S_Sur の画像を手前を拡大して３Ｄのように変形して D_Sur に格納する
// pos:スクロールＹの開始座標  rate:拡大率  sade:拡大の中心(2.0f:中央,10.0f:左端,1.1f:右端)
HRESULT  MyD3D::Scroll3D(LPDIRECT3DSURFACE9 D_Sur,LPDIRECT3DSURFACE9 S_Sur,RECT Rect,int Pos,float Rate,float Side)
{   D3DLOCKED_RECT  D_Rect,S_Rect;
    static int      dx,dy,xp,yp,dyw;
    static int      Width,Height;
    static float    width,height,newwx,a;
    static DWORD    *pDBits,*pSBits,*wkd,*wks;

    if (D_Sur->LockRect(&D_Rect,&Rect,D3DLOCK_DISCARD)!=D3D_OK)
    {   MessageBox(NULL,"LockRect Error","Surface Error",MB_OK);
        return FALSE;
    }
    if (S_Sur->LockRect(&S_Rect,&Rect,D3DLOCK_READONLY)!=D3D_OK)
    {   MessageBox(NULL,"LockRect Error","Surface Error",MB_OK);
        return FALSE;
    }
    pDBits= (DWORD*)D_Rect.pBits;
    pSBits= (DWORD*)S_Rect.pBits;
    Height= Rect.bottom-Rect.top;
    Width= Rect.right-Rect.left;
    height= (float)Height;
    width = (float)Width;
    newwx = width/Rate;
    for(dy=0; dy<Height; dy++)
    {   dyw= Height-dy;
		yp= (Pos+dyw)%Height;
        a= (width-newwx) * dyw / height + newwx;
        for(dx=0; dx<Width; dx++)
        {   xp= (int)(a * dx / width + (width-a)/Side);
            wks= pSBits+yp*Width+xp;
            wkd= pDBits+dy*Width+dx;
            *(wkd)= *(wks);
        }
	}
    D_Sur->UnlockRect();
    S_Sur->UnlockRect();
    return TRUE;
}

//☆ TEXT 形式のデータを float,int,str に変換して格納する
//   データは［，］で区切る。/ から行末まではコメント。
// 数字の先頭位置をポイントする
char* MyD3D::SetPos(char *buf)
{   char    *p;

    p= buf;
    while(*p!=0)
    {   for(; *p && *p!='/' && *p!='-' && *p!='+' && (*p<'0' || *p>'9'); p++);
        switch(*p)
        {   case '/':
                for(p++; *p && *p!='\n'; p++);
                continue;
            case '\0':
                return p; //NULL;
            default:
                return p;
        }
    }
    return p;
}
// TEXT 形式のデータを float に変換して格納する
char* MyD3D::SetFloat(float *tbl, char *buf, int cnt)
{   int     c;
    char    *p;

    for(p=buf,c=0; *p!=0 && c<cnt; c++)
    {   p= SetPos(p);
        if (*p)
        {   *(tbl+c)= (float)atof(p);
        }
        for(p++; *p && *p!=',' && *p!='\n'; p++);
        p++;
    }
    return(p);
}
// TEXT 形式のデータを int に変換して格納する
char* MyD3D::SetInt(int *val, char *buf, int cnt)
{   int     c;
    char    *p;

    for(p=buf,c=0; *p!=0 && c<cnt; c++)
    {   p= SetPos(p);
        if (*p)     val[c]= atoi(p);
        for(p++; *p && *p!=',' && *p!='\n'; p++);
        p++;
    }
    return(p);
}
// TEXT 形式のデータから文字列を抽出して格納する
char* MyD3D::SetStr(char *val, char *buf)
{   int     c;
    char    *p;

    p= buf;
    while(*p!=0)
    {   for(; *p==' '; p++);
        switch(*p)
        {   case '/':
                for(p++; *p && *p!='\n'; p++);
            case '\r': case '\n':
                for(; *p && (*p=='\r' || *p=='\n'); p++);
                continue;
        }
        break;
    }
    for(c=0; *p && *p!=' ' && *p!=',' && *p!='\n'; c++,p++)     val[c]= *p;
    val[c]= '\0';
    for(; *p && *p!=',' && *p!='\n'; p++);
    p++;
    return(p);
}

//★ fname から最大 len バイトのデータを buf に読み込む
//   buf は buf+1 バイト以上の領域を確保すること
DWORD  MyD3D::ReadData(LPSTR fname, void *buf, DWORD len)
{   HANDLE      hFile;
    DWORD       dwBytes;

    hFile = CreateFile(fname,GENERIC_READ,0,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    if (hFile==INVALID_HANDLE_VALUE)
    {   MessageBox(NULL,fname,"ReadData Open Error",MB_OK);
        return 0;
    }
    if (SetFilePointer(hFile,0,0,FILE_BEGIN)==0xFFFFFFFF) return 0;
    ReadFile(hFile,buf,len,&dwBytes,NULL);
    *((char*)buf+dwBytes)= 0;
    CloseHandle(hFile);
    return dwBytes;
}

//★★ キーフレームの座標設定
//D3DXVECTOR3(View, Terg, MatList) を設定
void  MyD3D::SetVect(D3DXVECTOR3 *v3, float fv[3])
{   v3->x= fv[0];
    v3->y= fv[1];
    v3->z= fv[2];
}

//D3DXVECTOR3 にブレンドして格納
void  MyD3D::BlendVect(D3DXVECTOR3 *v3, float fv1[3], float fv2[3], float fWeight)
{   float   weight= 1.0f - fWeight;
    v3->x= fWeight*fv1[0] + weight*fv2[0];
    v3->y= fWeight*fv1[1] + weight*fv2[1];
    v3->z= fWeight*fv1[2] + weight*fv2[2];
}

//float[6] をブレンドする(fWeight は fv1 の率)
void  MyD3D::BlendF6(float fv[6], float fv1[6], float fv2[6], float fWeight)
{   int     i;
    float   weight= 1.0f - fWeight;
    for(i=0; i<6; i++)  fv[i]= fWeight*fv1[i] + weight*fv2[0];
}

//viewt[6] を viewForm, tergForm に格納して、viewt[6] を WORLD 座標に設定
void  MyD3D::SetView(D3DXVECTOR3 *viewForm, D3DXVECTOR3 *tergForm, float viewt[12])
{   D3DXMATRIX      matRotX,matRotY,matRotZ;
    D3DXMATRIX      matRot,matTrans;
    D3DXMATRIX      matWorld;

    viewForm->x= viewt[0];
    viewForm->y= viewt[1];
    viewForm->z= viewt[2];
    tergForm->x= viewt[3];
    tergForm->y= viewt[4];
    tergForm->z= viewt[5];
    D3DXMatrixTranslation(&matTrans,viewt[6],viewt[7],viewt[8]);
    D3DXMatrixRotationX(&matRotX,viewt[9]);
    D3DXMatrixRotationY(&matRotY,viewt[10]);
    D3DXMatrixRotationZ(&matRotZ,viewt[11]);
    matRot= matRotZ * matRotY * matRotX;
    matWorld= matRot * matTrans;
    pDEV->SetTransform(D3DTS_WORLD,&matWorld);
}

//キーフレームをブレンドして座標の移動と回転を設定
void  MyD3D::BlendFrame(D3DXVECTOR3 *viewForm, D3DXVECTOR3 *tergForm,
                        float src1[12], float src2[12], float fWeight)
{   D3DXMATRIX      matRotX,matRotY,matRotZ;
    D3DXMATRIX      matRot,matTrans;
    D3DXMATRIX      matWorld;
    float           dst[12];
    float           finvWeight= 1.0f - fWeight;
    int             i;
    float           rot[3];

    for(i=0; i<9; i++)  dst[i]= fWeight*src1[i] + finvWeight*src2[i];
    //☆回転を最大180度に制限して、動きを補正する
    for(i=0; i<3; i++)
    {   rot[i]= src2[i+9]-src1[i+9];
        if (rot[i] > 3.14f)   rot[i]= rot[i]-6.28f;
        if (rot[i] < -3.14f)  rot[i]= rot[i]+6.28f;
        dst[i+9]= src1[i+9]+rot[i]*finvWeight;
    }
    SetView(viewForm,tergForm,dst);
}

//★ 画像ファイルを hBmpDC にロード
HRESULT  MyD3D::LoadImg(LPSTR ImageName)
{   int     i;
    strcpy_s(szFile,MAX_PATH,ImageName);
    for(i=strlen(ImageName)-1; i>0 && ImageName[i]!='.'; i--);
    if (ImageName[i+1]=='G' || ImageName[i+1]=='g') return LoadGif(ImageName);
    if (ImageName[i+1]=='J' || ImageName[i+1]=='j') return LoadJpeg(ImageName);
    if (ImageName[i+1]=='B' || ImageName[i+1]=='b') return LoadBmp(ImageName);
    else    return FALSE;
}

// Load BMP Image
HRESULT  MyD3D::LoadBmp(LPSTR szBitmap, WORD WN, WORD HN)
{   HDC         hdc;
    BITMAP      bmp;

    SAFE_DELDC(hBmpDC);
    SAFE_DELOBJ(hBmp);
    // ビットマップリソースからの読み込み
    hBmp= (HBITMAP)LoadImage(GetModuleHandle(NULL),szBitmap,IMAGE_BITMAP,
                                0,0,LR_CREATEDIBSECTION);
    // ビットファイルからの読み込み
    if (hBmp==NULL)
    {   hBmp= (HBITMAP)LoadImage(NULL,szBitmap,IMAGE_BITMAP,0,0,
                                    LR_LOADFROMFILE|LR_CREATEDIBSECTION);
    }
    if (hBmp==NULL)
    {   MessageBox(hWnd, szBitmap, "Load BMP Error", MB_OK);
        return  E_FAIL;
    }

    XNum= WN;
    YNum= HN;
    hdc= GetDC(hWnd);
    hBmpDC= CreateCompatibleDC(hdc);
    SelectObject(hBmpDC, hBmp);
    GetObject(hBmp,sizeof(BITMAP),&bmp);
    SetRect(&ImgRect,0,0,bmp.bmWidth,bmp.bmHeight);
    SWidth= bmp.bmWidth/XNum;
    SHeight= bmp.bmHeight/YNum;
    DeleteDC(hdc);
    return  S_OK;
}

//★ DLL を使って GIF(JPEG) をロード
//   CreateBuf() で古い hBmpDC,hBmp を開放している
HRESULT  MyD3D::LoadDLL(LPSTR ImageName, LPSTR SPI)
{   int         ret;
    HLOCAL      hBInfo;     //BITMAPINFO 構造体の LOCATION POINTER
    HLOCAL      hBm;        //ビットマップデータ本体の LOCATION POINTER
    HPALETTE    hPalette = NULL;   //パレット構造体の POINTER
    BITMAPINFO  *pBInfo;    //hBInfo の LocalLock POINTER
    BYTE        *pBm;       //hBm の LocalLock POINTER

    hSpi= LoadLibrary(SPI);
    F_GetPicture= (GetPicture)GetProcAddress(hSpi,"GetPicture");
    SWidth=SHeight= 0;

    // ★画像をロードする
    ret= (*F_GetPicture)(ImageName,0,0,&hBInfo,&hBm,NULL,1);
    if (ret!=0)
    {   MessageBox(NULL,ImageName,"Image Load Error",MB_OK);
        return FALSE;
    }
    if (hBInfo != NULL)
    {   hPalette = DIBtoPAL(hBInfo);  }
    pBInfo = (BITMAPINFO*)LocalLock(hBInfo);
    pBm = (BYTE*)LocalLock(hBm);
    SWidth= (WORD)pBInfo->bmiHeader.biWidth;
    SHeight= (WORD)pBInfo->bmiHeader.biHeight;
    SetRect(&ImgRect,0,0,SWidth,SHeight);
    CreateBuf(SWidth,SHeight);
    StretchDIBits(hBmpDC,0,0,SWidth,SHeight,0,0,SWidth,SHeight,pBm,pBInfo,DIB_RGB_COLORS,SRCCOPY);
    LocalUnlock(hBInfo);
    LocalUnlock(hBm);
    if (hBInfo)     LocalFree(hBInfo);
    if (hBm)        LocalFree(hBm);
    if (hPalette)   LocalFree(hPalette);
    FreeLibrary(hSpi);
    return TRUE;
}

//★ Sprite を切り出して描画する
//★ 透明色無しで描画する
HRESULT  MyD3D::Show(HDC hdc,long x,long y,long w,long h,long xd,long yd)
{   if (hBmpDC==NULL)   return FALSE;
    BitBlt(hdc,x,y,w,h,hBmpDC,xd,yd,SRCCOPY);
    return TRUE;
}
HRESULT  MyD3D::Show(long x,long y,long w,long h,long xd,long yd)
{   HDC     hdc;
    if (hBmpDC==NULL)   return FALSE;
    hdc= GetDC(hWnd);
    Show(hdc,x,y,w,h,xd,yd);
    ReleaseDC(hWnd,hdc);
    return TRUE;
}

//★ 透明色を設定して描画する
HRESULT  MyD3D::ShowBlack(HDC hdc,long x,long y,long w,long h,long xd,long yd)
{   if (hBmpDC==NULL)   return FALSE;
    TransparentBlt(hdc,x,y,w,h,hBmpDC,xd,yd,w,h,RGB(0,0,0));
    return TRUE;
}
HRESULT  MyD3D::ShowBlack(long x,long y,long w,long h,long xd,long yd)
{   HDC     hdc;
    if (hBmpDC==NULL)   return FALSE;
    hdc= GetDC(hWnd);
    ShowBlack(hdc,x,y,w,h,xd,yd);
    ReleaseDC(hWnd,hdc);
    return TRUE;
}

//★ Sprite を切り出して描画する
// n:番号  x:描画X  y:描画Y  w:幅  h:高さ  xoff:Sprite Xoff  yoff:Sprite Yoff
HRESULT  MyD3D::Show(HDC hdc,long n,long x,long y,long w,long h,long xoff,long yoff)
{   int     xp,yp;
    if (hBmpDC==NULL)   return FALSE;
    if (n>=XNum*YNum)   return FALSE;
    xp= (n%XNum)*SWidth+xoff;
    yp= (n/XNum)*SHeight+yoff;
    TransparentBlt(hdc,x,y,w,h,hBmpDC,xp,yp,w,h,RGB(0,0,0));
    return TRUE;
}
// n:番号  x:描画X  y:描画Y  w:幅  h:高さ  xoff:Sprite Xoff  yoff:Sprite Yoff
HRESULT  MyD3D::Show(long n,long x,long y,long w,long h,long xoff,long yoff)
{   HDC     hdc;
    if (hBmpDC==NULL)   return FALSE;
    if (n>=XNum*YNum)   return FALSE;
    hdc= GetDC(hWnd);
    Show(hdc,n,x,y,w,h,xoff,yoff);
    ReleaseDC(hWnd,hdc);
    return TRUE;
}

//★ 拡大／縮小して描画する
HRESULT  MyD3D::ShowSize(HDC hdc,long x,long y,float size)
{   int     ww,hw;
    ww= (int)(ImgRect.right*size);
    hw= (int)(ImgRect.bottom*size);
    TransparentBlt(hdc,x,y,ww,hw,hBmpDC,0,0,ImgRect.right,ImgRect.bottom,RGB(0,0,0));
    return TRUE;
}
HRESULT  MyD3D::ShowSize(long x,long y,float size)
{   HDC     hdc;
    if (hBmpDC==NULL)   return FALSE;
    hdc= GetDC(hWnd);
    ShowSize(hdc,x,y,size);
    ReleaseDC(hWnd,hdc);
    return TRUE;
}
HRESULT  MyD3D::ShowSize(HDC hdc,long n,long x,long y,float size)
{   int     xp,yp;
    long    ww,hw;
    if (hBmpDC==NULL)   return FALSE;
    if (n>=XNum*YNum)   return FALSE;
    xp= (n%XNum)*SWidth;
    yp= (n/XNum)*SHeight;
    ww= (long)(SWidth*size);
    hw= (long)(SHeight*size);
    TransparentBlt(hdc,x,y,ww,hw,hBmpDC,xp,yp,SWidth,SHeight,RGB(0,0,0));
    return TRUE;
}
HRESULT  MyD3D::ShowSize(long n,long x,long y,float size)
{   HDC     hdc;
    if (hBmpDC==NULL)   return FALSE;
    if (n>=XNum*YNum)   return FALSE;
    hdc= GetDC(hWnd);
    ShowSize(hdc,n,x,y,size);
    ReleaseDC(hWnd,hdc);
    return TRUE;
}

// 画像用の Buffer を取得
void  MyD3D::CreateBuf(long width, long height)
{   HDC     hDC;

    SAFE_DELDC(hBmpDC);
    SAFE_DELOBJ(hBmp);
    hDC= GetDC(hWnd);
    hBmpDC = CreateCompatibleDC(hDC);
    hBmp = CreateCompatibleBitmap(hDC,width,height);
    SelectObject(hBmpDC,hBmp);
    ReleaseDC(hWnd,hDC);
}

// BITMAPINFO からパレットを作ります
HPALETTE  MyD3D::DIBtoPAL(HANDLE hBInfo)
{   BITMAPINFO  *pBInfo;
    LOGPALETTE  *pPAL;
    HPALETTE    hPAL;
    int         i;

    pBInfo = (BITMAPINFO*)LocalLock(hBInfo);
    if(pBInfo->bmiHeader.biBitCount >= 16)
    {   LocalUnlock(hBInfo);
        return NULL;
    }
    pPAL = (LOGPALETTE*)LocalAlloc(LPTR, sizeof(LOGPALETTE) + sizeof(PALETTEENTRY) * 256);
    pPAL->palVersion = 0x3000;
    pPAL->palNumEntries = (WORD)(1L << pBInfo->bmiHeader.biBitCount);
    for(i=0;i<pPAL->palNumEntries;i++)
    {   pPAL->palPalEntry[i].peRed  = pBInfo->bmiColors[i].rgbRed;
        pPAL->palPalEntry[i].peGreen= pBInfo->bmiColors[i].rgbGreen;
        pPAL->palPalEntry[i].peBlue = pBInfo->bmiColors[i].rgbBlue;
        pPAL->palPalEntry[i].peFlags= pBInfo->bmiColors[i].rgbReserved;
    }
    hPAL = CreatePalette(pPAL);
    LocalFree(pPAL);
    LocalUnlock(hBInfo);
    return(hPAL);
}

// Window Size を画像に合わす
void    MyD3D::Adjust()
{   RECT    rc;
    int     x,y;

    GetWindowRect(hWnd, &rc);
    x= rc.left;
    y= rc.top;
    rc.right= x + SWidth;
    rc.bottom= y + SHeight;
    AdjustWindowRect(&rc,WS_OVERLAPPEDWINDOW,TRUE);
    MoveWindow(hWnd,x,y,rc.right-rc.left,rc.bottom-rc.top,TRUE);
}

// OPENFILENAME 構造体で BMP ファイルを Load する
void    MyD3D::Open(LPSTR szDir)
{   OPENFILENAME    ofn;

    memset(&ofn,0,sizeof(OPENFILENAME));
    ofn.lStructSize= sizeof(OPENFILENAME);
    ofn.hwndOwner= hWnd;
    ofn.lpstrFilter= "bmp(*.bmp)\0*.bmp\0All files(*.*)\0*.*\0\0";
    ofn.lpstrFile= szFile;
    ofn.lpstrFileTitle= NULL;
    ofn.nMaxFile= MAX_PATH;
    ofn.Flags= OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt= "bmp";
    ofn.lpstrTitle= "ファイルオープン！";
    ofn.lpstrInitialDir= szDir;
    if (GetOpenFileName(&ofn)!=TRUE)
    {   MessageBox(hWnd,"Open File Error","Bmp Open",MB_OK);  }
    else    LoadImg(szFile);
}

// Set SPRITE Num(Sprite の分割数を変更する)
HRESULT  MyD3D::SetNum(WORD WN, WORD HN)
{   XNum= WN;
    YNum= HN;
    SWidth= ImgRect.right/XNum;
    SHeight= ImgRect.bottom/YNum;
    return  S_OK;
}

// Set SPRITE Size(Sprite Size を変更する)
HRESULT  MyD3D::SetSize(WORD WSIZ, WORD HSIZ)
{   SWidth= WSIZ;
    SHeight= HSIZ;
    XNum= ImgRect.right/WSIZ;
    YNum= ImgRect.bottom/HSIZ;
    return  S_OK;
}

//文字フォントの設定
HFONT  MyD3D::SetMyFont(LPCTSTR face,int h,int angle)
{   HFONT   hFont;
    hFont=  CreateFont(h,       //フォント高さ
        0,                      //文字幅（高さと同じ）
        angle,                  //テキストの角度
        0,                      //ベースラインとｘ軸との角度
        FW_REGULAR,             //フォントの重さ（太さ）
        FALSE,                  //イタリック体
        FALSE,                  //アンダーライン
        FALSE,                  //打ち消し線
        SHIFTJIS_CHARSET,       //文字セット
        OUT_DEFAULT_PRECIS,     //出力精度
        CLIP_DEFAULT_PRECIS,    //クリッピング精度
        PROOF_QUALITY,          //出力品質
        FIXED_PITCH|FF_MODERN,  //ピッチとファミリー
        face);                  //書体名
    return hFont;
}

//★ Back Buffer の DC を取得する
//   取得した DC は ReleaseBackDC() で開放して下さい
HDC  MyD3D::GetBackDC()
{   HRESULT     hr;
    HDC         hDC;

    hr= pDEV->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&pBack);
    if (FAILED(hr))
    {   MessageBox(NULL,"GetBackBuffer Error","Error Message", MB_OK);
        return NULL;
    }
    hr= pBack->GetDC(&hDC);
    if (FAILED(hr))
    {   SAFE_RELEASE(pBack);
        return NULL;
    }
    return hDC;
}

//★ Back Buffer の DC を開放する
void  MyD3D::ReleaseBackDC(HDC hDC)
{
    pBack->ReleaseDC(hDC);
    SAFE_RELEASE(pBack);
}

//★ Load Image File to Texture
//   取得した Texture は責任を持って開放して下さい
LPDIRECT3DTEXTURE9  MyD3D::LoadTexture(LPSTR imgfile)
{   HRESULT         hr;

    if (PathCheck(imgfile)==-1) return NULL;
    hr = D3DXCreateTextureFromFile(pDEV,imgfile,&pTexture);
    if (FAILED(hr))     return NULL;
    return pTexture;
}

//★ Load Image File to Texture
//   取得した Texture は責任を持って開放して下さい
LPDIRECT3DTEXTURE9  MyD3D::LoadTexture(LPSTR imgfile, RECT rc)
{   HRESULT         hr;

    if (PathCheck(imgfile)==-1) return NULL;
    hr= D3DXCreateTextureFromFileEx(pDEV,imgfile,rc.right-rc.left,rc.bottom-rc.top,
          1,0,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,D3DX_DEFAULT,D3DX_DEFAULT,
          NULL,NULL,NULL,&pTexture);
    if (FAILED(hr))
    {   MessageBox(NULL,imgfile,"Create Texture Error",MB_OK);
        return NULL;
    }
    return pTexture;
}

//★ Load Image File set ColorKey
//   取得した Texture は責任を持って開放して下さい
LPDIRECT3DTEXTURE9  MyD3D::LoadTextureBlack(LPSTR imgfile)
{   RECT    rc;
    HRESULT hr;

    rc= GetFileRect(imgfile);
    if (rc.right==0)    return NULL;
    hr= D3DXCreateTextureFromFileEx(pDEV,imgfile,rc.right-rc.left,rc.bottom-rc.top,
          1,0,D3DFMT_UNKNOWN,D3DPOOL_MANAGED,D3DX_DEFAULT,D3DX_DEFAULT,
          D3DCOLOR_XRGB(0,0,0),NULL,NULL,&pTexture);
    if (FAILED(hr))
    {   MessageBox(NULL,imgfile,"Create Texture Error",MB_OK);
        return NULL;
    }
    return pTexture;
}

//★ Load Image File to Surface
//   取得した Surface は責任を持って開放して下さい
LPDIRECT3DSURFACE9  MyD3D::LoadSurface(LPSTR imgfile)
{   RECT    rc;
    HRESULT hr;

    rc= GetFileRect(imgfile);
    if (rc.right==0)    return NULL;
    //OffscreenSurface の作成
    if (FAILED(pDEV->CreateOffscreenPlainSurface(rc.right-rc.left,rc.bottom-rc.top,
        D3DFMT_X8R8G8B8,D3DPOOL_DEFAULT,&pSurface,NULL)))
    {   MessageBox(NULL,"OffscreenSurface の作成に失敗","Error Message", MB_OK);
        return NULL;
    }
    //画像ファイルの読み込み
    hr= D3DXLoadSurfaceFromFile(pSurface,NULL,NULL,imgfile,NULL,D3DX_FILTER_NONE,0,NULL);
    if (FAILED(hr))
    {   MessageBox(NULL,imgfile,"IMG File Open Error",MB_OK);
        return NULL;
    }
    return pSurface;
}

//★ BackBuffer と同じ状態の Offscreen Surface を生成
//   取得した Surface は責任を持って開放して下さい
LPDIRECT3DSURFACE9  MyD3D::CreateSurface()
{   D3DSURFACE_DESC desc;

    // バックバッファを取得
    if (FAILED(pDEV->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&pBack)))
    {   MessageBox(NULL,"GetBackBuffer Error","Error Message", MB_OK);
        return NULL;
    }
    // バックバッファの状態を取得
    pBack->GetDesc(&desc);
    // 画像ファイル用オフスクリーンサーフェースの作成
    if (FAILED(pDEV->CreateOffscreenPlainSurface(desc.Width, desc.Height, desc.Format,
                     D3DPOOL_DEFAULT, &pSurface, NULL )))
    {   MessageBox(NULL,"CreateOffscreenSurface Error","Sprite Error",MB_OK);
        return FALSE;
    }
    SAFE_RELEASE(pBack);
    return pSurface;
}

//★ Surface の サイズを取得する
RECT  MyD3D::GetSurfaceRect(LPDIRECT3DSURFACE9 sfc)
{   RECT            rect;
    D3DSURFACE_DESC desc;

    SetRect(&rect,0,0,0,0);
    if (sfc==NULL)  return rect;
    sfc->GetDesc(&desc);
    rect.right= desc.Width;
    rect.bottom= desc.Height;
    return rect;
};

//★ Texture の サイズを取得する
RECT  MyD3D::GetTextureRect(LPDIRECT3DTEXTURE9 tex, int lv)
{   RECT            rect;
    D3DSURFACE_DESC desc;

    SetRect(&rect,0,0,0,0);
    if (tex==NULL)  return rect;
    tex->GetLevelDesc(lv,&desc);
    rect.right= desc.Width;
    rect.bottom= desc.Height;
    return rect;
};

//★ Image File のサイズを取得
RECT  MyD3D::GetFileRect(LPSTR imgfile)
{   D3DXIMAGE_INFO  ImageInfo;
    RECT            rect;
    HRESULT         hr;

    SetRect(&rect,0,0,0,0);
    if (PathCheck(imgfile)==-1) return rect;
    //画像ファイル情報を取得
    hr= D3DXGetImageInfoFromFile(_T(imgfile),&ImageInfo);
    if (FAILED(hr))
    {   MessageBox(NULL,imgfile,"Image Info Error",MB_OK);
        return rect;
    }
    rect.right= ImageInfo.Width;
    rect.bottom= ImageInfo.Height;
    return rect;
}

// File Path Check
DWORD  MyD3D::PathCheck(LPSTR path)
{   char    file[MAX_PATH];
    int     p;
    DWORD   rc;

    for(p=strlen(path); p>0 && path[p]!='\\'; p--);
    p++;
    strcpy_s(file,MAX_PATH,path+p);
    rc= GetFileAttributes(file);    //カレントフォルダを調べる
    if (rc!=-1)
    {   strcpy_s(path,MAX_PATH,file);
        return rc;
    }
    rc= GetFileAttributes(path);    //path[] のフォルダを調べる
    if (rc!=-1)     return rc;
    MessageBox(NULL,path,"File Path Error",MB_OK);
    return -1;
}


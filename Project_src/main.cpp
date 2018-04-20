#include <iostream>
#include <fstream>
#include <iomanip>
#include <climits>
#include <limits>
#include <algorithm>
#include <vector>
#include <deque>
#include <queue>
#include <list>
#include <stack>
#include <string>
#include <functional>
#include <numeric>
#include <map>
#include <set>
#include <cstdlib>
#include <bitset>
#include <unordered_map>
#include <random>
#include <cmath>
#include <complex>

#include <Windows.h>

using namespace std;

typedef long long int ll;
typedef vector<int> vi;
typedef vector<vector<int>> vvi;
typedef pair<int, int> P;
typedef pair<ll, ll> Pll;
typedef vector<ll> vll;
typedef vector<vector<ll>> vvll;

const int INFL = (int)1e9;
const ll INFLL = (ll)1e18;
const double INFD = numeric_limits<double>::infinity();
const double PI = 3.14159265358979323846;
#define Loop(i, n) for(int i = 0; i < (int)n; i++)
#define Loopll(i, n) for(ll i = 0; i < (ll)n; i++)
#define Loop1(i, n) for(int i = 1; i <= (int)n; i++)
#define Loopll1(i, n) for(ll i = 1; i <= (ll)n; i++)
#define Loopr(i, n) for(int i = (int)n - 1; i >= 0; i--)
#define Looprll(i, n) for(ll i = (ll)n - 1; i >= 0; i--)
#define Loopr1(i, n) for(int i = (int)n; i >= 1; i--)
#define Looprll1(i, n) for(ll i = (ll)n; i >= 1; i--)
#define Loopitr(itr, container) for(auto itr = container.begin(); itr != container.end(); itr++)
#define printv(vector) Loop(i, vector.size()) { cout << vector[i] << " "; } cout << endl;
#define rnd(d) (ll)((double)d + (d >= 0 ? 0.5 : -0.5))
#define floorsqrt(x) ((ll)sqrt((double)x) + ((ll)sqrt((double)x) * (ll)sqrt((double)x) <= (ll)x ? 0 : -1))
#define ceilsqrt(x) ((ll)sqrt((double)x) + ((ll)x <= (ll)sqrt((double)x) * (ll)sqrt((double)x) ? 0 : 1))
#define ceildiv(a, b) ((ll)a / (ll)b + ((ll)a % (ll)b == 0 ? 0 : 1))
#define bitmanip(m,val) static_cast<bitset<(int)m>>(val)

/*******************************************************/


//Object Class Library から myd3d.h と myd3d.cpp を取得してプロジェクトに組み込んで下さい。
/*********************************************/
/*  メッシュ描画の基本プログラム    前田 稔  */
/*********************************************/
#define     NAME        "Material"
#include    "MyD3D.h"

//Global Area
HWND                    g_hWnd;
MyD3D                   *myd3d = NULL;     // MyD3D Object Class
LPDIRECT3DDEVICE9       g_p3DDev = NULL;
LPD3DXMESH              pMeshApp = NULL;
D3DXVECTOR3             ViewForm(0.0f, 0.0f, 200.0f);

//Function Prototype
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam);

//デバイス/モード等の初期化
HRESULT InitDevices(void)
{
	myd3d = new MyD3D(g_hWnd);
	myd3d->InitD3D(&g_p3DDev);
	//球メッシュの生成
	//D3DXCreateSphere(g_p3DDev,50,32,32,&pMeshApp,NULL);
	//トーラスメッシュの生成
	D3DXCreateTorus(g_p3DDev, 16, 40, 32, 32, &pMeshApp, NULL);
	//ライトを使用しない
	g_p3DDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	return  S_OK;
}

//描画環境の設定
void  SetupMatrices(void)
{
	RECT            rect;
	D3DXMATRIX      matView;
	D3DXMATRIX      matProj;
	D3DXMATRIX      matWorld;

	GetClientRect(g_hWnd, &rect);
	//ワールド座標の回転
	D3DXMatrixRotationY(&matWorld, timeGetTime() / 1000.0f);
	g_p3DDev->SetTransform(D3DTS_WORLD, &matWorld);
	//View 座標の設定
	D3DXMatrixLookAtLH(&matView, &ViewForm,
		&D3DXVECTOR3(0.0f, 0.0f, 0.0f), &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	g_p3DDev->SetTransform(D3DTS_VIEW, &matView);
	//透視変換の設定
	D3DXMatrixPerspectiveFovLH(&matProj, D3DXToRadian(45.0f), (float)rect.right / (float)rect.bottom, 100, 1000);
	g_p3DDev->SetTransform(D3DTS_PROJECTION, &matProj);
}

//描画処理
void Draw(void)
{
	//黒で塗りつぶして消去
	g_p3DDev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	if (SUCCEEDED(g_p3DDev->BeginScene()))
	{
		g_p3DDev->BeginScene();
		//描画環境の設定
		SetupMatrices();
		pMeshApp->DrawSubset(0);
		g_p3DDev->EndScene();
	}
	g_p3DDev->Present(NULL, NULL, NULL, NULL);
}

//オブジェクトの開放
void Cleanup(void)
{
	SAFE_RELEASE(pMeshApp);
	SAFE_DELETE(myd3d);
}

//CALLBACK Procedure
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0L;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

//★Win Main
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int)
{
	MSG     msg;

	WNDCLASSEX wc = { sizeof(WNDCLASSEX),CS_CLASSDC,WndProc,0L,0L,hInst,
		NULL,NULL,NULL,NULL,NAME,NULL };
	if (RegisterClassEx(&wc) == 0)    return FALSE;
	g_hWnd = CreateWindowEx(0, NAME, NAME, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInst, NULL);
	if (FAILED(InitDevices()))    return FALSE;

	ShowWindow(g_hWnd, SW_SHOWDEFAULT);
	UpdateWindow(g_hWnd);
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else    Draw();
	}
	Cleanup();
	UnregisterClass(NAME, wc.hInstance);
	return 0;
}

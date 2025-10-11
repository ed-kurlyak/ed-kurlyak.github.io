//==========================================================================
//	Ed Kurlyak 2023 Software Rendering
//==========================================================================

#ifndef _MESHMANAGER_
#define _MESHMANAGER_

#include <windows.h>
#include <windowsx.h>

#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include "d3dx12.h"
#include <comdef.h>

#include "d3dUtil.h"

#include "Timer.h"

#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

class CMeshManager
{
public:
	CMeshManager();
	~CMeshManager();

	void Init_MeshManager(HWND hWnd, int ClientWidth, int ClientHeight);
	void Update_MeshManager();
	void Draw_MeshManager();

private:
	void EnableDebugLayer_CreateFactory();
	void Create_Device();
	void CreateFence_GetDescriptorsSize();
	void Check_Multisample_Quality();
	void Create_CommandList_Allocator_Queue();
	void Create_SwapChain();
	void Create_RtvAndDsv_DescriptorHeaps();
	void Resize_SwapChainBuffers();
	void FlushCommandQueue();
	void Create_RenderTarget();
	void Create_DepthStencil_Buff_And_View();
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView();
	void Execute_Init_Commands();
	void Update_ViewPort_And_Scissor();
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView();
	ID3D12Resource* CurrentBackBuffer();
	void Draw_MeshManager2();
	
	CTimer m_Timer;

	unsigned char *m_BackBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap = nullptr;

	Microsoft::WRL::ComPtr<IDXGIFactory4> m_dxgiFactory = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Device> m_d3dDevice = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence = nullptr;

	UINT m_RtvDescriptorSize = 0;
	UINT m_DsvDescriptorSize = 0;
	UINT m_CbvSrvUavDescriptorSize = 0;

	DXGI_FORMAT m_BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	bool      m_4xMsaaState = false;
	UINT      m_4xMsaaQuality = 0;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_CommandQueue = nullptr;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_DirectCmdListAlloc = nullptr;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_CommandList = nullptr;

	Microsoft::WRL::ComPtr<IDXGISwapChain> m_SwapChain = nullptr;

	int m_ClientWidth = 800;
	int m_ClientHeight = 600;

	static const int m_SwapChainBufferCount = 2;

	HWND m_hWnd;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RtvHeap = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DsvHeap = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> m_SwapChainBuffer[m_SwapChainBufferCount];
	Microsoft::WRL::ComPtr<ID3D12Resource> m_DepthStencilBuffer = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> TexBackBuffer = nullptr;

	UINT64 m_CurrentFence = 0;
	int m_CurrBackBuffer = 0;

	DXGI_FORMAT m_DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	D3D12_VIEWPORT m_ScreenViewport;
	D3D12_RECT m_ScissorRect;
};

#endif
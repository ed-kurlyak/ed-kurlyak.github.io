//==========================================================================
//	Ed Kurlyak 2023 Software Rendering
//==========================================================================

#ifndef _MESHMANAGER_
#define _MESHMANAGER_

#include <windows.h>
#include <windowsx.h>
#include <math.h>
#include <stdio.h>

#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include "d3dx12.h"
#include <comdef.h>
#include <memory>
#include <unordered_map>
#include <array>
#include <DirectXMath.h>

#include "d3dUtil.h"

#include "Timer.h"

#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

#define PI 3.14159265358979f
#define PI2 (PI * 2.0f)

typedef float matrix4x4[4][4];

struct tex_coord2
{
	int tu, tv;
};

struct vector3
{
	vector3() {};
	vector3(float xi, float yi, float zi) : x(xi), y(yi), z(zi) {};

	union
	{
		float Vec[3];
		struct
		{
			float x, y, z;
		};
	};
};

struct VertexSAQ
{
	DirectX::XMFLOAT3 Pos;
};

struct SubmeshGeometry
{
	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	INT BaseVertexLocation = 0;
};

struct Texture
{
	std::string Name;

	std::wstring Filename;

	Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap = nullptr;
};

struct MeshGeometry
{
	std::string Name;

	std::unique_ptr<Texture> Textures;

	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferGPU = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferGPU = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferUploader = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferUploader = nullptr;

	UINT VertexByteStride = 0;
	UINT VertexBufferByteSize = 0;
	DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;
	UINT IndexBufferByteSize = 0;

	std::unordered_map<std::string, SubmeshGeometry> DrawArgs;

	D3D12_VERTEX_BUFFER_VIEW VertexBufferView()const
	{
		D3D12_VERTEX_BUFFER_VIEW vbv;
		vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
		vbv.StrideInBytes = VertexByteStride;
		vbv.SizeInBytes = VertexBufferByteSize;

		return vbv;
	}

	D3D12_INDEX_BUFFER_VIEW IndexBufferView()const
	{
		D3D12_INDEX_BUFFER_VIEW ibv;
		ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
		ibv.Format = IndexFormat;
		ibv.SizeInBytes = IndexBufferByteSize;

		return ibv;
	}

	void DisposeUploaders()
	{
		VertexBufferUploader = nullptr;
		IndexBufferUploader = nullptr;
	}
};

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

	void Create_ScreenAlignedQuad_Shaders_And_InputLayout();
	void Create_ScreenAlignedQuad_Geometry();
	void Create_PipelineStateObject();
	void Create_RootSignature();
	std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();
	void Create_ShaderRVHeap_And_View();
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTexture(
		ID3D12Device* device,
		ID3D12GraphicsCommandList* cmdList,
		const void* initData,
		UINT64 byteSize,
		Microsoft::WRL::ComPtr<ID3D12Resource>& UploadBuffer);
		
	void Draw_MeshManager2();
	
	CTimer m_Timer;
	
	unsigned char * m_BackBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap = nullptr;

	Microsoft::WRL::ComPtr<IDXGIFactory4> m_dxgiFactory;
	Microsoft::WRL::ComPtr<ID3D12Device> m_d3dDevice;
	Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence;

	UINT m_RtvDescriptorSize = 0;
	UINT m_DsvDescriptorSize = 0;
	UINT m_CbvSrvUavDescriptorSize = 0;

	DXGI_FORMAT m_BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	bool      m_4xMsaaState = false;
	UINT      m_4xMsaaQuality = 0;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_CommandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_DirectCmdListAlloc;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_CommandList;

	Microsoft::WRL::ComPtr<IDXGISwapChain> m_SwapChain;

	int m_ClientWidth = 800;
	int m_ClientHeight = 600;

	static const int m_SwapChainBufferCount = 2;

	HWND m_hWnd;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RtvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DsvHeap;

	Microsoft::WRL::ComPtr<ID3D12Resource> m_SwapChainBuffer[m_SwapChainBufferCount];
	Microsoft::WRL::ComPtr<ID3D12Resource> m_DepthStencilBuffer;

	UINT64 m_CurrentFence = 0;
	int m_CurrBackBuffer = 0;

	DXGI_FORMAT m_DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	D3D12_VIEWPORT m_ScreenViewport;
	D3D12_RECT m_ScissorRect;

	Microsoft::WRL::ComPtr<ID3DBlob> m_VsByteCodeSAQ = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> m_PsByteCodeSAQ = nullptr;

	std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputLayoutSAQ;

	std::unique_ptr<MeshGeometry> m_SQABuff = nullptr;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PSOSAQ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_SrvDescriptorHeapSAQ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature = nullptr;

};

#endif
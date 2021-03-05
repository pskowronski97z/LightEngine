#include <iostream>
#include <LECore.h>
#include <LEException.h>


LightEngine::Core::Core(HWND window_handle_) {

	const D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_11_1;
	DXGI_SWAP_CHAIN_DESC swap_chain_desc;

	swap_chain_desc.BufferDesc.Height = 0;
	swap_chain_desc.BufferDesc.Width = 0;
	swap_chain_desc.BufferDesc.RefreshRate.Denominator = 0;
	swap_chain_desc.BufferDesc.RefreshRate.Numerator = 0;
	swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
	swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	swap_chain_desc.SampleDesc.Count = 8;
	swap_chain_desc.SampleDesc.Quality = 0;
	
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.BufferCount = 1;
	swap_chain_desc.OutputWindow = window_handle_;
	swap_chain_desc.Windowed = true;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swap_chain_desc.Flags = 0;


	HRESULT hr = D3D11CreateDeviceAndSwapChain(
	nullptr, 
	D3D_DRIVER_TYPE_HARDWARE,
	nullptr,
	D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_DEBUG, 
	&feature_level,
	1,
	D3D11_SDK_VERSION,
	&swap_chain_desc,
	&swap_chain_ptr_,
	&device_ptr_,
	nullptr,
	&context_ptr_);
	
	if(FAILED(hr))
		throw LECoreException("<D3D11 ERROR> <Device and swap chain creation failed> ","LECore.cpp",__LINE__,hr);
	
	ID3D11Resource *back_buffer = nullptr;
	hr = swap_chain_ptr_->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&back_buffer));

	if(FAILED(hr))
		throw LECoreException("<D3D11 ERROR> <Cannot obtain an access to the back buffer> ","LECore.cpp",__LINE__,hr);
	
	hr = device_ptr_->CreateRenderTargetView(back_buffer, nullptr, &render_target_ptr_);

	if(FAILED(hr))
		throw LECoreException("<D3D11 ERROR> <Render target creation failed> ","LECore.cpp",__LINE__,hr);
	
	back_buffer->Release();
}

LightEngine::Core::~Core() {

	if (device_ptr_ != nullptr)
		device_ptr_->Release();

	if (context_ptr_ != nullptr)
		context_ptr_->Release();

	if (swap_chain_ptr_ != nullptr)
		swap_chain_ptr_->Release();

	if(device_ptr_ != nullptr)
		device_ptr_->Release();
}

void LightEngine::Core::clear_back_buffer(float r, float g, float b, float a) const {
	const float clear_color[]{r, g, b, a};
	context_ptr_->ClearRenderTargetView(render_target_ptr_,clear_color);
}

void LightEngine::Core::present_frame() const noexcept { swap_chain_ptr_->Present(1u, 0u); }

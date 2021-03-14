#include <iostream>
#include <LECore.h>
#include <LEException.h>

const D3D11_INPUT_ELEMENT_DESC LightEngine::Vertex3::vertex_desc_[3] = {
	{"VT3_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA},
	{"VT3_COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA},
	{"VT3_NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA}
};


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


	call_result_ = D3D11CreateDeviceAndSwapChain(
	nullptr, 
	D3D_DRIVER_TYPE_HARDWARE,
	nullptr,
	D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_DEBUG, 
	&feature_level,
	1,
	D3D11_SDK_VERSION,
	&swap_chain_desc,
	swap_chain_ptr_.GetAddressOf(),
	device_ptr_.GetAddressOf(),
	nullptr,
	context_ptr_.GetAddressOf());

	if (FAILED(call_result_))
		throw LECoreException("<D3D11 ERROR> <Device and swap chain creation failed> ", "LECore.cpp",__LINE__, call_result_);

	Microsoft::WRL::ComPtr<ID3D11Resource> back_buffer;

	call_result_ = swap_chain_ptr_->GetBuffer(0, __uuidof(ID3D11Resource), &back_buffer);

	if (FAILED(call_result_))
		throw LECoreException("<D3D11 ERROR> <Cannot obtain an access to the back buffer> ", "LECore.cpp",__LINE__, call_result_);

	call_result_ = device_ptr_->CreateRenderTargetView(back_buffer.Get(), nullptr, render_target_ptr_.GetAddressOf());

	if (FAILED(call_result_))
		throw LECoreException("<D3D11 ERROR> <Render target creation failed> ", "LECore.cpp",__LINE__, call_result_);
}

void LightEngine::Core::clear_back_buffer(float r, float g, float b, float a) const {
	const float clear_color[]{r, g, b, a};
	context_ptr_->ClearRenderTargetView(render_target_ptr_.Get(), clear_color);
}

void LightEngine::Core::present_frame() {
	call_result_ = swap_chain_ptr_->Present(1u, 0u);

	if (FAILED(call_result_))
		throw LECoreException("<D3D11 ERROR> <Frame rendering failed> ", "LECore.cpp",__LINE__, call_result_);
}

void LightEngine::Core::clear_back_buffer(float clear_color[4]) const {
	context_ptr_->ClearRenderTargetView(render_target_ptr_.Get(), clear_color);
}


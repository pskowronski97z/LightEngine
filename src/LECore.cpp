#include <iostream>
#include <LECore.h>
#include <LEException.h>
#include <d3dcompiler.h>


LightEngine::Core::Core(HWND window_handle_, int viewport_width, int viewport_height) {
	
	const D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_11_1;
	DXGI_SWAP_CHAIN_DESC swap_chain_desc;

	swap_chain_desc.BufferDesc.Height = 0;
	swap_chain_desc.BufferDesc.Width = 0;
	swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
	swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;
	swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_CENTERED;

	swap_chain_desc.SampleDesc.Count = 8;
	swap_chain_desc.SampleDesc.Quality = 0;
	
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.BufferCount = 1;
	swap_chain_desc.OutputWindow = window_handle_;
	swap_chain_desc.Windowed = true;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swap_chain_desc.Flags = 0;

	// Creating swap chain
	
	call_result_ = D3D11CreateDeviceAndSwapChain(
	nullptr, 
	D3D_DRIVER_TYPE_HARDWARE,
	nullptr,
	 D3D11_CREATE_DEVICE_DEBUG, 
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


	
	// Getting back buffer from swap chain
	
	Microsoft::WRL::ComPtr<ID3D11Resource> back_buffer;

	call_result_ = swap_chain_ptr_->GetBuffer(0, __uuidof(ID3D11Resource), &back_buffer);

	if (FAILED(call_result_))
		throw LECoreException("<D3D11 ERROR> <Cannot obtain an access to the back buffer> ", "LECore.cpp",__LINE__, call_result_);


	
	// Using back buffer to create render target to bind to the output-merger
	
	call_result_ = device_ptr_->CreateRenderTargetView(back_buffer.Get(), nullptr, render_target_ptr_.GetAddressOf());

	if (FAILED(call_result_))
		throw LECoreException("<D3D11 ERROR> <Render target creation failed> ", "LECore.cpp",__LINE__, call_result_);

	context_ptr_->OMSetRenderTargets(1,render_target_ptr_.GetAddressOf(),nullptr);



	
	// Loading shaders

	load_pixel_shader(L"B:\\source\\repos\\LightEngine\\bin\\Debug\\PixelShader.cso");
	load_pixel_shader(L"B:\\source\\repos\\LightEngine\\bin\\Debug\\PixelShader1.cso");
	load_vertex_shader(L"B:\\source\\repos\\LightEngine\\bin\\Debug\\VertexShader.cso");
	load_vertex_shader(L"B:\\source\\repos\\LightEngine\\bin\\Debug\\VertexShader1.cso");



	
	// Viewport set up
	
	D3D11_VIEWPORT viewport[]{{0,0,viewport_width,viewport_height,0,1}};
	
	context_ptr_->RSSetViewports(1,viewport);



	
}

void LightEngine::Core::clear_back_buffer(float r, float g, float b, float a) const {
	const float clear_color[]{r, g, b, a};
	context_ptr_->ClearRenderTargetView(render_target_ptr_.Get(), clear_color);
}

void LightEngine::Core::present_frame() {
	call_result_ = swap_chain_ptr_->Present(0u, 0u);

	if (FAILED(call_result_))
		throw LECoreException("<D3D11 ERROR> <Frame rendering failed> ", "LECore.cpp",__LINE__, call_result_);
}

void LightEngine::Core::vertex_buffer_setup(Vertex3 *vertex_buffer, int buffer_size) {

	D3D11_BUFFER_DESC buffer_desc;
	D3D11_SUBRESOURCE_DATA sr_data;
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	
	buffer_desc.ByteWidth = sizeof(Vertex3)*buffer_size;
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = sizeof(Vertex3);

	sr_data.pSysMem = vertex_buffer;

	call_result_ = device_ptr_->CreateBuffer(&buffer_desc, &sr_data, &buffer);

	if(FAILED(call_result_))
		throw LECoreException("<D3D11 ERROR> <Vertex buffer creation failed> ", "LECore.cpp",__LINE__, call_result_);

	UINT stride = sizeof(Vertex3);
	UINT offset = 0;
	
	context_ptr_->IASetVertexBuffers(0, 1, buffer.GetAddressOf(), &stride, &offset);
}

void LightEngine::Core::draw_setup() {
	
	//context_ptr_->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	context_ptr_->VSSetShader(vertex_shader_ptrs_.at(0).Get(), nullptr, 0);
	context_ptr_->PSSetShader(pixel_shader_ptrs_.at(0).Get(), nullptr, 0);
	context_ptr_->IASetInputLayout(input_layout_ptrs_.at(0).Get());
}

void LightEngine::Core::draw_to_back_buffer() const { context_ptr_->Draw(3, 0); }

void LightEngine::Core::load_vertex_shader(std::wstring path) {
	
	c_vertex_shader_ptrs_.emplace_back(Microsoft::WRL::ComPtr<ID3DBlob>());
	int end_index = c_vertex_shader_ptrs_.size()-1;
	
	call_result_ = D3DReadFileToBlob(path.c_str(), &c_vertex_shader_ptrs_.at(end_index));

	if (FAILED(call_result_))
		throw LECoreException("<D3D11 ERROR> <Cannot load compiled vertex shader> ", "LECore.cpp",__LINE__, call_result_);

	vertex_shader_ptrs_.emplace_back(Microsoft::WRL::ComPtr<ID3D11VertexShader>());
	
	call_result_ = device_ptr_->CreateVertexShader(
		c_vertex_shader_ptrs_.at(end_index)->GetBufferPointer(),
		c_vertex_shader_ptrs_.at(end_index)->GetBufferSize(),
		nullptr,
		&vertex_shader_ptrs_[end_index]);

	if (FAILED(call_result_))
		throw LECoreException("<D3D11 ERROR> <Vertex shader initialization fail> ", "LECore.cpp",__LINE__, call_result_);

	input_layout_ptrs_.emplace_back(Microsoft::WRL::ComPtr<ID3D11InputLayout>());
	
	call_result_ = device_ptr_->CreateInputLayout(
		Vertex3::vertex_desc_,
		3,
		c_vertex_shader_ptrs_.at(end_index)->GetBufferPointer(),
		c_vertex_shader_ptrs_.at(end_index)->GetBufferSize(),
		&input_layout_ptrs_.at(end_index));

	if (FAILED(call_result_))
		throw LECoreException("<D3D11 ERROR> <Input layout initialization fail> ", "LECore.cpp",__LINE__, call_result_);
}

void LightEngine::Core::load_pixel_shader(std::wstring path) {

	pixel_shader_ptrs_.emplace_back(Microsoft::WRL::ComPtr<ID3D11PixelShader>());
	int end_index = pixel_shader_ptrs_.size()-1;

	Microsoft::WRL::ComPtr<ID3DBlob> c_pixel_shader;
	call_result_ = D3DReadFileToBlob(path.c_str(), &c_pixel_shader);

	if (FAILED(call_result_))
		throw LECoreException("<D3D11 ERROR> <Cannot load compiled pixel shader> ", "LECore.cpp",__LINE__, call_result_);

	call_result_ = device_ptr_->CreatePixelShader(
		c_pixel_shader->GetBufferPointer(),
		c_pixel_shader->GetBufferSize(),
		nullptr,
		&pixel_shader_ptrs_.at(end_index));

	if (FAILED(call_result_))
		throw LECoreException("<D3D11 ERROR> <Pixel shader initialization fail> ", "LECore.cpp",__LINE__, call_result_);

}

Microsoft::WRL::ComPtr<ID3D11Device> LightEngine::Core::get_device_ptr_() {
	return device_ptr_;
}

Microsoft::WRL::ComPtr<ID3D11DeviceContext> LightEngine::Core::get_context_ptr_() {
	return context_ptr_;
}

void LightEngine::Core::clear_back_buffer(float clear_color[4]) const {
	context_ptr_->ClearRenderTargetView(render_target_ptr_.Get(), clear_color);
}


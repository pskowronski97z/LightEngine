#include <LEGeometry.h>
#include <LEException.h>
#include <d3d11.h>
#define M_PI 3.14159265358979323846

template <class T>
LightEngine::Geometry<T>::Geometry(std::shared_ptr<Core> core_ptr, std::vector<T> vertices, D3D11_PRIMITIVE_TOPOLOGY topology)
	: core_ptr_(core_ptr), vertices_vector_(vertices), primitive_topology_(topology) {

	indices_vector_.resize(0);

	D3D11_BUFFER_DESC buffer_descriptor_;
	D3D11_SUBRESOURCE_DATA sr_data;
	
	buffer_descriptor_.ByteWidth = sizeof(T)*vertices_vector_.size();
	buffer_descriptor_.Usage = D3D11_USAGE_DEFAULT;
	buffer_descriptor_.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffer_descriptor_.CPUAccessFlags = 0;
	buffer_descriptor_.MiscFlags = 0;
	buffer_descriptor_.StructureByteStride = sizeof(T);

	sr_data.pSysMem = vertices_vector_.data();

	call_result_ = core_ptr_->device_ptr_->CreateBuffer(&buffer_descriptor_, &sr_data, &vertex_buffer_);

	if(FAILED(call_result_))
		throw LECoreException("<D3D11 ERROR> <Vertex buffer creation failed> ", "LEGeometry.cpp",__LINE__, call_result_);
}

template<class T>
void LightEngine::Geometry<T>::set_indices(std::vector<unsigned int> indices) {

	indices_vector_ = indices;

	D3D11_BUFFER_DESC buffer_descriptor_;
	D3D11_SUBRESOURCE_DATA sr_data;
	
	buffer_descriptor_.ByteWidth = sizeof(unsigned int)*indices_vector_.size();
	buffer_descriptor_.Usage = D3D11_USAGE_DEFAULT;
	buffer_descriptor_.BindFlags = D3D11_BIND_INDEX_BUFFER;
	buffer_descriptor_.CPUAccessFlags = 0;
	buffer_descriptor_.MiscFlags = 0;
	buffer_descriptor_.StructureByteStride = sizeof(unsigned int);

	sr_data.pSysMem = indices_vector_.data();
	sr_data.SysMemPitch = 0;
	sr_data.SysMemSlicePitch = 0;
	
	call_result_ = core_ptr_->device_ptr_->CreateBuffer(&buffer_descriptor_, &sr_data, &index_buffer_);

	if(FAILED(call_result_))
		throw LECoreException("<D3D11 ERROR> <Index buffer creation failed> ", "LEGeometry.cpp",__LINE__, call_result_);
}

template<class T>
void LightEngine::Geometry<T>::bind() {
	UINT stride = sizeof(T);
	UINT offset = 0;
	core_ptr_->context_ptr_->IASetVertexBuffers(0, 1, vertex_buffer_.GetAddressOf(), &stride, &offset);
	if(indices_vector_.size())
		core_ptr_->context_ptr_->IASetIndexBuffer(index_buffer_.Get(), DXGI_FORMAT_R32_UINT,0);
	core_ptr_->context_ptr_->IASetPrimitiveTopology(primitive_topology_);
}

template<class T>
void LightEngine::Geometry<T>::draw(int start_loc) {
	if(indices_vector_.size())
		core_ptr_->context_ptr_->DrawIndexed(indices_vector_.size(),start_loc,0);
	else
		core_ptr_->context_ptr_->Draw(vertices_vector_.size(),start_loc);
}


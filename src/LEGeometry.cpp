#include <LEGeometry.h>
#include <LEException.h>
#include <d3d11.h>
#define M_PI 3.14159265358979323846

template <class T>
LightEngine::Geometry<T>::Geometry(std::shared_ptr<Core> core_ptr, std::vector<T> vertices, D3D11_PRIMITIVE_TOPOLOGY topology)
	: core_ptr_(core_ptr), vertices_vector_(vertices), primitive_topology_(topology) {

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
void LightEngine::Geometry<T>::bind() {
	UINT stride = sizeof(T);
	UINT offset = 0;
	core_ptr_->context_ptr_->IASetVertexBuffers(0, 1, vertex_buffer_.GetAddressOf(), &stride, &offset);
	core_ptr_->context_ptr_->IASetPrimitiveTopology(primitive_topology_);
}

LightEngine::Camera::Camera(std::shared_ptr<Core> core_ptr) : core_ptr_(core_ptr) {

	D3D11_BUFFER_DESC buffer_desc;
	D3D11_SUBRESOURCE_DATA sr_data;

	set_fov(60.0);
	set_clipping(1.0,10.0);

	transform_matrices_.camera_matrix_ = DirectX::XMMatrixSet(
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1);
	
	buffer_desc.ByteWidth = sizeof(transform_matrices_);
	buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = sizeof(float);

	sr_data.pSysMem = &transform_matrices_;

	call_result_ = core_ptr_->device_ptr_->CreateBuffer(&buffer_desc, &sr_data, &camera_cbuffer_ptr_);

	if(FAILED(call_result_))
		throw LECoreException("<D3D11 ERROR> <Camera constant buffer creation failed> ", "LEGeometry.cpp",__LINE__, call_result_);
}

void LightEngine::Camera::set_active() {
	// Binding buffer to b0 register on GPU
	core_ptr_->context_ptr_->VSSetConstantBuffers(0,1,camera_cbuffer_ptr_.GetAddressOf());
}

void LightEngine::Camera::update() {
	
	D3D11_MAPPED_SUBRESOURCE new_cbuffer;
	ZeroMemory(&new_cbuffer,sizeof(new_cbuffer));

	call_result_ = core_ptr_->context_ptr_->Map(camera_cbuffer_ptr_.Get(),0,D3D11_MAP_WRITE_DISCARD,0,&new_cbuffer);

	if(FAILED(call_result_))
		throw LECoreException("<Constant buffer mapping failed> ", "LEGeometry.cpp",__LINE__, call_result_);
	
	memcpy(new_cbuffer.pData,&transform_matrices_,sizeof(transform_matrices_));

	core_ptr_->context_ptr_->Unmap(camera_cbuffer_ptr_.Get(),0);
}

void LightEngine::Camera::rotate_x(float angle) {
	transform_matrices_.camera_matrix_*= DirectX::XMMatrixRotationX(angle*M_PI/180.0f);
}

void LightEngine::Camera::rotate_y(float angle) {
	transform_matrices_.camera_matrix_*= DirectX::XMMatrixRotationY(angle*M_PI/180.0f);
}

void LightEngine::Camera::rotate_z(float angle) {
	transform_matrices_.camera_matrix_*= DirectX::XMMatrixRotationZ(angle*M_PI/180.0f);
}

void LightEngine::Camera::move(float vector[3]) {
	transform_matrices_.camera_matrix_+=DirectX::XMMatrixSet(
		0,0,0,0,
		0,0,0,0,
		0,0,0,0,
		vector[0],vector[1],vector[2],0);
}

void LightEngine::Camera::set_clipping(float near_z, float far_z) {
	a_factor_ = 1 / (far_z - near_z);
	b_factor_ = -near_z / (far_z - near_z);

	transform_matrices_.projection_matrix_ = DirectX::XMMatrixSet(
		fov_,0,0,0,
		0,fov_,0,0,
		0,0,a_factor_,1,
		0,0,b_factor_,0);
}

void LightEngine::Camera::set_fov(float angle) {
	fov_ = 1.0/tan(angle*M_PI/360.0);

	transform_matrices_.projection_matrix_ = DirectX::XMMatrixSet(
		fov_,0,0,0,
		0,fov_,0,0,
		0,0,a_factor_,1,
		0,0,b_factor_,0);
}


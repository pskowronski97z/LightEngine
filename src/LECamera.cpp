#include <LECamera.h>
#include <LEException.h>

#define M_PI 3.14159265358979323846

LightEngine::Camera::Camera(std::shared_ptr<Core> core_ptr) : core_ptr_(core_ptr) {

	D3D11_BUFFER_DESC buffer_desc;
	D3D11_SUBRESOURCE_DATA sr_data;

	set_fov(45);
	set_clipping(0.0,10.0);
	set_scaling(16,9);

	reset_position();
	update_projection();
	
	buffer_desc.ByteWidth = sizeof(transform_matrices_);
	buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = sizeof(float);

	sr_data.pSysMem = &transform_matrices_;

	call_result_ = core_ptr_->device_ptr_->CreateBuffer(&buffer_desc, &sr_data, &constant_buffer_ptr_);

	if(FAILED(call_result_))
		throw LECoreException("<D3D11 ERROR> <Camera constant buffer creation failed> ", "LECamera.cpp",__LINE__, call_result_);
	
}

void LightEngine::Camera::set_fov(float angle) {
	temp_projection_matrix_[0][0] = 1.0/tan(angle*M_PI/360.0);
	temp_projection_matrix_[1][1] = 1.0/tan(angle*M_PI/360.0);
}

void LightEngine::Camera::set_clipping(float near_z, float far_z) {
	temp_projection_matrix_[2][2] = 1.0f / (far_z - near_z);
	temp_projection_matrix_[3][2] = -near_z / (far_z - near_z);
}

void LightEngine::Camera::set_scaling(short width, short height) { scaling_ = static_cast<float>(height) / static_cast<float>(width); }

void LightEngine::Camera::update_projection() {
	transform_matrices_.projection_matrix = DirectX::XMMatrixSet(
		temp_projection_matrix_[0][0]*scaling_, temp_projection_matrix_[0][1], temp_projection_matrix_[0][2], temp_projection_matrix_[0][3],
		temp_projection_matrix_[1][0], temp_projection_matrix_[1][1], temp_projection_matrix_[1][2], temp_projection_matrix_[1][3],
		temp_projection_matrix_[2][0], temp_projection_matrix_[2][1], temp_projection_matrix_[2][2], temp_projection_matrix_[2][3],
		temp_projection_matrix_[3][0], temp_projection_matrix_[3][1], temp_projection_matrix_[3][2], temp_projection_matrix_[3][3]
	);
}

void LightEngine::Camera::set_active(short slot) {
	core_ptr_->context_ptr_->VSSetConstantBuffers(slot,1,constant_buffer_ptr_.GetAddressOf());
}

void LightEngine::Camera::reset_position() {
	transform_matrices_.camera_matrix = DirectX::XMMatrixSet(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	);
}

void LightEngine::Camera::update() {

	update_projection();
	
	D3D11_MAPPED_SUBRESOURCE new_constant_buffer;
	ZeroMemory(&new_constant_buffer,sizeof(new_constant_buffer));

	call_result_ = core_ptr_->context_ptr_->Map(constant_buffer_ptr_.Get(),0,D3D11_MAP_WRITE_DISCARD,0,&new_constant_buffer);

	if(FAILED(call_result_))
		throw LECoreException("<Constant buffer mapping failed> ", "LEGeometry.cpp",__LINE__, call_result_);
	
	memcpy(new_constant_buffer.pData,&transform_matrices_,sizeof(transform_matrices_));

	core_ptr_->context_ptr_->Unmap(constant_buffer_ptr_.Get(),0);
}

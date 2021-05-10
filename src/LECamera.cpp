#include <iostream>
#include <LECamera.h>
#include <LEException.h>
#include <ostream>

#define M_PI 3.14159265358979323846

LightEngine::Camera::Camera(std::shared_ptr<Core> core_ptr) : core_ptr_(core_ptr) {

	D3D11_BUFFER_DESC buffer_desc;
	D3D11_SUBRESOURCE_DATA sr_data;

	set_fov(45);
	set_clipping(0.0,10.0);
	set_scaling(16,9);

	reset();

	update_projection();
	
	buffer_desc.ByteWidth = sizeof(transform_matrices_);
	buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = sizeof(float);

	TransformMatrices transposed_matrices;

	transposed_matrices.camera_matrix = XMMatrixTranspose(transform_matrices_.camera_matrix);
	transposed_matrices.projection_matrix = XMMatrixTranspose(transform_matrices_.projection_matrix);
	
	sr_data.pSysMem = &transposed_matrices;

	call_result_ = core_ptr_->device_ptr_->CreateBuffer(&buffer_desc, &sr_data, &constant_buffer_ptr_);

	if(FAILED(call_result_))
		throw LECoreException("<D3D11 ERROR> <Camera constant buffer creation failed> ", "LECamera.cpp",__LINE__, call_result_);
	
}

void LightEngine::Camera::set_fov(float angle) {
	temp_projection_matrix_[0][0] = 1.0/tan(angle*M_PI/360.0);
	temp_projection_matrix_[1][1] = 1.0/tan(angle*M_PI/360.0);
	need_projection_update = true;
}

void LightEngine::Camera::set_clipping(float near_z, float far_z) {
	temp_projection_matrix_[2][2] = 1.0f / (far_z - near_z);
	temp_projection_matrix_[3][2] = -near_z / (far_z - near_z);
	need_projection_update = true;
}

void LightEngine::Camera::set_scaling(short width, short height) {
	scaling_ = static_cast<float>(height) / static_cast<float>(width);
	need_projection_update = true;
}

void LightEngine::Camera::update_preprocess() {
	if(need_projection_update) 
		update_projection();
}

void LightEngine::Camera::update_projection() {
	transform_matrices_.projection_matrix = DirectX::XMMatrixSet(
		temp_projection_matrix_[0][0]*scaling_, temp_projection_matrix_[0][1], temp_projection_matrix_[0][2], temp_projection_matrix_[0][3],
		temp_projection_matrix_[1][0], temp_projection_matrix_[1][1], temp_projection_matrix_[1][2], temp_projection_matrix_[1][3],
		temp_projection_matrix_[2][0], temp_projection_matrix_[2][1], temp_projection_matrix_[2][2], temp_projection_matrix_[2][3],
		temp_projection_matrix_[3][0], temp_projection_matrix_[3][1], temp_projection_matrix_[3][2], temp_projection_matrix_[3][3]
	);
	need_projection_update = false;
}

void LightEngine::Camera::set_active(short slot) {
	core_ptr_->context_ptr_->VSSetConstantBuffers(slot,1,constant_buffer_ptr_.GetAddressOf());
}

void LightEngine::Camera::update() {

	update_preprocess();

	TransformMatrices transposed_matrices;

	transposed_matrices.camera_matrix = XMMatrixTranspose(transform_matrices_.camera_matrix);
	transposed_matrices.projection_matrix = XMMatrixTranspose(transform_matrices_.projection_matrix);
	
	D3D11_MAPPED_SUBRESOURCE new_constant_buffer;
	ZeroMemory(&new_constant_buffer,sizeof(new_constant_buffer));

	call_result_ = core_ptr_->context_ptr_->Map(constant_buffer_ptr_.Get(),0,D3D11_MAP_WRITE_DISCARD,0,&new_constant_buffer);

	if(FAILED(call_result_))
		throw LECoreException("<Constant buffer mapping failed> ", "LECamera.cpp",__LINE__, call_result_);
	
	memcpy(new_constant_buffer.pData,&transposed_matrices,sizeof(transposed_matrices));

	core_ptr_->context_ptr_->Unmap(constant_buffer_ptr_.Get(),0);
}

void LightEngine::Camera::reset() {
	transform_matrices_.camera_matrix = DirectX::XMMatrixSet(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	);
}


LightEngine::WorldCamera::WorldCamera(std::shared_ptr<Core> core_ptr) : Camera(core_ptr) {}

void LightEngine::WorldCamera::rotate_x(float angle) {
	transform_matrices_.camera_matrix*= DirectX::XMMatrixRotationX(angle*M_PI/180.0f);
}

void LightEngine::WorldCamera::rotate_y(float angle) {
	transform_matrices_.camera_matrix *= DirectX::XMMatrixRotationY(angle * M_PI / 180.0f);
}

void LightEngine::WorldCamera::rotate_z(float angle) {
	transform_matrices_.camera_matrix*= DirectX::XMMatrixRotationZ(angle*M_PI/180.0f);
}

void LightEngine::WorldCamera::move(float vector[3]) {
	transform_matrices_.camera_matrix += DirectX::XMMatrixSet(
		0,0,0,0,
		0,0,0,0,
		0,0,0,0,
		vector[0],vector[1],vector[2],0);
}


LightEngine::OrbitCamera::OrbitCamera(std::shared_ptr<Core> core_ptr) : Camera(core_ptr) {update_position(); update();}

void LightEngine::OrbitCamera::set_center(float center[3]) {
	center_[0]=center[0];
	center_[1]=center[1];
	center_[2]=center[2];
	update_position();
}

void LightEngine::OrbitCamera::adjust_azimuth(float azimuth_delta) {

	if(azimuth_delta==0.0)
		return;

	azimuth_ += azimuth_delta;

	if(azimuth_ == 360 || azimuth_ == -360)
		azimuth_=0;

	if(azimuth_delta > 0) {
		transform_matrices_.camera_matrix *= DirectX::XMMatrixRotationY((azimuth_delta*cos(elevation_*M_PI/180.0f))*M_PI/180.0f);
		transform_matrices_.camera_matrix *= DirectX::XMMatrixRotationZ(-(azimuth_delta*sin(elevation_*M_PI/180.0f))*M_PI/180.0f);
	}
	else {
		transform_matrices_.camera_matrix *= DirectX::XMMatrixRotationZ(-(azimuth_delta*sin(elevation_*M_PI/180.0f))*M_PI/180.0f);
		transform_matrices_.camera_matrix *= DirectX::XMMatrixRotationY((azimuth_delta*cos(elevation_*M_PI/180.0f))*M_PI/180.0f);
	}

	update_position();
}

void LightEngine::OrbitCamera::adjust_elevation(float elevation_delta) {

	if(elevation_delta==0.0)
		return;

	if(elevation_ == 360 || elevation_ == -360)
		elevation_=0;

	elevation_ += elevation_delta;

	transform_matrices_.camera_matrix *= DirectX::XMMatrixRotationX(-elevation_delta*M_PI/180.0f);

	update_position();
	
}

void LightEngine::OrbitCamera::adjust_radius(float radius_delta) {

	if(radius_ + radius_delta <= 0 || radius_delta==0.0)
		return;

	radius_ += radius_delta;
	
	update_position();
}

void LightEngine::OrbitCamera::reset() {
	
	Camera::reset();

	elevation_=0;
	azimuth_=0;
	radius_=2;

	update_position();
	
}

float LightEngine::OrbitCamera::get_azimuth() { return azimuth_; }

float LightEngine::OrbitCamera::get_elevation() { return elevation_; }

float LightEngine::OrbitCamera::get_radius() { return radius_; }

void LightEngine::OrbitCamera::get_center(float buffer[3]) {
	buffer[0]=center_[0];
	buffer[1]=center_[1];
	buffer[2]=center_[2];
}

void LightEngine::OrbitCamera::update_position() {

	float new_position[4]{0};

	new_position[0] = -radius_*cos(elevation_*M_PI/180.0f)*sin(azimuth_*M_PI/180.0f);
	new_position[1] = -radius_*sin(elevation_*M_PI/180.0f);
	new_position[2] = radius_*cos(elevation_*M_PI/180.0f)*cos(azimuth_*M_PI/180.0f);
	
	DirectX::XMMATRIX new_position_in_c_space = DirectX::XMMatrixSet(
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		new_position[0] - center_[0], new_position[1] - center_[1], new_position[2] - center_[2], 0)*transform_matrices_.camera_matrix;

	transform_matrices_.camera_matrix = DirectX::XMMatrixSet(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 0) * transform_matrices_.camera_matrix;
	
	transform_matrices_.camera_matrix += new_position_in_c_space;
}


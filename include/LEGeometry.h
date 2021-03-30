#pragma once
#include <LECore.h>
#include <memory>

namespace LightEngine {

	struct TransformMatrices {
		DirectX::XMMATRIX camera_matrix_;
		DirectX::XMMATRIX projection_matrix_;
	};
	
	class __declspec(dllexport) Camera {
	private:
		std::shared_ptr<Core> core_ptr_;
		TransformMatrices transform_matrices_;
		Microsoft::WRL::ComPtr<ID3D11Buffer> camera_cbuffer_ptr_;
		HRESULT call_result_;
		float fov_ = 0;
		float a_factor_ = 0;
		float b_factor_ = 0;

	public:
		Camera(std::shared_ptr<Core> core_ptr);
		void set_active();
		void update();
		void rotate_x(float angle);
		void rotate_y(float angle);
		void rotate_z(float angle);
		void move(float vector[3]);
		void set_clipping(float near_z, float far_z);
		void set_fov(float angle);
	};
	
	template <class T> 
	class Geometry {
	private:
		std::shared_ptr<Core> core_ptr_;
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer_;
		Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer_;
		D3D11_BUFFER_DESC buffer_descriptor_;
		D3D11_PRIMITIVE_TOPOLOGY primitive_topology_;
		std::vector<T> vertices_vector_;
		std::vector<int> indices_vector_;
		HRESULT call_result_;
	public:
		Geometry(std::shared_ptr<Core> core_ptr, std::vector<T> vertices, D3D11_PRIMITIVE_TOPOLOGY topology);
		void set_indices(std::vector<int> indices);
		void bind();
	};

	template class __declspec(dllexport) Geometry<Vertex3>;
}

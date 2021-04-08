#pragma once
#include <LECore.h>
#include <memory>

namespace LightEngine {

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
		//TODO: Indexed vertices handling
		void set_indices(std::vector<int> indices);
		void bind();
	};

	template class __declspec(dllexport) Geometry<Vertex3>;
}

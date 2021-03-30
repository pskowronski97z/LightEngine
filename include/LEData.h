#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

namespace LightEngine {

	struct __declspec(dllexport) Vertex3 {
		DirectX::XMFLOAT3 position_;
		DirectX::XMFLOAT4 color_;
		DirectX::XMFLOAT3 normal_;
		static const D3D11_INPUT_ELEMENT_DESC vertex_desc_[3];
	};

	struct ViewBuffer {
		float view_matrix[4][4];
		
	};
	
}

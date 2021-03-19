#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>
#include <vector>

namespace LightEngine {

	struct __declspec(dllexport) Vertex3 {
		DirectX::XMFLOAT3 position_;
		DirectX::XMFLOAT4 color_;
		DirectX::XMFLOAT3 normal_;
		static const D3D11_INPUT_ELEMENT_DESC vertex_desc_[3];
	};

	
	class __declspec(dllexport) Core {
	private:
		Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain_ptr_;
		Microsoft::WRL::ComPtr<ID3D11Device> device_ptr_;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> context_ptr_;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_ptr_;
		
		std::vector<Microsoft::WRL::ComPtr<ID3D11VertexShader>> vertex_shader_ptrs_;
		std::vector<Microsoft::WRL::ComPtr<ID3DBlob>> c_vertex_shader_ptrs_;
		std::vector<Microsoft::WRL::ComPtr<ID3D11InputLayout>> input_layout_ptrs_;
		std::vector<Microsoft::WRL::ComPtr<ID3D11PixelShader>> pixel_shader_ptrs_;
		
		HRESULT call_result_;
		
	public:
		Core(HWND window_handle_);
		Core(const Core&) = delete;	// deleting copy constructor (optional)
		Core& operator=(const Core&) = delete; // deleting assignment operator (optional)
		void clear_back_buffer(float r, float g, float b, float a) const;
		void clear_back_buffer(float clear_color[4]) const;
		void present_frame();
		void set_up_vertex_buffer(Vertex3 *vertex_buffer, int buffer_size);
		void draw();
		void load_vertex_shader(std::wstring path);
		void load_pixel_shader(std::wstring path);
	};

}

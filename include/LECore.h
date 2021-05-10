#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <wrl.h>
#include <vector>
#include <LEData.h>
#include <string>

namespace LightEngine {

	template <class T> class Geometry;
	class Camera;
	struct TransformMatrices;
	
	class __declspec(dllexport) Core {
		friend class Geometry<Vertex3>;
		friend class Camera;
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
		Core(HWND window_handle_, int viewport_width, int viewport_height);
		Core(const Core&) = delete;	// deleting copy constructor (optional)
		Core& operator=(const Core&) = delete; // deleting assignment operator (optional)
		void clear_back_buffer(float r, float g, float b, float a) const;
		void clear_back_buffer(float clear_color[4]) const;
		void present_frame();
		void vertex_buffer_setup(Vertex3 *vertex_buffer, int buffer_size);
		void draw_setup();
		//void draw_to_back_buffer(int vertex_count) const;
		void load_vertex_shader(std::wstring path);
		void load_pixel_shader(std::wstring path);
		Microsoft::WRL::ComPtr<ID3D11Device> get_device_ptr_();
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> get_context_ptr_();
	};

}

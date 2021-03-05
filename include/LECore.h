#pragma once
#include <Windows.h>
#include <d3d11.h>

namespace LightEngine {

	class __declspec(dllexport) Core {
	private:
		IDXGISwapChain *swap_chain_ptr_ = nullptr;
		ID3D11Device *device_ptr_ = nullptr;
		ID3D11DeviceContext *context_ptr_ = nullptr;
		ID3D11RenderTargetView *render_target_ptr_ = nullptr;
	public:
		Core(HWND window_handle_);
		~Core();
		Core(const Core&) = delete;	// deleting copy constructor (optional)
		Core& operator=(const Core&) = delete; // deleting assignment operator (optional)
		void clear_back_buffer(float r, float g, float b, float a) const;
		void present_frame() const noexcept;
	};

}

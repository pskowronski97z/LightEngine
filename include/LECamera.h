#pragma once
#include <DirectXMath.h>
#include <LECore.h>
#include <memory>

namespace LightEngine {

	class __declspec(dllexport) Camera {
	public:
		/// <summary>
		/// Passing camera and projection matrices as constant buffer to GPU's r<slot> register. 
		/// </summary>
		/// <param name="slot">Number of the register.</param>
		void set_active(short slot);
		void update();
		void set_clipping(float near_z, float far_z);
		void set_fov(float angle);
		void set_scaling(short width, short height);
		void reset_position();
		
	protected:
		bool need_projection_update = false;
		struct TransformMatrices {
			DirectX::XMMATRIX camera_matrix;
			DirectX::XMMATRIX projection_matrix;
		};
		TransformMatrices transform_matrices_;
		HRESULT call_result_;
		std::shared_ptr<Core> core_ptr_;
		Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer_ptr_;
		float temp_projection_matrix_[4][4] {
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 1,
			0, 0, 1, 0
		};
		float scaling_ = 1;
		
		void update_projection();
		Camera(std::shared_ptr<Core> core_ptr);
	};

	class __declspec(dllexport) WorldCamera : public Camera {
	public:
		WorldCamera(std::shared_ptr<Core> core_ptr);
		void rotate_x(float angle);
		void rotate_y(float angle);
		void rotate_z(float angle);
		void move(float vector[3]);
	};

	class __declspec(dllexport) OrbitCamera : Camera {
		
	};

	class __declspec(dllexport) FPSCamera : Camera {
		
	};
	
}

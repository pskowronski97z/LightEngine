#pragma once
#include <DirectXMath.h>
#include <LECore.h>
#include <memory>

namespace LightEngine {

	//TODO: fix virtual destructor, copy constructor, etc.
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
		virtual void reset();
		
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

		virtual void update_preprocess();
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

	class __declspec(dllexport) OrbitCamera : public Camera {		
	public:
		OrbitCamera(std::shared_ptr<Core> core_ptr);
		void set_center(float center[3]);
		void adjust_azimuth(float azimuth_delta);
		void adjust_elevation(float elevation_delta);
		void adjust_radius(float radius_delta);
		void reset() override;
		float get_azimuth();
		float get_elevation();
		float get_radius();
		void get_center(float buffer[3]);
		
	private:
		// angles in degrees
		float azimuth_ = 0;
		float elevation_ = 0;
		float radius_ = 1;
		float center_[3]{1,0,0};
		void update_position();
	};
	
}

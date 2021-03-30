struct VS_INPUT {
    float3 position : VT3_POSITION; // Dane wejœciowe o formacie zadanym w deskryptorze D3D11_INPUT_ELEMENT_DESC
    float4 color : VT3_COLOR;
    float3 normal : VT3_NORMAL;
};

cbuffer CAMERA : register(b0) {
    row_major matrix camera_matrix_;
    row_major matrix projection_matrix_;
}

struct PS_INPUT {
    float4 position : SV_POSITION;  // Dane wyjœciowe, musi siê znajdowaæ wœród nich wektor 4d oznaczony jako SV_POSITION (konwertuje interpolowane pozycje do przestrzeni ekranu)
    float4 color : VT3_COLOR;
    float3 normal : VT3_NORMAL;
};

PS_INPUT main(VS_INPUT input) {
	
    PS_INPUT result;
	
    result.position = mul(float4(input.position, 1.0f), camera_matrix_);
    result.position = mul(result.position, projection_matrix_);
	
    //result.position = float4(input.position[0] /*+ move_vector[0]*/, input.position[1] /*+ move_vector[1]*/, input.position[2] /*+ move_vector[2]*/, 1.0f);
    result.color = input.color;
    result.normal = input.normal;

    return result;
}
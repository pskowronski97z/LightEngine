struct VS_INPUT {
    float3 position : VT3_POSITION; // Dane wej�ciowe o formacie zadanym w deskryptorze D3D11_INPUT_ELEMENT_DESC
    float4 color : VT3_COLOR;
    float3 normal : VT3_NORMAL;
};

cbuffer CAMERA : register(b0) {
    matrix camera_matrix_;
    matrix projection_matrix_;
}

struct PS_INPUT {
    float4 position : SV_POSITION;  // Dane wyj�ciowe, musi si� znajdowa� w�r�d nich wektor 4d oznaczony jako SV_POSITION (konwertuje interpolowane pozycje do przestrzeni ekranu)
    float4 color : VT3_COLOR;
    float3 normal : VT3_NORMAL;
};

PS_INPUT main(VS_INPUT input) {
	
    PS_INPUT result;
	
    result.position = mul(float4(input.position, 1.0f), camera_matrix_);
    result.position = mul(result.position, projection_matrix_);
    result.color = input.color;
    result.normal = input.normal;

    return result;
}
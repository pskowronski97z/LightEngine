struct VS_INPUT
{
    float3 position : VT3_POSITION;
    float4 color : VT3_COLOR;
    float3 normal : VT3_NORMAL;
};

struct PS_INPUT {
    float4 position : SV_POSITION;
    float4 color : VT3_COLOR;
    float3 normal : VT3_NORMAL;
};

PS_INPUT main(VS_INPUT input) 
{
    PS_INPUT result;

    result.position = float4(input.position[0], input.position[1], input.position[2], 1.0f);
    result.color = input.color;
    result.normal = input.normal;

    return result;
}
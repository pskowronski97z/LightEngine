struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 color : VT3_COLOR;
    float3 normal : VT3_NORMAL;
};


float4 main(PS_INPUT input) : SV_TARGET
{
    return float4(0.5, 0.5, 0.5, 1.0f);
}
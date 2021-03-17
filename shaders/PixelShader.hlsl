struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 color : VT3_COLOR;
    float3 normal : VT3_NORMAL;
};


float4 main(PS_INPUT input) : SV_TARGET
{
    return float4(input.color[0], input.color[1], input.color[2], 1.0f);
}
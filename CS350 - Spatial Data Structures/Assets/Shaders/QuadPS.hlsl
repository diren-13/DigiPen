struct PixelShaderInput
{
    float4 position :   SV_POSITION;
    float4 colour   :   COLOUR;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
    return input.colour;
}
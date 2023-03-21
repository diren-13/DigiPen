struct VertexShaderInput
{
    float2  position    : POSITION;
    float4  colour      : COLOUR;
};

struct VertexToShader
{
    float4 position     : SV_POSITION;
    float4 colour       : COLOUR;
};

VertexToShader main( VertexShaderInput input )
{
    VertexToShader output;

    output.position = float4(input.position, 0.0f, 1.0f);
    output.colour   = input.colour;

    return output;
}
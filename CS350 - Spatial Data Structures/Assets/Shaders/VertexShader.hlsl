cbuffer constantBuffer : register(b0)
{
    float4              colour;
    row_major matrix    world;
    row_major matrix    viewProjection;
};

struct VertexShaderInput
{
    float3 position :   POSITION;
    float3 normal   :   NORMAL;
};

struct VertexToPixel
{
    float4 position :   SV_POSITION;
    float4 colour   :   COLOUR;
    float3 worldPos :   POSITION;
    float3 normal   :   NORMAL;
};

VertexToPixel main( VertexShaderInput input )
{
    VertexToPixel output;

    const matrix WORLD_VIEW_PROJECTION = mul(world, viewProjection);

    output.position = mul(float4(input.position, 1.0f), WORLD_VIEW_PROJECTION);
    output.colour   = colour;
    output.worldPos = mul(float4(input.position, 1.0f), WORLD_VIEW_PROJECTION).xyz;
    output.normal   = mul(input.normal, world);
    return output;
}
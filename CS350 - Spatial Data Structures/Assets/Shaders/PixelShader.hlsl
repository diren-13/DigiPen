#define MAX_LIGHTS 32

struct PixelShaderInput
{
    float4 position :   SV_POSITION;
    float4 colour   :   COLOUR;
    float3 worldPos :   POSITION;
    float3 normal   :   NORMAL;
};

struct DirectionalLight
{
    float4  ambientColour;
    float4  diffuseColour;
    float3  direction;
    float   ambientIntensity;
};

cbuffer constantBuffer : register(b0)
{
    DirectionalLight    directionalLights[MAX_LIGHTS];
    unsigned int        directionalLightCount;
    float3              cameraPos;
}

float4 ComputeDirectionalLight(DirectionalLight dirLight, float3 normal)
{
    normal = normalize(normal);

    float4 lightColour = dirLight.diffuseColour * (dirLight.ambientColour * dirLight.ambientIntensity);
    // Prevent need for tonemapping using saturate. No HDR colours!
    lightColour += saturate(dot(dirLight.direction, normal) * dirLight.diffuseColour * dirLight.diffuseColour);

    return float4(lightColour.rgb, dirLight.diffuseColour.a);
}

float4 main(PixelShaderInput input) : SV_TARGET
{
    float4 finalColour = input.colour;

    if (directionalLightCount > 0)
    {
        finalColour = float4(1.0f, 1.0f, 1.0f, 1.0f);

        for (unsigned int i = 0U; i < directionalLightCount; ++i)
        {
            finalColour *= ComputeDirectionalLight(directionalLights[i], input.normal);
        }

        // Standard Gamma Correction
        float3 gammaCorrection = (float3) lerp(finalColour, pow(finalColour, 1.0f / 2.2f), 1.0f);
        finalColour = float4(gammaCorrection, finalColour.a);
    }

    return finalColour;
}
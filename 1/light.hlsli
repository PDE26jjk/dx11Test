struct DirectionalLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float3 Direction;
    float pad;
};
struct PointLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float3 Position;
    float Range;
    float3 Att;
    float pad;
};
struct SpotLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float3 Position;
    float Range;
    float3 Direction;
    float Spot;
    float3 Att;
    float pad;
};
struct Material
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular; // w = SpecPower
    float4 Reflect;
};


// µÆ¹âÊýÁ¿
int num_DL;
int num_PL;
int num_SL;

DirectionalLight DLs[10];
PointLight PLs[10];
SpotLight SLs[10];

void ComputeDirectionalLight(Material mat, DirectionalLight L,
    float3 normal, float3 toEye,
    out float4 ambient,
    out float4 diffuse,
    out float4 spec)
{
    // Initialize outputs.
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
    // The light vector aims opposite the direction the light rays travel.
    float3 lightVec = -L.Direction;
    // Add ambient term.
    ambient = mat.Ambient * L.Ambient;
    // Add diffuse and specular term, provided the surface is in
    // the line of site of the light.
    float diffuseFactor = dot(lightVec, normal);
    // Flatten to avoid dynamic branching.
    [flatten]
    if (diffuseFactor > 0.0f)
    {
        float3 v = reflect(-lightVec, normal);
        float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);
        diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;
        spec = specFactor * mat.Specular * L.Specular;
    }
}

void ComputePointLight(Material mat, PointLight L, float3 pos,
    float3 normal, float3 toEye,
    out float4 ambient, out float4 diffuse, out float4 spec)
{
    // Initialize outputs.
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
    // The vector from the surface to the light.
    float3 lightVec = L.Position - pos;
    // The distance from surface to light.
    float d = length(lightVec);
    // Range test.
    if (d > L.Range)
        return;
    // Normalize the light vector.
    lightVec /= d;
    // Ambient term.
    ambient = mat.Ambient * L.Ambient;
    // Add diffuse and specular term, provided the surface is in
    // the line of site of the light.
    float diffuseFactor = dot(lightVec, normal);
    // Flatten to avoid dynamic branching.
    [flatten]
    if (diffuseFactor > 0.0f)
    {
        float3 v = reflect(-lightVec, normal);
        float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);
        diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;
        spec = specFactor * mat.Specular * L.Specular;
    } //Attenuate
    float att = 1.0f / dot(L.Att, float3(1.0f, d, d * d));
    diffuse *= att;
    spec *= att;
}

void ComputeSpotLight(Material mat, SpotLight L,
    float3 pos, float3 normal, float3 toEye,
    out float4 ambient, out float4 diffuse, out float4 spec)
{
    // Initialize outputs.
    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
    // The vector from the surface to the light.
    float3 lightVec = L.Position - pos;
    // The distance from surface to light.
    float d = length(lightVec);
    // Range test.
    if (d > L.Range)
        return;
    // Normalize the light vector.
    lightVec /= d;
    // Ambient term.
    ambient = mat.Ambient * L.Ambient;
    // Add diffuse and specular term, provided the surface is in
    // the line of site of the light.
    float diffuseFactor = dot(lightVec, normal);
    // Flatten to avoid dynamic branching.
    [flatten]
    if (diffuseFactor > 0.0f)
    {
        float3 v = reflect(-lightVec, normal);
        float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);
        diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;
        spec = specFactor * mat.Specular * L.Specular;
    } // Scale by spotlight factor and attenuate.
    float spot = pow(max(dot(-lightVec, L.Direction), 0.0f), L.Spot);
    // Scale by spotlight factor and attenuate.
    float att = spot / dot(L.Att, float3(1.0f, d, d * d));
    ambient *= spot;
    diffuse *= att;
    spec *= att;
}

float4x4 MatrixShadow(float4 P,float4 L) {
    L = -L;
    float3 n = P.xyz;
    float d = P.w;
    float nL = dot(n, L.xyz);
    return float4x4(
        nL+d*L.w-L.x*n.x,   -L.x*n.x,           -L.z*n.x,           -L.w*n.x,
        -L.x*n.y,           nL+d*L.w-L.y*n.y,   -L.z*n.y,           -L.w*n.y,
        -L.x*n.z,           -L.y*n.z,           nL+d*L.w-L.z*n.z,   -L.w*n.z,
        -L.x*d,             -L.y*d,             -L.z*d,             nL 
    );
}
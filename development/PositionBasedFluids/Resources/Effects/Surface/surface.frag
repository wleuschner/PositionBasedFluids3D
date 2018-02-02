#version 330
#extension GL_ARB_explicit_uniform_location : require
struct LightSource
{
    vec3 pos;
    vec3 ldir;
    vec3 amb;
    vec3 dif;
    vec3 spec;
};

uniform vec3 cPos;
uniform float vpWidth;
uniform float vpHeight;
uniform sampler2D depthMap;
uniform sampler2D thicknessMap;

uniform mat4 view;
uniform mat4 modelView;
uniform float fx;
uniform float fy;
uniform LightSource light;
uniform vec4 eye;

in vec2 fragTexCoord;
out vec4 fragColor;

vec3 getEyePos(vec2 texCoord,float depth)
{
    vec3 eyePos;
    eyePos.x = ((2.0*texCoord.x)-1.0)/fx;
    eyePos.y = ((2.0*texCoord.y)-1.0)/fy;
    eyePos.z = 1.0;
    return depth*eyePos;
}

void main()
{
    float dxTex = 1.0f/textureSize(depthMap,0).x;
    float dyTex = 1.0f/textureSize(depthMap,0).y;

    float depth = texture(depthMap,fragTexCoord).x;
    //float depth2 = texture(thicknessMap,fragTexCoord).x;
    if(depth>=1.0)
    {
        discard;
    }
    vec3 eyePos = getEyePos(fragTexCoord,depth);
    //vec3 eyePos2 = getEyePos(fragTexCoord,depth2);

    vec3 ddx = getEyePos(fragTexCoord+vec2(dxTex,0.0),texture(depthMap,fragTexCoord+vec2(dxTex,0.0)).x)-eyePos;
    vec3 ddx2 = eyePos-getEyePos(fragTexCoord+vec2(-dxTex,0.0),texture(depthMap,fragTexCoord+vec2(-dxTex,0.0)).x);
    if(abs(ddx.z)<abs(ddx2.z))
    {
        ddx = ddx2;
    }

    vec3 ddy = getEyePos(fragTexCoord+vec2(0.0,dyTex),texture(depthMap,fragTexCoord+vec2(0.0,dyTex)).x)-eyePos;
    vec3 ddy2 = eyePos-getEyePos(fragTexCoord+vec2(0.0,-dyTex),texture(depthMap,fragTexCoord+vec2(0.0,-dyTex)).x);
    if(abs(ddy.z)<abs(ddy2.z))
    {
        ddy = ddy2;
    }
    vec3 N = cross(ddx,ddy);
    N = normalize(N);

    //Compute Thickness
    float d = texture(thicknessMap,fragTexCoord).x;

    vec3 V = normalize(eyePos-vec3(modelView*vec4(cPos,1.0)));
    vec3 L = normalize(eyePos-light.pos);
    float R0 = (1.0-1.33)/(1.0+1.33);
    float RTheta = R0+(1.0-R0)*pow((1.0-dot(N,V)),5);
    float z = texture(depthMap,fragTexCoord).x;
    float cz = (2.0 * 0.1) / (10.0 + 0.1 - z * (10.0 - 0.1));
    //fragColor = vec4(cz,cz,cz,1.0);

    vec3 H = normalize(V+L);
    float diff = max(dot(N,L), 0.0);
    float spec = 0.0;
    if(diff>0.0)
    {
        spec = max(pow(dot(N,H),20.0),0.0);
    }
    //fragColor = vec4(d,d,d,1.0);

    //fragColor = vec4(texture(thicknessMap,fragTexCoord).xyz,1.0);
    vec3 c_fluid = mix(vec3(0.0,0.0,1.0),vec3(0.0,0.0,0.0),exp(0.9*-d));
    //fragColor = vec4(0.0,0.0,0.5,1.0)*diff+vec4(0.0,0.0,0.0,0.0)/*+vec4(0.7,0.7,0.7,0.0)*spec*/;
    fragColor = clamp(vec4(RTheta*vec3(0.0,0.0,0.7)+(1.0-RTheta)*c_fluid+vec3(0.7,0.7,0.7)*spec,1.0),vec4(0.0,0.0,0.0,1.0),vec4(1.0,1.0,1.0,1.0));
    //fragColor = clamp(,vec4(1.0,1.0,1.0,1.0));

}

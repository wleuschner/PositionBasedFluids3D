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

uniform float vpWidth;
uniform float vpHeight;
uniform sampler2D depthMap;

uniform mat4 view;
uniform mat4 modelView;
uniform float fx;
uniform float fy;
uniform LightSource light;

in vec2 fragTexCoord;
out vec4 fragColor;

vec3 getEyePos(vec2 texCoord,float depth)
{
    vec3 eyePos;
    eyePos.x = (((2.0*texCoord.x)/vpWidth)-1.0)/fx;
    eyePos.y = (((2.0*texCoord.y)/vpHeight)-1.0)/fy;
    eyePos.z = 1.0;
    return depth*eyePos;
}

void main()
{
    float dxTex = 1.0f/textureSize(depthMap,0).x;
    float dyTex = 1.0f/textureSize(depthMap,0).y;

    float depth = texture(depthMap,fragTexCoord).x;
    if(depth>=1.0)
    {
        discard;
    }
    vec3 eyePos = getEyePos(fragTexCoord,depth);
    vec3 ddx = getEyePos(fragTexCoord+vec2(dxTex,0.0),texture(depthMap,fragTexCoord+vec2(dxTex,0.0)).x)-eyePos;
    vec3 ddx2 = eyePos-getEyePos(fragTexCoord+vec2(-dxTex,0.0),texture(depthMap,fragTexCoord+vec2(-dxTex,0.0)).x);
    if(abs(ddx.z)>abs(ddx2.z))
    {
        ddx = ddx2;
    }

    vec3 ddy = getEyePos(fragTexCoord+vec2(0.0,dyTex),texture(depthMap,fragTexCoord+vec2(0.0,dyTex)).x)-eyePos;
    vec3 ddy2 = eyePos-getEyePos(fragTexCoord+vec2(0.0,-dyTex),texture(depthMap,fragTexCoord+vec2(0.0,-dyTex)).x);
    if(abs(ddy.z)>abs(ddy2.z))
    {
        ddy = ddy2;
    }

    vec3 N = -cross(ddx,ddy);
    N = normalize(N);

    float diff = max(0.0,dot(N,vec3(vec4(0.0,0.0,1.0,1.0))));
    float z = texture(depthMap,fragTexCoord).x;
    float cz = (2.0 * 0.1) / (10.0 + 0.1 - z * (10.0 - 0.1));
    //fragColor = vec4(cz,cz,cz,1.0);
    fragColor = vec4(diff*vec3(0.0,0.0,1.0),1.0);
}

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
uniform sampler2D thicknessMap;
uniform sampler2D background;
uniform samplerCube skybox;

uniform mat4 view;
uniform mat4 modelView;
uniform float fx;
uniform float fy;
uniform LightSource light;
uniform vec4 eye;

flat in float solidFrag;
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
    vec2 dxTex = vec2(1.0,1.0)/textureSize(depthMap,0);

    float depth = texture(depthMap,fragTexCoord).x;
    if(solidFrag==1.0)
    {
        discard;
    }
    if(depth>=1.0)
    {
        fragColor = vec4(texture(background,fragTexCoord).xyz,1.0);
        return;
    }
    vec3 eyePos = getEyePos(fragTexCoord,depth);

    vec3 ddx = getEyePos(fragTexCoord+vec2(dxTex.x,0.0),texture(depthMap,fragTexCoord+vec2(dxTex.x,0.0)).x)-eyePos;
    vec3 ddx2 = eyePos-getEyePos(fragTexCoord+vec2(-dxTex.x,0.0),texture(depthMap,fragTexCoord+vec2(-dxTex.x,0.0)).x);
    if(abs(ddx.z)<abs(ddx2.z))
    {
        ddx = ddx2;
    }

    vec3 ddy = getEyePos(fragTexCoord+vec2(0.0,dxTex.y),texture(depthMap,fragTexCoord+vec2(0.0,dxTex.y)).x)-eyePos;
    vec3 ddy2 = eyePos-getEyePos(fragTexCoord+vec2(0.0,-dxTex.y),texture(depthMap,fragTexCoord+vec2(0.0,-dxTex.y)).x);
    if(abs(ddy.z)<abs(ddy2.z))
    {
        ddy = ddy2;
    }
    vec3 N = cross(ddx,ddy);
    N = normalize(N);

    //Compute Thickness
    float d = texture(thicknessMap,fragTexCoord).x;

    vec3 V = normalize(eyePos);
    vec3 L = normalize(eyePos-light.pos);
    float R0 = (1.0-1.33)/(1.0+1.33);
    float RTheta = R0+(1.0-R0)*pow((1.0-dot(N,V)),5);

    vec3 H = normalize(V+L);
    float diff = max(dot(N,L), 0.0);
    float spec = 0.0;
    if(diff>0.0)
    {
        spec = max(pow(dot(N,H),50.0),0.0);
    }
    //vec3 reflectionCol = vec3(0.0,0.0,0.7);
    vec3 reflectionCol = texture(skybox,normalize(reflect(-N,V))).xyz;
    vec3 bgCol = texture(background,fragTexCoord+N.xy*d).xyz;
    //bgCol = vec3(0.0,0.0,0.0);
    vec3 c_fluid = mix(vec3(0.4,0.499,0.998),bgCol,exp(vec3(0.4,0.499,0.998)*-d));
    /*c_fluid.r = mix(1.0,bgCol.r,exp(0.40*-d));
    c_fluid.g = mix(1.0,bgCol.g,exp(0.499*-d));
    c_fluid.b = mix(1.0,bgCol.b,exp(0.998*-d));*/

    fragColor = clamp(vec4(RTheta*reflectionCol+(1.0-RTheta)*c_fluid+vec3(1.0,1.0,1.0)*spec,1.0),vec4(0.0,0.0,0.0,1.0),vec4(1.0,1.0,1.0,1.0));
}

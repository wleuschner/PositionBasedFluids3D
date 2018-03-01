#version 330

struct LightSource
{
    vec3 pos;
    vec3 ldir;
    vec3 amb;
    vec3 dif;
    vec3 spec;
};

in vec3 eyeSpacePos;
flat in float solidFrag;
layout(location=0) out vec4 fragColor;

uniform sampler2D bgDepthMap;
uniform float particleSize;
uniform mat4 modelView;
uniform mat4 projection;

void main()
{
    vec3 N;
    N.xy = 2.0*gl_PointCoord-vec2(1.0,1.0);
    float r2 = dot(N.xy,N.xy);
    float depth = texture(bgDepthMap,gl_FragCoord.xy).x;
    if(r2>1.0||solidFrag==1.0)
    {
        discard;
    }
    N.z = sqrt(1.0-r2);
    vec4 fragPos = vec4(eyeSpacePos+N*particleSize,1.0);
    vec4 clipPos = projection*fragPos;
    float zVal = clipPos.z/clipPos.w;
    if(zVal>depth)
    {
        discard;
    }

    gl_FragDepth = zVal;
    //fragColor = vec4(0.0,1.0,0.0,1.0);
    fragColor = vec4(2*(N.z*particleSize),2*(N.z*particleSize),2*(N.z*particleSize),1.0);

}

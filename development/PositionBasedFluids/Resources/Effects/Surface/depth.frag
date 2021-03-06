#version 330
in vec3 eyeSpacePos;
flat in float solidFrag;
layout(location=0) out vec4 fragColor;

uniform float particleSize;
uniform mat4 modelView;
uniform mat4 projection;
uniform sampler2D bgDepthMap;


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
    gl_FragDepth = clipPos.z/clipPos.w;

}

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
out vec4 fragColor;

uniform float particleSize;
uniform mat4 modelView;
uniform mat4 projection;
uniform LightSource light;

void main()
{
    vec3 N;
    N.xy = 2.0*gl_PointCoord-vec2(1.0,1.0);
    float r2 = dot(N.xy,N.xy);
    if(r2>1.0)
    {
        discard;
    }
    N.z = sqrt(1.0-r2);
    vec4 fragPos = vec4(eyeSpacePos+N*particleSize,1.0);
    vec4 clipPos = projection*fragPos;
    gl_FragDepth = clipPos.z/clipPos.w;
    //fragColor = vec4(0.0,1.0,0.0,1.0);
    fragColor = vec4(N.z*particleSize,N.z*particleSize,N.z*particleSize,1.0);

}

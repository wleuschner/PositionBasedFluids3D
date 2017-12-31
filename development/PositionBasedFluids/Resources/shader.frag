#version 330
flat in float density;
out vec4 fragColor;

void main()
{
    //More Than Resting Density RED
    if(density>0.2)
    {
        fragColor = vec4(clamp(+density/2.0,0.0,1.0),0.0,clamp(1.0-density/2.0,0.0,1.0),1.0);
    }
    //Less Than Resting Density GREEN
    else if(density<-0.2)
    {
        fragColor = vec4(0.0,clamp(-density/2.0,0.0,1.0),clamp(1.0+density/2.0,0.0,1.0),1.0);
    }
    else
    {
        vec4(0.0,0.0,1.0,1.0);
    }
}

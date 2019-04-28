#version 420

in vec2 loc;

out vec4 frag_colour;

void main() {
	int type = int(mod(floor(loc.x) + floor(loc.y), 2));
	frag_colour = vec4(mix(vec3(0.1, 0.1, 0.1), vec3(0.5, 0.5, 0.5), type), 0.4);
}

/*
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    
	vec2 interval = vec2(20.49999999,100.);
    vec2 origin = vec2(2.34,3.546);
    
    vec2 pixelSpaceInterval = iResolution.xy/interval;
    fragCoord += origin/interval*pixelSpaceInterval;
    
    fragColor = vec4(1);
    
    if (distance(
        mod(fragCoord,pixelSpaceInterval),
        mod(fragCoord+vec2(1,1),pixelSpaceInterval)
    	)>min(pixelSpaceInterval.x,pixelSpaceInterval.y)/2.) fragColor = vec4(0.);
}
*/
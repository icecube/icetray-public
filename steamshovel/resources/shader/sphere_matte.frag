#version 120

// Basic smooth-shaded spheres, the default for most visualization in steamshovel

uniform vec3 camera;
uniform float radius;
varying vec2 square_coord;
varying vec3 cameraSphereVertex;

void main()
{
    float lensq = dot( square_coord, square_coord );
    if( lensq > 1.0 ) discard;
    // normal and position of this fragment in camera coords
    vec3 cameraNormal = vec3( square_coord, sqrt( 1.0-lensq ) );
    vec3 cameraPos = ( radius * cameraNormal ) + cameraSphereVertex;

    // compute depth
    vec4 clipPos = gl_ProjectionMatrix * vec4(cameraPos,1.0);
    float ndcDepth = clipPos.z / clipPos.w;
    gl_FragDepth = ((gl_DepthRange.diff * ndcDepth) +gl_DepthRange.near + gl_DepthRange.far) / 2.0;

    vec3 light = vec3(0.0,0.0,0.0);
    vec3 n = cameraNormal;
    vec3 V = cameraPos;
    vec3 L = normalize( light-V );

    vec4 outcolor = vec4( 0.0, 0.0, 0.0, 1.0);

    //calculate Diffuse Term:  
    vec4 Idiff = gl_Color * 0.8 * max(dot(n,L), 0.0);
    Idiff = clamp(Idiff, 0.0, 1.0);     

    outcolor += vec4(.15,.15,.15,1.0) + Idiff;

    // write Total Color:  
    gl_FragColor = vec4(outcolor.rgb, gl_Color.a);

}

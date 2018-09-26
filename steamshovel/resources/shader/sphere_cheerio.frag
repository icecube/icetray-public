#version 120

// flat, hollow ring shader for 'empty'-looking spheres

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

    // magic handwaving sauce: estimate the width to make the ring look good at various camera distances
    float factor = 1000 / length(cameraPos);
    if( lensq < min(factor, 0.8) ) discard;

    // compute depth
    vec4 clipPos = gl_ProjectionMatrix * vec4(cameraPos,1.0);
    float ndcDepth = clipPos.z / clipPos.w;
    gl_FragDepth = ((gl_DepthRange.diff * ndcDepth) +gl_DepthRange.near + gl_DepthRange.far) / 2.0;

    gl_FragColor = gl_Color;

}

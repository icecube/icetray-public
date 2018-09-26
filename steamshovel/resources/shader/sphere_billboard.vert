#version 120

// Basic vertex shader to create "billboard" spheres that always face the camera

uniform vec3 camera;
uniform float radius;
attribute vec2 offset;

const float perspective_correction = 1.0;

varying vec2 square_coord;
varying vec3 cameraSphereVertex;

void main()
{

    vec3 right = normalize(cross( camera, vec3(0,0,1) ));
    vec3 true_up = normalize(cross( right, camera ));

    vec2 xyoff = offset * perspective_correction;

    vec4 tv = gl_Vertex;
    tv += vec4(right * xyoff.x, 0.0);
    tv += vec4(true_up * xyoff.y, 0.0);
    gl_Position = gl_ModelViewProjectionMatrix * tv;
    gl_FrontColor = gl_Color;

    square_coord = vec2( -offset[0], offset[1] ) * perspective_correction;
     
    cameraSphereVertex = vec3(gl_ModelViewMatrix * gl_Vertex);

}

#version 120

const float ldist = 10000;  

#define NLIGHTS 6
vec3 glLights[NLIGHTS] = vec3[NLIGHTS] ( 
    vec3( ldist, 0, 0 ), vec3( -ldist, 0, 0 ),
    vec3( 0, ldist, 0 ), vec3( 0, -ldist, 0 ),
    vec3( 0, 0, ldist ), vec3( 0, 0, -ldist ) );
                            
uniform vec3 camera;
uniform float radius;
varying vec2 square_coord;
varying vec3 cameraSphereVertex;

// imposter function borrowed from the http://www.arcsynthesis.org/gltut tutorial

/*
void Impostor(out vec3 cameraPos, out vec3 cameraNormal)
{
	vec3 cameraPlanePos = vec3(square_coord * radius, 0.0) + cameraSphereVertex;
	vec3 rayDirection = normalize(cameraPlanePos);
	
	float B = 2.0 * dot(rayDirection, -cameraSphereVertex);
	float C = dot(cameraSphereVertex, cameraSphereVertex) - (radius * radius);
	
	float det = (B * B) - (4.0 * C);
	if(det < 0.0)
		discard;
		
	float sqrtDet = sqrt(det);
	float posT = (-B + sqrtDet)/2.0;
	float negT = (-B - sqrtDet)/2.0;
	
	float intersectT = min(posT, negT);
	cameraPos = rayDirection * intersectT;
	cameraNormal = normalize(cameraPos - cameraSphereVertex);
}
*/

void main()
{
    //vec3 cameraPos;
    //vec3 cameraNormal;
    //Impostor(cameraPos, cameraNormal);

    float lensq = dot( square_coord, square_coord );
    if( lensq > 1.0 ) discard;
    // normal and position of this fragment in camera coords
    vec3 cameraNormal = vec3( square_coord, sqrt( 1.0-lensq ) );
    vec3 cameraPos = ( radius * cameraNormal ) + cameraSphereVertex;

    // compute depth
    vec4 clipPos = gl_ProjectionMatrix * vec4(cameraPos,1.0);
    float ndcDepth = clipPos.z / clipPos.w;
    gl_FragDepth = ((gl_DepthRange.diff * ndcDepth) +gl_DepthRange.near + gl_DepthRange.far) / 2.0;

    vec3 n = cameraNormal;

    vec3 V = cameraPos;

   vec4 outcolor = vec4( 0.0, 0.0, 0.0, 1.0);
   for( int i = 0; i < 6;i++){
       //vec4 lightpos = gl_ModelViewMatrix * gl_LightSource[i].position;
       vec4 lightpos = gl_ModelViewMatrix * vec4( glLights[i], 0.0 );
       vec3 light = lightpos.xyz;

       vec3 L = normalize( light-V );
       vec3 R = normalize(-reflect(L,n));  

       //calculate Diffuse Term:  
       vec4 Idiff = gl_Color * 0.4 * max(dot(n,L), 0.0);
       Idiff = clamp(Idiff, 0.0, 1.0);     

       vec3 E = normalize(-V); // we are in Eye Coordinates, so EyePos is 
       // calculate Specular Term:
       vec4 Ispec = vec4(1,1,1,gl_Color.a) * 0.5 * pow(max(dot(R,E),0.0),0.3*99);
       Ispec = clamp(Ispec, 0.0, 1.0); 
       outcolor += Idiff + Ispec; 
    }
    // write Total Color:  
    //gl_FragColor = gl_FrontLightModelProduct.sceneColor + outcolor;
    gl_FragColor = vec4( (outcolor.rgb * .85) + (0.15,0.15,0.15), gl_Color.a );

}

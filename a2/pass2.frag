// Pass 2 fragment shader
//
// Apply diffuse lighting to fragment.  Later do Phong lighting.
//
// Determine whether fragment is in shadow.  If so, reduce intensity to 50%.

#version 300 es
precision mediump float;

uniform vec3      lightDir;	    // direction to light in WCS
uniform vec3      eyePosition;	    // position of eye in WCS
uniform mat4      WCS_to_lightCCS;  // transform from WCS to light's CCS
uniform sampler2D shadowBuffer;     // texture [0,1]x[0,1] of depth from light.  Values in [0,1].
uniform sampler2D objTexture;       // object's texture (might not be provided)
uniform bool      texturing;        // =1 if object's texture is provided

in vec3 colour;        // fragment colour
in vec3 normal;        // fragment normal in WCS
in vec3 wcsPosition;   // fragment position in WCS
in vec2 texCoords;     // fragment texture coordinates (if provided) 

out vec4 fragColour;   // fragment's final colour

uniform vec3 kd;
uniform vec3 ks;
uniform vec3 Ia;
uniform vec3 Ie;
uniform vec3 Iin;
uniform float shininess;

void main()

{
  // Calculate the position of this fragment in the light's CCS.

  vec4 lightCCSPos  = WCS_to_lightCCS * vec4(wcsPosition, 1);

  // Calculate the depth of this fragment in the light's CCS in the range [0,1]
  
  float fragDepth = ((lightCCSPos.z / lightCCSPos.w) + 1.0) * 0.5;

  // Determine the (x,y) coordinates of this fragment in the light's
  // CCS in the range [0,1]x[0,1].

  vec2 fragCoords = vec2(lightCCSPos.x, lightCCSPos.y);

  // Look up the depth from the light in the shadowBuffer texture.

  vec3 fragShadowDepth = texture(shadowBuffer, fragCoords).xyz;

  // Determine whether the fragment is in shadow.
  //
  // If results look bad, add a bit to the shadow texture depth to
  // prevent z-fighting.

  float hasShadow = fragShadowDepth.z > lightCCSPos.z ? 1.0 : 0.0;

  // Compute illumination.
  
  mediump vec3 texColour = texture(objTexture, texCoords).rgb;

  mediump vec3 Iout = Ia;
  mediump vec3 N = normalize(normal);
  mediump float NdotL = dot(N, lightDir);

  if (texturing) {
     Iout += NdotL * (texColour * Iin);
  } else {
    Iout += kd;
  }

  if (NdotL < 0.0) {
     // diffuse lighting
     Iout += NdotL * vec3(kd.r * Iin.r, kd.g * Iin.g, kd.b * Iin.b);

     // specular component

     mediump vec3 R = (2.0 * NdotL) * N - lightDir;
     mediump vec3 V = normalize(-1.0 * eyePosition);

     mediump float RdotV = dot(R, V);

     if (RdotV > 0.0) {
     	Iout += pow( RdotV, shininess ) * vec3(ks.r * Iin.r, ks.g * Iin.g, ks.b * Iin.b);
     }

if (hasShadow == 1.0) {
   Iout = Iout * 0.2;
}
    
  }


  // Output the fragment colour, modified by the illumination model
  // and shadowing.

  fragColour = vec4(Iout, 1);
}

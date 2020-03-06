// Pass 1 vertex shader
//
// Output vertex depth in the light's CCS

#version 300 es
uniform mat4 MV;

precision mediump float;

uniform mat4 OCS_to_lightCCS; // Transform OCS into light's coordinate system

layout (location=0) in vec3 vertPosition; // OCS vertex position
layout (location=1) in mediump vec3 vertNormal;

out float ccsLightDepth; // output vertex depth in [0,1]

void main()

{
  // Calculate the position of this vertex in the light's CCS.
  vec4 ccsPos = OCS_to_lightCCS * vec4(vertPosition, 1);
  gl_Position = ccsPos;

  // compute vertex normal
  vec3 normal = vec3(MV * vec4(vertNormal, 0));

  // Output the ccsLightDepth in the range [0,1]
  ccsLightDepth = ((ccsPos.z / ccsPos.w) + 1.0) * 0.5;
}

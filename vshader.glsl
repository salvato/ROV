#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

attribute vec4 a_position;
attribute vec2 a_texcoord;

uniform mat4 MVP;
varying vec2 v_texcoord;

void main(void)
{
    // Calculate vertex position in screen space
    gl_Position = MVP *  a_position;

    // Pass texture coordinate to fragment shader
    // Value will be automatically interpolated to fragments inside polygon faces
    v_texcoord = a_texcoord;
}

//#version 330 core

//// Input vertex data, different for all executions of this shader.
//layout(location = 0) in vec3 vertexPosition_modelspace;
//layout(location = 1) in vec2 vertexUV;
//layout(location = 2) in vec3 vertexNormal_modelspace;

//// Output data ; will be interpolated for each fragment.
//out vec2 UV;
//out vec3 Position_worldspace;
//out vec3 Normal_cameraspace;
//out vec3 EyeDirection_cameraspace;
//out vec3 LightDirection_cameraspace;

//// Values that stay constant for the whole mesh.
//uniform mat4 MVP;
//uniform mat4 V;
//uniform mat4 M;
//uniform vec3 LightPosition_worldspace;

//void main(){

//  // Output position of the vertex, in clip space : MVP * position
//  gl_Position =  MVP * vec4(vertexPosition_modelspace, 1);

//  // Position of the vertex, in worldspace : M * position
//  Position_worldspace = vec3(M * vec4(vertexPosition_modelspace, 1)); // drop the 4th component

//  // Vector that goes from the vertex to the camera, in camera space.
//  // In camera space, the camera is at the origin (0,0,0).
//  vec3 vertexPosition_cameraspace = vec3(V * M * vec4(vertexPosition_modelspace, 1)); // drop the 4th component
//  EyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;

//  // Vector that goes from the vertex to the light, in camera space.
//  // M is ommited because it's identity.
//  vec3 LightPosition_cameraspace = vec3(V * vec4(LightPosition_worldspace, 1)); // drop the 4th component
//  LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;

//  // Normal of the the vertex, in camera space
//  // Only correct if ModelMatrix does not scale the model !
//  // Use its inverse transpose if not.
//  Normal_cameraspace = vec3(V * M * vec4(vertexNormal_modelspace, 0)); // drop the 4th component

//  // UV of the vertex. No special space for this one.
//  UV = vertexUV;
//}

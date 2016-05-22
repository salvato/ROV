#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

attribute vec3 qt_Vertex;
attribute vec3 vertexNormal_modelspace;
//attribute vec2 qt_MultiTexCoord0;

// Values that stay constant for the whole mesh.
uniform mat4 mvp_Matrix;
uniform mat4 view_Matrix;
uniform mat4 model_Matrix;
uniform mat4 model_MatrixIT;
uniform vec4 LightPosition_worldspace;

//// Output data ; will be interpolated for each fragment.
varying vec4 Position_worldspace;
varying vec4 Normal_cameraspace;
varying vec4 EyeDirection_cameraspace;
varying vec4 LightDirection_cameraspace;
varying vec2 qt_TexCoord0;

void
main(void) {
    // Calculate vertex position in screen space
    gl_Position = mvp_Matrix *  vec4(qt_Vertex, 1.0);

    // Position of the vertex, in worldspace : model_Matrix * position
    Position_worldspace = model_Matrix * vec4(qt_Vertex, 1.0);

    // Vector that goes from the vertex to the camera, in camera space.
    // In camera space, the camera is at the origin (0,0,0).
    vec4 vertexPosition_cameraspace = view_Matrix * model_Matrix * vec4(qt_Vertex, 1.0);
    EyeDirection_cameraspace = vec4(0, 0, 0, 1) - vec4(qt_Vertex, 1.0);

    // Vector that goes from the vertex to the light, in camera space.
    // M is omited because it's identity.
    vec4 LightPosition_cameraspace = view_Matrix * LightPosition_worldspace;
    LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;

    // Normal of the the vertex, in camera space
    // Only correct if ModelMatrix does not scale the model !
    // Use its inverse transpose if not.
    Normal_cameraspace = view_Matrix * model_MatrixIT * vec4(vertexNormal_modelspace, 1.0);

    // Pass texture coordinate to fragment shader
    // Value will be automatically interpolated to fragments inside polygon faces
    qt_TexCoord0 = qt_TexCoord0;
}

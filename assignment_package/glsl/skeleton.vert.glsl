#version 150
// ^ Change this to version 130 if you have compatibility issues

//This is a vertex shader. While it is called a "shader" due to outdated conventions, this file
//is used to apply matrix transformations to the arrays of vertex data passed to it.
//Since this code is run on your GPU, each vertex is transformed simultaneously.
//If it were run on your CPU, each vertex would have to be processed in a FOR loop, one at a time.
//This simultaneous transformation allows your program to run much faster, especially when rendering
//geometry with millions of vertices.

uniform mat4 u_Model;       // The matrix that defines the transformation of the
                            // object we're rendering. In this assignment,
                            // this will be the result of traversing your scene graph.

uniform mat4 u_ModelInvTr;  // The inverse transpose of the model matrix.
                            // This allows us to transform the object's normals properly
                            // if the object has been non-uniformly scaled.

uniform mat4 u_ViewProj;    // The matrix that defines the camera's transformation.
                            // We've written a static matrix for you to use for HW2,
                            // but in HW3 you'll have to generate one yourself
uniform mat4 u_Binds[100];
uniform mat4 u_Trans[100];

in vec4 vs_Pos;             // The array of vertex positions passed to the shader

in vec4 vs_Nor;             // The array of vertex normals passed to the shader

in vec4 vs_Col;             // The array of vertex colors passed to the shader.

in vec2 vs_Joints;
in vec2 vs_JointWeights;

out vec3 fs_Pos;
out vec4 fs_Nor;            // The array of normals that has been transformed by u_ModelInvTr. This is implicitly passed to the fragment shader.
out vec4 fs_Col;            // The color of each vertex. This is implicitly passed to the fragment shader.

void main()
{
    fs_Col = vs_Col;                         // Pass the vertex colors to the fragment shader for interpolation

    mat3 invTranspose = mat3(u_ModelInvTr);
    fs_Nor = vec4(invTranspose * vec3(vs_Nor), 0);          // Pass the vertex normals to the fragment shader for interpolation.
                                                            // Transform the geometry's normals by the inverse transpose of the
                                                            // model matrix. This is necessary to ensure the normals remain
                                                            // perpendicular to the surface after the surface is transformed by
                                                            // the model matrix.


    vec4 modelposition = u_Model * vs_Pos;   // Temporarily store the transformed vertex positions for use below
    fs_Pos = modelposition.xyz;

    gl_Position = u_ViewProj * modelposition;// gl_Position is a built-in variable of OpenGL which is
                                             // used to render the final positions of the geometry's vertices
    int joint1 = int(vs_Joints[0]);
    int joint2 = int(vs_Joints[1]);
    //check if bound mesh
    if(joint1 >= 0 && joint2 >= 0) {
        vec4 delta1 = u_Trans[joint1]*u_Binds[joint1]*modelposition;
        vec4 delta2 = u_Trans[joint2]*u_Binds[joint2]*modelposition;
        float f = vs_JointWeights[0]/(vs_JointWeights[0]+vs_JointWeights[1]);
        gl_Position = mix(u_ViewProj*delta1,u_ViewProj*delta2,f);
        fs_Pos = vec3(mix(delta1, delta2, f));
        //modify normals
        delta1 = u_Trans[joint1]*u_Binds[joint1]*fs_Nor;
        delta2 = u_Trans[joint2]*u_Binds[joint2]*fs_Nor;
        fs_Nor = mix(delta1, delta2, f);
    }
}

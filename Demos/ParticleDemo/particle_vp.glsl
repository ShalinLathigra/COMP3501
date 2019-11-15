#version 400

// Vertex buffer
in vec3 vertex;
in vec3 normal;
in vec3 color;

// Uniform (global) buffer
uniform mat4 world_mat;
uniform mat4 view_mat;
uniform mat4 normal_mat;
uniform float start;
uniform float timer;
uniform float end;
uniform vec3 momentum;
uniform vec3 color_att;

// Attributes forwarded to the geometry shader
out vec3 vertex_color;
out float timestep;


// Simulation parameters (constants)
uniform vec3 up_vec = vec3(0.0, 1.0, 0.0);
uniform vec3 object_color = vec3(0.8, 0.8, 0.8);
float grav = 0.3; // Gravity
float speed = 2.5; // Allows to control the speed of the explosion

void main()
{
	float lifetime = end - start;

    // Let time cycle every four seconds
    float circtime = lifetime * (1 - (end-timer) / (end-start));
    float t = circtime; // Our time parameter
    
    // Let's first work in model space (apply only world matrix)
    vec4 position = world_mat * vec4(vertex, 1.0);
    vec4 norm = normal_mat * vec4(normal, 1.0);

    // Move point along normal and down with t*t (acceleration under gravity)
    position.x += (norm.x+momentum.x)*t*speed - grav*speed*up_vec.x*t*t;
    position.y += (norm.y+momentum.y)*t*speed - grav*speed*up_vec.y*t*t;
    position.z += (norm.z+momentum.z)*t*speed - grav*speed*up_vec.z*t*t;
    
    // Now apply view transformation
    gl_Position = view_mat * position;
        
    // Define outputs
    // Define color of vertex
    //vertex_color = color.rgb; // Color defined during the construction of the particles
    vertex_color = color_att.xyz * (1-pow(circtime / lifetime, 2)); // Uniform color 
    //vertex_color = object_color * (1-pow(circtime / lifetime, 2)); // Uniform color 
    //vertex_color = vec3(t, 0.0, 1-t);
    //vertex_color = vec3(1.0, 1-t, 0.0);

    // Forward time step to geometry shader
    timestep = t;
}

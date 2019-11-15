#version 400

// Vertex buffer
in vec3 vertex;
in vec3 normal;
in vec3 color;

// Uniform (global) buffer
uniform mat4 world_mat;
uniform mat4 view_mat;
uniform mat4 normal_mat;
uniform float timer;

// Attributes forwarded to the geometry shader
out vec4 vertex_color;
out float particle_id;
out float timestep;


// Simulation parameters (constants)
uniform vec3 up_vec = vec3(0.0, 1.0, 0.0);
uniform vec3 object_color = vec3(0.3, 0.8, 1.0);
float grav = 0.3; // Gravity
float speed = 2.5; // Allows to control the speed of the explosion
float lifetime = 4.0;

void main()
{

    // Let time cycle every four seconds
    float circtime = mod(timer, lifetime);
    float t = circtime; // Our time parameter
    
    // Let's first work in model space (apply only world matrix)
    vec4 position = world_mat * vec4(vertex, 1.0);
    vec4 norm = normal_mat * vec4(normal, 1.0);

    // Move point along normal and down with t*t (acceleration under gravity)
    position.x += (norm.x)*t*speed;
    position.y += (norm.y)*t*speed;
    position.z += (norm.z)*t*speed;
    
    // Now apply view transformation
    gl_Position = view_mat * position;
        
    // Define outputs
    // Define color of vertex
    //vertex_color = color.rgb; // Color defined during the construction of the particles
    //vertex_color = vec3(t, 0.0, 1-t);
    //vertex_color = vec3(1.0, 1-t, 0.0);
	
    vertex_color = vec4(object_color.xyz, color.g - (1.4 * pow(circtime / lifetime, 2))); // Uniform color 

    // Forward time step to geometry shader
    timestep = t;
    particle_id = color.r; // Derived from the particle color. We use the id to keep track of particles
}

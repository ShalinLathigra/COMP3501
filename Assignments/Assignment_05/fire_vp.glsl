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
out vec4 particle_color;
out float particle_id;
out float particle_step;

// Simulation parameters (constants)
uniform vec3 up_vec = vec3(0.0, 1.0, 0.0); // Up direction
uniform vec3 back_vec = vec3(0.0, 0.0, -1.0); // Up direction
float up_acc = 0.01; // An acceleration applied to the particles coming from some attraction force
float back_acc = 0.05; // An acceleration applied to the particles coming from some attraction force

float speed = 5.0; // Control the speed of the motion

// Define some useful constants
const float pi = 3.1415926536;
const float pi_over_two = 1.5707963268;
const float two_pi = 2.0*pi;

const float lifetime = 4.0;

void main()
{
    // Define particle id
    particle_id = color.r; // Derived from the particle color. We use the id to keep track of particles

    // Define time in a cyclic manner
    float phase = two_pi*particle_id; // Start the sin wave later depending on the particle_id
    float param = timer / 10.0 + phase; // The constant that divides "timer" also helps to adjust the "speed" of the fire
    float rem = mod(param, pi_over_two); // Use the remainder of dividing by pi/2 so that we are always in the range [0..pi/2] where sin() gives values in [0..1]
    float circtime = sin(rem); // Get time value in [0..1], according to a sinusoidal wave

	// Let time cycle every four seconds
    float t = mod(timer + color.g, lifetime); // Our time parameter
    
    // Let's first work in model space (apply only world matrix)
    vec4 position = world_mat * vec4(vertex, 1.0);
    vec4 norm = normal_mat * vec4(normal, 1.0);

    // Move point along normal and down with t*t (acceleration under gravity)
    position.x += norm.x*t*speed - back_acc*speed*back_vec.x*t*t + up_acc*speed*up_vec.x*t*t;
    position.y += norm.y*t*speed - back_acc*speed*back_vec.y*t*t + up_acc*speed*up_vec.y*t*t;
    position.z += norm.z*t*speed - back_acc*speed*back_vec.z*t*t + up_acc*speed*up_vec.z*t*t;
    
    // Now apply view transformation
    gl_Position = view_mat * position;

    // Forward time step to geometry shader
    particle_step = t / lifetime;
	
    // Define amount of blending depending on the cyclic time
    float alpha = 0.5 * particle_step;
    particle_color = vec4(1.0, 1.0 - (t / lifetime), 1.0, alpha);
}

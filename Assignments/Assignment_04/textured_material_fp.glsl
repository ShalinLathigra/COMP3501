#version 130

// Attributes passed from the vertex shader
in vec3 position_interp;
in vec3 normal_interp;
in vec4 color_interp;
in vec2 uv_interp;
in vec3 light_pos;

// Uniform (global) buffer
uniform sampler2D texture_map;


// Material attributes (constants)
vec4 specular_color = vec4(0.0, 0.5, 0.9, 1.0);
float phong_exponent = 64.0;
float Ia = 0.2; // Ambient light amount

vec3 L_dir = normalize(vec3(1.0, 0.0, 0.0));
vec4 dir_diff = vec4(0.0, 0.5, 0.0, 1.0);
vec4 dir_spec = vec4(0.3, 0.3, 0.0, 1.0);
float dir_exponent = 96.0;

float Ra = 0.1; //



void main() 
{
    // Retrieve texture value
    vec4 pixel = texture(texture_map, uv_interp);
    // Use texture in determining fragment colour

    vec3 N, // Interpolated normal for fragment
         L, // Light-source direction
         V, // View direction
         R; // Reflected Light vector

    // Compute Lambertian term Id
    N = normalize(normal_interp);

    L = (light_pos - position_interp);
    L = normalize(L);

    float Id = max(dot(N, L), 0.0);

    // Compute specular term Is for Blinn-Phong shading
    // V = (eye_position - position_interp);
    V = - position_interp; // Eye position is (0, 0, 0) in view coordinates
    V = normalize(V);

    //R = -L + 2*(dot(N, L)*N); // Reflected Light Vector
    R = -L + 2*(dot(N, L)*N); // Reflected Light Vector (will be normalized anyway)
    R = normalize(R);

    float spec_angle_cos = max(dot(N, R), 0.0);
    float Is = pow(spec_angle_cos, phong_exponent);
        
	
	gl_FragColor = Ia*pixel + Id*pixel + Is*specular_color;
	
	float Rd = max(dot(N, -L_dir), 0.0);
	
	float spec_angle_dir = max(dot(N, -L_dir), 0.0);
    float Rs = pow(spec_angle_dir, dir_exponent);


    gl_FragColor += Rd * dir_diff + Rs * dir_spec;
}

// Illumination based on the traditional three-term model

#version 130

// Attributes passed from the vertex shader
in vec3 position_interp;
in vec3 normal_interp;
in vec3 light_pos;

// Material attributes (constants)
vec4 ambient_color = vec4(0.0, 0.0, 1.0, 1.0);
vec4 diffuse_color = vec4(0.0, 0.0, 0.7, 1.0);
vec4 specular_color = vec4(0.8, 0.5, 0.9, 1.0);
float phong_exponent = 64.0;
float Ia = 0.2; // Ambient light amount

// Global Lighting Attributes
vec3 L_directional = normalize(vec3(1, 0, -1));
vec4 directional_color = vec4(0.8, 0.5, 0.0, 1.0);
float Da = 0.1; //Ambient Directional Amount

void main() 
{
    vec3 N, // Interpolated normal for fragment
         L, // Light-source direction
         V, // View direction
         R; // Reflected vector
		 
	//LOCAL ILLUMINATION!!!!
    // Compute Lambertian term Id
    N = normalize(normal_interp);

    L = (light_pos - position_interp);
    L = normalize(L);

    float Id = max(dot(N, L), 0.0);
    
    // Compute specular term Is for Blinn-Phong shading
    // V = (eye_position - position_interp);
    V = - position_interp; // Eye position is (0, 0, 0) in view coordinates
    V = normalize(V);
	
	//R = -L + 2(dot(L, N) * N
	R = normalize(-L + 2(dot(L, N) * N);

    float spec_angle_cos = max(dot(V, R), 0.0);
    float Is = pow(spec_angle_cos, phong_exponent);
        
    // Assign illumination to the fragment
    gl_FragColor = Ia*ambient_color + Id*diffuse_color + Is*specular_color;
                    

	//GLOBAL ILLUMINATION!!!!
	L = L_directional;
	
	//float Dd = ciel(max(dot(N, L), 0.0));
	float Dd = max(dot(N, L), 0.0);

	gl_FragColor = (Da + Dd)*directional_color;

    // For debug, we can display the different values
    //gl_FragColor = ambient_color;
    //gl_FragColor = diffuse_color;
    //gl_FragColor = specular_color;
    //gl_FragColor = color_interp;
    //gl_FragColor = vec4(N.xyz, 1.0);
    //gl_FragColor = vec4(L.xyz, 1.0);
    //gl_FragColor = vec4(V.xyz, 1.0);
}

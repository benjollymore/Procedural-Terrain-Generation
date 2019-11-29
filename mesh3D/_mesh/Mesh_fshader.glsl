#version 330 core
uniform float time;
uniform sampler2D tex_day;
uniform sampler2D tex_night;

out vec3 color;
in vec3 fpoint;
in vec3 fnormal_cam;

vec3 R = vec3(1,0,0);
vec3 G = vec3(0,1,0);
vec3 B = vec3(0,0,1);
vec3 K = vec3(0,0,0);


void main() {      

    color = mix(R,B,fpoint[2]);
    
    /// Lambertian shading
    vec3 N = normalize(fnormal_cam);
    //vec3 L = vec3(0,0,-1); ///< fixed light pos
    vec3 L = normalize(vec3(cos(time), sin(time), -1)); ///< rotating light
    float lamb = dot(N,L);
    color = color * max(lamb, 0);


    /// Specular shading
    if(lamb>0){
        vec3 V = vec3(0,0,-1); //< we are in camera space
        vec3 R = reflect(V,N);
        float glossy = pow( max(dot(-R,L),0), 100);
        color += vec3(.5)*glossy;
    }
    

}

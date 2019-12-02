R"(
#version 330 core
uniform sampler2D noiseTex;

uniform sampler2D grass;
uniform sampler2D rock;
uniform sampler2D sand;
uniform sampler2D snow;
uniform sampler2D water;

// The camera position
uniform vec3 viewPos;

in vec2 uv;
// Fragment position in world space coordinates
in vec3 fragPos;

out vec4 color;

void main() {

    // Directional light source
    vec3 lightDir = normalize(vec3(1,1,1));

    // Texture size in pixels
    ivec2 size = textureSize(noiseTex, 0);

    /// TODO: Calculate surface normal N
    /// HINT: Use textureOffset(,,) to read height at uv + pixelwise offset
    /// HINT: Account for texture x,y dimensions in world space coordinates (default f_width=f_height=5)
    vec3 A = vec3( 0 );
    vec3 B = vec3( 0 );
    vec3 C = vec3( 0 );
    vec3 D = vec3( 0 );
    vec3 N = normalize( cross(normalize(A-B), normalize(C-D)) );

    /// TODO: Texture according to height and slope
    /// HINT: Read noiseTex for height at uv
	 float h = clamp((texture2D(noiseTex, uv)).r, 0, 1)*.1;

    /// TODO: Calculate ambient, diffuse, and specular lighting
    /// HINT: max(,) dot(,) reflect(,) normalize()

   if(h == 0){
		color = texture(water, uv);
   }
   if (h > 0 && h < .015){
   		color = texture(sand, uv);
   }
   if (h >= .015 && h < .03){
   		color = texture(grass, uv);
   }
   if (h >= .03 && h < .04){
   		color = texture(rock, uv);
   }
   if (h>=.04){
   		color = texture(snow, uv);
   }
   
}
)"

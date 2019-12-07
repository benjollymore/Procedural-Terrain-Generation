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

	float h = clamp((texture2D(noiseTex, uv)).r, 0, 10.0)*.2;
	
    // Directional light source
    vec3 lightDir = normalize(vec3(1,1,1));

    // Texture size in pixels
    ivec2 size = textureSize(noiseTex, 0);

    /// TODO: Calculate surface normal N
    /// HINT: Use textureOffset(,,) to read height at uv + pixelwise offset
    /// HINT: Account for texture x,y dimensions in world space coordinates (default f_width=f_height=5)
    vec3 A = vec3(uv.x, uv.y, h);
    vec3 B = vec3(textureOffset(noiseTex, uv, ivec2(-1,-1)));
    vec3 C = vec3(uv.x, uv.y, h);
    vec3 D = vec3(textureOffset(noiseTex, uv, ivec2(1,1)));
    vec3 N = normalize( cross(normalize(A-B), normalize(C-D)) );

    /// TODO: Texture according to height and slope
    /// HINT: Read noiseTex for height at uv

	//weights for texture blending
	float weightSand = 0.0f;
	float weightSnow = 0.0f;
	float weightRock = 0.0f;
	float weightGrass = 0.0f;
	float weightWater = 0.0f;

	//texture mixing based on height

	/*
	if(h <=0){
		weightWater = 1.0f;
	}
	if (h > 0 && h < .01){
			color = texture(sand, uv);
	}
	if (h > 0  && h < .02){
		color = mix(texture2D(sand, uv), texture2D(grass, uv), texture2D(noiseTex, uv).r);
    }
	if (h >= .02 && h < .03){
		color = texture(grass, uv);
	}
	if(h >= .02 && h < .06){
		color = mix(texture2D(grass, uv), texture2D(rock, uv), texture2D(noiseTex, uv).r);
	}
	if (h >= .06 && h < .09){
   		color = texture(rock, uv);
	}
	if (h>=.09){
   		color = mix(texture2D(rock, uv), texture2D(snow, uv), texture2D(noiseTex, uv).r);
	}
   */


	if(h <=0){
		weightWater = 1.0f;
	}
	else if (h > 0 && h < .01){
		weightSand = 1.0f;
	}
	else if (h >= 0.01 && h < 0.02) {
		weightSand = ((0.02 - h)/0.01);
		weightGrass = 1.0f - ((0.02 - h)/0.01);
	}
	else if (h >= 0.02 && h < 0.05) {
		weightGrass = 1.0f;
	}
	else if (h >= .05 && h < .08){
		weightGrass = (0.08 - h)/0.03;
		weightRock = 1.0f - (0.08 - h)/0.03;
	}
	else if (h >= 0.08 && h < 0.11) {
		weightRock = 1.0f;
	}
	else if (h >= 0.11 && h < .15){
		weightRock = (.15-h)/.04;
		weightSnow = 1.0f - ((.15-h)/.04);
	}
	else {
		weightSnow = 1.0f;
	}


	color = texture2D(sand, uv) * weightSand +
			texture2D(grass, uv) * weightGrass +
			texture2D(rock, uv) * weightRock +
			texture2D(water, uv) * weightWater +
			texture2D(snow, uv) * weightSnow;

    /// TODO: Calculate ambient, diffuse, and specular lighting
    /// HINT: max(,) dot(,) reflect(,) normalize()
	/// Lambertian shading
	//vec3 N = normalize(lightDir);
    vec3 L = vec3(-.4,-.4,-1); ///< fixed light pos
    // vec3 L = normalize(vec3(cos(time), sin(time), -1)); ///< rotating light
    float lamb = dot(N,L);
    color = color * max(lamb, 0.5);

	if(lamb>0){
        vec3 V = vec3(0,0,-1); //< we are in camera space
        vec3 R = reflect(V,N);
        float glossy = pow( max(dot(-R,L),0), 100);
        color += vec4(.4)*glossy;
    }



   
   
}
)"

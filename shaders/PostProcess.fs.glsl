#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;
uniform mat3 kernel;


uniform sampler2D	screenTexture;
uniform int			uEffect;

/*void main()
{ 
	if (uEffect == 0)
		FragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoords)), 1.0);
	else if (uEffect == 1)
		FragColor = vec4(texture(screenTexture, TexCoords).rgb, 1.0);
	else
		FragColor = vec4(vec3(1, 0, 1), 1.0);
}
*/

const float offset = 1.0 / 900.0;  

void main()
{
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
    );

    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i += 3)
	{
        col += sampleTex[i] * kernel[i / 3].x;
        col += sampleTex[i + 1] * kernel[i / 3].y;
        col += sampleTex[i + 2] * kernel[i / 3].z;
	}
    FragColor = vec4(col, 1.0);
}  

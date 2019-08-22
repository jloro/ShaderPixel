#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform mat3        uKernel;
uniform sampler2D	screenTexture;
uniform int			uEffect;
uniform float		uOffset;

void KernelEffect()
{
    vec2 uOffsets[9] = vec2[](
        vec2(-uOffset,  uOffset), // top-left
        vec2( 0.0f,    uOffset), // top-center
        vec2( uOffset,  uOffset), // top-right
        vec2(-uOffset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( uOffset,  0.0f),   // center-right
        vec2(-uOffset, -uOffset), // bottom-left
        vec2( 0.0f,   -uOffset), // bottom-center
        vec2( uOffset, -uOffset)  // bottom-right    
    );

    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + uOffsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i += 3)
	{
        col += sampleTex[i] * uKernel[i / 3].x;
        col += sampleTex[i + 1] * uKernel[i / 3].y;
        col += sampleTex[i + 2] * uKernel[i / 3].z;
	}
    FragColor = vec4(col, 1.0);
}

void main()
{
    if (uEffect == 0)
        FragColor = vec4(texture(screenTexture, TexCoords).rgb, 1.0);
    else if (uEffect == 1)
		FragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoords)), 1.0);
	else if (uEffect == 2)
    {
        FragColor = texture(screenTexture, TexCoords);
        float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
        FragColor = vec4(average, average, average, 1.0);
    }
	else
        KernelEffect();
    
}  

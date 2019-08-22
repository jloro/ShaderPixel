#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D	screenTexture;
uniform int			uEffect;

void main()
{ 
	if (uEffect == 0)
		FragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoords)), 1.0);
	else if (uEffect == 1)
		FragColor = vec4(texture(screenTexture, TexCoords).rgb, 1.0);
	else
		FragColor = vec4(vec3(1, 0, 1), 1.0);
}


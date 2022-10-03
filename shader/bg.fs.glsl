#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D bg_texture;

void main()
{
	FragColor = texture(bg_texture, TexCoord);
}
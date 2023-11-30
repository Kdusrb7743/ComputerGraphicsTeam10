#version 330

layout(location=0) out vec4 FragColor;

in vec3 v_Color;
in vec2 v_UV;

uniform sampler2D u_Texture;	// �ؽ�ó ���÷�

void main()
{
	//FragColor = vec4(v_Color, 1.0);
	FragColor = texture(u_Texture, v_UV);
}

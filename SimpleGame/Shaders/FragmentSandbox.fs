#version 330

layout(location=0) out vec4 FragColor;

in vec2 v_UV;

uniform vec2 u_Point;
uniform vec2 u_Points[3];
uniform float u_Time;

const float c_PI = 3.141592;

void circle()
{
	vec2 newValue = v_UV - u_Points[2];
	float d = length(newValue);
	if(d < 0.1f)
	{
		FragColor = vec4(v_UV.xy, 0.0, 1.0);
	}
	else
	{
		FragColor = vec4(0);
	}
}

void circles()
{
	vec2 newValue = v_UV - u_Point;
	float d = length(newValue);
	FragColor = vec4(sin(30*d));
}

void radar()
{
	vec2 newValue = v_UV - vec2(0.5, 0.5);
	float d = length(newValue);
	float value = sin(2*c_PI*d - 13*u_Time) - 0.9;
	float ring_mask = ceil(value);

	float obj_mask = 0.0;

	for(int i=0; i<3; i++)
	{
		vec2 temp = v_UV - u_Points[i];
		float d = length(temp);
		if(d<0.1)
		{
			obj_mask += 1.0;
		}
	}
	FragColor = vec4(ring_mask*obj_mask + 10*value);
	
}

void main()
{
	radar();
}

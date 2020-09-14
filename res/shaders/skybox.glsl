//vertex
#version 330 core

layout(location = 0) in vec3 aPosition;

out vec2 vPos;

uniform mat4 uPVInvMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

void main()
{
  vPos = aPosition.xy;
  gl_Position = vec4(aPosition, 1.0f);
}

//fragment
#version 330 core

in vec2 vPos;

out vec4 fColor;

uniform mat4 uPVInvMatrix;
uniform vec3 uSunDir = vec3(0.7, 0.7, 0.7);

void main()
{
  vec4 near4 = uPVInvMatrix * vec4(vPos, -1.0f, 1.0);
  vec4 far4 = uPVInvMatrix * vec4(vPos, 1.0f, 1.0);
  vec3 near = vec3(near4) / near4.w;
  vec3 dir = vec3(far4) / far4.w - near;

  vec3 unitDir = normalize(dir);
  float sun = 10 * pow(dot(normalize(uSunDir), unitDir), 400.0);
  float grad = (unitDir.y + 1.0) * 0.5;
  vec3 skycolor = mix(vec3(0.82, 0.49, 0.12), vec3(0.48, 0.23, 0.83), pow(grad, 2.0f));
  vec3 skyboxColor = skycolor;// + max(vec3(sun, sun, 0), vec3(0));
	fColor = vec4(skyboxColor, 1.0);
}

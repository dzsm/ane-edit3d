attribute vec4 vertex;
attribute vec4 color;
attribute vec3 normal;

//uniform mat4 modelView;
//uniform mat4 projection;
uniform mat4 projectionXmodelView;
uniform mat3 normalMatrix;

varying vec3 fnormal;

void main()
{
  gl_FrontColor = color; //vec4(color, 1.0);
  //gl_Position = projection * modelView * vertex;
  gl_Position = projectionXmodelView * vertex;
  fnormal = normalize(normalMatrix * normal);
}

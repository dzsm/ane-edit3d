attribute vec4 vertex;
attribute vec4 color;

uniform mat4 projectionXmodelView;

void main()
{
  gl_FrontColor = color;
  gl_Position = projectionXmodelView * vertex;
}

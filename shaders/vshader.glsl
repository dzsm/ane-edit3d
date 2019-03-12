attribute highp vec3 position;
attribute highp vec4 vertextex;

uniform highp mat4 projectionXmodelView;
uniform highp mat4 projection;
uniform highp mat4 modelView;
uniform highp vec2  viewport;

varying highp vec2 qt_TexCoord0;

// Given a clip coordinate, align the vertex to the nearest pixel center.
void alignToPixelCenter(inout vec4 clipCoord)
{
  // Half pixel increments (clip coord span / [2*numPixels] = [2*w] / [2*l]):
  vec2 inc = abs(clipCoord.w) / vec2(viewport);

  // Fix up coordinates -- pixel centers are at xy = (-w + (2*i + 1) * inc)
  // for the i'th pixel. First find i and floor it. Just solve the above for i:
  ivec2 pixels = ivec2(floor((clipCoord.xy + abs(clipCoord.ww) - inc)
                             / (2. * inc)));

  // Now reapply the equation to obtain a pixel centered offset.
  clipCoord.xy = -abs(clipCoord.ww) + (2. * vec2(pixels) + vec2(1., 1.)) * inc;
}

void main(void)
{

    vec2 offset = vertextex.xy;
    vec2 texCoord = vertextex.zw;

  // Transform to eye coordinates:
  vec4 eyeAnchor = modelView * vec4(position, 1.0);

  // Apply radius;
  eyeAnchor += vec4(-0.2, 0.2, 1.05, 0.);

  // Tranform to clip coordinates
  vec4 clipAnchor = projection * eyeAnchor;

  // Move the anchor to a pixel center:
  alignToPixelCenter(clipAnchor);

  // Align offset to cell centers using the w coordinate:
  // Since w determines whether or not the vertex is clipped, (-w, w) spans
  // the width/height of the display. Using the viewport width/height in pixels,
  // we can properly convert the offset into pixel units.
  vec2 conv = (2. * abs(clipAnchor.w)) / vec2(viewport);

  // Apply the offset:
  gl_Position = clipAnchor + vec4(offset.x * conv.x, offset.y * conv.y, 0., 0.);

  // Pass through the texture coordinate
  qt_TexCoord0 = texCoord;
}


/*


void main(void)
{
    vec2 vertex = vertextex.xy;
    vec2 texcor = vertextex.zw;

    vertex.x /= viewport.x;
    vertex.y /= viewport.y;

    //vertex.x

    //vertex.x *= 10;
    //vertex.y *= 10;




    vec4 coo = modelView*vec4(position,1.0);
    coo += vec4(0,0,1.0,0);

    coo.w = 1.0;

    vec4 po = projection*coo;

    po.w = 1.0;

    gl_Position = po+vec4(vertex.x-0.1,vertex.y+0.1,0,0);//-vec4(-0.01,0.01,0,0);
    gl_Position.z/=10;
    //gl_Position.w = 1.1;

    //gl_Position = vec4(vertex.xy,0.0,0.5);
    //gl_Position.z = 1.0;

    //gl_Position = vec4(vertex,1.0,0);//-vec4(-0.01,0.01,0,0);
    qt_TexCoord0 = texcor;
}

  */

uniform sampler2D texture;
varying highp vec2 qt_TexCoord0;

void main(void)
{
    //gl_FragColor = vec4(1.0,0.0,0.0,1.0); //texture2D(texture, qt_TexCoord0.st);
    gl_FragColor = texture2D(texture, qt_TexCoord0.st);
    //if(gl_FragColor.w<0.01) discard;
}


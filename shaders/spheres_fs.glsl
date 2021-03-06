
varying vec2 v_texCoord;
varying vec4 eyePosition;
varying vec4 fColor;
uniform mat3 normal;
varying float radius;

uniform mat4 projection;

void main()
{
  //float r2 = v_texCoord.x*v_texCoord.x + v_texCoord.y*v_texCoord.y;
  //if (r2<0.5) discard;
  
  // Figure out if we are inside our sphere.
  float zz = 1.0 - v_texCoord.x*v_texCoord.x - v_texCoord.y*v_texCoord.y;
  if (zz <= 0.0)
    discard;

  vec3 N = vec3(v_texCoord, sqrt(zz));
  vec3 L = normalize(vec3(0, 1, 1));
  vec3 E = vec3(0, 0, 1);
  vec3 H = normalize(L + E);
  float df = max(0.0, dot(N, L)); // cos_alpha
  float sf = max(0.0, dot(N, H)); // cos_beta
  vec3 ambient = 0.4 * fColor.xyz;
  vec3 diffuse = 0.55 * fColor.xyz;
  vec3 specular = 0.5 * (vec3(1, 1, 1) - fColor.xyz);
  vec3 color2 = ambient + df * diffuse + pow(sf, 190.0) * specular;
  gl_FragColor = vec4(color2, fColor.w);

  // determine fragment depth
  vec4 pos = eyePosition;
  pos.z += N.z * radius;//The radius is 1.0
  pos = projection * pos;
  //if(abs(pos.x/pos.w)<0.1) discard;
  gl_FragDepth = (pos.z / pos.w + 1.0) / 2.0;
}

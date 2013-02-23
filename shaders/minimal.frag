// http://www.khronos.org/files/opengl-quick-reference-card.pdf

uniform sampler2D colorMap;
uniform sampler2D normalMap;

void main()
{
	gl_FragColor = texture2D(colorMap, gl_TexCoord[0].xy) * texture2D(normalMap, gl_TexCoord[0].xy);
}

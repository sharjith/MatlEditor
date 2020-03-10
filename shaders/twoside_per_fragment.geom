#version 450 core

layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

in vec3 v_position[];
in vec3 v_normal[];
in vec2 v_texCoord2d[];

out vec3 g_normal;
out vec3 g_position;
out vec2 g_texCoord2d;	

noperspective out vec3 g_edgeDistance;
uniform mat4 viewportMatrix; // Viewport matrix

in float clipDistX[];
in float clipDistY[];
in float clipDistZ[];

void main()
{	
	/*
  for(int i=0; i<gl_in.length(); i++)
  {
	g_normal = v_normal[i];
	g_texCoord2d = v_texCoord2d[i];
	g_position = v_position[i];
    gl_Position = gl_in[i].gl_Position;
    EmitVertex();
  }
  EndPrimitive();
  */

  // Transform each vertex into viewport space
	vec3 p0 = vec3(viewportMatrix * (gl_in[0].gl_Position /
	gl_in[0].gl_Position.w));
	vec3 p1 = vec3(viewportMatrix * (gl_in[1].gl_Position /
	gl_in[1].gl_Position.w));
	vec3 p2 = vec3(viewportMatrix * (gl_in[2].gl_Position /
	gl_in[2].gl_Position.w));
	// Find the altitudes (ha, hb and hc)
	float a = length(p1 - p2);
	float b = length(p2 - p0);
	float c = length(p1 - p0);
	float alpha = acos( (b*b + c*c - a*a) / (2.0*b*c) );
	float beta = acos( (a*a + c*c - b*b) / (2.0*a*c) );
	float ha = abs( c * sin( beta ) );
	float hb = abs( c * sin( alpha ) );
	float hc = abs( b * sin( alpha ) );

	// Send the triangle along with the edge distances
	g_edgeDistance = vec3( ha, 0, 0 );
	g_normal = v_normal[0];
	g_texCoord2d = v_texCoord2d[0];
	g_position = v_position[0];
	gl_Position = gl_in[0].gl_Position;
	EmitVertex();
	g_edgeDistance = vec3( 0, hb, 0 );
	g_normal = v_normal[1];
	g_texCoord2d = v_texCoord2d[1];
	g_position = v_position[1];
	gl_Position = gl_in[1].gl_Position;
	EmitVertex();
	g_edgeDistance = vec3( 0, 0, hc );
	g_normal = v_normal[2];
	g_texCoord2d = v_texCoord2d[2];
	g_position = v_position[2];
	gl_Position = gl_in[2].gl_Position;
	EmitVertex();

	EndPrimitive();
}  

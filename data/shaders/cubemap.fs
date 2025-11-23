
varying vec3 v_world_position; // in world space

uniform samplerCube u_texture;
uniform	vec3 u_camera_position;


void main()
{
	//Dirección del punto a la camara, ha pillado el cubo desde dentro 
	vec3 dir = normalize(u_camera_position - v_world_position);

	gl_FragColor = textureCube( u_texture, dir );
}

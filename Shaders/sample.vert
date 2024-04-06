#version 330 core 

//Gets the data at Attrib Index 0
//Converts it and stores it into a Vec3

layout(location = 0) in vec3 aPos;

layout(location = 1) in vec3 vertexNormal;

layout(location = 2) in vec2 aTex;

//pass the tex cord to the fragment shader
out vec2 texCoord;

out vec3 normCoord;

out vec3 fragPos;

//creates a transform variable

uniform mat4 transform;

//Projection matrix
uniform mat4 projection;

//view variable where view matrix will be assigned
uniform mat4 view;

void main(){
	//Multiplying the transformation matrix to
	//the vec4 version of aPos to get the final Pos
	normCoord = mat3(transpose(inverse(transform))) * vertexNormal;

	fragPos = vec3(transform * vec4(aPos,1.0));

	gl_Position = projection * view * transform * vec4(aPos, 1.0);

	//assign the uv
	texCoord = aTex;
} 
    
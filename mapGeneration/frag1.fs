#version 330 core			
in vec4 outColor;
in vec2 TexCoord;
in float boolTexType;

uniform sampler2D texTop;
uniform sampler2D texSide;
uniform sampler2D texDirt;

out vec4 FragColor;
void main(){
	if (boolTexType == 1.0f){
		FragColor = texture(texSide, TexCoord);
	}
	else if (boolTexType == 2.0f){
		FragColor = texture(texDirt, TexCoord);
	}
	else{
		FragColor = texture(texTop, TexCoord);
	}
}


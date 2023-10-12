#version 330 core
out vec4 FragColor;

in vec3 Normal;


struct Material {
    vec4 diffuse;
    vec4 specular;
};

uniform Material material;

void main()
{    

    vec4 result = material.diffuse + material.specular;
    FragColor = result;
}

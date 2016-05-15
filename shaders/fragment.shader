#version 330 core

struct fMaterial{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct fLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 fNormal;
in vec3 fragPosition;
in vec2 textureCoor;

out vec4 color;

uniform fLight light;
uniform fMaterial material;
uniform vec3 viewPos;

void main(){
    // Ambient
    vec3 ambient = material.ambient * light.ambient;
    // Diffuse
    vec3 norm = normalize(fNormal);
    vec3 lightDir = normalize(light.position - fragPosition);
    float diff = max(dot(norm, lightDir),0.0);
    vec3 diffuse = (diff * material.diffuse) * light.diffuse;
    //Specular
    vec3 viewDir = normalize(viewPos - fragPosition);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir),0.0), material.shininess);
    vec3 specular = (spec * material.specular) * light.specular;
    //result
    vec3 result = ambient + diffuse + specular;
    color = vec4(result, 1.0f);
    }

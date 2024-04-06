#version 330 core 

// USING POINT LIGHT

// Texture to be passed
uniform sampler2D tex0;

// Uniforms for lighting
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float ambientStr;
uniform vec3 ambientColor;
uniform float specStr;
uniform float specPhong;
uniform float brightness;
uniform vec3 cameraPos; // Make sure to pass camera position

uniform vec3 dirlightColor; //for dir light

// Inputs from vertex shader
in vec2 texCoord;
in vec3 normCoord;
in vec3 fragPos;

out vec4 FragColor; // Output color

// Function to calculate directional light
vec3 Dirlight(vec3 normal, vec3 viewDir)
{
    vec3 direction = vec3(0.f, 2.f, 1.f);
    vec3 lightDir = normalize(-direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(reflectDir, viewDir), 0.0), specPhong);
    vec3 ambientCol = ambientColor * ambientStr;
    vec3 diffuse = diff * dirlightColor;
    vec3 specColor = spec * specStr * dirlightColor;
    return (diffuse + ambientCol + specColor); // Add return statement
}

// Function to calculate point light
vec3 Pointlight(vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(reflectDir, viewDir), 0.1), specPhong);
    float distance = length(lightPos - fragPos);
    float constant = 1.0f; // Constant attenuation
    float linear = 0.007f; // Linear attenuation
    float quadratic = 0.0002f; // Quadratic attenuation
    float attenuation = brightness / (constant + linear * distance + quadratic * (distance * distance));
    vec3 ambientCol = ambientColor * ambientStr;
    vec3 diffuse = diff * lightColor;
    vec3 specColor = spec * specStr * lightColor;
    ambientCol *= attenuation;
    diffuse *= attenuation;
    specColor *= attenuation;
    return (ambientCol + diffuse + specColor);
}

void main()
{
    vec3 normal = normalize(normCoord);

    vec3 viewDir = normalize(cameraPos - fragPos); // Use cameraPos for view direction

    // Calculate lighting using point light
    vec3 result = Dirlight(normal, viewDir);
    result+=Pointlight(normal, viewDir);

    FragColor = vec4(result, 1.0) * texture(tex0, texCoord); // Output final color with texture
}

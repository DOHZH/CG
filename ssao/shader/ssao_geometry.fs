#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

void main()
{    
    // 生成位置信息到gbuffer
    gPosition = FragPos;
    // 生成法线信息到gbuffer
    gNormal = normalize(Normal);
    // 生成颜色和散射信息到gbuffer
    gAlbedo.rgb = vec3(0.95);
}
#version 330 core
out float FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform vec3 samples[64];

int kernelSize = 64;
float radius = 0.5;
float bias = 0.025;

// 噪声纹理的值通过屏幕大小计算
const vec2 noiseScale = vec2(800.0/4.0, 600.0/4.0); 

uniform mat4 projection;

void main()
{
    // ssao算法
	// 平铺噪声纹理，并根据noisescale缩放
    vec3 fragPos = texture(gPosition, TexCoords).xyz;
    vec3 normal = normalize(texture(gNormal, TexCoords).rgb);
    vec3 randomVec = normalize(texture(texNoise, TexCoords * noiseScale).xyz);
    // 随机铺设噪声纹理，生成TBN矩阵
	//cpp中的noise_texture记得设置为GL_REPEAT，保证噪声纹理能够循环生成
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    // 对核心样本迭代
    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i)
    {
        vec3 samplePos = TBN * samples[i]; // 切线转观察空间
        samplePos = fragPos + samplePos * radius; 
        
        vec4 offset = vec4(samplePos, 1.0);
        offset = projection * offset; // view转裁剪空间
        offset.xyz /= offset.w; // 透视划分
        offset.xyz = offset.xyz * 0.5 + 0.5; // 变换到0.0-1.0
        
        float sampleDepth = texture(gPosition, offset.xy).z; // 获取样本深度值
        
        // range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
		//如果当前深度值大于存储深度值，就添加到贡献因子当中
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;           
    }
    occlusion = 1.0 - (occlusion / kernelSize);
    
    FragColor = occlusion;
}

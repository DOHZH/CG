#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <common/shader.h>
#include <common/camera.h>
#include <common/model.h>

#include <iostream>
#include <random>


//��������
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double x_pos, double y_pos);
void scroll_callback(GLFWwindow* window, double x_offset, double y_offset);
void processInput(GLFWwindow *window);
void renderQuad();
void renderCube();

// ���ô��ڴ�С
const unsigned int width = 1000;
const unsigned int height = 800;

// ���
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
float last_x = (float)width / 2.0;
float last_y = (float)height / 2.0;
bool first_mouse = true;

// ʱ��
float delta_time = 0.0f;
float last_frame = 0.0f;

// ���ٲ�ֵ����
float lerp(float a, float b, float f)
{
    return a + f * (b - a);
}

int main()
{
    // ��ʼ������
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // ��������
    GLFWwindow* window = glfwCreateWindow(width, height, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // ��궯����׽
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // ��ʼ��glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //������Ȳ���
    glEnable(GL_DEPTH_TEST);

    // ����shader
    Shader shaderGeometryPass("shader/ssao_geometry.vs", "shader/ssao_geometry.fs");
    Shader shaderLightingPass("shader/ssao.vs", "shader/ssao_lighting.fs");
    Shader shaderSSAO("shader/ssao.vs", "shader/ssao.fs");
    Shader shaderSSAOBlur("shader/ssao.vs", "shader/ssao_blur.fs");

    Model nanosuit("resources/nanosuit/nanosuit.obj");

    //-----------G_BUFFER---------------
    unsigned int g_buffer;
    glGenFramebuffers(1, &g_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, g_buffer);
    unsigned int g_pos, g_normal, g_albebo;
    // λ����ɫ����
    glGenTextures(1, &g_pos);
    glBindTexture(GL_TEXTURE_2D, g_pos);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g_pos, 0);
    // ���߻���
    glGenTextures(1, &g_normal);
    glBindTexture(GL_TEXTURE_2D, g_normal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, g_normal, 0);
    // ��ɫ+������ɫ����
    glGenTextures(1, &g_albebo);
    glBindTexture(GL_TEXTURE_2D, g_albebo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, g_albebo, 0);
    // �ṩcolor attachments��һ��������Ⱦ����
    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);
    // �����Ⱦ�������Ϊ���buffer
    unsigned int rbo_depth;
    glGenRenderbuffers(1, &rbo_depth);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_depth);
    
    // ֡������
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //------Ϊssao�����ṩ֡����-----
    unsigned int ssao_fbo, ssao_blur_fbo;
    glGenFramebuffers(1, &ssao_fbo);  glGenFramebuffers(1, &ssao_blur_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, ssao_fbo);
    // ssao��ɫ����
    unsigned int ssao_color_buffer, ssao_color_buffer_blur;
    glGenTextures(1, &ssao_color_buffer);
    glBindTexture(GL_TEXTURE_2D, ssao_color_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssao_color_buffer, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Framebuffer not complete!" << std::endl;
    // blur
    glBindFramebuffer(GL_FRAMEBUFFER, ssao_blur_fbo);
    glGenTextures(1, &ssao_color_buffer_blur);
    glBindTexture(GL_TEXTURE_2D, ssao_color_buffer_blur);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssao_color_buffer_blur, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // ���ݷ���������ɲ������ģ�����������
    std::uniform_real_distribution<GLfloat> random_floats(0.0, 1.0); // �������İ뾶Ϊ������ɣ���Χ0.0-1.0
    std::default_random_engine generator;
    std::vector<glm::vec3> ssao_kernel;//ssaoƬ�κ��ģ�����������뾶����Χ���������ֵ��������ֵ

    //���һ��ӵ�����64����ֵ�Ĳ������ģ�
    for (unsigned int i = 0; i < 64; ++i)
    {
        glm::vec3 sample(random_floats(generator) * 2.0 - 1.0, random_floats(generator) * 2.0 - 1.0, random_floats(generator));
        sample = glm::normalize(sample);
        sample *= random_floats(generator);
        float scale = GLfloat(i) / 64.0;
        // ���ٲ�ֵ�������������㣬�����ǵ������㿿��ԭ��ĺ��ģ�����������Ч������
        scale = lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        ssao_kernel.push_back(sample);
    }

    // ��������������ģ����Ӳ����������
    std::vector<glm::vec3> ssao_noise;
    for (unsigned int i = 0; i < 16; i++)
    {
        glm::vec3 noise(random_floats(generator) * 2.0 - 1.0, random_floats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
        ssao_noise.push_back(noise);
    }
    unsigned int noise_texture; 
    glGenTextures(1, &noise_texture);
    glBindTexture(GL_TEXTURE_2D, noise_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssao_noise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // ������Ϣ
    glm::vec3 light_pos = glm::vec3(2.0, 4.0, -2.0);
    glm::vec3 light_color = glm::vec3(0.8, 0.8, 0.6);

    // ����shader:lighting pass
    shaderLightingPass.use();
    shaderLightingPass.setInt("gPosition", 0);
    shaderLightingPass.setInt("gNormal", 1);
    shaderLightingPass.setInt("gAlbedo", 2);
    shaderLightingPass.setInt("ssao", 3);
    shaderSSAO.use();
    shaderSSAO.setInt("gPosition", 0);
    shaderSSAO.setInt("gNormal", 1);
    shaderSSAO.setInt("texNoise", 2);
    shaderSSAOBlur.use();
    shaderSSAOBlur.setInt("ssaoInput", 0);

    // render
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    while (!glfwWindowShouldClose(window))
    {
        //����delta_time��last_time
        float currentFrame = glfwGetTime();
        delta_time = currentFrame - last_frame;
        last_frame = currentFrame;
        processInput(window);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //------geometry pass: ���ε���״����ɫ��Ϣд��g_buffer--------
        glBindFramebuffer(GL_FRAMEBUFFER, g_buffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 50.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        shaderGeometryPass.use();
        shaderGeometryPass.setMat4("projection", projection);
        shaderGeometryPass.setMat4("view", view);
        
        // ���������
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0, 7.0f, 0.0f));
        model = glm::scale(model, glm::vec3(7.5f, 7.5f, 7.5f));
        shaderGeometryPass.setMat4("model", model);
        shaderGeometryPass.setInt("invertedNormals", 1); // invert normals as we're inside the cube
        renderCube();
        shaderGeometryPass.setInt("invertedNormals", 0); 

        // nanosuitģ�ͻ���
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
        model = glm::scale(model, glm::vec3(1.0f));
        shaderGeometryPass.setMat4("model", model);
        nanosuit.Draw(shaderGeometryPass);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //-----------------------------------------

        //------------����ssao������-----------------
        glBindFramebuffer(GL_FRAMEBUFFER, ssao_fbo);
        glClear(GL_COLOR_BUFFER_BIT);
        shaderSSAO.use();
        // ��ssao�İ�������Ϣд��
        for (unsigned int i = 0; i < 64; ++i)
            shaderSSAO.setVec3("samples[" + std::to_string(i) + "]", ssao_kernel[i]);
        shaderSSAO.setMat4("projection", projection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, g_pos);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, g_normal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, noise_texture);
        renderQuad();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //-------------------------------------------


        //----------ssaoģ������ʹ�ñ߽���ӹ⻬------
        glBindFramebuffer(GL_FRAMEBUFFER, ssao_blur_fbo);
        glClear(GL_COLOR_BUFFER_BIT);
        shaderSSAOBlur.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ssao_color_buffer);
        renderQuad();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //----------------------------------------------

        // 4. lighting pass�׶Σ����մ���
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderLightingPass.use();
        // ���ݹ�Դ��Ϣ
        glm::vec3 lightPosView = glm::vec3(camera.GetViewMatrix() * glm::vec4(light_pos, 1.0));
        shaderLightingPass.setVec3("light.Position", lightPosView);
        shaderLightingPass.setVec3("light.Color", light_color);
        // ����˥����Ϣ����
        const float linear    = 0.1;
        const float quadratic = 0.04;
        shaderLightingPass.setFloat("light.Linear", linear);
        shaderLightingPass.setFloat("light.Quadratic", quadratic);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, g_pos);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, g_normal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, g_albebo);
        glActiveTexture(GL_TEXTURE3); 
        glBindTexture(GL_TEXTURE_2D, ssao_color_buffer_blur);
        renderQuad();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

// ��Ⱦ������
unsigned int cube_vao = 0;
unsigned int cube_vbo = 0;
void renderCube()
{
    // ���û��cube�Ķ�����Ϣ����ʼ��
    if (cube_vao == 0)
    {
        GLfloat vertices[] = {
            // ����
            -1.0f, -1.0f, -3.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, 
            1.0f,  1.0f, -3.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, 
            1.0f, -1.0f, -3.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,          
            1.0f,  1.0f, -3.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, 
            -1.0f, -1.0f, -3.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
            -1.0f,  1.0f, -3.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,
            // ǰ��
            -1.0f, -1.0f,  3.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, 
             1.0f, -1.0f,  3.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,
             1.0f,  1.0f,  3.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
             1.0f,  1.0f,  3.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
            -1.0f,  1.0f,  3.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,
            -1.0f, -1.0f,  3.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
            // ����
            -1.0f,  1.0f,  3.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
            -1.0f,  1.0f, -3.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, -3.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, -3.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f,  3.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
            -1.0f,  1.0f,  3.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
            // ����
             1.0f,  1.0f,  3.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
             1.0f, -1.0f, -3.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
             1.0f,  1.0f, -3.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,        
             1.0f, -1.0f, -3.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
             1.0f,  1.0f,  3.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
             1.0f, -1.0f,  3.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,    
            // �ײ�
            -1.0f, -1.0f, -3.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, 
             1.0f, -1.0f, -3.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, 
             1.0f, -1.0f,  3.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, 
             1.0f, -1.0f,  3.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, 
            -1.0f, -1.0f,  3.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,
            -1.0f, -1.0f, -3.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
            // ����
            -1.0f,  1.0f, -3.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
             1.0f,  1.0f , 3.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
             1.0f,  1.0f, -3.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, 
             1.0f,  1.0f,  3.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
            -1.0f,  1.0f, -3.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
            -1.0f,  1.0f,  3.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f
        };

        glGenVertexArrays(1, &cube_vao);
        glGenBuffers(1, &cube_vbo);
        // ���buffer
        glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // ���Ӷ���
        glBindVertexArray(cube_vao);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // ��Ⱦ������
    glBindVertexArray(cube_vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}


//������Ⱦ
unsigned int quad_vao = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quad_vao == 0)
    {
        float quad_vertices[] = {
            // λ��        // ��������
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // ����ƽ��VAO
        glGenVertexArrays(1, &quad_vao);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quad_vao);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quad_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

//���봦��
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, delta_time);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, delta_time);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, delta_time);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, delta_time);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWNROTATE, delta_time);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(UPROTATE, delta_time);
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double x_pos, double y_pos)
{
    if (first_mouse)
    {
        last_x = x_pos;
        last_y = y_pos;
        first_mouse = false;
    }

    float x_offset = x_pos - last_x;
    float y_offset = last_y - y_pos; 

    last_x = x_pos;
    last_y = y_pos;

    camera.ProcessMouseMovement(x_offset, y_offset);
}

void scroll_callback(GLFWwindow* window, double x_offset, double y_offset)
{
    camera.ProcessMouseScroll(y_offset);
}

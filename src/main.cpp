#include<iostream>
#include <map>
#include <string>

#include "../include/luckydog.hpp"
#include "../include/graphics/texture.hpp"
#include "../include/graphics/shader.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <ft2build.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include FT_FREETYPE_H



struct Character {
    GLuint TextureID;   // ID handle of the glyph texture
    glm::ivec2 Size;    // Size of glyph
    glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
    GLuint Advance;    // Horizontal offset to advance to next glyph
};
std::map<GLchar, Character> Characters;
GLuint VAO, VBO;
void RenderText(Shader& shader, const wchar_t* text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
    int len = wcslen(text);
    // FreeType
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

    // Load font as face
    FT_Face face;
    if (FT_New_Face(ft, "fonts/msyh.ttc", 0, &face))
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

    // Set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 0, 48);

    // Disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Load first 128 characters of ASCII set
    for (int i=0; i < len; i++)
    {
        // Load character glyph 
        if (FT_Load_Char(face, text[i], FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        Characters.insert(std::pair<GLchar, Character>(i, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    // Destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);


    // Configure VAO/VBO for texture quads
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Activate corresponding render state	
    shader.use();
    glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // Iterate through all characters
    for (int i = 0; i < len; i++)
    {
        Character ch = Characters[i];

        GLfloat xpos = x + ch.Bearing.x * scale;
        GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;
        // Update VBO for each character
        GLfloat vertices[6][4] = {
            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos,     ypos,       0.0, 1.0 },
            { xpos + w, ypos,       1.0, 1.0 },

            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos + w, ypos,       1.0, 1.0 },
            { xpos + w, ypos + h,   1.0, 0.0 }
        };
        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(800, 640, "Lucky Dog Project - V1.0.0 Beta", NULL, NULL);
    if (window == NULL)
    {
        MessageBoxA(NULL, "Failed to create window.", "Error", MB_OK | MB_ICONERROR);
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        MessageBoxA(NULL, "Failed to initialize GL.", "Error", MB_OK | MB_ICONERROR);
        return -1;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Shader bg_shader("shader/bg.vs.glsl", "shader/bg.fs.glsl");
    float vertices[] = {
        // positions         // texture coords
         1.0f,  1.0f, 0.0f,  1.0f, 1.0f, // top right
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f, // bottom right
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, // bottom left
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f  // top left 
    };
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };
    unsigned int bgVBO, bgVAO, bgEBO;
    glGenVertexArrays(1, &bgVAO);
    glGenBuffers(1, &bgVBO);
    glGenBuffers(1, &bgEBO);

    glBindVertexArray(bgVAO);

    glBindBuffer(GL_ARRAY_BUFFER, bgVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bgEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    Texture bg_tex;
    bg_tex.loadTex("Assets/ui/background.png");

    bg_shader.use();
    bg_shader.setInt("bg_texture", 0);


    // Compile and setup the shader
    Shader shader("shader/text.vs.glsl", "shader/text.fs.glsl");
    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(800), 0.0f, static_cast<GLfloat>(640));
    shader.use();
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));



    App app;
    bool pressed = false;
    float pressedTime = 0;
    app.get_result();
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, bg_tex.texture);

        bg_shader.use();
        glBindVertexArray(bgVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        float deltaTime = glfwGetTime() - pressedTime;

        

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && (!pressed)) {
            pressedTime = glfwGetTime();
            pressed = true;
            app.result_list.clear();
            app.get_result();
           
        }
        if (deltaTime <= 1.8) {
            std::cout << app.result_lista[(int)(deltaTime*5)] << " "<< (int)(deltaTime * 5) << "\n";
            RenderText(shader, (app.result_list[(int)(deltaTime * 5)]).c_str(), 350, 380, 1.0f, glm::vec3(0.3, 0.6f, 0.0f));
        }
        else {
            std::cout << app.result_lista[9] << " eof"<< "\n";
            RenderText(shader, (app.result_list[9]).c_str() , 350, 380, 1.0f, glm::vec3(0.3, 0.6f, 0.0f));
        }
        
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
            pressed = false;
        }
        RenderText(shader, (wchar_t*)((int)glfwGetTime), 0,0, 1.0f, glm::vec3(0.3, 0.6f, 0.0f));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &bgVAO);
    glDeleteBuffers(1, &bgVBO);
    glDeleteBuffers(1, &bgEBO);

	glfwTerminate();

	return 0;
}
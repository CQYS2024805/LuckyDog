#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include<wchar.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

#include "shader.hpp"

struct Word {
    unsigned int texture;
    unsigned int VAO;
};

class Text
{
private:
	Shader shader;
	std::vector<Word> textVecs;
	FT_Face face;
public:
    Text(Shader& text_shader) {
		shader = text_shader;
		glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 640.0f);
		shader.use();
		shader.setMat4("projection", projection);

		FT_Library ft;
		if (FT_Init_FreeType(&ft))
			std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

		if (FT_New_Face(ft, "fonts/msyh.ttc", 0, &face))
			std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

		FT_Set_Pixel_Sizes(face, 0, 48);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		FT_Done_FreeType(ft);
	}
    ~Text()
    {
		FT_Done_Face(face);
    }
	void renderText(const wchar_t* text, GLfloat x, GLfloat y,glm::vec3 color);
};

void Text::renderText(const wchar_t* text, GLfloat x, GLfloat y,glm::vec3 color) {
	shader.setVec3("textColor", color);
	int lew_w = wcslen(text);

	for (GLubyte i = 0; i < lew_w; i++)
	{
		if (FT_Load_Char(face, text[i], FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}

		int font_width = face->glyph->bitmap.width;
		int font_rows = face->glyph->bitmap.rows;

		int font_left = face->glyph->bitmap_left;
		int font_top = face->glyph->bitmap_top;

		int font_x = face->glyph->advance.x;
		//
		unsigned char* data = face->glyph->bitmap.buffer;

		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, font_width, font_rows, 0, GL_RED, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		GLfloat xpos = x + font_left;
		GLfloat ypos = y - (font_rows - font_top);

		GLfloat w = font_width;
		GLfloat h = font_rows;

		GLfloat vertices[6][4] = {
			{ xpos,			     ypos + font_rows,   0.0, 0.0 },
			{ xpos,				 ypos,				 0.0, 1.0 },
			{ xpos + font_width, ypos,				 1.0, 1.0 },

			{ xpos,				 ypos + font_rows,   0.0, 0.0 },
			{ xpos + font_width, ypos,				 1.0, 1.0 },
			{ xpos + font_width, ypos + font_rows,   1.0, 0.0 }
		};

		unsigned int VAO, VBO;
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * 4, &vertices[0][0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		Word fw1;
		fw1.texture = texture;
		fw1.VAO = VAO;
		this->textVecs.push_back(fw1);

		x += (font_x >> 6);
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	for (int i = 0; i < this->textVecs.size(); i++)
	{
		Word wf = this->textVecs[i];

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, wf.texture);

		glBindVertexArray(wf.VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);


		glBindVertexArray(0);
		glActiveTexture(GL_TEXTURE0);
	}
}
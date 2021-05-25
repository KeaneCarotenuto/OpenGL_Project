#include "TextLabel.h"

TextLabel::TextLabel(std::string _text, std::string _font, glm::ivec2 _pixelSize, glm::vec2 _pos, glm::vec3 _color, glm::vec2 _scale)
{
    SetText(_text);
    SetColor(_color);
    SetScale(_scale);
    SetPosition(_pos);

    ProjectionMat = glm::ortho(0.0f, (float)utils::windowWidth, 0.0f, (float)utils::windowHeight, 0.0f, 100.0f);
    Program_Text = ShaderLoader::CreateProgram("Resources/Shaders/Text.vert", "Resources/Shaders/Text.frag");

    FT_Library FontLibrary;
    FT_Face FontFace;

    if (FT_Init_FreeType(&FontLibrary) != 0) {
        std::cout << "FreeType Error: Could not init FreeType Library" << std::endl;
        return;
    }

    if (FT_New_Face(FontLibrary, _font.c_str(), 0, &FontFace) != 0) {
        std::cout << "FreeType Error: Failed to Load Font" << std::endl;
        return;
    }

    FT_Set_Pixel_Sizes(FontFace, _pixelSize.x, _pixelSize.y);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (GLubyte Glyph = 0; Glyph < fontCharacterLimit; Glyph++) {
        if (FT_Load_Char(FontFace, Glyph, FT_LOAD_RENDER)) {
            std::cout << "FreeType Error: Failed to Load Glyph" << (unsigned char)Glyph << std::endl;
            continue;
        }

        FontChar FontCharacter = {
            GenerateTexture(FontFace),
            glm::ivec2(FontFace->glyph->bitmap.width, FontFace->glyph->bitmap.rows),
            glm::ivec2(FontFace->glyph->bitmap_left, FontFace->glyph->bitmap_top),
            (GLuint)FontFace->glyph->advance.x / 64
        };
        CharacterMap.insert(std::pair<GLchar, FontChar>(Glyph, FontCharacter));
    }

    FT_Done_Face(FontFace);
    FT_Done_FreeType(FontLibrary);


	//glGenVertexArrays(1, &m_VAO);
	//glBindVertexArray(m_VAO);

	////Gen EBO 
	//glGenBuffers(1, &m_EBO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	//int* ind = &m_VertexArray.indices[0];
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_VertexArray.indices.size() * sizeof(int), ind, GL_DYNAMIC_DRAW);

	////Gen VBO 
	//glGenBuffers(1, &m_VBO);
	////copy our vertices array in a buffer for OpenGL to use
	//glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	//float* verts = &m_VertexArray.vertices[0];
	//glBufferData(GL_ARRAY_BUFFER, m_VertexArray.vertices.size() * sizeof(float), verts, GL_DYNAMIC_DRAW);

	////Set the vertex attributes pointers (How to interperet Vertex Data)
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	//glEnableVertexAttribArray(2);

    glGenVertexArrays(1, &VAO_Text);
    glBindVertexArray(VAO_Text);

	//Gen EBO 
	glGenBuffers(1, &EBO_Text);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_Text);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

	//Gen VBO 
	glGenBuffers(1, &VBO_DynamicQuad);
	//copy our vertices array in a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO_DynamicQuad);
	glBufferData(GL_ARRAY_BUFFER, 4 * 4 * sizeof(float), NULL, GL_DYNAMIC_DRAW);

    //Set the vertex attributes pointers (How to interperet Vertex Data)
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    m_initialized = true;
}

TextLabel::~TextLabel()
{
}

void TextLabel::Render()
{
    if (m_initialized == false) {
        return;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(Program_Text);
    glUniform3fv(glGetUniformLocation(Program_Text, "TextColor"), 1, glm::value_ptr(m_color));
    glUniformMatrix4fv(glGetUniformLocation(Program_Text, "ProjectionMat"), 1, GL_FALSE, glm::value_ptr(ProjectionMat));
    glBindVertexArray(VAO_Text);

    glm::vec2 CharacterOrigin = m_position;

    for (std::string::const_iterator TextCharacter = m_text.begin(); TextCharacter != m_text.end(); TextCharacter++) {
        FontChar FontCharacter = CharacterMap[*TextCharacter];
        GLfloat PosX = CharacterOrigin.x + FontCharacter.bearing.x * m_scale.x;
        GLfloat PosY = CharacterOrigin.y - (FontCharacter.size.y - FontCharacter.bearing.y) * m_scale.y;
        GLfloat Width = FontCharacter.size.x * m_scale.x;
        GLfloat Height = FontCharacter.size.y * m_scale.y;

        GLfloat vertices[4][4] = {
			{PosX, PosY + Height, 0.0, 0.0}, {PosX, PosY, 0.0, 1.0},
			{PosX + Width, PosY, 1.0, 1.0}, {PosX + Width, PosY + Height, 1.0, 0.0}
        };

        glBindBuffer(GL_ARRAY_BUFFER, VBO_DynamicQuad);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, FontCharacter.textureID);
        glUniform1i(glGetUniformLocation(Program_Text, "TextTexture"), 0);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        CharacterOrigin.x += FontCharacter.advance * m_scale.x;
    }

    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glDisable(GL_BLEND);
}

GLuint TextLabel::GenerateTexture(FT_Face _face)
{
    GLuint TextureID;
    glGenTextures(1, &TextureID);
    glBindTexture(GL_TEXTURE_2D, TextureID);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RED,
        _face->glyph->bitmap.width,
        _face->glyph->bitmap.rows,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        _face->glyph->bitmap.buffer
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return TextureID;
}
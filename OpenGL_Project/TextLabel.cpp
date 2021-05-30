#include "TextLabel.h"

TextLabel::TextLabel(std::string _text, std::string _font, glm::ivec2 _pixelSize, glm::vec2 _pos, glm::vec3 _color, glm::vec2 _scale):
    m_copyPosition(_pos), m_pixelSize(_pixelSize), m_copyScale(_scale)
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


    //Disable depth test for text
    GLboolean _copyOfDepthTest = glIsEnabled(GL_DEPTH_TEST);
    if (_copyOfDepthTest) glDisable(GL_DEPTH_TEST);
    
    

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(Program_Text);
    glUniform3fv(glGetUniformLocation(Program_Text, "TextColor"), 1, glm::value_ptr(m_color));
    glUniformMatrix4fv(glGetUniformLocation(Program_Text, "ProjectionMat"), 1, GL_FALSE, glm::value_ptr(ProjectionMat));
    glBindVertexArray(VAO_Text);

    glUniform2fv(glGetUniformLocation(Program_Text, "XCliping"), 1, glm::value_ptr(glm::vec2(0, 800)));
    glUniform2fv(glGetUniformLocation(Program_Text, "CharacterSize"), 1, glm::value_ptr(m_pixelSize));

    glm::vec2 CharacterOrigin = m_position;

    m_height = 0.0f;
    m_width = 0.0f;

    m_unscaledHeight = 0;
    m_unscaledWidth = 0;

    for (std::string::const_iterator TextCharacter = m_text.begin(); TextCharacter != m_text.end(); TextCharacter++) {
        FontChar FontCharacter = CharacterMap[*TextCharacter];
        GLfloat PosX = CharacterOrigin.x + FontCharacter.bearing.x * m_scale.x;
        GLfloat PosY = CharacterOrigin.y - (FontCharacter.size.y - FontCharacter.bearing.y) * m_scale.y;
        GLfloat Width = FontCharacter.size.x * m_scale.x;
        GLfloat Height = FontCharacter.size.y * m_scale.y;

        if (Height > m_height) m_height = Height;
        if (FontCharacter.size.y > m_unscaledHeight) m_unscaledHeight = FontCharacter.size.y;

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

        m_width += FontCharacter.advance * m_scale.x;
        m_unscaledWidth += FontCharacter.advance;
    }

    glUseProgram(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glDisable(GL_BLEND);

    //Re enable depth testing if it was one previously
    if (_copyOfDepthTest) glEnable(GL_DEPTH_TEST);
}

void TextLabel::Update(float deltaTime, float currentTime)
{
    if (m_bounceText) {
        if (GetUnscaledWidth() > 0 && GetUnscaledHeight() > 0) {
            glm::vec2 pos = GetCopyPos();

            float width = GetUnscaledWidth() * GetCopyScale().x;
            float height = GetUnscaledHeight() * GetCopyScale().y;

            float sinVal = (((sin(currentTime - glm::half_pi<float>()) + 1) / 2)) + 1;

            SetScale(sinVal * GetCopyScale());
            SetPosition(glm::vec2(pos.x + (width/2) - (float)(sinVal * width / 2), pos.y + (height/2) - (float)(sinVal * height / 2)));
        }
    }
}

std::string TextLabel::GetText()
{
    return m_text;
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
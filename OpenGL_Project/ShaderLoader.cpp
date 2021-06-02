#include "ShaderLoader.h" 

ShaderLoader::ShaderLoader(void){}
ShaderLoader::~ShaderLoader(void){}

std::vector<CShader*> Globals::shaders;
std::map<std::string, CProgram*> Globals::programs;

/// <summary>
/// Create program with a Vertex Shader, and a Fragment Shader
/// </summary>
/// <param name="vertexShaderFilename"></param>
/// <param name="fragmentShaderFilename"></param>
/// <returns></returns>
GLuint ShaderLoader::CreateProgram(std::string _name, const char* vertexShaderFilename, const char* fragmentShaderFilename)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	std::cout << "Start Program Creation:" << std::endl;

	//Init vars
	GLuint vert_shader = NULL;
	GLuint frag_shader = NULL;
	CShader* vShader = nullptr;
	CShader* fShader = nullptr;

	//Check if program/shaders exists already, if so, use them
	for (auto it = Globals::programs.begin(); it != Globals::programs.end(); ++it)
	{
		if (it->second) {
			GLuint TEMP_vert_shader = NULL;
			GLuint TEMP_frag_shader = NULL;

			for (CShader* _shader : it->second->m_shaders)
			{
				//if vert shader exists
				if (_shader) {
					if (_shader->m_fileName == vertexShaderFilename) {
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
						std::cout << "--Vertex shader already exists, using existing." << std::endl;
						vert_shader = TEMP_vert_shader = _shader->m_id;
						vShader = _shader;
					}

					//If frag shader exists
					if (_shader->m_fileName == fragmentShaderFilename) {
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
						std::cout << "--Fragment shader already exists, using existing." << std::endl;
						frag_shader = TEMP_frag_shader = _shader->m_id;
						fShader = _shader;
					}
				}
			}

			//If both exist AKA program exists
			if (TEMP_vert_shader != NULL && TEMP_frag_shader != NULL) {
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
				std::cout << "-Program already exists, using existing." << std::endl;
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
				std::cout << "End Program Creation." << std::endl << std::endl;
				return it->second->m_id;
			}
		}
	}

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	std::cout << "-Creating new Program" << std::endl;

	//Create shaders if needed
	if (vert_shader == NULL) {
		vert_shader = CreateShader(GL_VERTEX_SHADER, vertexShaderFilename, &vShader);
	}

	if (frag_shader == NULL) {
		frag_shader = CreateShader(GL_FRAGMENT_SHADER, fragmentShaderFilename, &fShader);
	}
	
	// Create the program handle, attach the shaders and link it
	GLuint program = glCreateProgram();
	glAttachShader(program, vert_shader);
	glAttachShader(program, frag_shader);

	glLinkProgram(program);

	// Check for link errors
	int link_result = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &link_result);
	if (link_result == GL_FALSE)
	{
		std::string programName = vertexShaderFilename + *fragmentShaderFilename;
		PrintErrorDetails(false, program, programName.c_str());
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
		std::cout << "End Program Creation." << std::endl << std::endl;
		
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
		return 0;
	}

	Globals::programs[_name] = (new CProgram(program, std::vector<CShader*>{vShader, fShader}));

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
	std::cout << "--Program Created" << std::endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	std::cout << "End Program Creation." << std::endl << std::endl;
	return program;
}

/// <summary>
/// Returns program of name
/// </summary>
/// <param name="_name"></param>
/// <returns></returns>
CProgram* ShaderLoader::GetProgram(std::string _name)
{
	std::map<std::string, CProgram*>::iterator it = Globals::programs.find(_name);
	if (it != Globals::programs.end()) {
		return it->second;
	}
	{
		std::cout << "ERROR: Failed to get program named " << _name << "." << std::endl;
		return nullptr;
	}
}

/// <summary>
/// Create shader from file
/// </summary>
/// <param name="shaderType">e.g. GL_VERTEX_SHADER</param>
/// <param name="shaderName">The file name</param>
/// <param name="_shaderReturn">Returns CShader pointer</param>
/// <returns></returns>
GLuint ShaderLoader::CreateShader(GLenum shaderType, const char* shaderName, CShader ** _shaderReturn = nullptr)
{
	std::string shaderTypeName = (std::string)(shaderType == GL_VERTEX_SHADER ? "Vertex" : (shaderType == GL_FRAGMENT_SHADER ? "Fragment" : "?"));

	//This is redundant, but better to be safe than sorry I suppose
	for (CShader* _shader : Globals::shaders)
	{
		if (_shader) {
			if (_shader->m_fileName == shaderName) {
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
				std::cout << "--" + shaderTypeName + " shader already exists, using existing. (Missed in CreateProgram()!)" << std::endl;
				*_shaderReturn = _shader;
				return _shader->m_id;
			}
		}
	}

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	std::cout << "--Creating new " + shaderTypeName + " Shader" << std::endl;

	// Read the shader files and save the source code as strings
	std::string shaderSourceCode = ReadShaderFile(shaderName);
	
	//For hiding shader files vv
	//std::string shaderSourceCode = (shaderType == GL_VERTEX_SHADER ? "#version 460 core\n\nlayout(location = 0) in vec3 Pos; \nlayout(location = 1) in vec3 Col; \n\nout vec3 FragColor; \n\nvoid main() \n{ \n\tgl_Position = vec4(Pos, 1.0); \n\tFragColor = Col; \n }" : "#version 460 core\n\nin vec3 FragColor;\nuniform float CurrentTime;\n\nout vec4 FinalColor;\n\nvoid main() \n{\n\tFinalColor = vec4(FragColor, 1.0f) * (sin(CurrentTime) + 1);\n}");

	// Create the shader ID and create pointers for source code string and length
	GLuint shaderID = glCreateShader(shaderType);
	const GLchar* shader_code_ptr = shaderSourceCode.c_str();
	const GLint shader_code_size = shaderSourceCode.length();

	// Populate the Shader Object (ID) and compile
	glShaderSource(shaderID, 1, &shader_code_ptr, NULL);
	glCompileShader(shaderID);

	// Check for errors
	int compile_result = 0;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compile_result);
	if (compile_result == GL_FALSE)
	{
		PrintErrorDetails(true, shaderID, shaderName);
		return 0;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
		std::cout << "---Shader Failed" << std::endl;
	}
	*_shaderReturn = new CShader(shaderID, shaderName, shaderSourceCode);
	Globals::shaders.push_back(*_shaderReturn);

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
	std::cout << "---Shader Created" << std::endl;

	return shaderID;
}

/// <summary>
/// Read Shader file...
/// </summary>
/// <param name="filename"></param>
/// <returns></returns>
std::string ShaderLoader::ReadShaderFile(const char *filename)
{
	// Open the file for reading
	std::ifstream file(filename, std::ios::in);
	std::string shaderCode;

	// Ensure the file is open and readable
	if (!file.good()) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
		std::cout << "Cannot read file:  " << filename << std::endl;
		return "";
	}

	// Determine the size of of the file in characters and resize the string variable to accomodate
	file.seekg(0, std::ios::end);
	shaderCode.resize((unsigned int)file.tellg());

	// Set the position of the next character to be read back to the beginning
	file.seekg(0, std::ios::beg);
	// Extract the contents of the file and store in the string variable
	file.read(&shaderCode[0], shaderCode.size());
	file.close();
	return shaderCode;
}

void ShaderLoader::PrintErrorDetails(bool isShader, GLuint id, const char* name)
{
	int infoLogLength = 0;
	// Retrieve the length of characters needed to contain the info log
	(isShader == true) ? glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogLength) : glGetProgramiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);
	std::vector<char> log(infoLogLength);

	// Retrieve the log info and populate log variable
	(isShader == true) ? glGetShaderInfoLog(id, infoLogLength, NULL, &log[0]) : glGetProgramInfoLog(id, infoLogLength, NULL, &log[0]);	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
	std::cout << "Error compiling " << ((isShader == true) ? "shader" : "program") << ": " << name << std::endl;
	std::cout << &log[0] << std::endl;
}


//Constructors for Shaders and Programs

CShader::CShader(GLuint _id, const char* _fileName, std::string _shaderString)
{
	m_id = _id;
	m_fileName = _fileName;
	m_shaderString = _shaderString;
}

CShader::~CShader()
{
}

CProgram::CProgram(GLuint _id, std::vector<CShader*> _shaders)
{
	m_id = _id;
	m_shaders = _shaders;
}

CProgram::~CProgram()
{
}

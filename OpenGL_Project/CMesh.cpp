#include "CMesh.h"
#include <stb_image.h>
#include "Utility.h"

std::map<std::string, CMesh*> CMesh::meshMap;

CMesh::CMesh(VertType _type, std::vector<float> _vertices, std::vector<int> _indices, bool defaultBind = true) {
	type = _type;
	m_VertexArray.vertices = _vertices;
	m_VertexArray.indices = _indices;

	GenBindVerts();
}

/// <summary>
/// Bind all verts to buffers
/// </summary>
void CMesh::GenBindVerts()
{
	if (type == VertType::Patches) {
		glPatchParameteri(GL_PATCH_VERTICES, 4);
	}

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	//Gen EBO 
	glGenBuffers(1, &m_EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	int* ind = &m_VertexArray.indices[0];
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_VertexArray.indices.size() * sizeof(int), ind, GL_DYNAMIC_DRAW);

	//Gen VBO 
	glGenBuffers(1, &m_VBO);
	//copy our vertices array in a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	float* verts = &m_VertexArray.vertices[0];
	glBufferData(GL_ARRAY_BUFFER, m_VertexArray.vertices.size() * sizeof(float), verts, GL_DYNAMIC_DRAW);

	//Set the vertex attributes pointers (How to interperet Vertex Data)
	switch (type)
	{
	case VertType::Pos_Col_Tex:
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
		break;

	case VertType::Pos_Tex_Norm:
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
		break;

	case VertType::Pos:
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
		glEnableVertexAttribArray(0);
		break;

	case VertType::Pos_Col:
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
		break;

	case VertType::Patches:
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
		glEnableVertexAttribArray(0);
		break;

	default:
		break;
	}

	//Unbind vertex array
	glBindVertexArray(0);

	
}

/// <summary>
/// Creates new mesh
/// </summary>
/// <param name="_name"> name of mesh</param>
/// <param name="_vertices"> the list of verts</param>
/// <param name="_indices"> the list of indicies</param>
void CMesh::NewCMesh(std::string _name, VertType _type, std::vector<float> _vertices, std::vector<int> _indices)
{
	meshMap[_name] = new CMesh(_type, _vertices, _indices);
}

/// <summary>
/// Creates a regular polygon based off number of verts
/// </summary>
/// <param name="_verts"> how many verts </param>
void CMesh::NewCMesh(int _verts)
{
	float angle = 360.0f / (float)_verts;

	CVertexArray tempVertArray;

	tempVertArray.vertices = { 0.0f, 0.0f, 0.0f,		1.0f, 1.0f, 1.0f,		0.5f, 0.5f, };

	float near1 = 0;

	//Create all verticies, check which if any are equal to 1 on x or y, if not, scale the verts so that at least 1 coord x/y = 1 (otherwise images arent applied correctly)
	for (int i = 1; i <= _verts; i++) {
		float xCoord = cos((((float)i - 2) * angle + angle * 0.5f) * (float)M_PI / 180.0f);
		float yCoord = sin((((float)i - 2) * angle + angle * 0.5f) * (float)M_PI / 180.0f);

		if (abs(xCoord) >= 0.99f) continue;

		if (abs(xCoord) > near1) near1 = abs(xCoord);
		else if (abs(yCoord) > near1) near1 = abs(yCoord);
	}

	//For each vert desired, calculate its position and send info to the list
	for (int i = 1; i <= _verts; i++) {
		//Calc x y pos
		float xCoord = cos((((float)i - 2) * angle + angle * 0.5f) * (float)M_PI / 180.0f);
		float yCoord = sin((((float)i - 2) * angle + angle * 0.5f) * (float)M_PI / 180.0f);

		//Apply position
		float xPos = xCoord;
		float yPos = yCoord;
		float zPos = 0;

		//Make white
		float rCol = 1.0f;
		float gCol = 1.0f;
		float bCol = 1.0f;

		//Adjust x y pos for image points
		float xTex = (xCoord / (near1 > 0 ? near1 : 1) + 1) / 2;
		float yTex = (yCoord / (near1 > 0 ? near1 : 1) + 1) / 2;


		//Push all data
		tempVertArray.vertices.push_back(xPos);
		tempVertArray.vertices.push_back(yPos);
		tempVertArray.vertices.push_back(zPos);

		tempVertArray.vertices.push_back(rCol);
		tempVertArray.vertices.push_back(gCol);
		tempVertArray.vertices.push_back(bCol);

		tempVertArray.vertices.push_back(xTex);
		tempVertArray.vertices.push_back(yTex);

		tempVertArray.indices.push_back(0);
		tempVertArray.indices.push_back(i);
		tempVertArray.indices.push_back((i + 1 > _verts ? 1 : i + 1));
	}

	meshMap["poly" + std::to_string(_verts)] = new CMesh(VertType::Pos_Col_Tex ,tempVertArray.vertices, tempVertArray.indices);
}


/// <summary>
/// Creates a sphere
/// </summary>
/// <param name="_name"></param>
/// <param name="_radius"></param>
/// <param name="_fidelity"></param>
void CMesh::NewCMesh(std::string _name, float _radius, int _fidelity)
{
	int VertexAttrib = 8;	// Float components are needed for each vertex point
	int IndexPerQuad = 6;	// Indices needed to create a quad

	// Angles to keep track of the sphere points 
	float Phi = 0.0f;
	float Theta = 0.0f;

	// Create the vertex array to hold the correct number of elements based on the fidelity of the sphere
	int VertexCount = _fidelity * _fidelity * VertexAttrib;
	GLfloat* Vertices = new GLfloat[VertexCount];
	int Element = 0;

	// Each cycle moves down on the vertical (Y axis) to start the next ring
	for (int i = 0; i < _fidelity; i++)
	{
		// A new  horizontal ring starts at 0 degrees
		Theta = 0.0f;

		// Creates a horizontal ring and adds each new vertex point to the vertex array
		for (int j = 0; j < _fidelity; j++)
		{
			// Calculate the new vertex position point with the new angles
			float x = cos(Phi) * sin(Theta);
			float y = cos(Theta);
			float z = sin(Phi) * sin(Theta);

			// Set the position of the current vertex point ****(NOTE: This code was taken from the "Sphere.cpp" file provided to us, any warnings you see here were also present there)
			Vertices[Element++] = x * _radius;
			Vertices[Element++] = y * _radius;
			Vertices[Element++] = z * _radius;

			// Set the texture coordinates of the current vertex point
			Vertices[Element++] = (float)i / (_fidelity - 1);
			Vertices[Element++] = 1 - ((float)j / (_fidelity - 1)); // 1 minus in order to flip the direction of 0-1 (0 at the bottom)

			// Set the normal direction of the current vertex point
			Vertices[Element++] = x;
			Vertices[Element++] = y;
			Vertices[Element++] = z;

			// Theta (Y axis) angle is incremented based on the angle created by number of sections
			// As the sphere is built ring by ring, the theta is only needed to do half the circumferance therefore using just PI
			Theta += ((float)M_PI / ((float)_fidelity - 1.0f));
		}

		// Phi angle (X and Z axes) is incremented based on the angle created by the number of sections
		// Angle uses 2*PI to get the full circumference as this layer is built as a full ring
		Phi += (2.0f * (float)M_PI) / ((float)_fidelity - 1.0f);
	}

	// Create the index array to hold the correct number of elements based on the fidelity of the sphere
	int IndexCount = _fidelity * _fidelity * IndexPerQuad;
	GLuint* Indices = new GLuint[IndexCount];

	Element = 0;	// Reset the element offset for the new array
	for (int i = 0; i < _fidelity; i++)
	{
		for (int j = 0; j < _fidelity; j++)
		{
			// First triangle of the quad
			Indices[Element++] = (((i + 1) % _fidelity) * _fidelity) + ((j + 1) % _fidelity);
			Indices[Element++] = (i * _fidelity) + (j);
			Indices[Element++] = (((i + 1) % _fidelity) * _fidelity) + (j);

			// Second triangle of the quad
			Indices[Element++] = (i * _fidelity) + ((j + 1) % _fidelity);
			Indices[Element++] = (i * _fidelity) + (j);
			Indices[Element++] = (((i + 1) % _fidelity) * _fidelity) + ((j + 1) % _fidelity);
		}
	}

	std::vector<float> verts;
	for (int i = 0; i < VertexCount; i++) {
		verts.push_back(Vertices[i]);
	}

	std::vector<int> inds;
	for (int i = 0; i < IndexCount; i++) {
		inds.push_back(Indices[i]);
	}

	CMesh* tempPointer = new CMesh(VertType::Pos_Tex_Norm ,verts, inds);
	std::string tempName = (_name == "" ? "sphere-" + std::to_string(_radius) + "-" + std::to_string(_fidelity) : _name);
	CMesh::meshMap[tempName] = tempPointer;

	// Clean up the used memory
	delete[] Vertices;
	delete[] Indices;
}

/// <summary>
/// Create a terrain mesh with given size and divisions, using perlin noise
/// </summary>
/// <param name="_name"></param>
/// <param name="_width"></param>
/// <param name="_length"></param>
/// <param name="_divW"></param>
/// <param name="_divL"></param>
void CMesh::NewPlane(std::string _name, float _width, float _length, int _divW, int _divL)
{
	//Calculate the number of vertices and indices
	int numVerts = (_divW + 1) * (_divL + 1);
	int numIndices = _divW * _divL * 6;

	//Create the vertex array
	std::vector<float> verts;
	verts.resize(numVerts * 8);

	//Create the index array
	std::vector<int> inds;
	inds.resize(numIndices);

	//Calculate the width and length of each division
	float widthDiv = _width / _divW;
	float lengthDiv = _length / _divL;

	int seed = std::rand() % 100;

	//Loop through each division
	for (int i = 0; i <= _divL; i++)
	{
		for (int j = 0; j <= _divW; j++)
		{
			//Calculate the position of the current vertex
			float x = j * widthDiv;
			float z = i * lengthDiv;
			float y = utils::FinalNoise2D(x,z, seed, 8, 0.5, 100);

			//Set the position of the current vertex
			verts[(i * (_divW + 1) + j) * 8 + 0] = x;
			verts[(i * (_divW + 1) + j) * 8 + 1] = y;
			verts[(i * (_divW + 1) + j) * 8 + 2] = z;


			//Set the texture coordinates of the current vertex
			verts[(i * (_divW + 1) + j) * 8 + 3] = (float)j / (_divW + 10);
			verts[(i * (_divW + 1) + j) * 8 + 4] = (float)i / (_divL + 10);


			//Getting all connected particles to the current vertex
			glm::vec3 middle = glm::vec3(x, utils::FinalNoise2D(x, z, seed, 8, 0.5, 100), z);
			glm::vec3 topLeft = glm::vec3(x - widthDiv, utils::FinalNoise2D(x - widthDiv, z - lengthDiv, seed, 8, 0.5, 100), z - lengthDiv);
			glm::vec3 top = glm::vec3(x, utils::FinalNoise2D(x, z - lengthDiv, seed, 8, 0.5, 100), z - lengthDiv);
			glm::vec3 topRight = glm::vec3(x + widthDiv, utils::FinalNoise2D(x + widthDiv, z - lengthDiv, seed, 8, 0.5, 100), z - lengthDiv);
			glm::vec3 left = glm::vec3(x - widthDiv, utils::FinalNoise2D(x - widthDiv, z, seed, 8, 0.5, 100), z);
			glm::vec3 right = glm::vec3(x + widthDiv, utils::FinalNoise2D(x + widthDiv, z, seed, 8, 0.5, 100), z);
			glm::vec3 bottomLeft = glm::vec3(x - widthDiv, utils::FinalNoise2D(x - widthDiv, z + lengthDiv, seed, 8, 0.5, 100), z + lengthDiv);
			glm::vec3 bottom = glm::vec3(x, utils::FinalNoise2D(x, z + lengthDiv, seed, 8, 0.5, 100), z + lengthDiv);
			glm::vec3 bottomRight = glm::vec3(x + widthDiv, utils::FinalNoise2D(x + widthDiv, z + lengthDiv, seed, 8, 0.5, 100), z + lengthDiv);

			//calculate the normal of each triangle connected to the current vertex
			glm::vec3 normal1 = glm::normalize(glm::cross(middle - topLeft, middle - top));
			glm::vec3 normal2 = glm::normalize(glm::cross(middle - top, middle - topRight));
			glm::vec3 normal3 = glm::normalize(glm::cross(middle - topRight, middle - right));
			glm::vec3 normal4 = glm::normalize(glm::cross(middle - right, middle - bottomRight));
			glm::vec3 normal5 = glm::normalize(glm::cross(middle - bottomRight, middle - bottom));
			glm::vec3 normal6 = glm::normalize(glm::cross(middle - bottom, middle - bottomLeft));
			glm::vec3 normal7 = glm::normalize(glm::cross(middle - bottomLeft, middle - left));
			glm::vec3 normal8 = glm::normalize(glm::cross(middle - left, middle - topLeft));

			//add all the normals together
			glm::vec3 m_normal = normal1 + normal2 + normal3 + normal4 + normal5 + normal6 + normal7 + normal8;

			//normalize the normal
			m_normal = glm::normalize(-m_normal);

			//set the normal of the current vertex
			verts[(i * (_divW + 1) + j) * 8 + 5] = m_normal.x;
			verts[(i * (_divW + 1) + j) * 8 + 6] = m_normal.y;
			verts[(i * (_divW + 1) + j) * 8 + 7] = m_normal.z;

		}
	}

	//Loop through each division
	for (int i = 0; i < _divL; i++)
	{
		for (int j = 0; j < _divW; j++)
		{
			//Calculate the indices of the current quad
			int a = (i * (_divW + 1) + j);
			int b = (i * (_divW + 1) + j + 1);
			int c = ((i + 1) * (_divW + 1) + j);
			int d = ((i + 1) * (_divW + 1) + j + 1);

			//Set the indices of the current quad
			inds[(i * _divW + j) * 6 + 5] = a;
			inds[(i * _divW + j) * 6 + 4] = b;
			inds[(i * _divW + j) * 6 + 3] = c;
			inds[(i * _divW + j) * 6 + 2] = b;
			inds[(i * _divW + j) * 6 + 1] = d;
			inds[(i * _divW + j) * 6 + 0] = c;
		}
	}

	std::vector<float> verts2;
	for (int i = 0; i < verts.size(); i++) {
		verts2.push_back(verts[i]);
	}

	std::vector<int> inds2;
	for (int i = 0; i < inds.size(); i++) {
		inds2.push_back(inds[i]);
	}

	CMesh* tempPointer = new CMesh(VertType::Pos_Tex_Norm, verts2, inds2);
	std::string tempName = (_name == "" ? "plane-" + std::to_string(_width) + "-" + std::to_string(_length) + "-" + std::to_string(_divW) + "-" + std::to_string(_divL) : _name);
	tempPointer->m_width = _width;
	tempPointer->m_length = _length;

	tempPointer->m_widthDivs = _divW;
	tempPointer->m_lengthDivs = _divL;

	CMesh::meshMap[tempName] = tempPointer;
}

			


/// <summary>
/// Return mesh
/// </summary>
/// <param name="_name"> name of mesh to find</param>
/// <param name="_doesExist"> pass in bool to check if it does exist (or just check for nullptr)</param>
/// <returns></returns>
CMesh* CMesh::GetMesh(std::string _name, bool* _doesExist)
{
	std::map<std::string, CMesh*>::iterator it = meshMap.find(_name);
	if (it != meshMap.end()) {
		if (_doesExist)(*_doesExist) = true;
		return it->second;
	}
	else {
		if (_doesExist)(*_doesExist) = false;
		return nullptr;
	}
}

/// <summary>
/// Render specific mesh
/// </summary>
void CMesh::Render()
{
	glBindVertexArray(GetVAO());
	switch (type)
	{
	case VertType::Pos_Col_Tex:
		glDrawElements(GL_TRIANGLES, GetIndices().size(), GL_UNSIGNED_INT, 0);
		break;
	case VertType::Pos_Tex_Norm:
		glDrawElements(GL_TRIANGLES, GetIndices().size(), GL_UNSIGNED_INT, 0);
		break;
	case VertType::Pos:
		glDrawElements(GL_TRIANGLES, GetIndices().size(), GL_UNSIGNED_INT, 0);
		break;
	case VertType::Pos_Col:
		glDrawElements(GL_POINTS, GetIndices().size(), GL_UNSIGNED_INT, 0);
		break;
	case VertType::Patches:
		glDrawElements(GL_PATCHES, GetIndices().size(), GL_UNSIGNED_INT, 0);
		break;
	default:
		break;
	}
	glBindVertexArray(0);
}

#include "CMesh.h"

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

	default:
		break;
	}

	
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
	glDrawElements(GL_TRIANGLES, GetIndices().size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

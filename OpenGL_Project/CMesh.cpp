#include "CMesh.h"

std::map<std::string, CMesh*> CMesh::meshMap;

CMesh::CMesh(std::vector<float> _vertices, std::vector<int> _indices) {
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
}

/// <summary>
/// Creates new mesh
/// </summary>
/// <param name="_name"> name of mesh</param>
/// <param name="_vertices"> the list of verts</param>
/// <param name="_indices"> the list of indicies</param>
void CMesh::NewCMesh(std::string _name, std::vector<float> _vertices, std::vector<int> _indices)
{
	meshMap[_name] = new CMesh(_vertices, _indices);
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

	meshMap["poly" + std::to_string(_verts)] = new CMesh(tempVertArray.vertices, tempVertArray.indices);
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

#include "GeometryGenerator.h"
#include <algorithm>
using namespace DirectX;

void GeometryGenerator::CreateGrid(float width, float depth, UINT m, UINT n, MeshData& meshData)
{
	UINT vertexCount = m * n;
	UINT faceCount = (m - 1) * (n - 1) * 2;
	//
	// Create the vertices.
	//
	float halfWidth = 0.5f * width;
	float halfDepth = 0.5f * depth;
	float dx = width / (n - 1);
	float dz = depth / (m - 1);
	float du = 1.0f / (n - 1);
	float dv = 1.0f / (m - 1);
	meshData.Vertices.resize(vertexCount);
	for (UINT i = 0; i < m; ++i)
	{
		float z = halfDepth - i * dz;
		for (UINT j = 0; j < n; ++j)
		{
			float x = -halfWidth + j * dx;
			meshData.Vertices[i * n + j].Position = XMFLOAT3(x, 0.0f, z);

			// Ignore for now, used for lighting.
			meshData.Vertices[i * n + j].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			meshData.Vertices[i * n + j].TangentU = XMFLOAT3(1.0f, 0.0f, 0.0f);
			// Ignore for now, used for texturing.
			meshData.Vertices[i * n + j].TexC.y = j * du;
			meshData.Vertices[i * n + j].TexC.x = i * dv;
		}
	}
	meshData.Indices.resize(faceCount * 3); // 3 indices per face
	// Iterate over each quad and compute indices.
	LinkGrid(m, n, 0, meshData);

}

void GeometryGenerator::GreateRevolvingSolid(std::vector<XMFLOAT2> line, UINT slice, MeshData& meshData)
{
	slice = std::max(3u, slice);

	UINT pointCount = line.size();
	meshData.Vertices.resize(pointCount * slice + 2);
	UINT topV = pointCount * slice;
	UINT bottomV = pointCount * slice + 1;
	meshData.Vertices[topV].Position = XMFLOAT3(0.0f, line[0].y, 0.0f);
	meshData.Vertices[bottomV].Position = XMFLOAT3(0.0f, line[pointCount-1].y, 0.0f);

	float dRad = -2 * XM_PI / slice;
	for (UINT i = 0; i < slice; ++i) {
		for (UINT j = 0; j < pointCount; ++j) {
			meshData.Vertices[i * pointCount+j].Position 
				= XMFLOAT3(line[j].x * cosf(i * dRad), 
					line[j].y, 
					line[j].x * sinf(i * dRad));
		}
	}
	UINT indicesCount = 3 * slice * 2 + slice * (pointCount - 1) * 6;
	meshData.Indices.resize(indicesCount);
	// 连接网格
	UINT k = LinkGrid(slice, pointCount, 0, meshData);
	// 封边
	for (UINT i = 0; i < pointCount - 1; ++i)
	{
		meshData.Indices[k] = i;
		meshData.Indices[k + 1] = slice * pointCount - pointCount + i;
		meshData.Indices[k + 2] = slice * pointCount - pointCount + 1 + i;
		meshData.Indices[k + 3] = i;
		meshData.Indices[k + 4] = slice * pointCount - pointCount + 1 + i;
		meshData.Indices[k + 5] = i + 1;
		k += 6; // next quad
	}

	// 封上口
	for (UINT i = 0; i < slice - 1; ++i)
	{
		meshData.Indices[k] = topV;
		meshData.Indices[k + 1] = pointCount * i;
		meshData.Indices[k + 2] = pointCount * (i + 1);
		k += 3; // next quad
	}
	meshData.Indices[k] = topV;
	meshData.Indices[k + 1] = pointCount * (slice - 1);
	meshData.Indices[k + 2] = 0;
	k += 3; // next quad

	// 封下口
	for (UINT i = 1; i < slice; ++i)
	{
		meshData.Indices[k] = bottomV;
		meshData.Indices[k + 2] = pointCount * i - 1;
		meshData.Indices[k + 1] = pointCount * (i + 1) -1;
		k += 3; // next quad
	}
	meshData.Indices[k] = bottomV;
	meshData.Indices[k + 2] = pointCount * slice - 1;
	meshData.Indices[k + 1] = pointCount - 1;
	
	CreateNormal(meshData);
}

void GeometryGenerator::CreateCylinder(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, MeshData& meshData)
{
	stackCount = std::max(1u, stackCount);
	float radiusStep = (topRadius - bottomRadius) / stackCount;
	float stackHeight = height / stackCount;
	std::vector<XMFLOAT2> line(stackCount +1 );
	for (UINT i = 0; i < stackCount+1; ++i) {
		line[i] = XMFLOAT2(topRadius - i * radiusStep, height - i *  stackHeight);
	}
	GreateRevolvingSolid(line, sliceCount, meshData);
}

void GeometryGenerator::CreateNormal(MeshData& meshData)
{
	UINT numTriangles = meshData.Indices.size() / 3;
	UINT numVertices = meshData.Vertices.size();
	std::vector<XMVECTOR> normals(numVertices);
	// Input:
// 1. An array of vertices (mVertices). Each vertex has a
// position component (pos) and a normal component (normal).
// 2. An array of indices (mIndices).
// For each triangle in the mesh:
	for (UINT i = 0; i < numTriangles; ++i)
	{
		// indices of the ith triangle
		UINT i0 = meshData.Indices[i * 3 + 0];
		UINT i1 = meshData.Indices[i * 3 + 1];
		UINT i2 = meshData.Indices[i * 3 + 2];
		// vertices of ith triangle
		Vertex v0 = meshData.Vertices[i0];
		Vertex v1 = meshData.Vertices[i1];
		Vertex v2 = meshData.Vertices[i2];
		// compute face normal
		XMVECTOR e0 = XMLoadFloat3(&v1.Position) - XMLoadFloat3(&v0.Position);
		XMVECTOR e1 = XMLoadFloat3(&v2.Position) - XMLoadFloat3(&v0.Position);
		XMVECTOR faceNormal = XMVector3Cross(e0, e1);
		// This triangle shares the following three vertices,
		// so add this face normal into the average of these
		// vertex normals.
		normals[i0] += faceNormal;
		normals[i1] += faceNormal;
		normals[i2] += faceNormal;
	} //For each vertex v, we have summed the face normals of all
		// the triangles that share v, so now we just need to normalize.
		for (UINT i = 0; i < numVertices; ++i)
			XMStoreFloat3(&meshData.Vertices[i].Normal,XMVector3Normalize(normals[i]));
}

float AngleFromXY(float x, float y)
{
	float theta = 0.0f;

	// Quadrant I or IV
	if (x >= 0.0f)
	{
		// If x = 0, then atanf(y/x) = +pi/2 if y > 0
		//                atanf(y/x) = -pi/2 if y < 0
		theta = atanf(y / x); // in [-pi/2, +pi/2]

		if (theta < 0.0f)
			theta += 2.0f * XM_PI; // in [0, 2*pi).
	}

	// Quadrant II or III
	else
		theta = atanf(y / x) + XM_PI; // in [0, 2*pi).

	return theta;
}

void GeometryGenerator::CreateGeosphere(float radius, UINT numSubdivisions, MeshData& meshData)
{
	// Put a cap on the number of subdivisions.
	numSubdivisions = std::min(numSubdivisions, 5u);
	// Approximate a sphere by tessellating an icosahedron.
	const float X = 0.525731f;
	const float Z = 0.850651f;
	XMFLOAT3 pos[12] =
	{
		XMFLOAT3(-X, 0.0f, Z), XMFLOAT3(X, 0.0f, Z),
		XMFLOAT3(-X, 0.0f, -Z), XMFLOAT3(X, 0.0f, -Z),
		XMFLOAT3(0.0f, Z, X), XMFLOAT3(0.0f, Z, -X),
		XMFLOAT3(0.0f, -Z, X), XMFLOAT3(0.0f, -Z, -X),
		XMFLOAT3(Z, X, 0.0f), XMFLOAT3(-Z, X, 0.0f),
		XMFLOAT3(Z, -X, 0.0f), XMFLOAT3(-Z, -X, 0.0f)
	};
	DWORD k[60] =
	{
		1,4,0, 4,9,0, 4,5,9, 8,5,4, 1,8,4,
		1,10,8, 10,3,8, 8,3,5, 3,2,5, 3,7,2,
		3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
		10,1,6, 11,0,9, 2,11,9, 5,2,9, 11,2,7
	};
	meshData.Vertices.resize(12);
	meshData.Indices.resize(60);
	for (size_t i = 0; i < 12; ++i)
		meshData.Vertices[i].Position = pos[i];
	for (size_t i = 0; i < 60; ++i)
		meshData.Indices[i] = k[i];
	for (size_t i = 0; i < numSubdivisions; ++i)
		Subdivide(meshData);// Project vertices onto sphere and scale.
	for (size_t i = 0; i < meshData.Vertices.size(); ++i)
	{
		// Project onto unit sphere.
		XMVECTOR n = XMVector3Normalize(XMLoadFloat3(
			&meshData.Vertices[i].Position));
		// Project onto sphere.
		XMVECTOR p = radius * n;
		XMStoreFloat3(&meshData.Vertices[i].Position, p);
		XMStoreFloat3(&meshData.Vertices[i].Normal, n);
		// Derive texture coordinates from spherical coordinates.
		float theta = AngleFromXY(
			meshData.Vertices[i].Position.x,
			meshData.Vertices[i].Position.z);
		float phi = acosf(meshData.Vertices[i].Position.y / radius);
		meshData.Vertices[i].TexC.x = theta / XM_2PI;
		meshData.Vertices[i].TexC.y = phi / XM_PI;
		// Partial derivative of P with respect to theta
		meshData.Vertices[i].TangentU.x = -radius * sinf(phi) * sinf(theta);
		meshData.Vertices[i].TangentU.y = 0.0f;
		meshData.Vertices[i].TangentU.z = +radius * sinf(phi) * cosf(theta);
		XMVECTOR T = XMLoadFloat3(&meshData.Vertices[i].TangentU);
		XMStoreFloat3(&meshData.Vertices[i].TangentU,
			XMVector3Normalize(T));
	}
}


void GeometryGenerator::Subdivide(MeshData& meshData)
{
	// Save a copy of the input geometry.
	MeshData inputCopy = meshData;


	meshData.Vertices.resize(0);
	meshData.Indices.resize(0);

	//       v1
	//       *
	//      / \
	//     /   \
	//  m0*-----*m1
	//   / \   / \
	//  /   \ /   \
	// *-----*-----*
	// v0    m2     v2

	UINT numTris = inputCopy.Indices.size() / 3;
	for (UINT i = 0; i < numTris; ++i)
	{
		Vertex v0 = inputCopy.Vertices[inputCopy.Indices[i * 3 + 0]];
		Vertex v1 = inputCopy.Vertices[inputCopy.Indices[i * 3 + 1]];
		Vertex v2 = inputCopy.Vertices[inputCopy.Indices[i * 3 + 2]];

		//
		// Generate the midpoints.
		//

		Vertex m0, m1, m2;

		// For subdivision, we just care about the position component.  We derive the other
		// vertex components in CreateGeosphere.

		m0.Position = XMFLOAT3(
			0.5f * (v0.Position.x + v1.Position.x),
			0.5f * (v0.Position.y + v1.Position.y),
			0.5f * (v0.Position.z + v1.Position.z));

		m1.Position = XMFLOAT3(
			0.5f * (v1.Position.x + v2.Position.x),
			0.5f * (v1.Position.y + v2.Position.y),
			0.5f * (v1.Position.z + v2.Position.z));

		m2.Position = XMFLOAT3(
			0.5f * (v0.Position.x + v2.Position.x),
			0.5f * (v0.Position.y + v2.Position.y),
			0.5f * (v0.Position.z + v2.Position.z));

		//
		// Add new geometry.
		//

		meshData.Vertices.push_back(v0); // 0
		meshData.Vertices.push_back(v1); // 1
		meshData.Vertices.push_back(v2); // 2
		meshData.Vertices.push_back(m0); // 3
		meshData.Vertices.push_back(m1); // 4
		meshData.Vertices.push_back(m2); // 5

		meshData.Indices.push_back(i * 6 + 0);
		meshData.Indices.push_back(i * 6 + 3);
		meshData.Indices.push_back(i * 6 + 5);

		meshData.Indices.push_back(i * 6 + 3);
		meshData.Indices.push_back(i * 6 + 4);
		meshData.Indices.push_back(i * 6 + 5);

		meshData.Indices.push_back(i * 6 + 5);
		meshData.Indices.push_back(i * 6 + 4);
		meshData.Indices.push_back(i * 6 + 2);

		meshData.Indices.push_back(i * 6 + 3);
		meshData.Indices.push_back(i * 6 + 1);
		meshData.Indices.push_back(i * 6 + 4);
	}
}

void GeometryGenerator::CreateBox(float width, float height, float depth, MeshData& meshData)
{
	//
	// Create the vertices.
	//

	Vertex v[24];

	float w2 = 0.5f * width;
	float h2 = 0.5f * height;
	float d2 = 0.5f * depth;

	// Fill in the front face vertex data.
	v[0] = Vertex(-w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[1] = Vertex(-w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[2] = Vertex(+w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[3] = Vertex(+w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// Fill in the back face vertex data.
	v[4] = Vertex(-w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	v[5] = Vertex(+w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[6] = Vertex(+w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[7] = Vertex(-w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// Fill in the top face vertex data.
	v[8] = Vertex(-w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[9] = Vertex(-w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[10] = Vertex(+w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[11] = Vertex(+w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// Fill in the bottom face vertex data.
	v[12] = Vertex(-w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	v[13] = Vertex(+w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[14] = Vertex(+w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[15] = Vertex(-w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// Fill in the left face vertex data.
	v[16] = Vertex(-w2, -h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[17] = Vertex(-w2, +h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[18] = Vertex(-w2, +h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[19] = Vertex(-w2, -h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	// Fill in the right face vertex data.
	v[20] = Vertex(+w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	v[21] = Vertex(+w2, +h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	v[22] = Vertex(+w2, +h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	v[23] = Vertex(+w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

	meshData.Vertices.assign(&v[0], &v[24]);

	//
	// Create the indices.
	//

	UINT i[36];

	// Fill in the front face index data
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;

	// Fill in the back face index data
	i[6] = 4; i[7] = 5; i[8] = 6;
	i[9] = 4; i[10] = 6; i[11] = 7;

	// Fill in the top face index data
	i[12] = 8; i[13] = 9; i[14] = 10;
	i[15] = 8; i[16] = 10; i[17] = 11;

	// Fill in the bottom face index data
	i[18] = 12; i[19] = 13; i[20] = 14;
	i[21] = 12; i[22] = 14; i[23] = 15;

	// Fill in the left face index data
	i[24] = 16; i[25] = 17; i[26] = 18;
	i[27] = 16; i[28] = 18; i[29] = 19;

	// Fill in the right face index data
	i[30] = 20; i[31] = 21; i[32] = 22;
	i[33] = 20; i[34] = 22; i[35] = 23;

	meshData.Indices.assign(&i[0], &i[36]);
}


UINT GeometryGenerator::LinkGrid(UINT m, UINT n, UINT startInd, MeshData& meshData)
{
	UINT k = startInd;
	for (UINT i = 0; i < m - 1; ++i)
	{
		for (UINT j = 0; j < n - 1; ++j)
		{
			meshData.Indices[k] = i * n + j;
			meshData.Indices[k + 1] = i * n + j + 1;
			meshData.Indices[k + 2] = (i + 1) * n + j;
			meshData.Indices[k + 3] = (i + 1) * n + j;
			meshData.Indices[k + 4] = i * n + j + 1;
			meshData.Indices[k + 5] = (i + 1) * n + j + 1;
			k += 6; // next quad
		}
	}
	return k;
}

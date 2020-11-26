#pragma once
#include "h.h"
#include "Shape.h"
#include "vertex.h"
#include <vector>
#include <algorithm>

using MeshData = PDEshape::MeshData;

class GeometryGenerator {
	
public:
	GeometryGenerator() = default; 
	GeometryGenerator(const GeometryGenerator&) = delete;
	GeometryGenerator& operator=(const GeometryGenerator&) = delete;
	~GeometryGenerator() = default;

public:
	void CreateGrid(float width, float depth,UINT m, UINT n, MeshData& meshData);
	void GreateRevolvingSolid(std::vector<XMFLOAT2> line, UINT slice, MeshData& meshData);
	void CreateCylinder(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT
		stackCount, MeshData& meshData);
	void CreateNormal(MeshData& meshData);
	void CreateGeosphere(float radius, UINT numSubdivisions, MeshData& meshData);

	void Subdivide(MeshData& meshData);
	void CreateBox(float width, float height, float depth, MeshData& meshData);
private:
	UINT LinkGrid(UINT m, UINT n, UINT startInd, MeshData& meshData);
};
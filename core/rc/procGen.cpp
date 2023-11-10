#include "procGen.h"

namespace rc {
	ew::MeshData createSphere(float radius, int numSegments) {
		ew::MeshData sphereMesh;
		ew::Vertex vertex;

		int columns = numSegments + 1;
		float phi, theta;
		float thetaStep = 2 * ew::PI / numSegments;
		float phiStep = ew::PI / numSegments;
		for (int row = 0; row <= numSegments; row++) {
			phi = row * phiStep;
			for (int col = 0; col <= numSegments; col++) {
				theta = col * thetaStep;
				vertex.pos = ew::Vec3(radius * cos(theta) * sin(phi), radius * cos(phi), radius * sin(theta) * sin(phi));


				vertex.normal = ew::Normalize(vertex.pos);
				vertex.uv = ew::Vec2((float)col / columns, (((float)row / columns) - 1.0f) * -1);

				sphereMesh.vertices.push_back(vertex);
			}
		}


		int poleStart = 0;
		int sideStart = columns;

		for (int i = 0; i < numSegments; i++)
		{
			sphereMesh.indices.push_back(sideStart + i);
			sphereMesh.indices.push_back(poleStart + i); 
			sphereMesh.indices.push_back(sideStart + i + 1);
		}


		for (int row = 1; row < (numSegments - 1); row++)
		{
			for (int col = 0; col < numSegments; col++)
			{
				int start = row * columns + col;

				//Top Right Triangles
				sphereMesh.indices.push_back(start);
				sphereMesh.indices.push_back(start + 1);
				sphereMesh.indices.push_back(start + columns);
				sphereMesh.indices.push_back(start + columns);
				sphereMesh.indices.push_back(start + 1);
				sphereMesh.indices.push_back(start + columns + 1);
			
			}
		}

		//Bottom Cap
		poleStart = ((columns) * (columns - 1));
		sideStart = poleStart - columns;

		for (int i = 0; i < numSegments; i++)
		{
			sphereMesh.indices.push_back(sideStart + i + 1);

			sphereMesh.indices.push_back(poleStart + i); 

			sphereMesh.indices.push_back(sideStart + i);
		}

		return sphereMesh;
	}
	ew::MeshData createCylinder(float height, float radius, int numSegments) {
		ew::MeshData cylinderMesh;
		ew::Vertex vertex;
		float topY = height / 2;
		float bottomY = -topY;
		float theta;
		float thetaStep = 2 * ew::PI / numSegments;
		vertex.pos = ew::Vec3(0, topY, 0);
		vertex.normal = ew::Vec3(0, 1, 0);
		vertex.uv = ew::Vec2(0.5);
		cylinderMesh.vertices.push_back(vertex);
		for (int i = 0; i <= numSegments; i++) {
			theta = i * thetaStep;

			vertex.pos = ew::Vec3(cos(theta) * radius,  topY, sin(theta) * radius);


			vertex.normal = { 0, -1, 0 };

			vertex.uv = ew::Vec2((cos(theta) + 1) * 0.5f, (sin(theta) + 1) * 0.5f);

			cylinderMesh.vertices.push_back(vertex);
		}
	
		for (int i = 0; i <= numSegments; i++) {
			theta = i * thetaStep;

			vertex.pos = ew::Vec3(cos(theta) * radius, topY, sin(theta) * radius);

			vertex.normal = ew::Vec3(cos(theta), 0, sin(theta));
			vertex.uv = ew::Vec2((float)i / numSegments, topY > 0 ? 1 : 0);


			cylinderMesh.vertices.push_back(vertex);
		}


		for (int i = 0; i <= numSegments; i++) {
			theta = i * thetaStep;

			vertex.pos = ew::Vec3(cos(theta) * radius, bottomY, sin(theta) * radius);

			vertex.normal = ew::Vec3(cos(theta), 0, sin(theta));
			vertex.uv = ew::Vec2((float)i / numSegments, bottomY > 0 ? 1 : 0);

			cylinderMesh.vertices.push_back(vertex);
		}

		for (int i = 0; i <= numSegments; i++) {
			theta = i * thetaStep;

			vertex.pos = ew::Vec3(cos(theta) * radius, bottomY, sin(theta) * radius);
		

			vertex.normal = { 0, -1, 0 };

			vertex.uv = ew::Vec2((cos(theta) + 1) * 0.5f, (sin(theta) + 1) * 0.5f);

			cylinderMesh.vertices.push_back(vertex);
		}



		
		vertex.pos = ew::Vec3(0, bottomY, 0);
		vertex.normal = ew::Vec3(0, -1, 0);
		vertex.uv = ew::Vec2(0.5);
		cylinderMesh.vertices.push_back(vertex);


		
		//indices
		//top

		int start = 1;
		int center = 0;
		for (int i = 0; i < numSegments; i++)
		{
			cylinderMesh.indices.push_back(start + i);
			cylinderMesh.indices.push_back(center);
			cylinderMesh.indices.push_back(start + i + 1);
		}

		//sides
		int sideStart = 1 + numSegments + 1;
		int columns = numSegments + 1;

		for (int i = 0; i < columns; i++)
		{
			int start = sideStart + i;

		
			cylinderMesh.indices.push_back(start);
			cylinderMesh.indices.push_back(start + 1);
			cylinderMesh.indices.push_back(start + columns); 
			cylinderMesh.indices.push_back(start + 1);
			cylinderMesh.indices.push_back(start + columns + 1);
			cylinderMesh.indices.push_back(start + columns);
		}

		//Bottom
		center = (4 * columns) + 1;
		sideStart = center - columns;
		for (size_t i = 0; i < columns; i++)
		{
			cylinderMesh.indices.push_back(center);
			cylinderMesh.indices.push_back(sideStart + i);
			cylinderMesh.indices.push_back(sideStart + i + 1);
		}

		return cylinderMesh;
	}
	ew::MeshData createPlane(float size, int subdivisions) {

		ew::MeshData planeMesh;
		ew::Vertex vertex;

		float height = size, width = -size;

		for (int row = 0; row <= subdivisions; row++) {
			for (int col = 0; col <= subdivisions; col++) {
				vertex.pos = ew::Vec3(width * ((float)col / subdivisions),0, -height * ((float)row / subdivisions));
				vertex.normal = ew::Vec3(0.0f, 1.0f, 0.0f);
				vertex.uv = ew::Vec2((float)col / subdivisions, (float)row / subdivisions);

				planeMesh.vertices.push_back(vertex);


			}
		}

		int columns = subdivisions + 1;

		for (int row = 0; row < subdivisions; row++)
		{
			for (int col = 0; col < subdivisions; col++)
			{
				int start = row * columns + col;

				//Bottom right triangle
				planeMesh.indices.push_back(start);
				planeMesh.indices.push_back(start + 1);
				planeMesh.indices.push_back(start + columns + 1);

				//Top left triangle…
				planeMesh.indices.push_back(start);
				planeMesh.indices.push_back(start + columns + 1);
				planeMesh.indices.push_back(start + columns);
			}
		}

		return planeMesh;

	}

}
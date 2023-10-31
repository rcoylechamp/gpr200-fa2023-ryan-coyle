#include "procGen.h"

namespace rc {
	ew::MeshData createSphere(float radius, int numSegments) {
		ew::MeshData sphereMesh;
		ew::Vertex vertex;

		float phi, theta;
		float thetaStep = 2 * ew::PI / numSegments;
		float phiStep = ew::PI / numSegments;
		for (int row = 0; row <= numSegments; row++) {
			phi = row * phiStep;
			for (int col = 0; col <= numSegments; col++) {
				theta = col * thetaStep;
				vertex.pos = ew::Vec3(radius * sin(phi) * cos(theta), radius * cos(phi), radius * sin(phi) * sin(theta));
				sphereMesh.vertices.push_back(vertex);
			}
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

		for (int i = 0; i <= numSegments; i++) {
			theta = i * thetaStep;

			vertex.pos = ew::Vec3(cos(theta) * radius, sin(theta) * radius, topY);

			cylinderMesh.vertices.push_back({ 0,topY,0 });
			cylinderMesh.vertices.push_back({ 0,bottomY,0 });
			cylinderMesh.vertices.push_back(vertex);
		}
		return cylinderMesh;
	}
	ew::MeshData createPlane(float size, int subdivisions) {

		ew::MeshData planeMesh;
		ew::Vertex vertex;

		float height = size / size, width = size / size;

		for (int row = 0; row <= subdivisions; row++) {
			for (int col = 0; col <= subdivisions; col++) {
				vertex.pos = width * (col / subdivisions), -height * (row / subdivisions);
				planeMesh.vertices.push_back(vertex);
			}
		}
		return planeMesh;

	}

}
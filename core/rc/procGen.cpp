#include "procGen.h"


ew::MeshData createSphere(float radius, int numSegments) {
	ew::MeshData sphereMesh;
	ew::Vertex vertex;

	float phi;
	float theta;
	float thetaStep = 2* ew::PI / numSegments;
	float phiStep = ew::PI / numSegments;
	for (int row = 0; row <= numSegments; row++) {
		float phi = row * phiStep;
		for (int col = 0; col <= numSegments; col++) {
			theta = col * thetaStep;
			vertex.pos = ew::Vec3(radius * sin(phi) * sin(theta), radius * cos(phi), radius * sin(phi) * cos(theta));
			sphereMesh.vertices.push_back(vertex);
		}
	}

	return sphereMesh;
}
ew::MeshData createCylinder(float height, float radius, int numSegments);
ew::MeshData createPlane(float size, int subdivisions);
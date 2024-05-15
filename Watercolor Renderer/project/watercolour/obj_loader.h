#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <math.h>
#define OBJL_CONSOLE_OUTPUT

using namespace std;

namespace objl {
	struct Vector2 {
		float X, Y;
		Vector2(float X = 0.0f, float Y = 0.0f);
		bool operator==(const Vector2& other) const;
		bool operator!=(const Vector2& other) const;
		Vector2 operator+(const Vector2& right) const;
		Vector2 operator-(const Vector2& right) const;
		Vector2 operator*(const float& other) const;
	};

	struct Vector3 {
		float X, Y, Z;
		Vector3(float X = 0.0f, float Y = 0.0f, float Z = 0.0f);
		bool operator==(const Vector3& other) const;
		bool operator!=(const Vector3& other) const;
		Vector3 operator+(const Vector3& right) const;
		Vector3 operator-(const Vector3& right) const;
		Vector3 operator*(const float& other) const;
		Vector3 operator/(const float& other) const;
	};

	struct Vertex {
		Vector3 Position;
		Vector3 Normal;
		Vector2 TextureCoordinate;
	};

	struct Material {
		std::string name;
		Vector3 Ka;   // Ambient color
		Vector3 Kd;   // Diffuse color
		Vector3 Ks;   // Specular color
		float Ns;     // Specular exponent
		float Ni;     // Optical density
		float d;      // Dissolve
		int illum;    // Illumination model
		std::string map_Ka;     // Ambient texture map
		std::string map_Kd;     // Diffuse texture map
		std::string map_Ks;     // Specular texture map
		std::string map_Ns;     // Specular highlight map
		std::string map_d;      // Alpha texture map
		std::string map_bump;   // Bump map
		Material();
	};

	struct Mesh {
		std::string MeshName;
		std::vector<Vertex> Vertices;
		std::vector<unsigned int> Indices;
		Material MeshMaterial;
		Mesh();
		Mesh(std::vector<Vertex>& _Vertices, std::vector<unsigned int>& _Indices);
	};



	namespace math {
		Vector3 CrossV3(const Vector3 a, const Vector3 b);
		float MagnitudeV3(const Vector3 in);
		float DotV3(const Vector3 a, const Vector3 b);
		float AngleBetweenV3(const Vector3 a, const Vector3 b);
		Vector3 ProjV3(const Vector3 a, const Vector3 b);
	}



	namespace algorithm {
		Vector3 operator*(const float& left, const Vector3& right);
		bool SameSide(Vector3 p1, Vector3 p2, Vector3 a, Vector3 b);
		Vector3 GenTriNormal(Vector3 t1, Vector3 t2, Vector3 t3);
		bool inTriangle(Vector3 point, Vector3 tri1, Vector3 tri2, Vector3 tri3);
		inline void split(const std::string& in,
			std::vector<std::string>& out,
			std::string token);
		inline std::string tail(const std::string& in);
		inline std::string firstToken(const std::string& in);
		template <class T>
		inline const T& getElement(const std::vector<T>& elements, std::string& index);
	}



	class Loader {
	public:
		// Default Constructor
		Loader();
		~Loader();

		// Load a file into the loader
		bool LoadFile(std::string Path);
		// Loaded Mesh Objects
		std::vector<Mesh> LoadedMeshes;
		// Loaded Vertex Objects
		std::vector<Vertex> LoadedVertices;
		// Loaded Index Positions
		std::vector<unsigned int> LoadedIndices;
		// Loaded Material Objects
		std::vector<Material> LoadedMaterials;

	private:
		// Generate vertices from a list of positions, 
		//	tcoords, normals and a face line
		void GenVerticesFromRawOBJ(std::vector<Vertex>& oVerts,
			const std::vector<Vector3>& iPositions,
			const std::vector<Vector2>& iTCoords,
			const std::vector<Vector3>& iNormals,
			std::string icurline);
		// Triangulate a list of vertices into a face by printing
		//	inducies corresponding with triangles within it
		void VertexTriangluation(std::vector<unsigned int>& oIndices,
			const std::vector<Vertex>& iVerts);
		// Load Materials from .mtl file
		bool LoadMaterials(std::string path);
	};
}
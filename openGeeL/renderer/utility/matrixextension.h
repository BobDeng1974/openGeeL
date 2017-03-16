#ifndef MATRIXEXTENSION_H
#define MATRIXEXTENSION_H

#include <mat4x4.hpp>
#include "../../dependencies/assimp/scene.h"
#include <iostream>

namespace geeL {

	//Extension class that adds additional functionality to matrices
	class MatrixExtension {

	public:

		inline static void print(const glm::mat4& matrix) {
			std::cout << "| " << matrix[0][0] << " " << matrix[1][0] << " " << matrix[2][0] << " " << matrix[3][0] << " |\n";
			std::cout << "| " << matrix[0][1] << " " << matrix[1][1] << " " << matrix[2][1] << " " << matrix[3][1] << " |\n";
			std::cout << "| " << matrix[0][2] << " " << matrix[1][2] << " " << matrix[2][2] << " " << matrix[3][2] << " |\n";
			std::cout << "| " << matrix[0][3] << " " << matrix[1][3] << " " << matrix[2][3] << " " << matrix[3][3] << " |\n";
		}


		inline static glm::mat4 convertMatrix(aiMatrix4x4& from) {
			glm::mat4 matrix;

			matrix[0][0] = from.a1;
			matrix[1][0] = from.a2;
			matrix[2][0] = from.a3;
			matrix[3][0] = from.a4;

			matrix[0][1] = from.b1;
			matrix[1][1] = from.b2;
			matrix[2][1] = from.b3;
			matrix[3][1] = from.b4;

			matrix[0][2] = from.c1;
			matrix[1][2] = from.c2;
			matrix[2][2] = from.c3;
			matrix[3][2] = from.c4;

			matrix[0][3] = from.d1;
			matrix[1][3] = from.d2;
			matrix[2][3] = from.d3;
			matrix[3][3] = from.d4;

			return matrix;
		}

	};

}

#endif

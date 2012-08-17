//Copyright 2012, 2013 Tsvetan Tsvetanov
//This file is part of the Star Game.
//
//The Star Game is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//The Star Game is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with the Star Game.  If not, see <http://www.gnu.org/licenses/>.


/// \ingroup module_StarGame

/// \file Utility.h
/// \brief Utility functions which are of frequent need.

#ifndef UTILITY_H
#define UTILITY_H


#define ARRAY_COUNT( array ) (sizeof( array ) / (sizeof( array[0] ) * (sizeof( array ) != sizeof(void*) || sizeof( array[0] ) <= sizeof(void*))))


#include <glload/gl_3_3.h>
#include <glutil/glutil.h>
#include <glm/glm.hpp>

#include "../framework/framework.h"
#include "../framework/Mesh.h"
#include "../framework/Timer.h"

#include "../ProgramData/ProgramData.h"

#include <vector>


/// \namespace Utility
/// \brief The utility functions' namespace.
namespace Utility
{	
	/// \fn CorrectGamma()
	/// \brief Applies gamma correction to colors.
	glm::vec4 CorrectGamma(const glm::vec4 &inputColor, float gamma);
	
	// TODO: Implement.
	void CalculateFPS();

	/// \class Ray
	/// \brief Implements a very simple ray.
	class Ray
	{
	public:
		glm::vec4 origin;
		glm::vec4 direction;

		Ray();
		Ray(glm::vec4 newOrigin, glm::vec4 newDirection);
	};

	/// \namespace Utility::Intersections
	/// \brief Used to hold the intersection check functions.
	namespace Intersections
	{
		/// \fn RayIntersectsSphere(Ray mouseRay, glm::vec3 bodyPosition, float sphereRadius)
		/// \brief Checks for intersection between a ray and a sphere.

		/// To check for intersections we need the sphere equation and the ray equation.
		/// So, for point \f$p\f$, radius \f$r\f$ and position \f$p_c\f$ the sphere equation
		///	is: \f$(p - p_c) \cdot (p - p_c) - r^2 = 0\f$
		///
		/// The ray equation based on ray direction \f$d\f$, parametrized by \f$t\f$ and with 
		///	origin \f$o\f$ is: \f$p = t d + o = 0\f$
		///
		/// Pluging the ray equation in the sphere equation we now
		/// have: \f$(t d + o - p_c) \cdot (t d + o - p_c) - r^2 = 0\f$
		/// 
		/// To solve it for \f$t\f$ we will need the quadratic 
		/// formula: \f$t = \left(\frac{-b \pm \sqrt{b^2 - 4ac}}{2a}\right)\f$
		///		- \f$a = d \cdot d\f$
		///		- \f$b = 2 d \cdot (o - p_c)\f$
		///		- \f$c = (o - p_c) \cdot (o - p_c) - r^2\f$
		///
		/// If \f$t \ge 0\f$ we have intersection, else the ray doesn't intersect with the sphere.
		bool RayIntersectsSphere(Ray mouseRay, glm::vec3 bodyPosition, float sphereRadius);
		
		/// \fn RayIntersectsEllipsoid(Ray mouseRay, glm::vec3 bodyPosition, float sphereRadius, glm::mat4 distortionMat)
		/// \brief Checks for intersection between a ray and an ellipsoid.

		/// The solution for this problem is simple. We don't need to make complex computations. 
		/// Simply put, an ellipsoid is a sphere with applied scaling transformations on it. Therefore
		/// we can get the matrix which applied these transformations, deform the ray with it and then
		/// pass the deformed ray to the RayIntersectsSphere function.
		bool RayIntersectsEllipsoid(Ray mouseRay, glm::vec3 bodyPosition, 
								    float sphereRadius, glm::mat4 deformationMat);
	}

	/// \namespace Utility::BasicMeshGeneration
	/// \brief Has functions for generating basic figures like circles, toruses and others.
	namespace BasicMeshGeneration
	{
		/// \class Torus2D
		/// \brief Generates and renders a 2D torus.
		class Torus2D
		{
		private:			
			glm::vec4 color; ///< The color of the object.
			glm::vec4 position; ///< Its position.

			unsigned int vao; ///< The vertex array object.
			unsigned int indexBO; ///< The index buffer object for the vertices' indices.
			unsigned int vertexBO; ///< The vertex buffer object for the vertices with their attributes.

			float innerRadius;
			float outerRadius;
			short resolution; ///< The resolution with which the torus will be rendered.

		public:
			Torus2D();
			Torus2D(glm::vec4 newColor, glm::vec4 newPosition,
					float newInnerRadius, float newOuterRadius, 
					int newResolution);

			/// \fn Init
			/// \brief Generates the vertex and index data for the object and binds OpenGL's buffers.

			/// For the two radiuses we use a formula similar to the one calculating the satellite's 
			///	position (see Satellite::Update()). That way we generate the vertex data. 
			/// 
			/// The index data is little more complex. We will be rendering via triangle strips. So,
			/// we need the first index to be the first outer radius' vertex and the second index to be
			/// the first inner radius' vertex. That is repeated for all vertices. At the end we 'sew' the 
			/// torus (see the three lines of code after the `for` cycle).
			///
			/// The buffers initialization is straightforward if you are familiar with OpenGL. For further
			/// research see [this book](http://www.arcsynthesis.org/gltut/).
			void Init();
		
			/// \fn Draw
			/// \brief Draws the torus on the scene. 
			void Draw(glutil::MatrixStack &modelMatrix, const SimpleProgData &data);
		};

		/// \class Circle
		/// \brief Generates and renders a circle.
		class Circle
		{
		private:
			glm::vec4 color; ///< The color of the object.
			glm::vec4 position; ///< Its position.

			unsigned int vao; ///< The vertex array object.
			unsigned int indexBO; ///< The index buffer object for the vertices' indices.
			unsigned int vertexBO; ///< The vertex buffer object for the vertices with their attributes.

			float radius;
			short resolution; ///< The resolution with which the circle will be rendered.

		public:
			Circle(glm::vec4 newColor, glm::vec4 newPosition, 
				   float newRadius, short newResolution);

			/// \fn Init
			/// \brief Generates the vertex and index data for the object and binds OpenGL's buffers.

			/// The generation of the vertex data is pretty straightforward 
			/// (see Satellite::Update()).
			///
			/// The index data is simply every vertex of the circle.
			/// 
			/// The buffers initialization is straightforward if you are familiar with OpenGL. For further
			/// research see [this book](http://www.arcsynthesis.org/gltut/).
			void Init();

			/// \fn Draw
			/// \brief Draws the circle on the scene. 
			void Draw(glutil::MatrixStack &modelMatrix, const SimpleProgData &data);
		};
	}
}

#endif
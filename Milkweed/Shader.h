/*
* File: Shader.h
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Created: 2020.10.30
*/

#ifndef MW_SHADER_H
#define MW_SHADER_H

#include <string>
#include <vector>
#include <GL/glew.h>

namespace Milkweed {
	/*
	* An input vertex attribute for a Shader
	*/
	class VertexAttribute {
	public:
		/*
		* Create a vertex attribute with the given information
		* 
		* @param name: The name of the vertex attribute in the shader program
		* @param componentCount: The number of coordinates in the attribute
		* @param type: The data type of the attribute coordinates
		* @param normalized: Whether the data must be normalized by OpenGL
		* @param stride: The number of bytes between the instances of this
		* attribute in the VAO passed to the shader
		* @param offset: The number of bytes before the first instance of this
		* attribute in the VAO
		*/
		VertexAttribute(const std::string& name, unsigned int componentCount,
			int type, bool normalized, unsigned int stride,
			unsigned int offset) : m_name(name), m_componentCount(componentCount),
			m_type(type), m_normalized(normalized), m_stride(stride),
			m_offset(offset) {}

	private:
		friend class Shader;

		// The name of the vertex attribute in the shader program
		std::string m_name = "";
		// The number of coordinates in the attribute
		unsigned int m_componentCount = 0;
		// The data type of the attribute coordinates
		unsigned int m_type = 0;
		// Whether the data must be normalized by OpenGL
		bool m_normalized = 0;
		// The number of bytes between the instances of this attribute in the
		// VAO passed to the shader
		unsigned int m_stride = 0;
		// The number of bytes before the first instance of this attribute
		// in the VAO
		unsigned int m_offset = 0;
	};

	/*
	* A compiler and wrapper for an OpenGL vertex and fragment shader program
	*/
	class Shader {
	public:
		/*
		* Generated default vertex attributes for 2D sprite shader (3D position
		* and 2D texture coordinates)
		* 
		* @param positionName: The name of the position input in this shader
		* @param textureCoordsName: The name of the texture coordinates input
		* in this shader
		*/
		static std::vector<VertexAttribute> getDefaultVertexAttributes(
			const std::string& positionName,
			const std::string& textureCoordsName) {
			return {
				VertexAttribute(positionName, 3, GL_FLOAT, GL_FALSE,
					5 * sizeof(float), 0),
				VertexAttribute(textureCoordsName, 2, GL_FLOAT, GL_FALSE,
					5 * sizeof(float), 3 * sizeof(float)),
			};
		}
		/*
		* Locate this shader's GLSL source code, compile it, and create this
		* shader with OpenGL
		* 
		* @param vFileName: The path to the file containing the vertex shader
		* source
		* @param fFileName: The path to the file containing the fragment shader
		* source
		* @param attributes: The vertex attributes this shader will use when
		* rendering a VAO from OpenGL
		* @param cameraUniformName: The uniform name for the projection matrix
		* of this shader's camera, empty by default
		* @param camera: The camera this shader uses for rendering
		* @return Whether the shader could be created successfully
		*/
		bool init(const std::string& vFileName, const std::string& fFileName,
			const std::vector<VertexAttribute>& attributes,
			const std::string& cameraUniformName, Camera* camera);
		/*
		* Use this shader to draw graphics
		*/
		void begin();
		/*
		* Upload an integer uniform to this shader
		* 
		* @param name: The name of the uniform variable
		* @param value: The integer to upload
		* @return Whether the integer could be uploaded
		*/
		bool uploadInt(const std::string& name, int value);
		/*
		* Upload an unsigned integer uniform to this shader
		* 
		* @param name: The name of the uniform variable
		* @param value: The unsigned integer to upload
		* @return Whether the unsigned integer could be uploaded
		*/
		bool uploadUInt(const std::string& name, unsigned int value);
		/*
		* Upload an float uniform to this shader
		*
		* @param name: The name of the uniform variable
		* @param value: The float to upload
		* @return Whether the float could be uploaded
		*/
		bool uploadFloat(const std::string& name, float value);
		/*
		* Upload a 2-float vector uniform to this shader
		* 
		* @param name: The name of the uniform variable
		* @param value: The vector to upload
		* @return Whether the vector could be uploaded
		*/
		bool upload2fVector(const std::string& name, const glm::vec2& value);
		/*
		* Upload a 3-float vector uniform to this shader
		* 
		* @param name: The name of the uniform variable
		* @param value: The vector to upload
		* @return Whether the vector could be uploaded
		*/
		bool upload3fVector(const std::string& name, const glm::vec3& value);
		/*
		* Upload a 4x4 matrix uniform to this shader
		* 
		* @param name: The name of the uniform variable
		* @param value: The matrix to upload
		* @return Whether the matrix could be uploaded
		*/
		bool upload4x4Matrix(const std::string& name, const glm::mat4& value);
		/*
		* Tell OpenGL to stop using this shader to draw graphics
		*/
		void end();
		/*
		* Delete this shader program from OpenGL's memory and stop using it
		*/
		void destroy();
		/*
		* Get a pointer to this shader's camera
		*/
		Camera* getCamera() { return m_camera; }
		/*
		* Set this shader's camera
		*/
		void setCamera(Camera* camera) { m_camera = camera; }

	private:
		// The OpenGL ID of the vertex shader
		GLuint m_vID = 0;
		// The OpenGL ID of the fragment shader
		GLuint m_fID = 0;
		// The OpenGL ID of this shader program
		GLuint m_programID = 0;
		// The number of attributes this shader uses
		unsigned int m_attributeCount = 0;
		// The camera this shader gets its projection matrix from
		Camera* m_camera = nullptr;
		// The uniform name of the camera's projection matrix in this shader
		std::string m_cameraUniformName = "";
	};
}

#endif
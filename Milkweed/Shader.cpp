/*
* File: Shader.cpp
* Author: Keegan MacDonald (keeganm742@gmail.com)
* Created: 2020.10.30
*/

#include <fstream>

#include "MW.h"

namespace Milkweed {
	bool Shader::init(const std::string& vFileName,
		const std::string& fFileName,
		const std::vector<VertexAttribute>& attributes,
		const std::string& cameraUniformName, Camera* camera) {
		// Initialize the camera if this shader needs one
		m_cameraUniformName = cameraUniformName;
		m_camera = camera;

		// Locate and read the GLSL source code for the vertex shader
		std::ifstream vFile(vFileName);
		if (vFile.fail()) {
			// The vertex shader's file was not found
			MWLOG(Warning, Shader, "Failed to open vertex shader file ",
				vFileName);
			return false;
		}
		std::string vFileContent((std::istreambuf_iterator<char>(vFile)),
			std::istreambuf_iterator<char>());
		const char* vSource = vFileContent.c_str();
		vFile.close();

		// Locate and read the GLSL source code for the fragment shader
		std::ifstream fFile(fFileName);
		if (fFile.fail()) {
			// The fragment shader's file was not found
			MWLOG(Warning, Shader, "Failed to open fragment shader file ",
				fFileName);
			return false;
		}
		std::string fFileContent((std::istreambuf_iterator<char>(fFile)),
			std::istreambuf_iterator<char>());
		const char* fSource = fFileContent.c_str();
		fFile.close();

		// Create the vertex and fragmnet shaders and upload their source code
		m_vID = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(m_vID, 1, &vSource, NULL);
		m_fID = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(m_fID, 1, &fSource, NULL);

		// Compile the shaders' source code
		glCompileShader(m_vID);
		GLint vStatus = 0;
		glGetShaderiv(m_vID, GL_COMPILE_STATUS, &vStatus);
		if (vStatus != GL_TRUE) {
			// The vertex shader has failed to compile
			char buffer[1024]; // TODO: Modify hardcoded buffer sizes
			glGetShaderInfoLog(m_vID, 1024, NULL, buffer);
			MWLOG(Warning, Shader, "Failed to compile vertex shader source ",
				vFileName, "\n", buffer);
			return false;
		}
		MWLOG(Info, Shader, "Compiled vertex shader source ", vFileName);
		glCompileShader(m_fID);
		GLint fStatus = 0;
		glGetShaderiv(m_fID, GL_COMPILE_STATUS, &fStatus);
		if (fStatus != GL_TRUE) {
			// The fragment shader has failed to compile
			char buffer[1024];
			glGetShaderInfoLog(m_fID, 1024, NULL, buffer);
			MWLOG(Warning, Shader, "Failed to compile fragment shader source",
				fFileName, "\n", buffer);
			return false;
		}
		MWLOG(Info, Shader, "Compiled fragment shader source ", fFileName);

		// Create the full shader program, attach the shaders and link
		m_programID = glCreateProgram();
		glAttachShader(m_programID, m_vID);
		glAttachShader(m_programID, m_fID);
		glLinkProgram(m_programID);

		MWLOG(Info, Shader, "Created shader program with ID ", m_programID);

		// Add all the vertex attributes to the program
		m_attributeCount = (unsigned int)attributes.size();
		glUseProgram(m_programID);
		for (VertexAttribute attribute : attributes) {
			GLint position = glGetAttribLocation(m_programID,
				attribute.m_name.c_str());
			glVertexAttribPointer(position, attribute.m_componentCount,
				attribute.m_type, attribute.m_normalized, attribute.m_stride,
				(void*)attribute.m_offset);
		}

		return true;
	}

	void Shader::begin() {
		// Tell OpenGL to use this shader and enable vertex attributes
		glUseProgram(m_programID);
		for (unsigned int i = 0; i < m_attributeCount; i++) {
			glEnableVertexAttribArray(i);
		}

		// Upload this shader's camera matrix if it needs one
		if (!m_cameraUniformName.empty()) {
			upload4x4Matrix(m_cameraUniformName, m_camera->getCameraMatrix());
		}
	}

	bool Shader::uploadInt(const std::string& name, int value) {
		GLint location = glGetUniformLocation(m_programID, name.c_str());
		if (location == -1) {
			return false;
		}
		glUniform1i(location, value);
		return true;
	}

	bool Shader::uploadUInt(const std::string& name, unsigned int value) {
		GLint location = glGetUniformLocation(m_programID, name.c_str());
		if (location == -1) {
			return false;
		}
		glUniform1ui(location, value);
		return true;
	}

	bool Shader::uploadFloat(const std::string& name, float value) {
		GLint location = glGetUniformLocation(m_programID, name.c_str());
		if (location == -1) {
			return false;
		}
		glUniform1f(location, value);
		return true;
	}

	bool Shader::upload2fVector(const std::string& name,
		const glm::vec2& value) {
		GLint location = glGetUniformLocation(m_programID, name.c_str());
		if (location == -1) {
			return false;
		}
		glUniform2f(location, value.x, value.y);
		return true;
	}

	bool Shader::upload3fVector(const std::string& name,
		const glm::vec3& value) {
		GLint location = glGetUniformLocation(m_programID, name.c_str());
		if (location == -1) {
			return false;
		}
		glUniform3f(location, value.x, value.y, value.z);
		return true;
	}

	bool Shader::upload4x4Matrix(const std::string& name,
		const glm::mat4& value) {
		GLint location = glGetUniformLocation(m_programID, name.c_str());
		if (location == -1) {
			return false;
		}
		glUniformMatrix4fv(location, 1, GL_FALSE, &(value[0][0]));
		return true;
	}

	void Shader::end() {
		// Disable the vertex attributes for this shader
		for (unsigned int i = 0; i < m_attributeCount; i++) {
			glDisableVertexAttribArray(i);
		}
	}

	void Shader::destroy() {
		MWLOG(Info, Shader, "Destroying shader ", m_programID);

		// Detach the vertex and fragment shaders and delete the program
		glDetachShader(m_programID, m_fID);
		glDetachShader(m_programID, m_vID);
		glDeleteShader(m_vID);
		glDeleteShader(m_fID);
		glDeleteProgram(m_programID);
	}
}
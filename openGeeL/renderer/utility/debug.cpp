#define GLEW_STATIC
#include <glew.h>
#include <glfw3.h>
#include <iostream>
#include "debug.h"

using namespace std;

namespace geeL {

	void APIENTRY debugOutput(GLenum source, GLenum type, GLuint id, GLenum severity,
		GLsizei length, const GLchar * message, const void * userParam) {

		cout << "Message (" << id << "): " << message << "\n";
		switch (source) {
			case GL_DEBUG_SOURCE_API:             cout << "Source: API"; break;
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   cout << "Source: Window System"; break;
			case GL_DEBUG_SOURCE_SHADER_COMPILER: cout << "Source: Shader Compiler"; break;
			case GL_DEBUG_SOURCE_THIRD_PARTY:     cout << "Source: Third Party"; break;
			case GL_DEBUG_SOURCE_APPLICATION:     cout << "Source: Application"; break;
			case GL_DEBUG_SOURCE_OTHER:           cout << "Source: Other"; break;
		} 
		
		cout << "\n";
		switch (type) {
			case GL_DEBUG_TYPE_ERROR:               cout << "Type: Error"; break;
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: cout << "Type: Deprecated Behaviour"; break;
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  cout << "Type: Undefined Behaviour"; break;
			case GL_DEBUG_TYPE_PORTABILITY:         cout << "Type: Portability"; break;
			case GL_DEBUG_TYPE_PERFORMANCE:         cout << "Type: Performance"; break;
			case GL_DEBUG_TYPE_MARKER:              cout << "Type: Marker"; break;
			case GL_DEBUG_TYPE_PUSH_GROUP:          cout << "Type: Push Group"; break;
			case GL_DEBUG_TYPE_POP_GROUP:           cout << "Type: Pop Group"; break;
			case GL_DEBUG_TYPE_OTHER:               cout << "Type: Other"; break;
		} 
		
		cout << "\n";
		switch (severity) {
			case GL_DEBUG_SEVERITY_HIGH:         cout << "Severity: High"; break;
			case GL_DEBUG_SEVERITY_MEDIUM:       cout << "Severity: Medium"; break;
			case GL_DEBUG_SEVERITY_LOW:          cout << "Severity: Low"; break;
			case GL_DEBUG_SEVERITY_NOTIFICATION: cout << "Severity: Notification"; break;
		} 
		
		cout << "\n";
	}

	void Debugger::init() {

		GLint flags; 
		glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
		if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {

			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(debugOutput, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		}
	}
}
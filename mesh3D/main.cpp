#include "icg_common.h"
#include "_mesh/Mesh.h"
#include "OpenGP/GL/Eigen.h"

int window_width = 800;
int window_height = 800;

Mesh mesh;

void init(){
    glClearColor(1,1,1, /*solid*/1.0 );    
    glEnable(GL_DEPTH_TEST);
    mesh.init();
}

void display(){
    glViewport(0,0,window_width,window_height);    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    GLuint pid = mesh.getProgramID();
    glUseProgram(pid);
        mat4 MODEL = mat4::Identity();
        glUniformMatrix4fv(glGetUniformLocation(pid, "MODEL"), 1, GL_FALSE, MODEL.data());

        mat4 VIEW = OpenGP::lookAt( vec3(1,1,1), vec3(0,0,0), vec3(0,1,0) );
        glUniformMatrix4fv(glGetUniformLocation(pid, "VIEW"), 1, GL_FALSE, VIEW.data());
        
        mat4 PROJ = OpenGP::perspective(45.0f, window_width/(float)window_height, 0.1f, 10.0f);
        glUniformMatrix4fv(glGetUniformLocation(pid, "PROJ"), 1, GL_FALSE, PROJ.data());
    glUseProgram(pid);
        
    mesh.draw();
}

void cleanup() { mesh.cleanup(); }


int main(int, char**){
	GLFWwindow* window;
	
	glewExperimental = true; // for core profile

	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	//versions
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context

	window = glfwCreateWindow(1024, 768, "GLSL", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window); // Initialize GLEW

	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}
	init();

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	do {
		// Clear the screen. 
		glClear(GL_COLOR_BUFFER_BIT);
		display();

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	cleanup();
    
    return EXIT_SUCCESS;
}

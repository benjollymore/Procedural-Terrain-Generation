#pragma once
#include "icg_common.h"
#include "OpenGP/external/LodePNG/lodepng.cpp"

typedef Eigen::Transform<float,3,Eigen::Affine> Transform;
using namespace OpenGP;

class Mesh{
protected:
    GLuint _vao; ///< vertex array object
    GLuint _pid; ///< GLSL shader program ID
    GLuint _tex_day; ///< Texture ID
    GLuint _tex_night; ///< Texture ID

    OpenGP::SurfaceMesh mesh;    
    GLuint _vpoint;    ///< memory buffer
    GLuint _vnormal;   ///< memory buffer

public:    
    void init(){
        //const std::string& filename = "_mesh/sphere.obj";
		//const std::string& filename = "_mesh/armadillo.obj";
        const std::string& filename = "_mesh/bunny.obj";
        
        check_error_gl();
        bool success = mesh.read(filename.c_str());
        assert(success);
        mesh.triangulate();
        mesh.update_vertex_normals();
        printf("Loaded mesh '%s' (#V=%d, #F=%d)\n", filename.c_str(), mesh.n_vertices(), mesh.n_faces());
        
        ///--- Vertex one vertex Array
        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);
        check_error_gl();                      
        
        ///--- Vertex Buffer
		SurfaceMesh::Vertex_property<Point> vpoints = mesh.get_vertex_property<Point>("v:point");
        glGenBuffers(ONE, &_vpoint);
        glBindBuffer(GL_ARRAY_BUFFER, _vpoint);
        glBufferData(GL_ARRAY_BUFFER, mesh.n_vertices() * sizeof(vec3), vpoints.data(), GL_STATIC_DRAW);
        check_error_gl();        
    
        ///--- Normal Buffer
		SurfaceMesh::Vertex_property<Normal> vnormals = mesh.get_vertex_property<Normal>("v:normal");
        glGenBuffers(ONE, &_vnormal);
        glBindBuffer(GL_ARRAY_BUFFER, _vnormal);
        glBufferData(GL_ARRAY_BUFFER, mesh.n_vertices() * sizeof(vec3), vnormals.data(), GL_STATIC_DRAW);
        check_error_gl();        
    
        ///--- Index Buffer
        std::vector<unsigned int> indices;
        for(SurfaceMesh::Face_iterator fit = mesh.faces_begin(); fit != mesh.faces_end(); ++fit) {
            unsigned int n = mesh.valence(*fit);
			SurfaceMesh::Vertex_around_face_circulator vit = mesh.vertices(*fit);
            for(unsigned int v = 0; v < n; ++v) {
                indices.push_back((*vit).idx());
                ++vit;
            }
        }

        GLuint _vbo_indices;
        glGenBuffers(ONE, &_vbo_indices);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbo_indices);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        check_error_gl();        

        ///--- Compile the shaders
        _pid = OpenGP::load_shaders("_mesh/Mesh_vshader.glsl", "_mesh/Mesh_fshader.glsl");
        if(!_pid) exit(EXIT_FAILURE);
        glUseProgram(_pid);
        
        ///--- Load texture
        this->loadTextures("_mesh/day.png", _tex_day);
		glUniform1i(glGetUniformLocation(_pid, "tex_day"), 0 /*GL_TEXTURE0*/);
        
           
        ///--- Load texture
        this->loadTextures("_mesh/night.png", _tex_night);
		glUniform1i(glGetUniformLocation(_pid, "tex_night"), 1 /*GL_TEXTURE1*/);
    }

	void loadTextures(const std::string filename, GLuint _tex) {
		// Used snippet from https://raw.githubusercontent.com/lvandeve/lodepng/master/examples/example_decode.cpp
		std::vector<unsigned char> image; //the raw pixels
		unsigned width, height;
		//decode
		unsigned error = lodepng::decode(image, width, height, filename);
		//if there's an error, display it
		if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
		//the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA..., use it as texture, draw it, ...

		// unfortunately they are upside down...lets fix that
		unsigned char* row = new unsigned char[4 * width];
		for (int i = 0; i < int(height) / 2; ++i) {
			memcpy(row, &image[4 * i * width], 4 * width * sizeof(unsigned char));
			memcpy(&image[4 * i * width], &image[image.size() - 4 * (i + 1) * width], 4 * width * sizeof(unsigned char));
			memcpy(&image[image.size() - 4 * (i + 1) * width], row, 4 * width * sizeof(unsigned char));
		}
		delete row;

		glBindVertexArray(_vao);
		check_error_gl();

		glUseProgram(_pid);

		glGenTextures(1, &_tex);
		glBindTexture(GL_TEXTURE_2D, _tex);

		check_error_gl();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		check_error_gl();

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
			width, height, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);
		check_error_gl();



	}
           
    void cleanup(){
        glBindVertexArray(0);
		glUseProgram(0);
		glDeleteBuffers(1, &_vpoint);
		glDeleteBuffers(1, &_vnormal);
		glDeleteProgram(_pid);
		glDeleteVertexArrays(1, &_vao);
		glDeleteTextures(1, &_tex_day);
		glDeleteTextures(1, &_tex_night);
    }

    void draw(){
        bindShader();  
            ///--- Setup the texture units
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, _tex_day);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, _tex_night);
        
            glUniform1f(glGetUniformLocation(_pid, "time"), glfwGetTime());
            // glDrawElements(GL_TRIANGLES, /*#vertices*/ 3*mesh.n_faces(), GL_UNSIGNED_INT, ZERO_BUFFER_OFFSET);
            glDrawElementsInstanced(GL_TRIANGLES, /*#vertices*/ 3*mesh.n_faces(), GL_UNSIGNED_INT, ZERO_BUFFER_OFFSET,2 /*#instances*/);
        unbindShader();
    }
    
    GLuint getProgramID(){ return _pid; }
    
private:
    void bindShader() {
        glUseProgram(_pid);
        glBindVertexArray(_vao);
        check_error_gl();

        ///--- Vertex Attribute ID for Positions
        GLint vpoint_id = glGetAttribLocation(_pid, "vpoint");
        if (vpoint_id >= 0) {
            glEnableVertexAttribArray(vpoint_id);
            check_error_gl();
            glBindBuffer(GL_ARRAY_BUFFER, _vpoint);
            glVertexAttribPointer(vpoint_id, 3 /*vec3*/, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
            check_error_gl();
        }

        ///--- Vertex Attribute ID for Normals
        GLint vnormal_id = glGetAttribLocation(_pid, "vnormal");
        if (vnormal_id >= 0) {
            glEnableVertexAttribArray(vnormal_id);
            glBindBuffer(GL_ARRAY_BUFFER, _vnormal);
            glVertexAttribPointer(vnormal_id, 3 /*vec3*/, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
            check_error_gl();
        }
    }

    void unbindShader() {
        GLint vpoint_id = glGetAttribLocation(_pid, "vpoint");
        if (vpoint_id >= 0)
            glDisableVertexAttribArray(vpoint_id);
        GLint vnormal_id = glGetAttribLocation(_pid, "vnormal");
        if (vnormal_id >= 0)
            glDisableVertexAttribArray(vnormal_id);
        glUseProgram(0);
        glBindVertexArray(0);
    }
};

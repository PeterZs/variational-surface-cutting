#pragma once


#ifdef __APPLE__
    #define GLFW_INCLUDE_GLCOREARB 
    #include <GLFW/glfw3.h>
#else
    #include <glad/glad.h>
#endif

#include <string>
#include <vector>

#include <shaders.h>
#include <vector2.h>
#include <vector3.h>
#include <utilities.h>
#include <colors.h>
#include <colormaps.h>

// The drawing modes available
enum class DrawMode {Points, LinesAdjacency, Triangles, TrianglesAdjacency, Patches, IndexedTriangles, Lines, IndexedLines, IndexedLineStrip, IndexedLinesAdjacency, IndexedLineStripAdjacency};

// Class to encapsulate a program
class GLProgram {

    public:

        // Constructors
        GLProgram(const VertShader* vShader, const FragShader* fShader, DrawMode dm); 
        GLProgram(const VertShader* vShader, const GeomShader* gShader, const FragShader* fShader, DrawMode dm);
        GLProgram(const VertShader* vShader, const TessShader* tShader, const FragShader* fShader, DrawMode dm, int nPatchVertices);
        GLProgram(const VertShader* vShader, const EvalShader* eShader, const FragShader* fShader, DrawMode dm, int nPatchVertices);
        GLProgram(const VertShader* vShader, const TessShader* tShader, const EvalShader* eShader, const FragShader* fShader, DrawMode dm, int nPatchVertices);
        GLProgram(const VertShader* vShader, const TessShader* tShader, const EvalShader* eShader, const GeomShader* gShader, const FragShader* fShader, DrawMode dm, int nPatchVertices);

        // Destructor (free gl buffers)
        ~GLProgram(); 
        

        // === Store data 
        // If update is set to "true", data is updated rather than allocated (must be allocated first)

        // Uniforms
        void setUniform(std::string name, int val);
        void setUniform(std::string name, unsigned int val);
        void setUniform(std::string name, float val);
        void setUniform(std::string name, double val); // WARNING casts down to float
        void setUniform(std::string name, float* val);
        void setUniform(std::string name, Vector2 val);
        void setUniform(std::string name, Vector3 val);
        void setUniform(std::string name, float x, float y, float z, float w);

        // Attributes
        void setAttribute(std::string name, const std::vector<Vector2> &data, bool update = false,
                          int offset = 0, int size = -1);
        void setAttribute(std::string name, const std::vector<Vector3> &data, bool update = false,
                          int offset = 0, int size = -1);
        void setAttribute(std::string name, const std::vector<double> &data, bool update = false,
                          int offset = 0, int size = -1);
        void setAttribute(std::string name, const std::vector<int> &data, bool update = false,
                          int offset = 0, int size = -1);

        // Textures
        void setTexture1D(std::string name, unsigned char* texData, unsigned int length);
        void setTexture2D(std::string name, unsigned char* texData, unsigned int width, unsigned int height);
        void setTextureFromColormap(std::string name, Colormap colormap);


        // Indices
        void setIndex(std::vector<uint3> indices);
        void setIndex(std::vector<unsigned int> indices);

        // Call once to initialize GLSL code used by multiple shaders
        static void initCommonShaders();
        
        // Draw!
        void draw();

    private:


        // Classes to keep track of attributes and uniforms
        struct GLUniform {
            std::string name;
            GLData type;
            GLuint location;
            bool isSet;  // has a value been assigned to this uniform?
        };
        struct GLAttribute {
            std::string name;
            GLData type;
            GLuint location;
            GLuint VBOLoc;
            long int dataSize; // the size of the data currently stored in this attribute (-1 if nothing)
        };
        struct GLTexture {
            std::string name;
            unsigned int dim;
            GLuint location;
            GLuint bufferLoc;
            unsigned int index;
            bool isSet;
        };

       
        // The shader objects to use, which generally come from shaders.h
        const VertShader* vertShader = nullptr;
        const TessShader* tessShader = nullptr;
        const EvalShader* evalShader = nullptr;
        const GeomShader* geomShader = nullptr;
        const FragShader* fragShader = nullptr;

        
        // Lists of attributes and uniforms that need to be set
        std::vector<GLUniform> uniforms;
        std::vector<GLAttribute> attributes;
        std::vector<GLTexture> textures;

        // What mode does this program draw in?
        DrawMode drawMode; 

        // How much data is there to draw
        unsigned int drawDataLength;

        // Does this program use indexed drawing?
        bool useIndex = false;
        long int indexSize = -1;
        
        // Tessellation parameters
        GLint nPatchVertices;

        // Setup routines
        void compileGLProgram();
        void setDataLocations();
        void createBuffers();
        void addUniqueAttribute(ShaderAttribute attribute);
        void deleteAttributeBuffer(GLAttribute attribute);
        void addUniqueUniform(ShaderUniform uniform);
        void addUniqueTexture(ShaderTexture texture);
        void freeTexture(GLTexture t);

        // Drawing related
        void validateData();

        // GL pointers for various useful things
        GLuint programHandle = 0;
        GLuint vertShaderHandle = 0; // vertex shader
        GLuint tessShaderHandle = 0; // tessellation control shader
        GLuint evalShaderHandle = 0; // tesselation evaluation shader
        GLuint geomShaderHandle = 0; // geometry shader
        GLuint fragShaderHandle = 0; // fragment shader
        GLuint vaoHandle;
        GLuint indexVBO;

        static GLuint commonShaderHandle; // functions accessible to all shaders
};



// Utility functions
void printShaderInfoLog(GLuint shaderHandle);
void printProgramInfoLog(GLuint handle);
void checkGLError(bool fatal = false);

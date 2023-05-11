#include <iostream>             // cout, cerr
#include <cstdlib>              // EXIT_FAILURE
#include <GLEW/glew.h>            // GLEW library
#include <GLFW/glfw3.h>         // GLFW library

// GLM Math Header inclusions
#include <glm/glm/glm.hpp>
#include <glm/glm/gtx/transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>      // Image loading Utility functions

// Camera class (added to project directory because I edited the file)
#include "camera.h"

using namespace std; // Standard namespace

/*Shader program Macro*/
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif

// Unnamed namespace
namespace
{
    const char* const WINDOW_TITLE = "Tutorial 4.4"; // Macro for window title

    // Variables for window width and height
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;

    // Stores the GL data relative to a given mesh
    struct GLMesh
    {
        GLuint vao;         // Handle for the vertex array object
        GLuint vbo;         // Handle for the vertex buffer object
        GLuint ebo;         // EBO handle
        GLuint nVertices;    // Number of indices of the mesh
    };

    // Main GLFW window
    GLFWwindow* gWindow = nullptr;
    // Triangle mesh data
    GLMesh cylinderMesh, planeMesh, circleMesh;
    // Shader program
    GLuint gProgramId;

    // camera
    Camera gCamera(glm::vec3(0.0f, 4.0f, 10.0f)); // starting position
    float gLastX = WINDOW_WIDTH / 2.0f;
    float gLastY = WINDOW_HEIGHT / 2.0f;
    bool gFirstMouse = true;

    // Camera Field of View
    GLfloat fov = 45.0f;

    // View Tracker
    bool inPerspectiveView = true;

    // Checking to see if projection was changed on last frame for smoother changes
    bool lastFrameCheck = false;

    // timing
    float gDeltaTime = 0.0f; // time between current frame and last frame
    float gLastFrame = 0.0f;

    // Vertex data
    // -----------------------------------------------------------
    // Cylinder with Cone top
    GLfloat cylinderVertices[] = {

        1.0f, 0.0f, 0.0f, // vert 0 Bottom clockwise
        0.0f, 1.0f, 0.0f, // green
        0.0, 0.0, // Texture coord
        0.75f, 0.25f, 0.0f,  // Normal coordinate positive z ??? will need to test

        0.5f, 0.0f, 0.866f, // vert 1 Bottom counter clockwise
        0.0f, 1.0f, 0.0f, // green
        1.0, 0.0, // Texture coord
        0.75f, 0.25f, 0.0f,  // Normal coordinate positive z ??? will need to test

        0.5f, 3.0f, 0.866f, // vert 2 Top base counter clockwise
        0.0f, 1.0f, 0.0f, // green
        1.0, 0.75, // Texture coord
        0.75f, 0.25f, 0.0f,  // Normal coordinate positive z ??? will need to test

        1.0f, 3.0f, 0.0f, // vert 3 Top base clockwise
        0.0f, 1.0f, 0.0f, // green	
        0.0, 0.75, // Texture coord
        0.75f, 0.25f, 0.0f,  // Normal coordinate positive z ??? will need to test

        0.0f, 4.0f, 0.0f, // vert 4 Top point
        1.0f, 0.0f, -1.0f, // purp
        0.5, 1.0, // Texture coord
        0.75f, 0.25f, 0.0f,  // Normal coordinate positive z ??? will need to test
    };

    // Define cylinder indices
    GLubyte cylinderIndices[] = {
        0, 1, 2,
        2, 0, 3,
        3, 4, 2
    };

    // Plane y = 0
    GLfloat planeVertices[] = {
        // positon attributes (x,y,z)
        1.0f, 0.0f, 1.0f,  // vert 0 Top right
        0.0f, 0.0f, 1.0f, // blue
        1.0, 1.0, // Texture coord
        0.1f, 0.8f, 0.1,  // Normal coordinate positive y

        1.0f, 0.0f, -1.0f, // vert 1 Bottom right
        0.0f, 0.0f, 1.0f, // blue
        1.0, 0.0, // Texture coord
        0.1f, 0.8f, 0.1,  // Normal coordinate positive y

        -1.0f, 0.0f, -1.0f, // vert 2 Bottom left
        0.0f, 0.0f, 1.0f, // blue
        0.0, 0.0, // Texture coord
        0.1f, 0.8f, 0.1,  // Normal coordinate positive y

        -1.0f, 0.0f, 1.0f, // vert 3 Top left
        0.0f, 0.0f, 1.0f, // blue
        0.0, 1.0, // Texture coord
        0.1f, 0.8f, 0.1,  // Normal coordinate positive y
    };

    // Define plane indices
    GLubyte planeIndices[] = {
        0, 1, 2,
        2, 3, 0,
    };

    // Circle
    GLfloat circleVertices[] = {

        1.0f, 0.0f, 0.0f, // vert 0 Bottom clockwise
        0.0f, 1.0f, 0.0f, // green
        0.0, 0.0, // Texture coord
        0.75f, 0.25f, 0.0f,  // Normal coordinate positive z ??? will need to test

        0.5f, 0.0f, 0.866f, // vert 1 Bottom counter clockwise
        0.0f, 1.0f, 0.0f, // green
        1.0, 0.0, // Texture coord
        0.75f, 0.25f, 0.0f,  // Normal coordinate positive z ??? will need to test

        0.5f, 1.0f, 0.866f, // vert 2 Top base counter clockwise
        0.0f, 1.0f, 0.0f, // green
        1.0, 0.5, // Texture coord
        0.75f, 0.25f, 0.0f,  // Normal coordinate positive z ??? will need to test

        1.0f, 1.0f, 0.0f, // vert 3 Top base clockwise
        0.0f, 1.0f, 0.0f, // green	
        0.0, 0.5, // Texture coord
        0.75f, 0.25f, 0.0f,  // Normal coordinate positive z ??? will need to test

        0.0f, 1.0f, 0.0f, // vert 4 Top point
        1.0f, 0.0f, -1.0f, // purp
        0.5, 1.0, // Texture coord
        0.75f, 0.25f, 0.0f,  // Normal coordinate positive z ??? will need to test
    };

    // Define cylinder indices
    GLubyte circleIndices[] = {
        0, 1, 2,
        2, 0, 3,
        3, 4, 2
    };

    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerColor = 3;
    const GLuint floatsPerUV = 2;
    const GLuint floatsPerNormal = 3;

    // Shape positions
    glm::vec3 bottlePosition = glm::vec3(3.0f, 0.0f, -3.0f);
    glm::vec3 floorPosition = glm::vec3(0.f, 0.f, 0.f);
    glm::vec3 windowPosition = glm::vec3(0.f, 5.0f, -5.0f);
    glm::vec3 circlePosition = glm::vec3(2.0f, 0.0f, 2.0f);

    glm::vec3 tapePosition1 = glm::vec3(-3.0f, 1.0f, -2.0f);  // right
    glm::vec3 tapePosition2 = glm::vec3(-3.0f, 1.0f, -3.0f);  // left
    glm::vec3 tapePosition3 = glm::vec3(-1.0f, 1.0f, -2.15f);  // front
    glm::vec3 tapePosition4 = glm::vec3(-5.0f, 1.0f, -2.85f);  // back
    glm::vec3 tapePosition5 = glm::vec3(-3.0f, 2.0f, -2.5f);  // top

    glm::vec3 staplerPosition1 = glm::vec3(-3.0f, 1.0f, 2.0f);  // right
    glm::vec3 staplerPosition2 = glm::vec3(-2.5f, 1.0f, 1.5f);  // left
    glm::vec3 staplerPosition3 = glm::vec3(-1.6f, 1.0f, 3.05f);  // front
    glm::vec3 staplerPosition4 = glm::vec3(-3.88f, 1.0f, 0.47f);  // back
    glm::vec3 staplerPosition5 = glm::vec3(-2.75f, 2.0f, 1.75f);  // top

    // Light source position
    glm::vec3 lightPosition(0.0f, 10.0f, 5.0f);

    // Shape transformations
    glm::vec3 windowFloorScale = glm::vec3(5.0f, 5.0f, 5.0f);
    glm::vec3 bottleScale = glm::vec3(0.75f, 1.0f, 0.75f);  // x and z must be equal or roation gets off
    glm::vec3 circleScale = glm::vec3(1.0f, 0.75f, 1.0f);

    glm::vec3 tapeSideScale = glm::vec3(2.0f, 2.0f, 1.0f);
    glm::vec3 tapeFrontBackScale = glm::vec3(0.5f, 1.0f, 1.0f);
    glm::vec3 tapeTopScale = glm::vec3(2.0f, 1.0f, 0.5f);

    glm::vec3 staplerSideScale = glm::vec3(1.5f, 2.0f, 1.0f);
    glm::vec3 staplerFrontBackScale = glm::vec3(0.35f, 1.0f, 1.0f);
    glm::vec3 staplerTopScale = glm::vec3(1.7f, 1.0f, 0.35f);

    glm::vec3 lightSize = glm::vec3(1.0f, 1.0f, 1.0f);

    // Matrix declarations
    // -----------------------------------------------------------
    // Models
    glm::mat4 modelMatrix;

    // Camera/view transformation
    glm::mat4 viewMatrix;

    // Creates a perspective projection
    glm::mat4 projectionMatrix;

    // Texture handles
    GLuint bottleTexture, floorTexture, windowTexture, circleTexture;
    GLuint tapeSideTexture, tapeFrontTexture, tapeBackTexture, tapeTopTexture;
    GLuint staplerSideTexture, staplerFrontTexture, staplerBackTexture, staplerTopTexture;

    // Lighting variables
    // -----------------------------------------------------------

    GLMesh lampMesh;

    // Add lamp shader ID
    GLuint gLampProgramId;

    GLfloat lampVertices[] = {
        1.0f, 0.0f, 1.0f,  // vert 0 Top right
        1.0f, 0.0f, -1.0f, // vert 1 Bottom right
        -1.0f, 0.0f, -1.0f, // vert 2 Bottom left
        -1.0f, 0.0f, 1.0f, // vert 3 Top left
    };
}

/* User-defined Function prototypes to:
 * initialize the program, set the window size,
 * redraw graphics on the window when resized,
 * and render graphics on the screen
 */
bool UInitialize(int, char* [], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
void UProcessInput(GLFWwindow* window);
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos);
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void UCreateMesh();
void UDestroyMesh(GLMesh& mesh);
void URender();
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId);
void UDestroyShaderProgram(GLuint programId);

bool UCreateTexture(const char* filename, GLuint& textureId);
void UDestroyTexture(GLuint textureId);


/* Vertex Shader Source Code*/
const GLchar* vertexShaderSource = GLSL(440,
    layout(location = 0) in vec3 position; // Vertex data from Vertex Attrib Pointer 0
    //layout(location = 1) in vec3 color;  // Color data from Vertex Attrib Pointer 1
    layout(location = 2) in vec2 textureCoordinate;
    layout(location = 3) in vec3 normal;

    out vec3 colorCoordinate;  // Added to mirror PR, test if used
    out vec2 vertexTextureCoordinate;
    out vec3 vertexNormal; // For outgoing normals to fragment shader
    out vec3 vertexFragmentPos; // For outgoing color / pixels to fragment shader

    //Global variables for the  transform matrices
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main()
    {
        gl_Position = projection * view * model * vec4(position, 1.0f); // transforms vertices to clip coordinates
        //colorCoordinate = color;  // Added to mirror PR, test if used
        vertexTextureCoordinate = textureCoordinate;
        // Add normals
        vertexNormal = mat3(transpose(inverse(model))) * normal; // get normal vectors in world space only and exclude normal translation properties
        vertexFragmentPos = vec3(model * vec4(position, 1.0f)); // Gets fragment / pixel position in world space only (exclude view and projection)
    }
    );


/* Fragment Shader Source Code*/
const GLchar* fragmentShaderSource = GLSL(440,
    //in vec3 colorCoordinate;  // Added to mirror PR, test if used
    in vec2 vertexTextureCoordinate;
    in vec3 vertexNormal; // For incoming normals
    in vec3 vertexFragmentPos; // For incoming fragment position

    out vec4 fragmentColor;

    uniform sampler2D uTexture;

    // Add light uniforms in shader
    uniform vec3 objectColor;
    uniform vec3 lightColor;
    uniform vec3 lightPosition;
    uniform vec3 viewPosition;

    void main()
    {
        // Calculate Ambient lighting
        float ambientStrength = 0.6f; // Set ambient or global lighting strength
        vec3 ambient = ambientStrength * lightColor; // Generate ambient light color

        //Calculate Diffuse lighting
        vec3 norm = normalize(vertexNormal); // Normalize vectors to 1 unit
        vec3 lightDirection = normalize(lightPosition - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
        float impact = max(dot(norm, lightDirection), 0.0);// Calculate diffuse impact by generating dot product of normal and light
        vec3 diffuse = impact * lightColor; // Generate diffuse light color

        //Calculate Specular lighting*/
        float specularIntensity = 1.5f; // Set specular light strength
        float highlightSize = 16.0f; // Set specular highlight size

        vec3 viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
        vec3 reflectDir = reflect(-lightDirection, norm);// Calculate reflection vector

        //Calculate specular component
        float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
        vec3 specular = specularIntensity * specularComponent * lightColor;

        vec3 result = (ambient + diffuse + specular) * objectColor;

        // Multiply by light uniforms
        fragmentColor = texture(uTexture, vertexTextureCoordinate) * vec4(result, 1.0f); // Sends texture to the GPU for rendering
    }
    );


// Lighting shaders
// -------------------------------------------------------------------------------------
/* Lamp Shader Source Code*/
const GLchar* lampVertexShaderSource = GLSL(440,

    layout(location = 0) in vec3 position; // VAP position 0 for vertex position data

        //Uniform / Global variables for the  transform matrices
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main()
    {
        gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates
    }
    );


    /* Lamp Fragment Shader Source Code*/
    const GLchar* lampFragmentShaderSource = GLSL(440,

        out vec4 fragmentColor; // For outgoing lamp color (smaller cube) to the GPU

    void main()
    {
        fragmentColor = vec4(1.0f); // Set color to white (1.0f,1.0f,1.0f) with alpha 1.0
    }
    );


// Images are loaded with Y axis going down, but OpenGL's Y axis goes up, so let's flip it
void flipImageVertically(unsigned char* image, int width, int height, int channels)
{
    for (int j = 0; j < height / 2; ++j)
    {
        int index1 = j * width * channels;
        int index2 = (height - 1 - j) * width * channels;

        for (int i = width * channels; i > 0; --i)
        {
            unsigned char tmp = image[index1];
            image[index1] = image[index2];
            image[index2] = tmp;
            ++index1;
            ++index2;
        }
    }
}


int main(int argc, char* argv[])
{
    if (!UInitialize(argc, argv, &gWindow))
        return EXIT_FAILURE;

    // Create the mesh
    UCreateMesh(); // Calls the function to create the Vertex Buffer Object

    // Create the shader program
    if (!UCreateShaderProgram(vertexShaderSource, fragmentShaderSource, gProgramId))
        return EXIT_FAILURE;

    // Create lamp shaders
    if (!UCreateShaderProgram(lampVertexShaderSource, lampFragmentShaderSource, gLampProgramId))
        return EXIT_FAILURE;

    // Load bottle texture
    const char* texFilename = "bottle2.jpg";
    if (!UCreateTexture(texFilename, bottleTexture))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    // Load floor texture
    texFilename = "floor_smaller.jpg";
    if (!UCreateTexture(texFilename, floorTexture))
    {
        cout << "Failed to load floor texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    // Load window texture
    texFilename = "window.jpg";
    if (!UCreateTexture(texFilename, windowTexture))
    {
        cout << "Failed to load window texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    // Load circle texture
    texFilename = "circle2.jpg";
    if (!UCreateTexture(texFilename, circleTexture))
    {
        cout << "Failed to load circle texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    // Load tape textures
    //------------------------------------------------------------
    texFilename = "tape side.jpg";
    if (!UCreateTexture(texFilename, tapeSideTexture))
    {
        cout << "Failed to load tape texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    texFilename = "tape front.jpg";
    if (!UCreateTexture(texFilename, tapeFrontTexture))
    {
        cout << "Failed to load tape texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    texFilename = "tape back.jpg";
    if (!UCreateTexture(texFilename, tapeBackTexture))
    {
        cout << "Failed to load tape texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    texFilename = "tape top.jpg";
    if (!UCreateTexture(texFilename, tapeTopTexture))
    {
        cout << "Failed to load tape texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    // Load stapler textures
    //------------------------------------------------------------
    texFilename = "stapler side.jpg";
    if (!UCreateTexture(texFilename, staplerSideTexture))
    {
        cout << "Failed to load stapler texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    texFilename = "stapler front.jpg";
    if (!UCreateTexture(texFilename, staplerFrontTexture))
    {
        cout << "Failed to load stapler texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    texFilename = "stapler back.jpg";
    if (!UCreateTexture(texFilename, staplerBackTexture))
    {
        cout << "Failed to load stapler texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    texFilename = "stapler top.jpg";
    if (!UCreateTexture(texFilename, staplerTopTexture))
    {
        cout << "Failed to load stapler texture " << texFilename << endl;
        return EXIT_FAILURE;
    }

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    glUseProgram(gProgramId);
    // We set the texture as texture unit 0
    glUniform1i(glGetUniformLocation(gProgramId, "uTexture"), 0);

    // Sets the background color of the window to black (it will be implicitely used by glClear)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(gWindow))
    {
        // per-frame timing
        // --------------------
        float currentFrame = glfwGetTime();
        gDeltaTime = currentFrame - gLastFrame;
        gLastFrame = currentFrame;

        // input
        // -----
        UProcessInput(gWindow);

        // Render this frame
        URender();

        glfwPollEvents();
    }

    // Release mesh data
    UDestroyMesh(cylinderMesh);
    UDestroyMesh(planeMesh);
    UDestroyMesh(circleMesh);

    // Release texture
    UDestroyTexture(bottleTexture);
    UDestroyTexture(floorTexture);
    UDestroyTexture(windowTexture);
    UDestroyTexture(circleTexture);

    UDestroyTexture(tapeSideTexture);
    UDestroyTexture(tapeFrontTexture);
    UDestroyTexture(tapeBackTexture);
    UDestroyTexture(tapeTopTexture);

    UDestroyTexture(staplerSideTexture);
    UDestroyTexture(staplerFrontTexture);
    UDestroyTexture(staplerBackTexture);
    UDestroyTexture(staplerTopTexture);

    // Release shader program
    UDestroyShaderProgram(gProgramId);

    // Release lamp shader program
    UDestroyShaderProgram(gLampProgramId);

    exit(EXIT_SUCCESS); // Terminates the program successfully
}


// Initialize GLFW, GLEW, and create a window
bool UInitialize(int argc, char* argv[], GLFWwindow** window)
{
    // GLFW: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // GLFW: window creation
    // ---------------------
    * window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (*window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(*window);
    glfwSetFramebufferSizeCallback(*window, UResizeWindow);
    glfwSetCursorPosCallback(*window, UMousePositionCallback);
    glfwSetScrollCallback(*window, UMouseScrollCallback);
    glfwSetMouseButtonCallback(*window, UMouseButtonCallback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // GLEW: initialize
    // ----------------
    // Note: if using GLEW version 1.13 or earlier
    glewExperimental = GL_TRUE;
    GLenum GlewInitResult = glewInit();

    if (GLEW_OK != GlewInitResult)
    {
        std::cerr << glewGetErrorString(GlewInitResult) << std::endl;
        return false;
    }

    // Displays GPU OpenGL version
    cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;

    return true;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void UProcessInput(GLFWwindow* window)
{
    static const float cameraSpeed = 2.5f;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        gCamera.ProcessKeyboard(FORWARD, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        gCamera.ProcessKeyboard(BACKWARD, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        gCamera.ProcessKeyboard(LEFT, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        gCamera.ProcessKeyboard(RIGHT, gDeltaTime);
    // Added Q = up function (ADDED CODE TO CAMERA.H FOR THIS)
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        gCamera.ProcessKeyboard(UP, gDeltaTime);
    // Added E = down function (ADDED CODE TO CAMERA.H FOR THIS)
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        gCamera.ProcessKeyboard(DOWN, gDeltaTime);

    // Add perspective switch
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        if (!lastFrameCheck) {
            //glOrtho(-2.0f, 2.0f, 0.0f, 10.0f, -2.0f, 2.0f);
            inPerspectiveView = !inPerspectiveView;
            cout << "Perspective changed!" << endl;
            lastFrameCheck = true;
        }
        else {
            //glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
            lastFrameCheck = false;
        }
    }
}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void UResizeWindow(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (gFirstMouse)
    {
        gLastX = xpos;
        gLastY = ypos;
        gFirstMouse = false;
    }

    float xoffset = xpos - gLastX;
    float yoffset = gLastY - ypos; // reversed since y-coordinates go from bottom to top

    gLastX = xpos;
    gLastY = ypos;

    gCamera.ProcessMouseMovement(xoffset, yoffset);
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    gCamera.MovementSpeed += yoffset; // Updated so scroll changes movement speed
}

// glfw: handle mouse button events
// --------------------------------
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    switch (button)
    {
    case GLFW_MOUSE_BUTTON_LEFT:
    {
        if (action == GLFW_PRESS)
            cout << "Left mouse button pressed" << endl;
        else
            cout << "Left mouse button released" << endl;
    }
    break;

    case GLFW_MOUSE_BUTTON_MIDDLE:
    {
        if (action == GLFW_PRESS)
            cout << "Middle mouse button pressed" << endl;
        else
            cout << "Middle mouse button released" << endl;
    }
    break;

    case GLFW_MOUSE_BUTTON_RIGHT:
    {
        if (action == GLFW_PRESS)
            cout << "Right mouse button pressed" << endl;
        else
            cout << "Right mouse button released" << endl;
    }
    break;

    default:
        cout << "Unhandled mouse button event" << endl;
        break;
    }
}


// Function called to render a frame
void URender()
{
    // Pre-drawing/Set up phase
    //---------------------------------------------------------------------------------------------------------

    // Enable z-depth
    glEnable(GL_DEPTH_TEST);

    // Clear the frame and z buffers
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set the shader to be used
    glUseProgram(gProgramId);

    // Define projection matrix and allow perspective switch
    if (inPerspectiveView){
        projectionMatrix = glm::perspective(fov, GLfloat(WINDOW_WIDTH / WINDOW_HEIGHT), 0.1f, 100.0f);
    }
    else{
        projectionMatrix = glm::ortho(0.0f, 5.0f, 0.0f, 5.0f, 0.1f, 100.0f);
    }

    // Define View Matrix
    viewMatrix = gCamera.GetViewMatrix();

    // Get matrix's uniform location and set matrix
    GLint modelLoc = glGetUniformLocation(gProgramId, "model");
    GLint viewLoc = glGetUniformLocation(gProgramId, "view");
    GLint projLoc = glGetUniformLocation(gProgramId, "projection");

    // Get light and object color, and light position location
    GLint objectColorLoc = glGetUniformLocation(gProgramId, "objectColor");
    GLint lightColorLoc = glGetUniformLocation(gProgramId, "lightColor");
    GLint lightPositionLoc = glGetUniformLocation(gProgramId, "lightPosition");
    GLint viewPositionLoc = glGetUniformLocation(gProgramId, "viewPosition");

    // Assign object color
    glUniform3f(objectColorLoc, 0.64f, 0.64f, 0.63f); // White part of floor color

    // Assign light color
    glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f); // White light

    // Assign light position
    glUniform3f(lightPositionLoc, lightPosition.x, lightPosition.y, lightPosition.z);

    // Specify view and location details to shader
    const glm::vec3 cameraPosition = gCamera.Position;
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Drawing phase
    //---------------------------------------------------------------------------------------------------------

    // Draw floor
    //---------------------------------------------------------------------------------------------------------
    glBindVertexArray(planeMesh.vao);

    // Bind floor texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, floorTexture);

    // Clear model Matrix
    modelMatrix = glm::mat4(1.0f);

    // Perform transformations
    modelMatrix = glm::translate(modelMatrix, floorPosition);
    modelMatrix = glm::scale(modelMatrix, windowFloorScale);

    // Send model matrix to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Draw
    glDrawElements(GL_TRIANGLES, sizeof(planeIndices), GL_UNSIGNED_BYTE, nullptr);

    // End draw floor
    // Deactivate the Vertex Array Object
    glBindVertexArray(0);

    // Draw window
    //---------------------------------------------------------------------------------------------------------
    glBindVertexArray(planeMesh.vao);

    // Bind window texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, windowTexture);

    // Clear model Matrix
    modelMatrix = glm::mat4(1.0f);

    // Tansformations
    // Move back on Z and up on y
    modelMatrix = glm::translate(modelMatrix, windowPosition);
    // Rotate 90 degrees on x axis
    modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    modelMatrix = glm::scale(modelMatrix, windowFloorScale);

    // Send model matrix to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Draw
    glDrawElements(GL_TRIANGLES, sizeof(planeIndices), GL_UNSIGNED_BYTE, nullptr);

    // End draw window
    // Deactivate the Vertex Array Object
    glBindVertexArray(0);

    // Draw Bottle
    //---------------------------------------------------------------------------------------------------------
    glBindVertexArray(cylinderMesh.vao);

    // Bind bottle texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bottleTexture);


    // Array for triangle rotations
    glm::float32 triRotations[] = { 0.0f, 60.0f, 120.0f, 180.0f, 240.0f, 300.0f };

    // Use loop to build Model matrix for triangle
    for (int i = 0; i < 6; i++) {
        // Clears model matrix
        modelMatrix = glm::translate(glm::mat4(1.0f), bottlePosition); // Position strip at 0,0,0

        // Rotate around y axis
        modelMatrix = glm::rotate(modelMatrix, glm::radians(triRotations[i]), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate strip on z by increments in array		

        modelMatrix = glm::scale(modelMatrix, bottleScale);

        // Send model matrix to shader
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

        // Draw
        glDrawElements(GL_TRIANGLES, sizeof(cylinderIndices), GL_UNSIGNED_BYTE, nullptr);
    }

    // End draw cylinder
    // Deactivate the Vertex Array Object
    glBindVertexArray(0);

    // Draw tape
    //---------------------------------------------------------------------------------------------------------
    glBindVertexArray(planeMesh.vao);

    // Bind tape texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tapeSideTexture);

    // Tape right side (camera facing)
    // Clear model Matrix
    modelMatrix = glm::mat4(1.0f);

    // Tansformations
    // Move back on Z and up on y
    modelMatrix = glm::translate(modelMatrix, tapePosition1);
    // Rotate 90 degrees on x axis
    modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    modelMatrix = glm::scale(modelMatrix, tapeSideScale);

    // Add tape skew
    modelMatrix = glm::rotate(modelMatrix, glm::radians(10.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    // Send model matrix to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Draw
    glDrawElements(GL_TRIANGLES, sizeof(planeIndices), GL_UNSIGNED_BYTE, nullptr);

    // Tape left side
    // Clear model Matrix
    modelMatrix = glm::mat4(1.0f);

    // Tansformations
    // Move back on Z and up on y
    modelMatrix = glm::translate(modelMatrix, tapePosition2);

    // Rotate 90 degrees on x axis
    modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    modelMatrix = glm::scale(modelMatrix, tapeSideScale);

    // Add tape skew
    modelMatrix = glm::rotate(modelMatrix, glm::radians(10.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    // Send model matrix to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Draw
    glDrawElements(GL_TRIANGLES, sizeof(planeIndices), GL_UNSIGNED_BYTE, nullptr);

    // Tape front
    // Bind tape front texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tapeFrontTexture);

    // Clear model Matrix
    modelMatrix = glm::mat4(1.0f);

    // Tansformations
    // Move back on Z and up on y
    modelMatrix = glm::translate(modelMatrix, tapePosition3);

    // Rotate 90 degrees on x axis
    modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    
    // Rotate 90 degrees on z axis
    modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    // Add tape skew DOESN'T WORK?
    //modelMatrix = glm::rotate(modelMatrix, glm::radians(10.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    modelMatrix = glm::scale(modelMatrix, tapeFrontBackScale);

    // Send model matrix to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Draw
    glDrawElements(GL_TRIANGLES, sizeof(planeIndices), GL_UNSIGNED_BYTE, nullptr);

    // Tape back
    // Bind tape front texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tapeBackTexture);

    // Clear model Matrix
    modelMatrix = glm::mat4(1.0f);

    // Tansformations
    // Move back on Z and up on y
    modelMatrix = glm::translate(modelMatrix, tapePosition4);

    // Rotate 90 degrees on x axis
    modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    // Add tape skew UNECESSARY AGAIN
    //modelMatrix = glm::rotate(modelMatrix, glm::radians(10.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    // Rotate 90 degrees on z axis
    modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    modelMatrix = glm::scale(modelMatrix, tapeFrontBackScale);

    // Send model matrix to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Draw
    glDrawElements(GL_TRIANGLES, sizeof(planeIndices), GL_UNSIGNED_BYTE, nullptr);
    
    // Tape top
    // Bind tape texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tapeTopTexture);

    // Clear model Matrix
    modelMatrix = glm::mat4(1.0f);

    // Tansformations
    // Move back on Z and up on y
    modelMatrix = glm::translate(modelMatrix, tapePosition5);

    // Add tape skew
    modelMatrix = glm::rotate(modelMatrix, glm::radians(-10.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    modelMatrix = glm::scale(modelMatrix, tapeTopScale);

    // Stretch top (NO IDEA WHY THIS IS NECESSARY)
    modelMatrix = glm::rotate(modelMatrix, glm::radians(2.5f), glm::vec3(0.0f, 1.0f, 0.0f));

    // Send model matrix to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Draw
    glDrawElements(GL_TRIANGLES, sizeof(planeIndices), GL_UNSIGNED_BYTE, nullptr);

    // Draw stapler
   //---------------------------------------------------------------------------------------------------------

    // Bind stapler texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, staplerSideTexture);

    // Stapler right side (camera facing)
    // Clear model Matrix
    modelMatrix = glm::mat4(1.0f);

    // Tansformations
    // Move back on Z and up on y
    modelMatrix = glm::translate(modelMatrix, staplerPosition1);
    // Rotate 90 degrees on x axis
    modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    modelMatrix = glm::scale(modelMatrix, staplerSideScale);
    
    // Add stapler skew rotate 40 degrees on z axis
    modelMatrix = glm::rotate(modelMatrix, glm::radians(40.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    // Send model matrix to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Draw
    glDrawElements(GL_TRIANGLES, sizeof(planeIndices), GL_UNSIGNED_BYTE, nullptr);
    
    // Stapler left side
    // Clear model Matrix
    modelMatrix = glm::mat4(1.0f);

    // Tansformations
    // Move back on Z and up on y
    modelMatrix = glm::translate(modelMatrix, staplerPosition2);
    // Rotate 90 degrees on x axis
    modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    modelMatrix = glm::scale(modelMatrix, staplerSideScale);

    // Add stapler skew rotate 40 degrees on z axis
    modelMatrix = glm::rotate(modelMatrix, glm::radians(40.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    // Send model matrix to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Draw
    glDrawElements(GL_TRIANGLES, sizeof(planeIndices), GL_UNSIGNED_BYTE, nullptr);

    // Stapler front
    // Bind stapler texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, staplerFrontTexture);

    // Stapler front
    // Clear model Matrix
    modelMatrix = glm::mat4(1.0f);

    // Tansformations
    // Move back on Z and up on y
    modelMatrix = glm::translate(modelMatrix, staplerPosition3);

    // Rotate 90 degrees on x axis
    modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    // Rotate  degrees on z axis
    modelMatrix = glm::rotate(modelMatrix, glm::radians(135.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    modelMatrix = glm::scale(modelMatrix, staplerFrontBackScale);

    // Send model matrix to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Draw
    glDrawElements(GL_TRIANGLES, sizeof(planeIndices), GL_UNSIGNED_BYTE, nullptr);

   // Stapler back
   // Bind stapler texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, staplerBackTexture);

   // Stapler back
   // Clear model Matrix
    modelMatrix = glm::mat4(1.0f);

    // Tansformations
    // Move back on Z and up on y
    modelMatrix = glm::translate(modelMatrix, staplerPosition4);

    // Rotate 90 degrees on x axis
    modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    // Rotate  degrees on z axis
    modelMatrix = glm::rotate(modelMatrix, glm::radians(135.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    modelMatrix = glm::scale(modelMatrix, staplerFrontBackScale);

    // Send model matrix to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Draw
    glDrawElements(GL_TRIANGLES, sizeof(planeIndices), GL_UNSIGNED_BYTE, nullptr);

   // Stapler top
   // Bind stapler texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, staplerTopTexture);

   // Clear model Matrix
    modelMatrix = glm::mat4(1.0f);

    // Tansformations
    // Move back on Z and up on y
    modelMatrix = glm::translate(modelMatrix, staplerPosition5);

    // Add stapler skew (I know there's math to get these numbers, just winging it though)
    modelMatrix = glm::rotate(modelMatrix, glm::radians(131.5f), glm::vec3(0.0f, 1.0f, 0.0f));

    modelMatrix = glm::scale(modelMatrix, staplerTopScale);

    // Send model matrix to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Draw
    glDrawElements(GL_TRIANGLES, sizeof(planeIndices), GL_UNSIGNED_BYTE, nullptr);

    // Draw Circle
    //---------------------------------------------------------------------------------------------------------
    glBindVertexArray(circleMesh.vao);

    // Bind circle texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, circleTexture);

    // Use loop to build Model matrix for triangle
    for (int i = 0; i < 6; i++) {
        // Clears model matrix
        modelMatrix = glm::translate(glm::mat4(1.0f), circlePosition); // Position strip at 0,0,0

        // Rotate around y axis
        modelMatrix = glm::rotate(modelMatrix, glm::radians(triRotations[i]), glm::vec3(0.0f, 1.0f, 0.0f)); // Rotate strip on z by increments in array		

        modelMatrix = glm::scale(modelMatrix, circleScale);

        // Send model matrix to shader
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

        // Draw
        glDrawElements(GL_TRIANGLES, sizeof(cylinderIndices), GL_UNSIGNED_BYTE, nullptr);
    }

    // End draw cylinder

    // ADD OTHER DRAWS HERE
    ///////////////////////////////////////////////////

    // Deactivate the Vertex Array Object
    glBindVertexArray(0);

    // Unbind shader
    glUseProgram(0);

    // Bind lamp shader
    glUseProgram(gLampProgramId);

    // Get matrix's uniform location and set matrix
    GLint lampModelLoc = glGetUniformLocation(gLampProgramId, "model");
    GLint lampViewLoc = glGetUniformLocation(gLampProgramId, "view");
    GLint lampProjLoc = glGetUniformLocation(gLampProgramId, "projection");

    // Trying without model here
    //glUniformMatrix4fv(lampModelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glUniformMatrix4fv(lampViewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(lampProjLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Start draw lamp
    glBindVertexArray(lampMesh.vao);

    // Clear model Matrix
    modelMatrix = glm::mat4(1.0f);

    // Transformations
    modelMatrix = glm::translate(modelMatrix, floorPosition + lightPosition);
    modelMatrix = glm::scale(modelMatrix, lightSize);

    // Send model to shader
    glUniformMatrix4fv(lampModelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Draw
    glDrawElements(GL_TRIANGLES, sizeof(planeIndices), GL_UNSIGNED_BYTE, nullptr);

    // End draw lamp
    // Deactivate the Vertex Array Object
    glBindVertexArray(0);

    // Unbind shader
    glUseProgram(0);

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    glfwSwapBuffers(gWindow);    // Flips the the back buffer with the front buffer every frame.*/
}


// Implements the UCreateMesh function
void UCreateMesh()
{
    // Strides between vertex coordinates
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerUV + floatsPerNormal);

    // Create handles
    glGenBuffers(1, &cylinderMesh.vbo);
    glGenBuffers(1, &cylinderMesh.ebo);
    glGenVertexArrays(1, &cylinderMesh.vao);

    glGenBuffers(1, &planeMesh.vbo);
    glGenBuffers(1, &planeMesh.ebo);
    glGenVertexArrays(1, &planeMesh.vao);

    glGenBuffers(1, &circleMesh.vbo);
    glGenBuffers(1, &circleMesh.ebo);
    glGenVertexArrays(1, &circleMesh.vao);

    // Cylinder-------------------------------------------------------
    // Load VBO and EBO into VAO
    glBindVertexArray(cylinderMesh.vao);
    glBindBuffer(GL_ARRAY_BUFFER, cylinderMesh.vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cylinderMesh.ebo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cylinderVertices), cylinderVertices, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cylinderIndices), cylinderIndices, GL_STATIC_DRAW); // Load indices

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerColor)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerUV)));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0); // Unbind VOA or close off (Must call VOA explicitly in loop)

    // Plane-------------------------------------------------------
    // Load VBO and EBO into VAO
    glBindVertexArray(planeMesh.vao);
    glBindBuffer(GL_ARRAY_BUFFER, planeMesh.vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planeMesh.ebo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(planeIndices), planeIndices, GL_STATIC_DRAW); // Load indices

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerColor)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerUV)));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0); // Unbind VOA or close off (Must call VOA explicitly in loop)

    // Circle-------------------------------------------------------
    // Load VBO and EBO into VAO
    glBindVertexArray(circleMesh.vao);
    glBindBuffer(GL_ARRAY_BUFFER, circleMesh.vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, circleMesh.ebo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(circleVertices), circleVertices, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(circleIndices), circleIndices, GL_STATIC_DRAW); // Load indices

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerColor, GL_FLOAT, GL_FALSE, stride, (char*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerColor)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerColor + floatsPerUV)));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0); // Unbind VOA or close off (Must call VOA explicitly in loop)

    // Lamp-------------------------------------------------------
    // Load VBO and EBO into VAO

    // Generate handles
    glGenBuffers(1, &lampMesh.vbo);
    glGenBuffers(1, &lampMesh.ebo);
    glGenVertexArrays(1, &lampMesh.vao);

    // Load VAO 
    glBindVertexArray(lampMesh.vao);
    glBindBuffer(GL_ARRAY_BUFFER, lampMesh.vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lampMesh.ebo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lampVertices), lampVertices, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(planeIndices), planeIndices, GL_STATIC_DRAW); // Load indices

    // Create Vertex Attribute Pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(float), 0);  // shortcut 3 vertex attributes, stride is 0
    glEnableVertexAttribArray(0);

    glBindVertexArray(0); // Unbind VOA or close off (Must call VOA explicitly in loop)
}


void UDestroyMesh(GLMesh& mesh)
{
    glDeleteVertexArrays(1, &mesh.vao);
    glDeleteBuffers(1, &mesh.vbo);
}


// Implements the UCreateShaders function
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId)
{
    // Compilation and linkage error reporting
    int success = 0;
    char infoLog[512];

    // Create a Shader program object.
    programId = glCreateProgram();

    // Create the vertex and fragment shader objects
    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    // Retrive the shader source
    glShaderSource(vertexShaderId, 1, &vtxShaderSource, NULL);
    glShaderSource(fragmentShaderId, 1, &fragShaderSource, NULL);

    // Compile the vertex shader, and print compilation errors (if any)
    glCompileShader(vertexShaderId); // compile the vertex shader
    // check for shader compile errors
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glCompileShader(fragmentShaderId); // compile the fragment shader
    // check for shader compile errors
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShaderId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    // Attached compiled shaders to the shader program
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);

    glLinkProgram(programId);   // links the shader program
    // check for linking errors
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(programId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;

        return false;
    }

    glUseProgram(programId);    // Uses the shader program

    return true;
}


void UDestroyShaderProgram(GLuint programId)
{
    glDeleteProgram(programId);
}

/*Generate and load the texture*/
bool UCreateTexture(const char* filename, GLuint& textureId)
{
    int width, height, channels;
    unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);
    if (image)
    {
        flipImageVertically(image, width, height, channels);

        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (channels == 3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        else if (channels == 4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        else
        {
            cout << "Not implemented to handle image with " << channels << " channels" << endl;
            return false;
        }

        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(image);
        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

        return true;
    }

    // Error loading the image
    return false;
}

void UDestroyTexture(GLuint textureId)
{
    glGenTextures(1, &textureId);
}
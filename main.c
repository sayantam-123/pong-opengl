#include<stdio.h>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<stdbool.h>
#include<cglm/cglm.h>
#include<math.h>
#include<stb_easy_font.h>
#define STB_EASY_FONT_IMPLEMENTATION

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct shaders{
	int ID;
	int vertexsource;
	int fragsource;
}shaders;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

const unsigned int width = 800;
const unsigned int height = 600;

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "uniform mat4 model;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = model*vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";

mat4 model;
mat4 circle_model;
vec3 circle_pos;
vec3 box_pos;


int main(){
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width,height,"pong",NULL,NULL);
	if(window == NULL){
		printf("Failed to create window");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
		printf("failed to initialize GLAD");
		return -1;

	}



	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

	unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        -0.05f, -0.25f, 0.0f, // left
        -0.05f, 0.25f, 0.0f, // right
         0.05f, -0.25f, 0.0f,  // top
        -0.05f, 0.25f, 0.0f,
         0.05f, 0.25f, 0.0f,
         0.05f, -0.25f, 0.0f
    };

    float box_hor = 0.05f;
    float box_ver = 0.25f;


    float circleverts[(32+2)*3];
    float radius = 0.05f;


    int score = 0;

    //verices for circles;

    circleverts[0] = 0.0f;
    circleverts[1] = 0.0f;
    circleverts[2] = 0.0f;

    for(int i = 0;i<=32;i++){
        float angle = (2*M_PI*i)/32;
        float x = cosf(angle)*radius;
        float y = sinf(angle)*radius;
        float z = 0.0f;

        circleverts[(i + 1) * 3 + 0] = x;
        circleverts[(i + 1) * 3 + 1] = y;
        circleverts[(i + 1) * 3 + 2] = z;

    }


    //vec3 for going

    vec3 circle_go = {0.02,0.01,0};

    //for square

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);


    //for circle
    unsigned int circleVBO,circleVAO;
    glGenVertexArrays(1,&circleVAO);
    glGenBuffers(1, &circleVBO);
    glBindVertexArray(circleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, circleVBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(circleverts), circleverts, GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float), (void*)0);

    glEnableVertexAttribArray(0);


    glm_mat4_identity(model);

    glm_mat4_identity(circle_model);

    glm_translate(model,(vec3){0.95f,0,0});

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram,"model"),1,GL_FALSE,(float*)model);


	while(!glfwWindowShouldClose(window)){
		processInput(window);
		
            //getting circle gl_Position
            circle_pos[0] = circle_model[3][0];
            circle_pos[1] = circle_model[3][1];
            circle_pos[2] = circle_model[3][2];

            //getting box position
            box_pos[0] = model[3][0];
            box_pos[1] = model[3][1];
            box_pos[2] = model[3][2];


            //checking wall collision
            //for horizontal walls
            if(circle_pos[0]+radius>1.0f){
                //right collision
                circle_go[0]*=-1.0f;
            }
            else if(circle_pos[0]-radius<-1.0f){
                //left collision
                circle_go[0]*=-1.0f;
            }

            //for vertical walls
            if(circle_pos[1]+radius>1.0f){
                //top collision
                circle_go[1]*=-1.0f;
            }
            else if(circle_pos[1]-radius<-1.0f){
                //bottom collision
                circle_go[1]*=-1.0f;
            }


            //circle box collision

            if((circle_pos[0]+radius>box_pos[0]-box_hor)&&(circle_pos[1]-radius<box_pos[1]+box_ver)&&(circle_pos[1]+radius>box_pos[1]-box_ver)){
                circle_go[0]*=-1.0f;
            }
            else if((circle_pos[1]-radius<box_pos[1]+box_ver)&&(circle_pos[0]+radius>box_pos[0]-box_hor)){
                circle_go[1]*=-1.0f;
            }
            else if((circle_pos[1]+radius>box_pos[1]-box_ver)&&(circle_pos[0]+radius>box_pos[0]-box_hor)){
                circle_go[1]*=-1.0f;
            }


		// ------
        	glClearColor(0.2f, 0.3f, 0.6f, 1.0f);
        	glClear(GL_COLOR_BUFFER_BIT);

        	// draw our first triangle
        	glUseProgram(shaderProgram);
        	glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram,"model"),1,GL_FALSE,(float*)model);
        	glDrawArrays(GL_TRIANGLES, 0, 6);

            glUseProgram(shaderProgram);
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram,"model"),1,GL_FALSE,(float*)circle_model);
            glBindVertexArray(circleVAO);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 34);


		glfwSwapBuffers(window);


            glm_translate(circle_model,circle_go);

		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window){
	if (glfwGetKey(window, GLFW_KEY_ESCAPE)==GLFW_PRESS){
		glfwSetWindowShouldClose(window, true);
	}


    if (glfwGetKey(window, GLFW_KEY_UP)==GLFW_PRESS){
        glm_translate_y(model,0.03);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN)==GLFW_PRESS){
        glm_translate_y(model,-0.03);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
	glViewport(0,0,width,height);
}

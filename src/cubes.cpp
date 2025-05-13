/*Because OpenGL works majorly with triangle primitives, in order to draw a
rectangle two triagles are drawn with the help of element buffer object*/

#include <iostream>
#include <cmath>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
extern "C" {
	#include <stb_image.h>
	#include "glad/glad.h"	//include glad.h before glfw3.h
	#include <GLFW/glfw3.h>
	
}

#define pi 3.142

using namespace std;
/*vertex buffer object stores a large number of vertices in the GPU memory
vertex array object stores pointers to vertex attributes in the bound VBO
as well as stores element buffer object of the bound EBO*/
unsigned int VAO,VBO,EBO,vertexShader,fragmentShader,shaderProgram;
unsigned int texture[2];
float wWidth = 600;
float wHeight=500;
int texWidth,texHeight,texChannels;
unsigned char *data;
const char *fileName;
float cameraRadius = 3.0f;
//normalUp is parallel to current vertical-axis. Default is y-axis
float normalUp[]={0.0f,1.0f,0.0f}; 
float lastCursorPos[]={300,250};
float cursorOffset[2];
float cursorSensitivity = 0.1f;
float yaw = 0.5*pi; //initially at (0,0,1)= pi/2 radians
float pitch =0;  //initially at y=0
bool firstCursor = true; //to detect first cursor movement
float FOV = 62.5f;//Field of View
unsigned char viewIndex=1;
unsigned char selectedView=2;
glm::mat4 model=glm::mat4(1.0f);//identity matrix
glm::mat4 view=glm::mat4(1.0f);
glm::mat4 projection;
glm::vec3 cubePositions[] = {
  glm::vec3( 0.0f, 0.0f,0.0f),
  glm::vec3( 2.0f, 5.0f, -4.3f),
  glm::vec3(-1.5f, -2.2f, -2.5f),
  glm::vec3(-3.8f, -2.0f, 6.0f),
  glm::vec3( 2.4f, -0.4f, 3.5f),
  glm::vec3(-1.7f, 3.0f, 5.5f),
  glm::vec3( 1.3f, -2.0f, 2.5f),
  glm::vec3( 1.5f, 2.0f, -3.5f),
  glm::vec3( 1.5f, 0.2f, 1.5f),
  glm::vec3(-1.3f, 1.0f, -1.5f)
};
float viewPosition[][3]={
  {-0.707f, 0.0f,  0.707f},	//1
  { 0.0f,   0.0f,  1.0f},	//2
  { 0.707f, 0.0f,  0.707f},	//3
  {-1.0f,   0.0f,  0.0f},	//4
  { 0.0f,   1.0f,  0.1f},  //5=top,z=0.01f allow calculation of cameraRight
  { 1.0f,   0.0f,  0.0f},	//6
  {-0.707f, 0.0f, -0.707f},	//7
  { 0.0f,   0.0f, -1.0f},	//8
  { 0.707f, 0.0f, -0.707f},	//9
  { 0.0f,   0.0f,  1.0f} //updated viewPosition by cursor and key=1 intially
};
glm::vec3 cameraPosition,cameraTarget,cameraDirection,cameraRight,cameraUp;
void framebuffer_size_reshape(GLFWwindow * window, int width, int height){
  //if window is resized,also resize viewport and aspect ratio
  glViewport(0,0,width,height);
  projection=glm::perspective(glm::radians(62.5f), (float)width/(float)height,
   0.1f,100.0f);
  wWidth = (float)width;
  wHeight = (float)height;
}
float tanInverse(float x, float z){
  if(x<0){
    return pi+atan(z/x);
  }else{
    return atan(z/x);
    cout<<atan(z/x)<<endl;
  }
}
void mouseCallback(GLFWwindow *window,double xPos,double yPos){
  if(firstCursor){
   lastCursorPos[0]=xPos;
   lastCursorPos[1]=yPos;
   firstCursor = false;
  }
  cursorOffset[0]=(xPos-lastCursorPos[0])*cursorSensitivity;
  cursorOffset[1]=(yPos-lastCursorPos[1])*cursorSensitivity;
  lastCursorPos[0]=xPos;
  lastCursorPos[1]=yPos;
  /*yaw is sum of previous yaw and angle due to offset in the x-z plane
  pitch is sum of previous pitch and angle due to offset in the z-y plane*/
  yaw +=(cursorOffset[0]*pi/(float)180);
  pitch +=(cursorOffset[1]*pi/(float)180);
  if(pitch<(-89*pi/180)){
    pitch = -89*pi/180;
  }
  if(pitch>(89*pi/180)){
    pitch = 89*pi/180;
  }
  viewPosition[9][0]=cos(yaw)*cos(pitch);
  viewPosition[9][1]=sin(pitch);
  viewPosition[9][2]=sin(yaw)*cos(pitch);
  viewIndex=9;
}
void scrollCallback(GLFWwindow * window, double xOffset, double yOffset){ //for zooming
  FOV+=(float)yOffset;
  if(FOV<30){
    FOV=30;
  }
  if(FOV>90){
    FOV=90;
  }
  projection=glm::perspective(glm::radians(FOV),wWidth/wHeight,
   0.1f,100.0f);
}
void keyCallback(GLFWwindow * window,int key,int scancode,int action,int mod){
/*this function is called once every action which can either be GLW_PRESS
or GLFW_RELEASE. Thus is its called twice when a key is pressed then released.*/
  if(action !=GLFW_PRESS){//consider GLFW_PRESS only
    return;
  }
  pitch = 0;
  firstCursor = true;
  switch (key){
    case GLFW_KEY_ESCAPE:
      //enable cursor
      glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_NORMAL);
      //glfwSetWindowShouldClose(window, true);
      break;
    case (GLFW_KEY_KP_1):
      viewIndex=0;
      selectedView=1;
      viewPosition[9][0]=viewPosition[0][0];
      viewPosition[9][1]=viewPosition[0][1];
      viewPosition[9][2]=viewPosition[0][2];
      yaw=0.75*pi;
      break;
    case (GLFW_KEY_KP_2):
      viewIndex=1;
      selectedView=2;
      viewPosition[9][0]=viewPosition[1][0];
      viewPosition[9][1]=viewPosition[1][1];
      viewPosition[9][2]=viewPosition[1][2];
      yaw=0.5*pi;
      break;
    case (GLFW_KEY_KP_3):
      viewIndex=2;
      selectedView=3;
      viewPosition[9][0]=viewPosition[2][0];
      viewPosition[9][1]=viewPosition[2][1];
      viewPosition[9][2]=viewPosition[2][2];
      yaw=0.25*pi;
      break;
    case (GLFW_KEY_KP_4):
      viewIndex=3;
      selectedView=4;
      viewPosition[9][0]=viewPosition[3][0];
      viewPosition[9][1]=viewPosition[3][1];
      viewPosition[9][2]=viewPosition[3][2];
      yaw=pi;
      break;
    case (GLFW_KEY_KP_5):
      viewIndex=4;
      selectedView=5;
      viewPosition[9][0]=viewPosition[4][0];
      viewPosition[9][1]=viewPosition[4][1];
      viewPosition[9][2]=viewPosition[4][2];
      yaw=0.5*pi;
      pitch=89*pi/180;
      break;
    case (GLFW_KEY_KP_6):
      viewIndex=5;
      selectedView=6;
      viewPosition[9][0]=viewPosition[5][0];
      viewPosition[9][1]=viewPosition[5][1];
      viewPosition[9][2]=viewPosition[5][2];
      yaw=0;
      break;
    case (GLFW_KEY_KP_7):
      viewIndex=6;
      selectedView=7;
      viewPosition[9][0]=viewPosition[6][0];
      viewPosition[9][1]=viewPosition[6][1];
      viewPosition[9][2]=viewPosition[6][2];
      yaw=-0.75*pi;
      break;
    case (GLFW_KEY_KP_8):
      viewIndex=7;
      selectedView=8;
      viewPosition[9][0]=viewPosition[7][0];
      viewPosition[9][1]=viewPosition[7][1];
      viewPosition[9][2]=viewPosition[7][2];
      yaw=-0.5*pi;
      break;
    case (GLFW_KEY_KP_9):
      viewIndex=8;
      selectedView=9;
      viewPosition[9][0]=viewPosition[8][0];
      viewPosition[9][1]=viewPosition[8][1];
      viewPosition[9][2]=viewPosition[8][2];
      yaw=-0.25*pi;
      break;
    case (GLFW_KEY_C):
      glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);
      break;
    default:
     return;
  }
}
void processHold(GLFWwindow * window){
  if(glfwGetKey(window,GLFW_KEY_KP_SUBTRACT)==GLFW_PRESS){
    if(cameraRadius>0.125){
      cameraRadius-=0.125;
    }
  }
  if(glfwGetKey(window,GLFW_KEY_KP_ADD)==GLFW_PRESS){
    cameraRadius+=0.125;
  }
}
void init_rectangle(){
  float vertices[]={	//in main memory
     //vertices		//texture co-ords 
    -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,//bottom left
     0.5f, -0.5f, -0.5f, 1.0f,  0.0f,//bottom right
     0.5f,  0.5f, -0.5f, 1.0f,  1.0f,//top right
     0.5f,  0.5f, -0.5f, 1.0f,  1.0f,//top right
    -0.5f,  0.5f, -0.5f, 0.0f,  1.0f, //top left
    -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,//bottom left
    /*face 2*/
    -0.5f, -0.5f, 0.5f, 0.0f,  0.0f,//bottom left
     0.5f, -0.5f, 0.5f, 1.0f,  0.0f,//bottom right
     0.5f,  0.5f, 0.5f, 1.0f,  1.0f,//top right
     0.5f,  0.5f, 0.5f, 1.0f,  1.0f,//top right
    -0.5f,  0.5f, 0.5f, 0.0f,  1.0f,//top left
    -0.5f, -0.5f, 0.5f, 0.0f,  0.0f,//bottom left
    /*face 3*/
    -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,//bottom left
    -0.5f, -0.5f,  0.5f, 1.0f,  0.0f,//bottom right
    -0.5f,  0.5f,  0.5f, 1.0f,  1.0f,//top right
    -0.5f,  0.5f,  0.5f, 1.0f,  1.0f,//top right
    -0.5f,  0.5f, -0.5f, 0.0f,  1.0f,//top left
    -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,//bottom left
    /*face 4*/
    0.5f, -0.5f, -0.5f, 0.0f,  0.0f,//bottom left
    0.5f, -0.5f,  0.5f, 1.0f,  0.0f,//bottom right
    0.5f,  0.5f,  0.5f, 1.0f,  1.0f,//top right
    0.5f,  0.5f,  0.5f, 1.0f,  1.0f,//top right
    0.5f,  0.5f, -0.5f, 0.0f,  1.0f,//top left
    0.5f, -0.5f, -0.5f, 0.0f,  0.0f,//bottom left
    /*face 5*/
    -0.5f, -0.5f,  0.5f, 0.0f,  0.0f,//bottom left
     0.5f, -0.5f,  0.5f, 1.0f,  0.0f,//bottom right
     0.5f, -0.5f, -0.5f, 1.0f,  1.0f,//top right
     0.5f, -0.5f, -0.5f, 1.0f,  1.0f,//top right
    -0.5f, -0.5f, -0.5f, 0.0f,  1.0f,//top left
    -0.5f, -0.5f,  0.5f, 0.0f,  0.0f,//bottom left
    /*face 6*/
    -0.5f,  0.5f,  0.5f, 0.0f,  0.0f,//bottom left
     0.5f,  0.5f,  0.5f, 1.0f,  0.0f,//bottom right
     0.5f,  0.5f, -0.5f, 1.0f,  1.0f,//top right
     0.5f,  0.5f, -0.5f, 1.0f,  1.0f,//top right
    -0.5f,  0.5f, -0.5f, 0.0f,  1.0f,//top left
    -0.5f,  0.5f,  0.5f, 0.0f,  0.0f,//bottom left
  };
  //generate VAO and return ID reference to VBO
  glGenVertexArrays(1,&VAO);
  //generate buffer object and return ID reference to VBO
  glGenBuffers(1,&VBO);
  /*bind to VAO first such that binding VBO and EBO automatically stores
  VBO attribute pointers and EBO to the bound VAO.
  Note that binding to the VAO will automaticaly bind the associated VBO and EBO*/
  glBindVertexArray(VAO);  
  //Bind BO to GL_ARRAY_BUFFER target*/
  glBindBuffer(GL_ARRAY_BUFFER,VBO);
  //copy vertex data from main memoty into buffer memory in GPU
  glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);
  /*Bind EBO to GL_ELEMENT_ARRAY_BUFFER target.
  instruct OpenGL how to connect vertex data in GL_ARRAY_BUFFER
  to vertex attributes in veterxShader;which part of input vertex data
  goes to what part of vertex attribute in vertexShader*/
  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)0);
  //enable vertex attribute by passing vertex attribute location as variable*/
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,5*sizeof(float),
   (void*)(3*sizeof(float)));
  //enable vertex attribute by passing vertex attribute location as variable*/
  glEnableVertexAttribArray(1);
}
void createTexture(string imageFile, int texPostfix){
  fileName=imageFile.c_str();
  //generate 1 texture and assign its ID to texture*/
  glGenTextures(1,&texture[texPostfix]);
  //activate texture unit. There are 16 texture units from GL_TEXTURE0 to 15
  glActiveTexture(GL_TEXTURE0 + texPostfix);
  //bind the generated texture to GL_TEXTURE_2D object and active texture unit
  glBindTexture(GL_TEXTURE_2D,texture[texPostfix]);
  //set texture wrapping method
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
  //float borderColor[]={0.5f,0.7f,0.2f,1.0f};
  //glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,borderColor);
  //set texture filtering method 
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  //load texture image
  try{
    /*OpenGLexpects 0.0 co-ordinate on the y-axis at the bottom,
    but images have 0.0 co-ordinate on y-axis at the top.
    Therefore set flip vertically to true just before loading the image*/
    stbi_set_flip_vertically_on_load(true);
    data=stbi_load(fileName,&texWidth,&texHeight,&texChannels,0);
    if(data){
      /*for image.png use GL_RGBA because it has an alpha(transparency) channel.
      Otherwise use GL_RGB such as with image.jpeg*/
      if(imageFile.find(".png")!=string::npos){
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,texWidth,texHeight,0,
         GL_RGBA,GL_UNSIGNED_BYTE,data);
      }else{
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,texWidth,texHeight,0,
         GL_RGB,GL_UNSIGNED_BYTE,data);
      }
      glGenerateMipmap(GL_TEXTURE_2D);
    }else{
      throw -1;
    }
  }catch(...){
    cout<<"Failed to load "<<fileName<<endl;
  }
  //free up memory
  stbi_image_free(data);
}
void shaders(){
  //vertex_shader source in GLSL
  const char * vertexShaderSource="#version 330 core\n"
    "layout (location =0 ) in vec3 aPos;\n"
    "layout (location =1 ) in vec2 aTexCoord;\n"
    "out vec2 texCoord;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main(){\n"
    "gl_Position=projection*view*model*vec4(aPos.x,aPos.y,aPos.z,1.0);\n"
    "texCoord=aTexCoord;\n"
    "}\0";
  const char * fragmentShaderSource="#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec2 texCoord;\n"
    "uniform vec4 someColor;\n"
    "uniform sampler2D texture0;\n"
    "uniform sampler2D texture1;\n"
    "uniform float mixRatio;\n"
    "void main(){\n"
    "FragColor=mix(texture(texture0,texCoord),texture(texture1,texCoord),\n"
    "mixRatio)*someColor;\n"
    "}\0";
  //create shader of type GL_VERTEX_SHADER,return ID reference to vertexShader
  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  //attach vertexShaderSource to the vertexShader object
  glShaderSource(vertexShader,1,&vertexShaderSource,NULL);
  //compile vertexShaderSource dynamically;at runtime and report compile error
  try{
    int vertexSuccess;
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader,GL_COMPILE_STATUS,&vertexSuccess);
    if(!vertexSuccess){
      throw -1;
    }
  }
  catch(int ex){
    char vertexErrorLog[512];
    glGetShaderInfoLog(vertexShader,512,NULL,vertexErrorLog);
    cout<<"1"<<vertexErrorLog<<endl;
  }
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader,1,&fragmentShaderSource,NULL);
  try{
    int fragmentSuccess;
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader,GL_COMPILE_STATUS,&fragmentSuccess);
    if(!fragmentSuccess){
      throw -1;
    }
  }
  catch(int ex){    
    char fragmentErrorLog[512];
    glGetShaderInfoLog(fragmentShader,512,NULL,fragmentErrorLog);
    cout<<"2"<<fragmentErrorLog<<endl;
  }
  //create shader program and return ID reference to shaderProgram
  shaderProgram=glCreateProgram();
  //Attach shaders to shaderProgram
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram,fragmentShader);
  //link shaders and check for linking error
  try{
    int linkerSuccess;
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram,GL_LINK_STATUS,&linkerSuccess);
    if(!linkerSuccess){
      throw -1;
    }
  }
  catch(int ex){    
    char linkerErrorLog[512];
    glGetProgramInfoLog(shaderProgram,512,NULL,linkerErrorLog);
    cout<<"3"<<linkerErrorLog<<endl;
  }
  //activate shaderProgram for use by subsequent rendering calls
  glUseProgram(shaderProgram);
  //delete shader objects after linking
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
}
void transformations(){
  //rotate 65degrees clockwise about x-axis
  model=glm::rotate(model, glm::radians(-65.0f),glm::vec3(1.0f,0.0f,0.0f));
  //move the camera backwards +z; translate object in the -z direction
  view=glm::translate(view, glm::vec3(0.0f,0.0f,-3.0f));
  /*perspective projection with FOV=62.5degrees, aspect ratio=800/600,
  near plane = 0.1f and far plane=100.0f*/
  projection=glm::perspective(glm::radians(FOV), 600.0f/500.0f,0.1f,100.0f);
  //projection=glm::ortho(0.0f,600.0f,0.0f,500.0f,0.1f,100.0f);
}
void cameraSpace(unsigned char viewIndex){
  /*view = glm::lookAt(glm::vec3(cameraX,0.0f,cameraZ),
   glm::vec3(0.0f,0.0f,0.0f),glm::vec3(0.0,1.0f,0.0f));*/
  cameraPosition=cameraRadius*glm::vec3(viewPosition[viewIndex][0],
   viewPosition[viewIndex][1],viewPosition[viewIndex][2]);
  cameraTarget=glm::vec3(0.0f,0.0f,0.0f);
  cameraDirection=glm::normalize(cameraPosition-cameraTarget);
  cameraRight=glm::normalize(glm::cross(glm::vec3(normalUp[0],
   normalUp[1],normalUp[2]),cameraDirection));
  cameraUp=glm::normalize(glm::cross(cameraDirection,cameraRight));
  glm::mat4 Mt=glm::translate(glm::mat4(1.0f), -cameraPosition);
  glm::mat4 Mr = glm::mat4(1.0f);
  //glm and OpenGL use column-major-order for matrices
  Mr[0]=glm::vec4(cameraRight, 0.0f);	 //column0
  Mr[1]=glm::vec4(cameraUp, 0.0f);	 //column1
  Mr[2]=glm::vec4(cameraDirection, 0.0f);//column2
  Mr[3]=glm::vec4(0.0f,0.0f,0.0f,1.0f);  //column3
  Mr=glm::transpose(Mr);                 //transpose
  view=Mr*Mt;
}
int main( int argc, char *argv[] )
{
  GLint width, height;
  float colorValue;   
  int vertexColorLocation;
  glfwInit();
  //minimum OpenGL version is 3.3
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  //I don't need backwards-compatible features
  glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);//For Mac OS X
  glfwWindowHint(GLFW_DEPTH_BITS, 16);
  //glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
  
  //*create a window object; holds all windowing data
  GLFWwindow * window=glfwCreateWindow(600,500,"Example1",NULL,NULL);
  if(window==NULL){
    cout<<"Error 1: Failed to create GLFW window!"<<endl;
    glfwTerminate();
    return -1;
  }
  //make the context of the created window the main context on current thread
  glfwMakeContextCurrent(window);
  //pass to GLAD the function to load address of OpenGL function pointers
  if(!(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))){	//gladLoadGL()
    cout<<"Error 2: glfwGetProcAddress fail to load address of OpenGL ";
    cout<<"function pointers!"<<endl;
    return -1;
  }
  
  /*set global states; used throught the program execution*/
  glfwGetFramebufferSize(window, &width, &height);  
  framebuffer_size_reshape(window,width,height);
  glClearColor(0.0, 0.0, 0.0, 0.0);
  //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE); 
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
/*enable depth test so that fragments at the fore override those in the rear*/
  glEnable(GL_DEPTH_TEST);
  //glfwSwapInterval(1);
/*Hide cursor and maintain it at centre of the window if the app has focus*/ 
  glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);
  
  /*register callback functions*/
  glfwSetFramebufferSizeCallback(window, framebuffer_size_reshape);
  glfwSetKeyCallback(window, keyCallback);
  glfwSetCursorPosCallback(window,mouseCallback);
  glfwSetScrollCallback(window,scrollCallback);
  
  shaders();
  init_rectangle();
  createTexture("/usr/share/cubes/c++.png",0);
  createTexture("/usr/share/cubes/linux.jpg",1);
  transformations();
  //render loop/frames
  while(!glfwWindowShouldClose(window)){
    /*clear color buffer and depth buffer,fill with color specified by
    glClearColor() in set state*/
    processHold(window);
    cameraSpace(viewIndex);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//avoid flickering
    colorValue=sin(glfwGetTime());//glfwGetTime return runningtime in seconds     
    //get location of uniform attribute in every frame
    vertexColorLocation = glGetUniformLocation(shaderProgram,"someColor");
    //updating value of uniform attribute. Done after glUseProgram(shaderProgram)
    glUniform4f(vertexColorLocation,colorValue*colorValue,colorValue,
     1-colorValue,1.0f);
    glUniform1f(glGetUniformLocation(shaderProgram,"mixRatio"),colorValue);
    glUniform1i(glGetUniformLocation(shaderProgram,"texture0"),0);
    glUniform1i(glGetUniformLocation(shaderProgram,"texture1"),1);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram,"view"),1,
     GL_FALSE,glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram,"projection"),1,
     GL_FALSE,glm::value_ptr(projection));
    for(unsigned char i=0;i<10;i++){
      model=glm::mat4(1.0f);
      model=glm::translate(model,cubePositions[i]);
      model=glm::rotate(model,(float)glfwGetTime()*glm::radians(20.0f*(i+1)),
       glm::vec3(1.0f,0.5f,0.0f));
      glUniformMatrix4fv(glGetUniformLocation(shaderProgram,"model"),1,
       GL_FALSE,glm::value_ptr(model));
      glDrawArrays(GL_TRIANGLES,0,36);//set primitive shader
    }
    glfwSwapBuffers(window);//swap front and back buffer to remove flickering
    glfwPollEvents();
   //keep polling user events,call corresponding callback to update window state
  }
  //clean up all the resources to properly exit app
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteTextures(1, &texture[0]);
  glDeleteTextures(1, &texture[1]);
  glDeleteProgram(shaderProgram);
  glfwTerminate();
  return 0;
}


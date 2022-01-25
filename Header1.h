#pragma once
#include<stdio.h>   //for file io
#include<windows.h>
#include<gl/glew.h>  
#include<gl/GL.h>
#include "vmath.h"

#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"Kernel32.lib")
#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"opengl32.lib")

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

using namespace vmath;

enum
{
	RRH_ATTRIBUTE_POSITION = 0,
	RRH_ATTRIBUTE_COLOR,
	RRH_ATTRIBUTE_NORMAL,
	RRH_ATTRIBUTE_TEXCOORD,
};


//prototype of wndProc() declered Globally
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//global variable declration
FILE* gpfile = NULL;

HWND ghwnd;
HDC ghdc;
HGLRC ghrc;

DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

bool gbActiveWindow = false;
bool gbEscapeKeyIsPressed = false;
bool gbFullscreen = false;

GLuint vertexShaderObject;
GLuint fragmentShaderObject;
GLuint shaderProgramObject;
GLuint vao_cube;
GLuint vbo_cube_position;
GLuint gMVPUniform;  //mvpMatrixUniform
GLuint skyboxSamplerUniform;

mat4 gPerspectiveProjectionMatrix;
GLfloat s_Angle = 0.0f;

unsigned int cubemapTexture;

//Error checking variable Declaration
GLint iInfoLogLength = 0;  
GLint iShaderCompiledStatus = 0;
char* szBuffer = NULL;


// function Declaration
void VertexShaderErrorChecking(void);
void FragmentShaderErrorChecking(void);
void ShaderProgramErrorChecking(void);



void VertexShaderErrorChecking(void)
{
    iInfoLogLength = 0; 
    iShaderCompiledStatus = 0;
    szBuffer = NULL;  


	//error checking
	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);

	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);

		if (iInfoLogLength > 0)
		{
			szBuffer = (char*)malloc(sizeof(char) * iInfoLogLength);
			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(vertexShaderObject, iInfoLogLength, &written, szBuffer);
				fprintf(gpfile, "Vertex Shader compilation log : %s\n ", szBuffer);
				free(szBuffer);
				DestroyWindow(ghwnd);

			}
		}
	}

}


void FragmentShaderErrorChecking(void)
{
	//compilation error checking
	iInfoLogLength = 0;  //infoLogLength
	iShaderCompiledStatus = 0;
	szBuffer = NULL;  //szBuffer


	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{

		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szBuffer = (char*)malloc(iInfoLogLength);
			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(fragmentShaderObject, iInfoLogLength, &written, szBuffer);

				fprintf(gpfile, "Fragment Shader compilation log : %s\n ", szBuffer);
				free(szBuffer);
				DestroyWindow(ghwnd);
			}
		}
	}
}


// shader program Error checking
void ShaderProgramErrorChecking(void)
{
	//program link Errorchecking
	GLint iShaderProgramLinkStatus = 0;
	iInfoLogLength = 0;
	szBuffer = NULL;
	glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &iShaderProgramLinkStatus);
	if (iShaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);

		if (iInfoLogLength > 0)
		{
			szBuffer = (char*)malloc(iInfoLogLength);
			if (szBuffer != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(shaderProgramObject, iInfoLogLength, &written, szBuffer);
				fprintf(gpfile, "shader program Link log: %s\n ", szBuffer);
				free(szBuffer);
				DestroyWindow(ghwnd);
			}
		}
	}

}

















	 



#include "Header1.h" 
#include "resource.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//main()
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpszCmdLine,int iCmdShow)
{
	//function declaration
	void initialize(void);
	void uninitialize(void);
	void display(void);

	//variable Declaraion
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szClassName[] =TEXT("OpenGLPP");
	bool bDone=false;
    //code
	//create log file
	if (fopen_s(&gpfile,"Log.txt", "w")!=0)
	{
		MessageBox(NULL,TEXT("LOG File can not Be Created\n Exitting..."),TEXT("ERROR"),MB_OK|MB_TOPMOST|MB_ICONSTOP);
		exit(0);
	}
	else
	{
		fprintf(gpfile,"Log File  Is Successfully Opened.\n");
	}

	//initializing member in struct WNDCLASSEX
	wndclass.cbSize=sizeof(WNDCLASSEX);
	wndclass.style=CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
	wndclass.cbClsExtra=0;
	wndclass.cbWndExtra=0;
	wndclass.hInstance=hInstance;
	wndclass.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hIcon= LoadIcon(hInstance,MAKEINTRESOURCE(MY_ICON));
    wndclass.hCursor=LoadCursor(NULL,IDC_ARROW);
	wndclass.hIconSm= LoadIcon(hInstance,MAKEINTRESOURCE(MY_ICON));
	wndclass.lpfnWndProc=WndProc;
	wndclass.lpszClassName=szClassName;
	wndclass.lpszMenuName=NULL;

    //Registering class
	RegisterClassEx(&wndclass);

	//create Window
	
	hwnd = CreateWindow(szClassName, 
		   TEXT("Opengl Programable pipeline:Texture"), 
		   WS_OVERLAPPEDWINDOW, 
		   100, 
		   100,
		   WIN_WIDTH, 
		   WIN_HEIGHT, 
		   NULL, 
		   NULL, 
		   hInstance,
		   NULL);
	
	ghwnd=hwnd;

	ShowWindow(hwnd,iCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	initialize();

	while(bDone==false)
	{
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message==WM_QUIT)
                  bDone=true;
			else
			   {
				   TranslateMessage(&msg);
				   DispatchMessage(&msg);
			   }
	    }
		else
		{
			//rendering function
			display();
			if(gbActiveWindow==true)
			{
				if(gbEscapeKeyIsPressed==true)
					bDone=true;
			}
		}
	}
	   uninitialize();

	   return((int)msg.wParam);
}

//WndProc()
LRESULT CALLBACK WndProc(HWND hwnd,UINT iMsg,WPARAM wParam,LPARAM lParam)
{
	//function prototype
	void resize(int,int);
	void ToggleFullscreen(void);
	void uninitialize(void);

	//variable declaraions
	static WORD xMouse=NULL;
	static WORD yMouse=NULL;
   
	//code
	switch(iMsg)
	{
		case WM_ACTIVATE:
			if(HIWORD(wParam)==0)        //if 0 window is active
				gbActiveWindow=true;
			else                         //if non zero ,the window is not active
				gbActiveWindow=false;
			break;
		
		case WM_ERASEBKGND:
			return(0);
			break;
		
		case WM_SIZE:
			resize(LOWORD(lParam),HIWORD(lParam));
			break;
		
		case WM_KEYDOWN:
			switch(wParam)
			{
			  case VK_ESCAPE:
			    if(gbEscapeKeyIsPressed==false)
				{
					gbEscapeKeyIsPressed=true;
				}
				break;

			  case 0X46:
				if(gbFullscreen==false)
				{
					ToggleFullscreen();
					gbFullscreen=false;
				}
				else
				{
					ToggleFullscreen();
					gbFullscreen=false;
				}
				break;
			 default:
				break;
			}
			break;
		case WM_LBUTTONDOWN:
			break;
		case WM_CLOSE:
			uninitialize();
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			break;

   }
	return(DefWindowProc(hwnd,iMsg,wParam,lParam));
}

void ToggleFullscreen(void)
{
    MONITORINFO mi={sizeof(MONITORINFO)};

	//code
	if(gbFullscreen ==false)
	{
		dwStyle=GetWindowLong(ghwnd,GWL_STYLE);
		if(dwStyle & WS_OVERLAPPEDWINDOW)
		{
			
			if(GetWindowPlacement(ghwnd,&wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd,MONITORINFOF_PRIMARY),&mi))
			{
				SetWindowLong(ghwnd,GWL_STYLE,dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd,HWND_TOP,mi.rcMonitor.left,mi.rcMonitor.top,mi.rcMonitor.right-mi.rcMonitor.left,mi.rcMonitor.bottom-mi.rcMonitor.top,SWP_NOZORDER|SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
	}

	else
	{
		//code
		SetWindowLong(ghwnd,GWL_STYLE,dwStyle|WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd,&wpPrev);
		SetWindowPos(ghwnd,HWND_TOP,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOOWNERZORDER|SWP_NOZORDER|SWP_FRAMECHANGED);
		ShowCursor((TRUE));
	}
}


//Function Defination
void initialize(void)
{
	//function  prototypes
	void uninitialize(void);
	void resize(int,int);
	unsigned int loadCubemap(char *[]);

	//variable declration
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelformatindex;

	//code
	ZeroMemory(&pfd,sizeof( PIXELFORMATDESCRIPTOR));
	pfd.nSize=sizeof( PIXELFORMATDESCRIPTOR);
	pfd.nVersion=1;
	pfd.dwFlags=PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER;
	pfd.iPixelType=PFD_TYPE_RGBA;
	pfd.cColorBits=32;
	pfd.cRedBits=8;
	pfd.cGreenBits=8;
    pfd.cBlueBits=8;
	pfd.cAlphaBits=8;
	pfd.cDepthBits=32;
	 
	ghdc=GetDC(ghwnd);

	iPixelformatindex=ChoosePixelFormat(ghdc,&pfd);
	if(iPixelformatindex==0)
	{
		 ReleaseDC(ghwnd,ghdc);
		 ghdc=NULL;
	}
	if(SetPixelFormat(ghdc,iPixelformatindex,&pfd)==false)
	{
		 ReleaseDC(ghwnd,ghdc);
		 ghdc=NULL;
	}
	 
	 ghrc=wglCreateContext(ghdc);
	 
    if(ghrc==NULL)
	{
		 ReleaseDC(ghwnd,ghdc);
		 ghdc=NULL;
	}
	
	if(wglMakeCurrent(ghdc,ghrc)==FALSE)
	{
		wglDeleteContext(ghrc);
		ghrc=NULL;
		ReleaseDC(ghwnd,ghdc);
		ghdc=NULL;
	}

	//GLEW INItiaalization code for GLSL (IMPORTANt : It most Must be Here.After Creating OpenGL Context But Before  using OpennGL function)
	GLenum glew_error=glewInit();
	if(glew_error!=GLEW_OK)
	{
		 wglDeleteContext(ghrc);
          ghrc=NULL;
		  ReleaseDC(ghwnd,ghdc);
		  ghdc=NULL;
	}

	// Extension and version checking

	//*** VERTEX SHADER ***
	vertexShaderObject=glCreateShader(GL_VERTEX_SHADER);
	
	const GLchar *VertexShaderSourceCode="#version 460 core"\
		                                   "\n"\
										   "in vec3 vPosition;"\
										   "out vec3 out_texcoord;"\
										   "uniform mat4 u_mvpMatrix;"\
										   "void main(void)"\
										   "{"\
										   "out_texcoord = vPosition;"\
										   "gl_Position=u_mvpMatrix*vec4(vPosition,1.0);"\
										   "}";
	glShaderSource(vertexShaderObject, 1, (const GLchar**)&VertexShaderSourceCode, NULL);	
	glCompileShader(vertexShaderObject);
	VertexShaderErrorChecking();
     
	 
	// *** FRAGMENT SHADER ***
		 
	fragmentShaderObject=glCreateShader(GL_FRAGMENT_SHADER);

	//provide source code to shader
	const GLchar *fragmentShaderSourceCode= "#version 460 core"\
			                                      "\n"\
												  "in vec3 out_texcoord;"\
										          "out vec4 FragColor;"\
												  "uniform samplerCube u_skybox;"\
			  									  "void main(void)"\
			                                      "{"\
											         "FragColor = texture(u_skybox,out_texcoord);"\
												  "}";
	glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);
	//compile shader
	glCompileShader(fragmentShaderObject);
	FragmentShaderErrorChecking();


	// *** Shaader program ***
	shaderProgramObject=glCreateProgram();                                                                                                         
    glAttachShader(shaderProgramObject,vertexShaderObject);
    glAttachShader(shaderProgramObject,fragmentShaderObject);
    
	glBindAttribLocation(shaderProgramObject,RRH_ATTRIBUTE_POSITION,"vPosition");

	glLinkProgram(shaderProgramObject);
          
	ShaderProgramErrorChecking();

			
	gMVPUniform=glGetUniformLocation(shaderProgramObject,"u_mvpMatrix");
    skyboxSamplerUniform = glGetUniformLocation(shaderProgramObject,"u_skybox");
					 
	const GLfloat cubeVertices[]=  
	                             { 
		-1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
 
		};

			 
			
	glGenVertexArrays(1,&vao_cube);
	glBindVertexArray(vao_cube);
	glGenBuffers(1,&vbo_cube_position);
    glBindBuffer(GL_ARRAY_BUFFER,vbo_cube_position);
	glBufferData(GL_ARRAY_BUFFER,sizeof(cubeVertices),cubeVertices,GL_STATIC_DRAW);
	glVertexAttribPointer(RRH_ATTRIBUTE_POSITION,3,GL_FLOAT,GL_FALSE,0,NULL);
    glEnableVertexAttribArray(RRH_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindVertexArray(0);
	
	glShadeModel(GL_SMOOTH);
   
    //set-up depth buffer
	glClearDepth(1.0f);

	//enable depth testing
	glEnable(GL_DEPTH_TEST);

	//depth test to do
	glDepthFunc(GL_LEQUAL);
             
	//set really nice perspective calculation ?
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);

	//we should always cull back faces for better performance
	glEnable(GL_CULL_FACE);
     
    char *path[] ={"skybox/right.jpg","skybox/left.jpg","skybox/top.jpg","skybox/bottom.jpg","skybox/front.jpg","skybox/back.jpg"};       
    cubemapTexture = loadCubemap(path);  
	
	//set  backgrount color to which it display even if it will empty this line can  be in drawRect()
    glClearColor(0.0f,0.0f,0.0f,0.0f);//blue
   
    //set ortographic matrix to identity matrix
	gPerspectiveProjectionMatrix=mat4::identity();
			 
	//resize
	resize(WIN_WIDTH,WIN_HEIGHT);
}
 
 
unsigned int loadCubemap(char* faces[])
{
    //Variable Declaration
   unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrComponents;
    for (unsigned int i = 0; i < 6; i++)
    {
        unsigned char *data = stbi_load(faces[i], &width, &height, &nrComponents, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
             fprintf(gpfile,"Cubemap texture failed to load at path: %s", faces[i]);
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}	


void resize(int width ,int height)
{
	//code
	if(height==0)
	height=1;
	glViewport(0,0,(GLsizei)width,(GLsizei)height);

	gPerspectiveProjectionMatrix=vmath::perspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);


}


void display(void)
{
	//code
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
          
    glDepthFunc(GL_LEQUAL);
	
	glUseProgram(shaderProgramObject);

			 
	mat4 modelVeiwMatrix=mat4::identity();
	mat4 modelVeiwProjectionMatrix = mat4::identity();
    mat4 translateMatrix;
	mat4 rotationMatrix_x = mat4::identity();
				 
	translateMatrix=vmath::translate(0.0f,0.0f,-1.0f);
	rotationMatrix_x=vmath::rotate(s_Angle,0.0f,1.0f,0.0f);

	modelVeiwMatrix=translateMatrix*rotationMatrix_x;		  
	modelVeiwProjectionMatrix=gPerspectiveProjectionMatrix*modelVeiwMatrix; 		
	glUniformMatrix4fv(gMVPUniform,1,GL_FALSE,modelVeiwProjectionMatrix);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glUniform1i(skyboxSamplerUniform,0);
	glBindVertexArray(vao_cube);
    glDrawArrays(GL_TRIANGLES,0,36);
	glBindVertexArray(0);
    
	glDepthFunc(GL_LESS);
    
	//stop using opengl program object
	glUseProgram(0);
    
	s_Angle = s_Angle+0.1f;
	SwapBuffers(ghdc);
}
		
void uninitialize(void)
{
	
	if(gbFullscreen==true)
	{
	    dwStyle=GetWindowLong(ghwnd,GWL_STYLE);
		SetWindowLong(ghwnd,GWL_STYLE,(dwStyle|WS_OVERLAPPEDWINDOW));
		SetWindowPlacement(ghwnd,&wpPrev);
		SetWindowPos(ghwnd,HWND_TOP,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOOWNERZORDER|SWP_FRAMECHANGED|SWP_NOZORDER);
	    ShowCursor(TRUE);
	}

	//destrov vao
	if(vao_cube)
	{
		glDeleteVertexArrays(1,&vao_cube);
		vao_cube=0;
	}
	
	//destroy vbo
	if(vbo_cube_position)
	{
		glDeleteBuffers(1,&vbo_cube_position);
		vbo_cube_position=0;
	}

	//safe shader cleanup
	if(shaderProgramObject)
	{
		glUseProgram(shaderProgramObject);
		GLsizei shaderCount=0;
		glGetProgramiv(shaderProgramObject,GL_ATTACHED_SHADERS,&shaderCount);
		GLuint *pShader=NULL;
	    pShader=(GLuint*)malloc(sizeof(GLuint)*shaderCount);
		glGetAttachedShaders(shaderProgramObject,shaderCount,&shaderCount,pShader);
		for(GLsizei i=0;i<shaderCount;i++)
		{
			glDetachShader(shaderProgramObject,pShader[i]);
			glDeleteShader(pShader[i]);
			pShader[i]=0;
		}
		free(pShader);
		glDeleteProgram(shaderProgramObject);
		shaderProgramObject=0;
		glUseProgram(0);
	}

	//unlink shader program
	glUseProgram(0);

	//Deselect rendering context

	wglMakeCurrent(NULL,NULL);
			 
	//delete rendering context
	wglDeleteContext(ghrc);
	ghrc=NULL;

	//delete device context
	ReleaseDC(ghwnd,ghdc);
	ghdc=NULL;

	if(gpfile)
	{
		fprintf(gpfile,"log file is close sucessfully. program ended sucessfully");
	    fclose(gpfile);
	    gpfile=NULL;
	
	}

}

		 
		








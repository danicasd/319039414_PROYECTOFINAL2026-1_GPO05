// 319039414

#include <iostream>
#include <cmath>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include "stb_image.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Load Models
#include "SOIL2/SOIL2.h"


// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

// Function prototypes
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement();

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera
Camera  camera(glm::vec3(0.0f, 0.0f, 3.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;
// Light attributes
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
bool active;

// Puerta Entrada
float rotacionPuertaEntrada = 0.0f; // ángulo actual de rotación de la puerta
bool puertaEntradaAbierta = false; // estado actual de la puerta. true: abierta / false: cerrada.
bool animandoPuertaEntrada = false; // estado de transición

// Puerta Cocina
float rotacionPuertaCocina = 0.0f; // ángulo actual de rotación de la puerta
bool puertaCocinaAbierta = false; // estado actual de la puerta. true -> abierta / false ? -> cerrada.
bool animandoPuertaCocina = false; // estado de transición

// Puerta Madera
float rotacionPuertaMadera = 0.0f; // ángulo actual de rotación de la puerta
bool puertaMaderaAbierta = false; // estado actual de la puerta. true -> abierta / false ? -> cerrada.
bool animandoPuertaMadera = false; // estado de transición

// Radio
bool radioEncendida = false; // ON/OFF de la radio
bool togglingRadio = false;  // evita múltiples toggles con la misma tecla
float vibFase = 0.0f;  // fase de la vibración 
float vibInt = 0.0f; // intensidad actual 
float vibIntObjetivo = 0.0f; // intensidad objetivo
// Parámetros de la vibración 
const float VIB_FREC = 2.5f; // oscilaciones por segundo
const float VIB_AMP_POS = 0.025f;  // amplitud de traslación 
const float VIB_AMP_ROT = 2.0f;  // amplitud de rotación 
const float VIB_SLEW = 3.0f; // qué tan rápido sube/baja la intensidad

// Reloj
// Manecilla Hora
bool animarHora = false; // Indica si la animación de la manecilla de hora está activa
bool toggleHora = false; // Evita múltiples toggles con la misma tecla
float anguloHora = 0.0f;  // Ángulo actual de rotación acumulado
// Manecilla Minutos
bool animarMinutos = false; // Indica si la animación de la manecilla de minutos está activa
bool toggleMinutos = false; // Evita múltiples toggles con la misma tecla
float anguloMinutos = 0.0f; // Ángulo actual de rotación acumulado

// Balón
// Posiciones base 
const glm::vec3 BALL_START = glm::vec3(-24.81f, 0.6f, 22.093f); // posición inicial del balón en el piso
const float     FLOOR_Y = 0.6f;                               // altura del piso donde rebota el balón
// Estado de animación
enum BalonFase { FASE_FIN, FASE_BOTES, FASE_TIRO, FASE_CAIDA, FASE_RODAR }; // fases del movimiento del balón
BalonFase faseBalon = FASE_FIN; // inicia sin animación activa
bool animarBalon = false; // controla si la animación está corriendo
bool toggleBalon = false;  // evita múltiples activaciones con una sola tecla
float tBalon = 0.0f;           // tiempo dentro de la fase actual
glm::vec3 posBalon = BALL_START; // posición actual del balón
float rotBalon = 0.0f; // rotación del balón

// Botella y copa
float rotacionBotella = 0.0f; // Ángulo de rotación de la botella
bool botellaAnimando = false; // Controla si la animación está activa
bool botellaCaida = false; // Indica si la botella ya terminó de caer
bool toggleBotella = false; // Evita múltiples toggles con la misma tecla
// Copa
float anguloCopa1 = 0.0f; // Ángulo de vibración durante el temblor
float rotacionCaidaCopa1 = 0.0f; // Ángulo de rotación durante la caída
const float FLOOR_Y_COPAS = 1.0f;  // Altura del piso 
const float MESA_Y = 5.052f; // Altura de la superficie de la mesa
const glm::vec3 COPA1_BASE = glm::vec3(5.999f, MESA_Y, 5.194f); // Posición base inicial de la copa sobre la mesa
float copaY1 = MESA_Y; // Altura actual de la copa (se actualiza con la animación)
glm::vec3 COPA1_PIVOT = glm::vec3(COPA1_BASE.x, MESA_Y, COPA1_BASE.z); // Punto de pivote para rotaciones
// //Fases de la animación
enum FaseBotella { FASE_INICIO, FASE_CAIDA_BOTELLA, FASE_TEMBLAR_COPAS, FASE_DESLIZAR_COPAS, FASE_CAIDA_COPAS, FASE_FINAL };
FaseBotella faseBotella = FASE_INICIO; // Fase actual de la animación
float tAnim = 0.0f; // tiempo acumulado
float copaZ1 = 0.0f; // Desplazamiento de la copa hacia el frente (eje Z)
// Pequeño hundimiento al acostarse sobre la barra (opcional)
const float PENETRACION_SOBRE_BARRA = 0.05f; // Ajuste visual: ligera penetración en la barra para evitar flotación

// Positions of the point lights
glm::vec3 pointLightPositions[] = {
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, 0.0f)
};

float vertices[] = {
	 -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	   -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	   -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	   -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	   -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

	   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	   -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	   -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	   -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	   -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	   -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};



glm::vec3 Light1 = glm::vec3(0);
glm::vec3 Light2 = glm::vec3(0);
glm::vec3 Light3 = glm::vec3(0);
glm::vec3 Light4 = glm::vec3(0);

//coordenadas posicion spot
glm::vec3 posicionSpot = glm::vec3(3.5f, 5.0f, 3.5f);
glm::vec3 direccionSpot = glm::vec3(0.0f, -1.0f, 0.0f);

// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

int main()
{
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);*/

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Proyecto Final", nullptr, nullptr);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);

	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	// Set the required callback functions
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);

	// GLFW Options
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// Define the viewport dimensions
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);



	Shader lightingShader("Shaders/lighting.vs", "Shaders/lighting.frag");
	Shader lampShader("Shaders/lamp.vs", "Shaders/lamp.frag");
	Model Piso((char*)"Models/Muebles/Piso.obj");
	Model Sillon((char*)"Models/Muebles/sillon_rojo.obj");
	Model Mesa((char*)"Models/Muebles/mesa_cafe.obj");
	Model Lampara((char*)"Models/Muebles/lampara.obj");
	Model Sofa((char*)"Models/Muebles/sofa_rojo.obj");
	Model Radio((char*)"Models/Muebles/radio.obj");
	Model Casa((char*)"Models/Muebles/casa.obj");
	Model LamparaTecho((char*)"Models/Muebles/lampara_tcho.obj");
	Model PuertaEntrada((char*)"Models/Muebles/puertaEntrada.obj");
	Model PuertaCocina((char*)"Models/Muebles/puertaCocina.obj");
	Model PuertaMadera((char*)"Models/Muebles/puertaMadera.obj");
	Model Ventana1((char*)"Models/Muebles/ventana1.obj");
	Model Ventana2((char*)"Models/Muebles/ventana2.obj");
	Model Ventana3((char*)"Models/Muebles/ventana3.obj");
	Model Banco((char*)"Models/Muebles/banco.obj");
	Model Reloj((char*)"Models/Muebles/reloj.obj");
	Model ManecillaHora((char*)"Models/Muebles/manecillaHora.obj");
	Model ManecillaMin((char*)"Models/Muebles/manecillaMin.obj");
	Model Ball((char*)"Models/Muebles/ball.obj");
	Model CenManecillas((char*)"Models/Muebles/centroManecillas.obj");
	Model Botella((char*)"Models/Muebles/botella.obj");
	Model Copa1((char*)"Models/Muebles/copa1.obj");


	// First, set the container's VAO (and VBO)
	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Set texture units
	lightingShader.Use();
	glUniform1i(glGetUniformLocation(lightingShader.Program, "material.diffuse"), 0);
	glUniform1i(glGetUniformLocation(lightingShader.Program, "material.specular"), 1);

	glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 100.0f);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{

		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		DoMovement();

		// Clear the colorbuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// OpenGL options
		glEnable(GL_DEPTH_TEST);



		//Load Model


		// Use cooresponding shader when setting uniforms/drawing objects
		lightingShader.Use();

		glUniform1i(glGetUniformLocation(lightingShader.Program, "diffuse"), 0);
		//glUniform1i(glGetUniformLocation(lightingShader.Program, "specular"),1);

		GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
		glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);


		// Directional light
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.5f, 0.5f, 0.5f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.5f, 0.5f, 0.5f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 1.0f, 1.0f, 1.0f);

		// Point light 1
		glm::vec3 lightColor;
		lightColor.x = abs(sin(glfwGetTime() * Light1.x));
		lightColor.y = abs(sin(glfwGetTime() * Light1.y));
		lightColor.z = sin(glfwGetTime() * Light1.z);


		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), lightColor.x, lightColor.y, lightColor.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), 1.0f, 1.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.045f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.075f);



		// Point light 2
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].diffuse"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].specular"), 0.0f, 0.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].linear"), 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].quadratic"), 0.0f);

		// Point light 3
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].ambient"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].diffuse"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].specular"), 0.0f, 0.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].linear"), 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].quadratic"), 0.0f);

		// Point light 4
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].ambient"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].diffuse"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].specular"), 0.0f, 0.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].linear"), 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].quadratic"), 0.0f);


		// SpotLight
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), pointLightPositions[4].x, pointLightPositions[4].y, pointLightPositions[4].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), direccionSpot.x, direccionSpot.y, direccionSpot.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.14f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.07f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(12.5f)));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(15.0f)));


		// Set material properties
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 16.0f);

		// Create camera transformations
		glm::mat4 view;
		view = camera.GetViewMatrix();

		// Get the uniform locations
		GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
		GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");

		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


		glm::mat4 model(1);

		// Orden para acomodar los objetos: opacos, transparentes y traslucidos

		//Carga de modelo 
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0.0f, -2.5f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniform1i(glGetUniformLocation(lightingShader.Program, "activaTrans"), 0);
		Piso.Draw(lightingShader);

		//	Sillón
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(20.0f, 1.6f, 19.0f));
		model = glm::scale(model, glm::vec3(13.0f, 13.0f, 13.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Sillon.Draw(lightingShader);

		// Mesa
		model = glm::translate(model, glm::vec3(0.8f, -0.13f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Mesa.Draw(lightingShader);

		// Lámpara encima de la mesa
		model = glm::translate(model, glm::vec3(-0.01f, 0.169f, 0.0f));
		model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Lampara.Draw(lightingShader);

		// Sofá
		model = glm::translate(model, glm::vec3(2.4f, -0.48f, -2.5f));
		model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.73f, 0.73f, 0.73f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Sofa.Draw(lightingShader);

		// Radio
		/*model = glm::translate(model, glm::vec3(0.75f, 0.0f, -1.5f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Radio.Draw(lightingShader);*/

		model = glm::mat4(1);
		glm::vec3 radioBasePos = glm::vec3(-5.8f, 1.6f, 19.8f); // Posición base de la radio

		// Cálculos de vibración 
		float ox = VIB_AMP_POS * vibInt * sin(vibFase); // Desplazamiento en eje X (vibración)
		float oy = VIB_AMP_POS * 0.5f * vibInt * sin(2.0f * vibFase + 1.2f); // Desplazamiento en eje Y (vibración)
		float oz = VIB_AMP_POS * vibInt * cos(vibFase + 0.6f); // Desplazamiento en eje Z (vibración)
		float rotY = VIB_AMP_ROT * vibInt * sin(vibFase * 1.0f); // Rotación ligera en eje Y
		float rotZ = VIB_AMP_ROT * 0.6f * vibInt * sin(vibFase * 1.7f + 0.8f); // Rotación ligera en eje Z

		// Aplica posición base + vibración local
		model = glm::translate(model, radioBasePos + glm::vec3(ox, oy, oz)); // vibra en su lugar
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotY), glm::vec3(0, 1, 0)); // vibración rotacional en Y
		model = glm::rotate(model, glm::radians(rotZ), glm::vec3(0, 0, 1)); // vibración rotacional en Z
		model = glm::scale(model, glm::vec3(6.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Radio.Draw(lightingShader);

		// Lámpara del techo
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(7.0f, 11.1f, 17.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		LamparaTecho.Draw(lightingShader);

		// Banco 1
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(5.5f, -1.5f, 8.0f));
		model = glm::scale(model, glm::vec3(0.9f, 0.9f, 0.9f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Banco.Draw(lightingShader);

		// Banco 2
		model = glm::translate(model, glm::vec3(4.3f, 0.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(0.9f, 0.9f, 0.9f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Banco.Draw(lightingShader);

		// Casa
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0.0f, -2.6f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Casa.Draw(lightingShader);

		// Reloj
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0.0f, -2.6f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Reloj.Draw(lightingShader);

		// Centro Manecillas
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0.0f, -2.6f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		CenManecillas.Draw(lightingShader);

		// Manecilla Hora

		// Animación 
		if (animarHora) { // Si la animación está activa
			anguloHora += deltaTime * 6.0f; // Incrementa el ángulo (6° por segundo -> 1 vuelta en 60s)
			if (anguloHora >= 360.0f) anguloHora -= 360.0f; // Reinicia el ángulo al completar una vuelta
		}

		glm::vec3 pivoteHora = glm::vec3(23.957f, 11.822f, 17.441f); // centro de rotación de la manecilla
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0.0f, -2.6f, 0.0f));
		model = glm::translate(model, pivoteHora); // Traslada el modelo al pivote
		model = glm::rotate(model, glm::radians(-anguloHora), glm::vec3(-1.0f, 0.0f, 0.0f)); // rota la manecilla sobre el eje X
		model = glm::translate(model, -pivoteHora); // Regresa el modelo a su posición original
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		ManecillaHora.Draw(lightingShader);

		// Manecilla Minutos

		// Animación
		if (animarMinutos) { // Si la animación está activa
			anguloMinutos += deltaTime * 72.0f;  // Incrementa el ángulo (72° por segundo -> 1 vuelta en 5s)
			if (anguloMinutos >= 360.0f) anguloMinutos -= 360.0f; // Reinicia el ángulo al completar una vuelta
		}

		glm::vec3 pivoteMin = glm::vec3(24.014f, 11.822f, 17.441f); // centro de rotación de la manecilla
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0.0f, -2.6f, 0.0f));
		model = glm::translate(model, pivoteMin); // Traslada el modelo al pivote
		model = glm::rotate(model, glm::radians(-anguloMinutos), glm::vec3(-1.0f, 0.0f, 0.0f)); // rota la manecilla sobre el eje X
		model = glm::translate(model, -pivoteMin); // Regresa el modelo a su posición original
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		ManecillaMin.Draw(lightingShader);

		// Balón

		//Animación

		if (animarBalon) {  // Si la animación está activa
			tBalon += deltaTime; // Avanza tiempo de la fase actual

			switch (faseBalon) { // Controla qué parte de la animación se ejecuta

			// 1) BOTES EN EL PISO 
			case FASE_BOTES: {
				// Dos botes con amortiguación, cada uno dura 0.9 s
				const float DUR_TOTAL = 1.8f;      // Duración total de los dos botes (2 * 0.9)
				const float DUR_BOTE = 0.9f; // Duración de un solo bote
				float t = glm::min(tBalon, DUR_TOTAL); // Tiempo limitado al total de rebotes

				int k = int(t / DUR_BOTE);  // Índice del bote actual (0 o 1)
				float u = (t - k * DUR_BOTE) / DUR_BOTE;  // Progreso 0..1 dentro del bote actual

				// Amplitud decreciente por bote
				float amp0 = 1.5f;                  // Altura del primer bote
				float decay = 0.65f;                // Factor de amortiguación
				float amp = amp0 * pow(decay, (float)k); // Altura ajustada por bote

				// Trayectoria del bote: y = FLOOR + amp * sin(pi*u)
				posBalon.x = BALL_START.x; // Mantiene la posición en X
				posBalon.z = BALL_START.z; // Mantiene la posición en Z
				posBalon.y = FLOOR_Y + amp * sin(3.14159265f * glm::clamp(u, 0.0f, 1.0f)); // Rebote vertical

				rotBalon += 220.0f * deltaTime; // Rotación del balón mientras bota

				if (tBalon >= DUR_TOTAL) { // Si terminó los dos botes
					posBalon = glm::vec3(BALL_START.x, FLOOR_Y, BALL_START.z); // Fija posición al piso
					faseBalon = FASE_TIRO; // Pasa al tiro parabólico
					tBalon = 0.0f; // Reinicia tiempo
				}
				break;
			}

			// 2) TIRO PARABÓLICO HACIA EL ARO 
			case FASE_TIRO: {
				const float DUR = 1.8f;   // Duración del tiro
				float u = glm::clamp(tBalon / DUR, 0.0f, 1.0f); // Normaliza tiempo 0..1

				glm::vec3 P0 = glm::vec3(-24.81f, FLOOR_Y, 22.093f); // Punto inicial del tiro
				glm::vec3 P2 = glm::vec3(-14.5f, 13.0f, 22.469f);   // Punto final: centro del aro
				glm::vec3 P1 = glm::vec3(-14.5f, 20.717f, 22.3f);  // Punto de control: define la altura máxima y el arco de la parábola

				posBalon = (1 - u) * (1 - u) * P0 + 2 * (1 - u) * u * P1 + u * u * P2; // Curva Bézier cuadrática que forma la parábola del tiro
				rotBalon += 350.0f * deltaTime; // Rotación del balón (gira mientras viaja)

				if (tBalon >= DUR) { // Si completa la trayectoria
					posBalon = P2; // Llega al aro
					faseBalon = FASE_CAIDA; // Cambia a fase de caída
					tBalon = 0.0f; // Reinicia tiempo
				}
				break;
			}

			// 3) CAE POR EL ARO HASTA EL PISO 
			case FASE_CAIDA: {
				const float DUR = 0.6f; // Duración de la caída
				float u = glm::clamp(tBalon / DUR, 0.0f, 1.0f); // Normaliza progreso 0..1

				// Caída con aceleración 
				posBalon.x = -14.5f;                           // mismo X del aro
				posBalon.z = 22.3f;                            // mismo Z del aro
				posBalon.y = glm::mix(13.0f, FLOOR_Y, u * u);     // Interpola altura con aceleración, desde el aro hasta el suelo

				rotBalon += 280.0f * deltaTime; // Gira durante la caída

				if (tBalon >= DUR) { // Si llegó al piso
					posBalon = glm::vec3(-14.5f, FLOOR_Y, 22.3f); // Asegura posición en el suelo
					faseBalon = FASE_RODAR; // Pasa a la fase de rodado
					tBalon = 0.0f; // Reinicia tiempo
				}
				break;
			}

			// 4) RUEDA DE REGRESO AL PUNTO INICIAL 
			case FASE_RODAR: {
				const float DUR = 2.0f; // Duración del rodado de regreso
				float u = glm::clamp(tBalon / DUR, 0.0f, 1.0f); // Normaliza 0..1

				glm::vec3 P0 = glm::vec3(-14.5f, FLOOR_Y, 22.3f);  // Punto de partida (debajo del aro)
				glm::vec3 P1 = BALL_START;  // Punto final (posición inicial)

				posBalon = glm::mix(P0, P1, u);  // Interpola posición del rodado
				rotBalon += 420.0f * deltaTime;  // Gira mientras rueda

				if (tBalon >= DUR) {  // Si llega al punto inicial
					posBalon = BALL_START; // Fija posición exacta
					faseBalon = FASE_FIN; // Finaliza animación
					animarBalon = false; // Detiene animación
					tBalon = 0.0f;  // Reinicia tiempo
					rotBalon = 0.0f; // Reinicia rotación
				}
				break;
			}

			default: break;  // En caso de fase inválida
			}
		}

		model = glm::mat4(1);
		model = glm::translate(model, posBalon); // Aplica la posición animada del balón
		model = glm::rotate(model, glm::radians(rotBalon), glm::vec3(0, 0, 1)); // Rotación del balón sobre su eje z
		model = glm::scale(model, glm::vec3(1.5f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Ball.Draw(lightingShader);

		// Botella y copa

		if (botellaAnimando) { // Ejecuta animación solo si está activa
			tAnim += deltaTime; // Acumula tiempo transcurrido en la fase actual

			switch (faseBotella) {

			// 1) BOTELLA CAYENDO 
			case FASE_CAIDA_BOTELLA: {
				float vel = 40.0f * deltaTime; // Velocidad de rotación de la botella
				if (rotacionBotella < 90.0f) {
					rotacionBotella += vel; // Gira hacia atrás
				}
				else {
					rotacionBotella = 90.0f; // Limita la rotación máxima
					faseBotella = FASE_TEMBLAR_COPAS; // Pasa a siguiente fase
					tAnim = 0.0f; // Reinicia el temporizador
				}
				break;
			}

			// 2?) COPA TEMBLANDO 
			case FASE_TEMBLAR_COPAS: {
				anguloCopa1 = sin(glfwGetTime() * 10.0f) * 5.0f; // Oscila con seno para simular vibración

				if (tAnim > 1.2f) { // tiemblan durante 1.2 s
					faseBotella = FASE_CAIDA_COPAS; // Pasa a fase de caída
					tAnim = 0.0f; // Reinicia el tiempo
					copaZ1 = 0.0f; // Reinicia desplazamiento frontal
				}
				break;
			}
			
			// 3) COPA: SOBRE LA BARRA – RODAR – CAER 
			case FASE_CAIDA_COPAS: {
				const float T_TOTAL = 2.1f; // Duración total de esta fase
				float u = glm::clamp(tAnim / T_TOTAL, 0.0f, 1.0f); // Progreso normalizado[0, 1]

				if (u < 0.35f) { // Se acuesta sobre la barra
					float f = u / 0.35f; // Progreso local del tramo
					float anguloRad = glm::radians(glm::mix(0.0f, -85.0f, f));
					float compensacion = abs(sin(anguloRad)) * 0.3f; // Ajuste de altura por inclinación
					copaY1 = glm::mix(MESA_Y, MESA_Y - PENETRACION_SOBRE_BARRA, f) + compensacion; // Baja y ajusta altura

					rotacionCaidaCopa1 = glm::mix(0.0f, -85.0f, f); // Rota para acostarse
					copaZ1 =  0.0f;
				}
				else if (u < 0.75f) { // Rueda sobre la barra
					float f = (u - 0.35f) / 0.40f; // Progreso local de rodado
					copaY1 = MESA_Y - PENETRACION_SOBRE_BARRA + 0.25f; // Mantiene altura ligeramente arriba
					float avance = glm::mix(0.0f, 0.55f, glm::smoothstep(f, 1.0f, 0.0f));
					copaZ1 = avance;
					rotacionCaidaCopa1 = -85.0f + sin(f * 3.14159f * 3.0f) * 10.0f; // Oscilación de rodar
				}
				else { // Cae al borde del piso
					float f = (u - 0.75f) / 0.25f; // Progreso local de caída
					float ea = f * f; // Movimiento con aceleración cuadrática
					copaY1 = glm::mix(MESA_Y - PENETRACION_SOBRE_BARRA + 0.25f, FLOOR_Y_COPAS, ea); // Baja hasta el suelo
					float extra = glm::mix(0.55f, 0.95f, f); // Desplazamiento final hacia adelante
					copaZ1 = extra;
					rotacionCaidaCopa1 = glm::mix(-85.0f, -120.0f, f); // Termina acostada
				}

				if (tAnim >= T_TOTAL) { // Fin de la animación
					faseBotella = FASE_FINAL; // Cambia a fase final
					copaY1 = FLOOR_Y_COPAS; // Fija altura al piso
					rotacionCaidaCopa1 = -120.0f; // Fija rotación final
					tAnim = 0.0f;  // Reinicia el tiempo
				}
				break;
			}
			
			}
		}

			// Botella
			glm::vec3 pivoteBotella = glm::vec3(6.294f, 5.05f, 4.107f);  // Punto de rotación de la botella
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(0.0f, -2.6f, 0.3f));
			model = glm::translate(model, pivoteBotella); // Traslada al pivote
			model = glm::rotate(model, glm::radians(rotacionBotella), glm::vec3(-1.0f, 0.0f, 0.0f)); // Gira en eje -X
			model = glm::translate(model, -pivoteBotella); // Regresa al origen
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			Botella.Draw(lightingShader);

			// Copa 
			glm::vec3 pivoteCopa1 = glm::vec3(COPA1_BASE.x, MESA_Y, COPA1_BASE.z + copaZ1); // Pivote temporal según avance Z
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(0.0f, -2.6f, 0.0f));
			model = glm::translate(model, glm::vec3(0.0f, copaY1 - MESA_Y , 0.0f)); // Desplaza según altura actual
			model = glm::translate(model, pivoteCopa1);  // Traslada al pivote actual
			// Fase de temblor
			if (faseBotella == FASE_TEMBLAR_COPAS)
				model = glm::rotate(model, glm::radians(anguloCopa1), glm::vec3(0.0f, 0.0f, 1.0f));
			// Fase de caída
			if (faseBotella == FASE_CAIDA_COPAS || faseBotella == FASE_FINAL)
				model = glm::rotate(model, glm::radians(rotacionCaidaCopa1), glm::vec3(-1.0f, 0.0f, 0.0f));
			model = glm::translate(model, -pivoteCopa1); // Regresa al origen local
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			Copa1.Draw(lightingShader);

			// Puerta madera

			 // Animación
			if (puertaMaderaAbierta && rotacionPuertaMadera > -90.0f) { // Si la puerta debe abrirse y no ha llegado al límite
				rotacionPuertaMadera -= 50.0f * deltaTime; // Gira la puerta en sentido de apertura
				if (rotacionPuertaMadera < -90.0f) // Evita que supere el ángulo máximo
					rotacionPuertaMadera = -90.0f; // Fija el ángulo de apertura máxima
			}
			else if (!puertaMaderaAbierta && rotacionPuertaMadera < 0.0f) { // Si la puerta debe cerrarse y no está completamente cerrada
				rotacionPuertaMadera += 50.0f * deltaTime; // Gira la puerta hacia su posición original
				if (rotacionPuertaMadera > 0.0f) // Evita que sobrepase el ángulo inicial
					rotacionPuertaMadera = 0.0f; // Fija el ángulo de cierre total
			}

			glm::vec3 pivoteMadera = glm::vec3(-8.122f, 7.391f, 2.3f); // Posición del pivote (bisagras de la puerta)
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(0.0f, -2.6f, 0.0f));
			model = glm::translate(model, pivoteMadera); // Traslada el modelo al pivote de rotación
			model = glm::rotate(model, glm::radians(rotacionPuertaMadera), glm::vec3(0.0f, 1.0f, 0.0f)); // Rota la puerta en el eje Y
			model = glm::translate(model, -pivoteMadera); // Regresa el modelo a su posición original tras rotar
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			PuertaMadera.Draw(lightingShader);

			// Puerta entrada

			// Animación
			if (puertaEntradaAbierta && rotacionPuertaEntrada > -100.0f) { // Si la puerta debe abrirse y no ha llegado al límite
				rotacionPuertaEntrada -= 50.0f * deltaTime; // Gira la puerta en sentido de apertura
				if (rotacionPuertaEntrada < -100.0f) // Evita que supere el ángulo máximo
					rotacionPuertaEntrada = -100.0f;  // Fija el ángulo de apertura máxima
			}
			else if (!puertaEntradaAbierta && rotacionPuertaEntrada < 0.0f) { // Si la puerta debe cerrarse y no está completamente cerrada
				rotacionPuertaEntrada += 50.0f * deltaTime; // Gira la puerta hacia su posición original
				if (rotacionPuertaEntrada > 0.0f) // Evita que sobrepase el ángulo inicial
					rotacionPuertaEntrada = 0.0f; // Fija el ángulo de cierre total
			}

			glEnable(GL_BLEND); // Habilita transparencia
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glm::vec3 pivoteEntrada = glm::vec3(9.886f, 7.391f, 32.378f); // Posición del pivote (bisagras de la puerta)
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(0.0f, -2.6f, 0.0f));
			model = glm::translate(model, pivoteEntrada); // Traslada el modelo al pivote de rotación
			model = glm::rotate(model, glm::radians(rotacionPuertaEntrada), glm::vec3(0.0f, 1.0f, 0.0f)); // Rota la puerta en el eje Y
			model = glm::translate(model, -pivoteEntrada); // Regresa el modelo a su posición original tras rotar
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glUniform1i(glGetUniformLocation(lightingShader.Program, "activaTrans"), 0);
			glUniform4f(glGetUniformLocation(lightingShader.Program, "tras"), 1.0, 1.0, 1.0, 0.75);
			PuertaEntrada.Draw(lightingShader);
			glDisable(GL_BLEND);
			glUniform4f(glGetUniformLocation(lightingShader.Program, "tras"), 1.0, 1.0, 1.0, 1.0);

			// Puerta cocina

			 // Animación
			if (puertaCocinaAbierta && rotacionPuertaCocina > -90.0f) { // Si la puerta debe abrirse y no ha llegado al límite
				rotacionPuertaCocina -= 50.0f * deltaTime; // Gira la puerta en sentido de apertura
				if (rotacionPuertaCocina < -90.0f) // Evita que supere el ángulo máximo
					rotacionPuertaCocina = -90.0f; // Fija el ángulo de apertura máxima
			}
			else if (!puertaCocinaAbierta && rotacionPuertaCocina < 0.0f) { // Si la puerta debe cerrarse y no está completamente cerrada
				rotacionPuertaCocina += 50.0f * deltaTime; // Gira la puerta hacia su posición original
				if (rotacionPuertaCocina > 0.0f) // Evita que sobrepase el ángulo inicial
					rotacionPuertaCocina = 0.0f; // Fija el ángulo de cierre total
			}

			glEnable(GL_BLEND); // Habilita transparencia
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glm::vec3 pivoteCocina = glm::vec3(-10.103f, 7.391f, -16.656f); // Posición del pivote (bisagras de la puerta)
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(0.0f, -2.6f, 0.0f));
			model = glm::translate(model, pivoteCocina); // Traslada el modelo al pivote de rotación
			model = glm::rotate(model, glm::radians(-rotacionPuertaCocina), glm::vec3(0.0f, 1.0f, 0.0f)); // Rota la puerta en el eje Y
			model = glm::translate(model, -pivoteCocina); // Regresa el modelo a su posición original tras rotar
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glUniform1i(glGetUniformLocation(lightingShader.Program, "activaTrans"), 0);
			glUniform4f(glGetUniformLocation(lightingShader.Program, "tras"), 1.0, 1.0, 1.0, 0.75);
			PuertaCocina.Draw(lightingShader);
			glDisable(GL_BLEND);
			glUniform4f(glGetUniformLocation(lightingShader.Program, "tras"), 1.0, 1.0, 1.0, 1.0);

			// Ventanas
			glEnable(GL_BLEND); // Habilita transparencia
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(0.0f, -2.6f, 0.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glUniform1i(glGetUniformLocation(lightingShader.Program, "activaTrans"), 0);
			glUniform4f(glGetUniformLocation(lightingShader.Program, "tras"), 1.0, 1.0, 1.0, 0.75);
			Ventana1.Draw(lightingShader);
			glDisable(GL_BLEND);
			glUniform4f(glGetUniformLocation(lightingShader.Program, "tras"), 1.0, 1.0, 1.0, 1.0);

			glEnable(GL_BLEND); // Habilita transparencia
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(0.0f, -2.6f, 0.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glUniform1i(glGetUniformLocation(lightingShader.Program, "activaTrans"), 0);
			glUniform4f(glGetUniformLocation(lightingShader.Program, "tras"), 1.0, 1.0, 1.0, 0.75);
			Ventana2.Draw(lightingShader);
			glDisable(GL_BLEND);
			glUniform4f(glGetUniformLocation(lightingShader.Program, "tras"), 1.0, 1.0, 1.0, 1.0);

			glEnable(GL_BLEND); // Habilita transparencia
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			model = glm::mat4(1);
			model = glm::translate(model, glm::vec3(0.0f, -2.6f, 0.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glUniform1i(glGetUniformLocation(lightingShader.Program, "activaTrans"), 0);
			glUniform4f(glGetUniformLocation(lightingShader.Program, "tras"), 1.0, 1.0, 1.0, 0.75);
			Ventana3.Draw(lightingShader);
			glDisable(GL_BLEND);
			glUniform4f(glGetUniformLocation(lightingShader.Program, "tras"), 1.0, 1.0, 1.0, 1.0);

			glBindVertexArray(0);


			// Also draw the lamp object, again binding the appropriate shader
			lampShader.Use();
			// Get location objects for the matrices on the lamp shader (these could be different on a different shader)
			modelLoc = glGetUniformLocation(lampShader.Program, "model");
			viewLoc = glGetUniformLocation(lampShader.Program, "view");
			projLoc = glGetUniformLocation(lampShader.Program, "projection");

			// Set matrices
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
			model = glm::mat4(1);
			model = glm::translate(model, lightPos);
			model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			// Draw the light object (using light's vertex attributes)
			for (GLuint i = 0; i < 5; i++)
			{
				model = glm::mat4(1);
				model = glm::translate(model, pointLightPositions[i]);
				model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
				glBindVertexArray(VAO);
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
			glBindVertexArray(0);

			// Swap the screen buffers
			glfwSwapBuffers(window);
		}

		// Terminate GLFW, clearing any resources allocated by GLFW.
		glfwTerminate();

		return 0;


}


	// Moves/alters the camera positions based on user input
	void DoMovement()
	{

		// Camera controls
		if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
		{
			camera.ProcessKeyboard(FORWARD, deltaTime);

		}

		if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
		{
			camera.ProcessKeyboard(BACKWARD, deltaTime);


		}

		if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
		{
			camera.ProcessKeyboard(LEFT, deltaTime);


		}

		if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
		{
			camera.ProcessKeyboard(RIGHT, deltaTime);


		}

		if (keys[GLFW_KEY_T])
		{
			pointLightPositions[4].x += 0.01f;
		}
		if (keys[GLFW_KEY_G])
		{
			pointLightPositions[4].x -= 0.01f;
		}

		if (keys[GLFW_KEY_Y])
		{
			pointLightPositions[4].y += 0.01f;
		}

		if (keys[GLFW_KEY_H])
		{
			pointLightPositions[4].y -= 0.01f;
		}
		if (keys[GLFW_KEY_U])
		{
			pointLightPositions[4].z -= 0.1f;
		}
		if (keys[GLFW_KEY_J])
		{
			pointLightPositions[4].z += 0.01f;
		}

		// Puerta Entrada
		if (keys[GLFW_KEY_O] && !animandoPuertaEntrada) { // Si se presiona O y no se está animando
			puertaEntradaAbierta = !puertaEntradaAbierta; // Cambia el estado (abrir/cerrar)
			animandoPuertaEntrada = true; // Evita que se repita mientras está presionada
		}
		if (!keys[GLFW_KEY_O]) {  // Si se suelta la tecla O
			animandoPuertaEntrada = false; // Permite volver a activar la animación
		}

		// Puerta Madera
		if (keys[GLFW_KEY_P] && !animandoPuertaMadera) { // Si se presiona P y no se está animando
			puertaMaderaAbierta = !puertaMaderaAbierta; // Cambia el estado (abrir/cerrar)
			animandoPuertaMadera = true; // Evita que se repita mientras está presionada
		}
		if (!keys[GLFW_KEY_P]) { // Si se suelta la tecla P
			animandoPuertaMadera = false; // Permite volver a activar la animación
		}

		// Puerta Cocina
		if (keys[GLFW_KEY_Q] && !animandoPuertaCocina) { // Si se presiona Q y no se está animando
			puertaCocinaAbierta = !puertaCocinaAbierta; // Cambia el estado (abrir/cerrar)
			animandoPuertaCocina = true; // Evita que se repita mientras está presionada
		}
		if (!keys[GLFW_KEY_Q]) { // Si se suelta la tecla Q
			animandoPuertaCocina = false; // Permite volver a activar la animación
		}

		// Radio
		if (keys[GLFW_KEY_R] && !togglingRadio) {  // Si se presiona R y no está en cambio
			radioEncendida = !radioEncendida;  // Alterna el estado de encendido/apagado
			vibIntObjetivo = radioEncendida ? 1.0f : 0.0f; // Define la intensidad objetivo según el estado
			togglingRadio = true; // Evita múltiples activaciones por la misma tecla
		}
		if (!keys[GLFW_KEY_R]) togglingRadio = false;   // Restablece el toggle al soltar la tecla

		vibFase += deltaTime * VIB_FREC * 2.0f * 3.14159265f; // Avanza la fase de la vibración con el tiempo
		if (vibFase > 6.28318531f) vibFase -= 6.28318531f;    // Mantiene la fase dentro del rango 

		float ruido = (sin(vibFase * 0.7f + 1.3f) + sin(vibFase * 1.3f + 0.5f)) * 0.5f; // Pequeño ruido para irregularidad natural 
		float suavizador = (sin(vibFase * 0.25f) + 1.0f) * 0.5f; // Oscilador lento para que la intensidad suba/baje levemente

		// Suavizado de intensidad de encendido/apagado
		float diff = vibIntObjetivo - vibInt; // Diferencia entre intensidad actual y deseada
		float paso = VIB_SLEW * deltaTime; // Paso de ajuste por frame
		if (diff > paso) vibInt += paso; // Incrementa suavemente la intensidad
		else if (diff < -paso) vibInt -= paso;  // Disminuye suavemente la intensidad
		else vibInt = vibIntObjetivo; // Ajusta exacto si está cerca del objetivo

		// Movimiento ligeramente irregular en X, Y, Z
		float ox = VIB_AMP_POS * vibInt * (0.8f + 0.2f * suavizador) * sin(vibFase + ruido * 0.3f); // X
		float oy = VIB_AMP_POS * 0.4f * vibInt * sin(vibFase * 2.0f + 1.1f + ruido * 0.2f); // Y
		float oz = VIB_AMP_POS * vibInt * cos(vibFase + ruido * 0.2f); // Z

		// Rotación con modulación suave
		float rotY = VIB_AMP_ROT * vibInt * (0.8f + 0.3f * suavizador) * sin(vibFase * 1.1f + ruido); // Rotación ligera e irregular sobre eje Y
		float rotZ = VIB_AMP_ROT * 0.6f * vibInt * sin(vibFase * 1.8f + ruido * 0.5f);  // Rotación más sutil sobre eje Z

		// Reloj
		if (keys[GLFW_KEY_I] && !toggleHora) {      // Si se presiona I y no está bloqueado el toggle
			animarHora = !animarHora;               // Cambia el estado de la manecilla de la hora
			animarMinutos = animarHora;             // Sincroniza la manecilla de minutos con la de hora
			toggleHora = true;                      // Activa bloqueo para evitar múltiples toggles
		}
		if (!keys[GLFW_KEY_I]) {                    // Al soltar la tecla
			toggleHora = false;                     // Desbloquea para permitir nuevas pulsaciones
		}

		// Balón
		if (keys[GLFW_KEY_B] && !toggleBalon) {   // Si se presiona B y no está en modo toggle
			animarBalon = true;                    // Activa la animación del balón
			faseBalon = FASE_BOTES;                // Comienza desde la fase de rebotes
			tBalon = 0.0f;                         // Reinicia el tiempo de animación
			posBalon = BALL_START;                 // Coloca el balón en su posición inicial
			rotBalon = 0.0f;                       // Reinicia la rotación
			toggleBalon = true;                    // Marca que la tecla ya fue presionada
		}
		if (!keys[GLFW_KEY_B]) toggleBalon = false; // Permite volver a activar cuando se suelte la tecla

		// Botella y copas

		if (keys[GLFW_KEY_V] && !toggleBotella) // Si se presiona V y no está en modo toggle
		{
			botellaAnimando = true; // Activa la animación
			faseBotella = FASE_CAIDA_BOTELLA; // Comienza en la fase de caída de la botella
			tAnim = 0.0f; // Reinicia el temporizador

			// Reinicia los valores 
			rotacionBotella = 0.0f; // Botella en posición vertical
			anguloCopa1 = 0.0f; // Ángulo de temblor en cero
			rotacionCaidaCopa1 = 0.0f; // Rotación de caída en cero
			copaY1 = MESA_Y;  // Copa colocada sobre la mesa
			toggleBotella = true; // Evita reiniciar la animación mientras se mantiene presionada la tecla
		}

		if (!keys[GLFW_KEY_V]) {
			toggleBotella = false; // Permite volver a activar cuando se suelte la tecla
		}
		
		// Direccion
		if (keys[GLFW_KEY_1])
		{
			direccionSpot.x -= 0.01f;
		}
		if (keys[GLFW_KEY_2])
		{
			direccionSpot.x += 0.01f;
		}
		if (keys[GLFW_KEY_3])
		{
			direccionSpot.y -= 0.01f;
		}
		if (keys[GLFW_KEY_4])
		{
			direccionSpot.y += 0.01f;
		}
		if (keys[GLFW_KEY_5])
		{
			direccionSpot.z -= 0.01f;
		}
		if (keys[GLFW_KEY_6])
		{
			direccionSpot.z += 0.01f;
		}
	}

// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
		}
	}

	//if (keys[GLFW_KEY_SPACE])
	//{
	//	active = !active;
	//	if (active)
	//	{
	//		// Delanteras: verde y azul
	//		Light1 = glm::vec3(0.0f, 1.0f, 0.0f); // PointLight1
	//		Light2 = glm::vec3(0.0f, 0.0f, 1.0f); // PointLight2

	//		// Traseras: rojo y rosa
	//		Light3 = glm::vec3(1.0f, 0.0f, 0.0f); // PointLight3
	//		Light4 = glm::vec3(1.0f, 0.0f, 1.0f); // PointLight4
	//	}
	//	else
	//	{
	//		Light1 = glm::vec3(0);//Cuado es solo un valor en los 3 vectores pueden dejar solo una componente
	//		Light2 = glm::vec3(0);
	//		Light3 = glm::vec3(0);
	//		Light4 = glm::vec3(0);
	//	}
	//}

}

void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}
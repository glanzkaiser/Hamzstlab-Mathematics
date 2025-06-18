
// We define this so that the demo does not accidentally use deprecated API
#ifndef IMPLOT_DISABLE_OBSOLETE_FUNCTIONS
#define IMPLOT_DISABLE_OBSOLETE_FUNCTIONS
#endif

#define IMGUI_DEFINE_MATH_OPERATORS // Access to math operators
#include <math.h>
#include "imgui.h"
#include "imgui_internal.h"
#include <utility>

#include "node_editor.h"
#include <imnodes.h>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>

#include "implot.h"

#define CHECKBOX_FLAG(flags, flag) ImGui::CheckboxFlags(#flag, (unsigned int*)&flags, flag)

#ifndef PI
#define PI 3.14159265358979323846
#endif

#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

// This example can also compile and run with Emscripten! See 'Makefile.emscripten' for details.
#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

// Main code
int main(int, char**)
{
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return 1;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
	// GL ES 2.0 + GLSL 100
	const char* glsl_version = "#version 100";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
	// GL 3.2 + GLSL 330
	const char* glsl_version = "#version 330";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 330";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

	// Create window with graphics context
	GLFWwindow* window = glfwCreateWindow(1920, 1080, "Hamzstlab Mathematics", nullptr, nullptr);
	if (window == nullptr)
		return 1;
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImNodes::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();
	ImNodes::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
	ImGui_ImplGlfw_InstallEmscriptenCanvasResizeCallback("#canvas");
#endif
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Our state
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// Main loop
#ifdef __EMSCRIPTEN__
	io.IniFilename = nullptr;
	EMSCRIPTEN_MAINLOOP_BEGIN
#else
	while (!glfwWindowShouldClose(window))
#endif
	{
	glfwPollEvents();

        // Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
	{
		ImGui::Begin("ImNodes Test", NULL);
		ImVec2 size(980.0f, 600.0f);
		
		ImNodes::BeginNodeEditor();
		ImNodes::BeginNode(1);

		ImNodes::BeginNodeTitleBar();
		ImGui::TextUnformatted("VCC 5 Volt");
		ImNodes::EndNodeTitleBar();

		ImNodes::BeginInputAttribute(11);
		ImGui::Text("input: Adaptor");
		ImNodes::EndInputAttribute();

		ImNodes::BeginOutputAttribute(12);
		ImGui::Indent(40);
		ImGui::Text("output");
		ImNodes::EndOutputAttribute();

		ImNodes::EndNode();

		ImNodes::BeginNode(2);

		ImNodes::BeginNodeTitleBar();
		ImGui::TextUnformatted("MQ-7");
		ImNodes::EndNodeTitleBar();

		ImNodes::BeginInputAttribute(21);
		ImGui::Text("input: 5 Volt");
		ImNodes::EndInputAttribute();

		ImNodes::BeginOutputAttribute(22);
		ImGui::Indent(40);
		ImGui::Text("output: CO2 Measurement");
		ImNodes::EndOutputAttribute();

		ImNodes::EndNode();

		ImNodes::BeginNode(3);

		ImNodes::BeginNodeTitleBar();
		ImGui::TextUnformatted("Resistor 10,000 Ohm");
		ImNodes::EndNodeTitleBar();

		ImNodes::BeginInputAttribute(31);
		ImGui::Text("input: GND");
		ImNodes::EndInputAttribute();

		ImNodes::BeginOutputAttribute(32);
		ImGui::Indent(40);
		ImGui::Text("output");
		ImNodes::EndOutputAttribute();

		ImNodes::EndNode();

		ImNodes::BeginNode(4);

		ImNodes::BeginNodeTitleBar();
		ImGui::TextUnformatted("LCD 20x4 I2C");
		ImNodes::EndNodeTitleBar();

		ImNodes::BeginInputAttribute(41);
		ImGui::Text("input: GND and VCC");
		ImNodes::EndInputAttribute();

		ImNodes::BeginOutputAttribute(42);
		ImGui::Indent(40);
		ImGui::Text("output");
		ImNodes::EndOutputAttribute();

		ImNodes::EndNode();

		ImNodes::BeginNode(5);

		ImNodes::BeginNodeTitleBar();
		ImGui::TextUnformatted("IRLZ34N");
		ImNodes::EndNodeTitleBar();

		ImNodes::BeginInputAttribute(51);
		ImGui::Text("input: GND and Resistor");
		ImNodes::EndInputAttribute();

		ImNodes::BeginOutputAttribute(52);
		ImGui::Indent(40);
		ImGui::Text("output: D13");
		ImNodes::EndOutputAttribute();

		ImNodes::EndNode();

		ImNodes::BeginNode(6);

		ImNodes::BeginNodeTitleBar();
		ImGui::TextUnformatted("D13");
		ImNodes::EndNodeTitleBar();

		ImNodes::BeginInputAttribute(61);
		ImGui::Text("input: IRLZ34N");
		ImNodes::EndInputAttribute();

		ImNodes::BeginOutputAttribute(62);
		ImGui::Indent(40);
		ImGui::Text("output");
		ImNodes::EndOutputAttribute();

		ImNodes::EndNode();

		ImNodes::BeginNode(7);

		ImNodes::BeginNodeTitleBar();
		ImGui::TextUnformatted("A4");
		ImNodes::EndNodeTitleBar();

		ImNodes::BeginInputAttribute(71);
		ImGui::Text("input: LCD 20x4");
		ImNodes::EndInputAttribute();

		ImNodes::BeginOutputAttribute(72);
		ImGui::Indent(40);
		ImGui::Text("output");
		ImNodes::EndOutputAttribute();

		ImNodes::EndNode();

		ImNodes::BeginNode(8);

		ImNodes::BeginNodeTitleBar();
		ImGui::TextUnformatted("A5");
		ImNodes::EndNodeTitleBar();

		ImNodes::BeginInputAttribute(81);
		ImGui::Text("input: LCD 20x4");
		ImNodes::EndInputAttribute();

		ImNodes::BeginOutputAttribute(82);
		ImGui::Indent(40);
		ImGui::Text("output");
		ImNodes::EndOutputAttribute();

		ImNodes::EndNode();
	
		// To link the nodes
		ImNodes::Link(0, 12, 21);
		ImNodes::Link(1, 32, 51);
		ImNodes::Link(2, 52, 61);
		ImNodes::Link(3, 12, 41);
		ImNodes::Link(4, 42, 71);
		ImNodes::Link(5, 42, 81);

		ImNodes::EndNodeEditor();

		ImGui::End();
	}


	// Rendering 
	ImGui::Render();
	int display_w, display_h;
	glfwGetFramebufferSize(window, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(window);
	}
#ifdef __EMSCRIPTEN__
	EMSCRIPTEN_MAINLOOP_END;
#endif

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImNodes::DestroyContext();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
//-----------------------------------------------------------------------------

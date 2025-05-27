
// We define this so that the demo does not accidentally use deprecated API
#ifndef IMPLOT_DISABLE_OBSOLETE_FUNCTIONS
#define IMPLOT_DISABLE_OBSOLETE_FUNCTIONS
#endif

#define IMGUI_DEFINE_MATH_OPERATORS // Access to math operators
#include <math.h>
#include "imgui.h"
#include "imgui_internal.h"

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>

#include "implot.h"
#include "implot3d.h"
#include "implot3d_internal.h"

namespace MyImPlot3D {

// Example for Custom Styles section
void StyleSeaborn();

} // namespace MyImPlot3D

namespace ImPlot3D {}


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
	ImPlot::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

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
	//ImGui::CreateContext();
        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
	{
		ImGui::Begin("3D Line Plot", NULL);
		ImPlot3D::CreateContext();

		static float xs1[1001], ys1[1001], zs1[1001];
		for (int i = 0; i < 1001; i++) 
		{
			xs1[i] = i * 0.001f;
			ys1[i] = 0.5f + 0.5f * cosf(50 * (xs1[i] + (float)ImGui::GetTime() / 10));
			zs1[i] = 0.5f + 0.5f * sinf(50 * (xs1[i] + (float)ImGui::GetTime() / 10));
		}
		static double xs2[20], ys2[20], zs2[20];
		for (int i = 0; i < 20; i++) 
		{
			xs2[i] = i * 1 / 19.0f;
			ys2[i] = xs2[i] * xs2[i];
			zs2[i] = xs2[i] * ys2[i];
		}
		if (ImPlot3D::BeginPlot("Line Plots")) 
		{
			ImPlot3D::SetupAxes("x", "y", "z");
			ImPlot3D::PlotLine("f(x)", xs1, ys1, zs1, 1001);
			ImPlot3D::SetNextMarkerStyle(ImPlot3DMarker_Circle);
			ImPlot3D::PlotLine("g(x)", xs2, ys2, zs2, 20, ImPlot3DLineFlags_Segments);
			ImPlot3D::EndPlot();
		}
		ImPlot3D::DestroyContext();
		ImGui::End();		
	}	

	{
		ImGui::Begin("3D Scatter Plot", NULL);		
		ImPlot3D::CreateContext();

		srand(0);
		static float xs1[100], ys1[100], zs1[100];
		for (int i = 0; i < 100; i++) 
		{
			xs1[i] = i * 0.01f;
			ys1[i] = xs1[i] + 0.1f * ((float)rand() / (float)RAND_MAX);
			zs1[i] = xs1[i] + 0.1f * ((float)rand() / (float)RAND_MAX);
		}
		static float xs2[50], ys2[50], zs2[50];
	    	for (int i = 0; i < 50; i++) 
		{
			xs2[i] = 0.25f + 0.2f * ((float)rand() / (float)RAND_MAX);
			ys2[i] = 0.50f + 0.2f * ((float)rand() / (float)RAND_MAX);
			zs2[i] = 0.75f + 0.2f * ((float)rand() / (float)RAND_MAX);
		}

	    	if (ImPlot3D::BeginPlot("Scatter Plots")) 
		{
			ImPlot3D::PlotScatter("Data 1", xs1, ys1, zs1, 100);
			ImPlot3D::PushStyleVar(ImPlot3DStyleVar_FillAlpha, 0.25f);
			ImPlot3D::SetNextMarkerStyle(ImPlot3DMarker_Square, 6, ImPlot3D::GetColormapColor(1), IMPLOT3D_AUTO, ImPlot3D::GetColormapColor(1));
			ImPlot3D::PlotScatter("Data 2", xs2, ys2, zs2, 50);
			ImPlot3D::PopStyleVar();
			ImPlot3D::EndPlot();
		}
		ImPlot3D::DestroyContext();
		ImGui::End();
	}

	{
		ImGui::Begin("3D Surface Plot", NULL);
		ImPlot3D::CreateContext();

		constexpr int N = 20;
		static float xs[N * N], ys[N * N], zs[N * N];
		static float t = 0.0f;
		t += ImGui::GetIO().DeltaTime;

		// Define the range for X and Y
		constexpr float min_val = -1.0f;
		constexpr float max_val = 1.0f;
		constexpr float step = (max_val - min_val) / (N - 1);

		// Populate the xs, ys, and zs arrays
		for (int i = 0; i < N; i++) 
		{
			for (int j = 0; j < N; j++) 
			{
				int idx = i * N + j;
				xs[idx] = min_val + j * step;                                             // X values are constant along rows
				ys[idx] = min_val + i * step;                                             // Y values are constant along columns
				zs[idx] = ImSin(2 * t + ImSqrt((xs[idx] * xs[idx] + ys[idx] * ys[idx]))); // z = sin(2t + sqrt(x^2 + y^2))
			}
		}

		// Choose fill color
		ImGui::Text("Fill color");
		static int selected_fill = 1; // Colormap by default
		static ImVec4 solid_color = ImVec4(0.8f, 0.8f, 0.2f, 0.6f);
		const char* colormaps[] = {"Viridis", "Plasma", "Hot", "Cool", "Pink", "Jet", "Twilight", "RdBu", "BrBG", "PiYG", "Spectral", "Greys"};
		static int sel_colormap = 5; // Jet by default
		{
			ImGui::Indent();

		// Choose solid color
		ImGui::RadioButton("Solid", &selected_fill, 0);
		if (selected_fill == 0) 
		{
			ImGui::SameLine();
			ImGui::ColorEdit4("##SurfaceSolidColor", (float*)&solid_color);
		}

		// Choose colormap
		ImGui::RadioButton("Colormap", &selected_fill, 1);
		if (selected_fill == 1) 
		{
			ImGui::SameLine();
			ImGui::Combo("##SurfaceColormap", &sel_colormap, colormaps, IM_ARRAYSIZE(colormaps));
			}
			ImGui::Unindent();
		}

		// Choose range
		static bool custom_range = false;
		static float range_min = -1.0f;
		static float range_max = 1.0f;
		ImGui::Checkbox("Custom range", &custom_range);
		{
			ImGui::Indent();

			if (!custom_range)
				{
				ImGui::BeginDisabled();
				ImGui::SliderFloat("Range min", &range_min, -1.0f, range_max - 0.01f);
				ImGui::SliderFloat("Range max", &range_max, range_min + 0.01f, 1.0f);
				}
			if (!custom_range)
				{
				ImGui::EndDisabled();
				}

			ImGui::Unindent();
		}

		// Begin the plot
		if (selected_fill == 1)
			{ImPlot3D::PushColormap(colormaps[sel_colormap]);}
		if (ImPlot3D::BeginPlot("Surface Plots", ImVec2(-1, 400), ImPlot3DFlags_NoClip)) 
		{
		// Set styles
		ImPlot3D::SetupAxesLimits(-1, 1, -1, 1, -1.5, 1.5);
		ImPlot3D::PushStyleVar(ImPlot3DStyleVar_FillAlpha, 0.8f);
		if (selected_fill == 0)
			{
				ImPlot3D::SetNextFillStyle(solid_color);	
				ImPlot3D::SetNextLineStyle(ImPlot3D::GetColormapColor(1));}

			// Plot the surface
			if (custom_range)
				{ImPlot3D::PlotSurface("Wave Surface", xs, ys, zs, N, N, (double)range_min, (double)range_max);}
			else
				{ImPlot3D::PlotSurface("Wave Surface", xs, ys, zs, N, N);}

			// End the plot
			ImPlot3D::PopStyleVar();
			ImPlot3D::EndPlot();
			}
			if (selected_fill == 1)
			{ImPlot3D::PopColormap();}

		ImPlot3D::DestroyContext();
		ImGui::End();		
	}

	//ImGui::DestroyContext();
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
	ImPlot::DestroyContext();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

//-----------------------------------------------------------------------------

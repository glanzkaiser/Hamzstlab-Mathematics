
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

double RandomGauss() {
	static double V1, V2, S;
	static int phase = 0;
	double X;
	if(phase == 0) {
		do {
			double U1 = (double)rand() / RAND_MAX;
			double U2 = (double)rand() / RAND_MAX;
			V1 = 2 * U1 - 1;
			V2 = 2 * U2 - 1;
			S = V1 * V1 + V2 * V2;
			} while(S >= 1 || S == 0);

		X = V1 * sqrt(-2 * log(S) / S);
	} else
		X = V2 * sqrt(-2 * log(S) / S);
	phase = 1 - phase;
	return X;
}

template <int N>
struct NormalDistribution {
    NormalDistribution(double mean, double sd) {
        for (int i = 0; i < N; ++i)
            Data[i] = RandomGauss()*sd + mean;
    }
    double Data[N];
};

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

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
	{
		ImGui::Begin("Stairstep Plot", NULL);
		ImVec2 size(720.0f, 360.0f);
		static float ys1[21], ys2[21];
		for (int i = 0; i < 21; ++i) 
		{
			ys1[i] = 0.75f + 0.2f * sinf(10 * i * 0.05f);
			ys2[i] = 0.25f + 0.2f * sinf(10 * i * 0.05f);
		}
		static ImPlotStairsFlags flags = 0;
		CHECKBOX_FLAG(flags, ImPlotStairsFlags_Shaded);
		if (ImPlot::BeginPlot("Stairstep Plot")) 
		{
	        ImPlot::SetupAxes("x","f(x)");
	        ImPlot::SetupAxesLimits(0,1,0,1);

	        ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(0.5f,0.5f,0.5f,1.0f));
	        ImPlot::PlotLine("##1",ys1,21,0.05f);
	        ImPlot::PlotLine("##2",ys2,21,0.05f);
	        ImPlot::PopStyleColor();

	        ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
	        ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.25f);
	        ImPlot::PlotStairs("Post Step (default)", ys1, 21, 0.05f, 0, flags);
	        ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
		ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL, 0.25f);
		ImPlot::PlotStairs("Pre Step", ys2, 21, 0.05f, 0, flags|ImPlotStairsFlags_PreStep);

		ImPlot::EndPlot();
		}
		ImGui::End();
	}

	{
		ImGui::Begin("Line Plot", NULL);
		ImVec2 size(720.0f, 360.0f);
		static float xs1[1001], ys1[1001];
		for (int i = 0; i < 1001; ++i) 
		{
			xs1[i] = i * 0.001f;
			ys1[i] = 0.5f + 0.5f * sinf(50 * (xs1[i] + (float)ImGui::GetTime() / 10));
		}
		static double xs2[20], ys2[20];
		for (int i = 0; i < 20; ++i) 
		{
			xs2[i] = i * 1/19.0f;
			ys2[i] = xs2[i] * xs2[i];
		}
		if (ImPlot::BeginPlot("Sine Bam Bam Plot")) 
		{
			ImPlot::SetupAxes("x","y");
			ImPlot::PlotLine("f(x)", xs1, ys1, 1001);
			ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
			ImPlot::PlotLine("g(x)", xs2, ys2, 20,ImPlotLineFlags_Segments);
			ImPlot::EndPlot();
		}
		ImGui::End();
	}
	
	{
		ImGui::Begin("Histogram", NULL);
		ImVec2 size(720.0f, 360.0f);
		static ImPlotHistogramFlags hist_flags = ImPlotHistogramFlags_Density;
		static int  bins       = 50;
		static double mu       = 5;
		static double sigma    = 2;
		ImGui::SetNextItemWidth(200);
		if (ImGui::RadioButton("Sqrt",bins==ImPlotBin_Sqrt))       { bins = ImPlotBin_Sqrt;    } ImGui::SameLine();
		if (ImGui::RadioButton("Sturges",bins==ImPlotBin_Sturges)) { bins = ImPlotBin_Sturges; } ImGui::SameLine();
		if (ImGui::RadioButton("Rice",bins==ImPlotBin_Rice))       { bins = ImPlotBin_Rice;    } ImGui::SameLine();
		if (ImGui::RadioButton("Scott",bins==ImPlotBin_Scott))     { bins = ImPlotBin_Scott;   } ImGui::SameLine();
		if (ImGui::RadioButton("N Bins",bins>=0))                  { bins = 50;                }
		if (bins>=0) 
		{
			ImGui::SameLine();
			ImGui::SetNextItemWidth(200);
			ImGui::SliderInt("##Bins", &bins, 1, 100);
		}
		ImGui::CheckboxFlags("Horizontal", (unsigned int*)&hist_flags, ImPlotHistogramFlags_Horizontal);
		ImGui::SameLine();
		ImGui::CheckboxFlags("Density", (unsigned int*)&hist_flags, ImPlotHistogramFlags_Density);
		ImGui::SameLine();
		ImGui::CheckboxFlags("Cumulative", (unsigned int*)&hist_flags, ImPlotHistogramFlags_Cumulative);

		static bool range = false;
		ImGui::Checkbox("Range", &range);
		static float rmin = -3;
		static float rmax = 13;
		if (range) 
		{
			ImGui::SameLine();
			ImGui::SetNextItemWidth(200);
			ImGui::DragFloat2("##Range",&rmin,0.1f,-3,13);
			ImGui::SameLine();
			ImGui::CheckboxFlags("Exclude Outliers", (unsigned int*)&hist_flags, ImPlotHistogramFlags_NoOutliers);
		}
		static NormalDistribution<10000> dist(mu, sigma);
		static double x[100];
		static double y[100];
		if (hist_flags & ImPlotHistogramFlags_Density) 
		{
			for (int i = 0; i < 100; ++i) 
			{
				x[i] = -3 + 16 * (double)i/99.0;
				y[i] = exp( - (x[i]-mu)*(x[i]-mu) / (2*sigma*sigma)) / (sigma * sqrt(2*3.141592653589793238));
			}
			if (hist_flags & ImPlotHistogramFlags_Cumulative) 
			{
				for (int i = 1; i < 100; ++i)
				{
					y[i] += y[i-1];
				}
				for (int i = 0; i < 100; ++i)
				{
					y[i] /= y[99];
				}
			}
    		}

		if (ImPlot::BeginPlot("##Histograms")) 
		{
			ImPlot::SetupAxes(nullptr,nullptr,ImPlotAxisFlags_AutoFit,ImPlotAxisFlags_AutoFit);
			ImPlot::SetNextFillStyle(IMPLOT_AUTO_COL,0.5f);
			ImPlot::PlotHistogram("Empirical", dist.Data, 10000, bins, 1.0, range ? ImPlotRange(rmin,rmax) : ImPlotRange(), hist_flags);
			if ((hist_flags & ImPlotHistogramFlags_Density) && !(hist_flags & ImPlotHistogramFlags_NoOutliers)) 
			{
			if (hist_flags & ImPlotHistogramFlags_Horizontal)
		        	ImPlot::PlotLine("Theoretical",y,x,100);
			else
		        	ImPlot::PlotLine("Theoretical",x,y,100);
			}
			ImPlot::EndPlot();
    		}
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
	ImPlot::DestroyContext();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
//-----------------------------------------------------------------------------

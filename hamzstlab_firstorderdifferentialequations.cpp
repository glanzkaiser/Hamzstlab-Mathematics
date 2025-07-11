// MIT License

// Copyright (c) 2023 Evan Pezent

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// ImPlot v0.17

// We define this so that the demo does not accidentally use deprecated API
#ifndef IMPLOT_DISABLE_OBSOLETE_FUNCTIONS
#define IMPLOT_DISABLE_OBSOLETE_FUNCTIONS
#endif

#include "implot.h"
#ifndef IMGUI_DISABLE
#include <math.h>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef _MSC_VER
#define sprintf sprintf_s
#endif

#ifndef PI
#define PI 3.14159265358979323846
#endif

#include <iostream>
#define CHECKBOX_FLAG(flags, flag) ImGui::CheckboxFlags(#flag, (unsigned int*)&flags, flag)

#if !defined(IMGUI_DISABLE_DEMO_WINDOWS)

// Encapsulates examples for customizing ImPlot.
namespace MyImPlot {

// Example for Custom Data and Getters section.
struct Vector2f {
    Vector2f(float _x, float _y) { x = _x; y = _y; }
    float x, y;
};

// Example for Custom Data and Getters section.
struct WaveData {
    double X, Amp, Freq, Offset;
    WaveData(double x, double amp, double freq, double offset) { X = x; Amp = amp; Freq = freq; Offset = offset; }
};
ImPlotPoint SineWave(int idx, void* wave_data);
ImPlotPoint SawWave(int idx, void* wave_data);
ImPlotPoint Spiral(int idx, void* wave_data);

// Example for Tables section.
void Sparkline(const char* id, const float* values, int count, float min_v, float max_v, int offset, const ImVec4& col, const ImVec2& size);

// Example for Custom Plotters and Tooltips section.
void PlotCandlestick(const char* label_id, const double* xs, const double* opens, const double* closes, const double* lows, const double* highs, int count, bool tooltip = true, float width_percent = 0.25f, ImVec4 bullCol = ImVec4(0,1,0,1), ImVec4 bearCol = ImVec4(1,0,0,1));

// Example for Custom Styles section.
void StyleSeaborn();

} // namespace MyImPlot

namespace ImPlot {

template <typename T>
inline T RandomRange(T min, T max) {
    T scale = rand() / (T) RAND_MAX;
    return min + scale * ( max - min );
}

ImVec4 RandomColor() {
    ImVec4 col;
    col.x = RandomRange(0.0f,1.0f);
    col.y = RandomRange(0.0f,1.0f);
    col.z = RandomRange(0.0f,1.0f);
    col.w = 1.0f;
    return col;
}

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

// utility structure for realtime plot
struct ScrollingBuffer {
    int MaxSize;
    int Offset;
    ImVector<ImVec2> Data;
    ScrollingBuffer(int max_size = 2000) {
        MaxSize = max_size;
        Offset  = 0;
        Data.reserve(MaxSize);
    }
    void AddPoint(float x, float y) {
        if (Data.size() < MaxSize)
            Data.push_back(ImVec2(x,y));
        else {
            Data[Offset] = ImVec2(x,y);
            Offset =  (Offset + 1) % MaxSize;
        }
    }
    void Erase() {
        if (Data.size() > 0) {
            Data.shrink(0);
            Offset  = 0;
        }
    }
};

// utility structure for realtime plot
struct RollingBuffer {
    float Span;
    ImVector<ImVec2> Data;
    RollingBuffer() {
        Span = 10.0f;
        Data.reserve(2000);
    }
    void AddPoint(float x, float y) {
        float xmod = fmodf(x, Span);
        if (!Data.empty() && xmod < Data.back().x)
            Data.shrink(0);
        Data.push_back(ImVec2(xmod, y));
    }
};

// Huge data used by Time Formatting example (~500 MB allocation!)
struct HugeTimeData {
    HugeTimeData(double min) {
        Ts = new double[Size];
        Ys = new double[Size];
        for (int i = 0; i < Size; ++i) {
            Ts[i] = min + i;
            Ys[i] = GetY(Ts[i]);
        }
    }
    ~HugeTimeData() { delete[] Ts;  delete[] Ys; }
    static double GetY(double t) {
        return 0.5 + 0.25 * sin(t/86400/12) +  0.005 * sin(t/3600);
    }
    double* Ts;
    double* Ys;
    static const int Size = 60*60*24*366;
};

//-----------------------------------------------------------------------------
// [SECTION] Demo Functions
//-----------------------------------------------------------------------------

void Demo_Help() {
    ImGui::Text("ABOUT THIS DEMO:");
    ImGui::BulletText("We are demonstrating the solution of first order differential equations");
    ImGui::Separator();
    ImGui::Text("PROGRAMMER GUIDE:");
    ImGui::BulletText("See the ShowFirstOrderDEWindow() code in hamzstlab-firstorderdifferentialequations.cpp. <- you are here!");
    ImGui::BulletText("If you see visual artifacts, do one of the following:");
    ImGui::Indent();
    ImGui::BulletText("Handle ImGuiBackendFlags_RendererHasVtxOffset for 16-bit indices in your backend.");
    ImGui::BulletText("Or, enable 32-bit indices in imconfig.h.");
    ImGui::BulletText("Your current configuration is:");
    ImGui::Indent();
    ImGui::BulletText("ImDrawIdx: %d-bit", (int)(sizeof(ImDrawIdx) * 8));
    ImGui::BulletText("ImGuiBackendFlags_RendererHasVtxOffset: %s", (ImGui::GetIO().BackendFlags & ImGuiBackendFlags_RendererHasVtxOffset) ? "True" : "False");
    ImGui::Unindent();
    ImGui::Unindent();
    ImGui::Separator();
    ImGui::Text("USER GUIDE:");
    ShowUserGuide();
}

//-----------------------------------------------------------------------------

void ButtonSelector(const char* label, ImGuiMouseButton* b) {
    ImGui::PushID(label);
    if (ImGui::RadioButton("LMB",*b == ImGuiMouseButton_Left))
        *b = ImGuiMouseButton_Left;
    ImGui::SameLine();
    if (ImGui::RadioButton("RMB",*b == ImGuiMouseButton_Right))
        *b = ImGuiMouseButton_Right;
    ImGui::SameLine();
    if (ImGui::RadioButton("MMB",*b == ImGuiMouseButton_Middle))
        *b = ImGuiMouseButton_Middle;
    ImGui::PopID();
}

void ModSelector(const char* label, int* k) {
    ImGui::PushID(label);
    ImGui::CheckboxFlags("Ctrl", (unsigned int*)k, ImGuiMod_Ctrl); ImGui::SameLine();
    ImGui::CheckboxFlags("Shift", (unsigned int*)k, ImGuiMod_Shift); ImGui::SameLine();
    ImGui::CheckboxFlags("Alt", (unsigned int*)k, ImGuiMod_Alt); ImGui::SameLine();
    ImGui::CheckboxFlags("Super", (unsigned int*)k, ImGuiMod_Super);
    ImGui::PopID();
}

void InputMapping(const char* label, ImGuiMouseButton* b, int* k) {
    ImGui::LabelText("##","%s",label);
    if (b != nullptr) {
        ImGui::SameLine(100);
        ButtonSelector(label,b);
    }
    if (k != nullptr) {
        ImGui::SameLine(300);
        ModSelector(label,k);
    }
}

void ShowInputMapping() {
    ImPlotInputMap& map = ImPlot::GetInputMap();
    InputMapping("Pan",&map.Pan,&map.PanMod);
    InputMapping("Fit",&map.Fit,nullptr);
    InputMapping("Select",&map.Select,&map.SelectMod);
    InputMapping("SelectHorzMod",nullptr,&map.SelectHorzMod);
    InputMapping("SelectVertMod",nullptr,&map.SelectVertMod);
    InputMapping("SelectCancel",&map.SelectCancel,nullptr);
    InputMapping("Menu",&map.Menu,nullptr);
    InputMapping("OverrideMod",nullptr,&map.OverrideMod);
    InputMapping("ZoomMod",nullptr,&map.ZoomMod);
    ImGui::SliderFloat("ZoomRate",&map.ZoomRate,-1,1);
}

void Demo_Config() {
    ImGui::ShowFontSelector("Font");
    ImGui::ShowStyleSelector("ImGui Style");
    ImPlot::ShowStyleSelector("ImPlot Style");
    ImPlot::ShowColormapSelector("ImPlot Colormap");
    ImPlot::ShowInputMapSelector("Input Map");
    ImGui::Separator();
    ImGui::Checkbox("Use Local Time", &ImPlot::GetStyle().UseLocalTime);
    ImGui::Checkbox("Use ISO 8601", &ImPlot::GetStyle().UseISO8601);
    ImGui::Checkbox("Use 24 Hour Clock", &ImPlot::GetStyle().Use24HourClock);
    ImGui::Separator();
    if (ImPlot::BeginPlot("Preview")) {
        static double now = (double)time(nullptr);
        ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);
        ImPlot::SetupAxisLimits(ImAxis_X1, now, now + 24*3600);
        for (int i = 0; i < 10; ++i) {
            double x[2] = {now, now + 24*3600};
            double y[2] = {0,i/9.0};
            ImGui::PushID(i);
            ImPlot::PlotLine("##Line",x,y,2);
            ImGui::PopID();
        }
        ImPlot::EndPlot();
    }
}

//-----------------------------------------------------------------------------
double df(double x, double y)
{
return 3-2*x-0.5*y;
}

void Demo_LinePlots() {
    static float xs1[1001], ys1[1001];
    for (int i = 0; i < 1001; ++i) {
        xs1[i] = i * 0.001f;
        ys1[i] = 14.0f - (4.0f * (xs1[i]) ) - 13.0f*(exp(-0.5f*xs1[i]));
    }
   float x0 = 0;
   float y0 = 1;
   float h = 0.2;
   float x = 1;
   float c;
   int n = x / 0.2;
   
   static double xs2[6], ys2[6];
   xs2[0] = x0;
   ys2[0] = y0;
   for (int i = 1; i <= n+1; ++i) 
   {    
        c = ys2[i-1];
	xs2[i] = xs2[i-1] + h;
	ys2[i] = c + df(xs2[i-1],ys2[i-1])*h;	
   }
    if (ImPlot::BeginPlot("Initial value Problem with dy/dt = 3 - 2t - 0.5y and y(0)=1")) {
        ImPlot::SetupAxes("t","y");
	ImPlot::SetupLegend(ImPlotLocation_East, ImPlotLegendFlags_Outside);
        
        ImPlot::PlotLine("y(t) = 14 - 4t - 13exp(-t/2)", xs1, ys1, 1001);
        ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
        ImPlot::PlotScatter("Euler's Approximation", xs2, ys2, n+1);
	ImPlot::EndPlot();
    }
}

//-----------------------------------------------------------------------------
void Demo_DirectionFields() {
	// y' = 3 - 2t - 0.5y
	static double xs[50], ys[50], xs2[50], ys2[50], xs3[50], ys3[50], xs4[50], ys4[50], xs5[50], ys5[50], xs6[50], ys6[50];

	xs[0] = 0;
	ys[0] = 0;
	double m = 3 - 2*(xs[0]) - 0.5*(ys[0]);
	for (int i = 1; i < 50; ++i) 
	{
		xs[i] = (i * 1/19.0f) ;
		ys[i] = ys[i-1] + m*(xs[i] - xs[i-1]);
		m = 3 - 2*xs[i] - 0.5*ys[i]; // y' = 3 - 2t - 0.5y
	}
	xs2[0] = 0;
	ys2[0] = 0.82;
	m = 3 - 2*(xs2[0]) - 0.5*(ys2[0]);
	for (int i = 1; i < 50; ++i) 
	{
		xs2[i] = (i * 1/19.0f) ;
		ys2[i] = ys2[i-1] + m*(xs2[i] - xs2[i-1]);
		m = 3 - 2*xs2[i] - 0.5*ys2[i]; // y' = 3 - 2t - 0.5y
	}
	xs3[0] = 0;
	ys3[0] = -0.82;
	m = 3 - 2*(xs3[0]) - 0.5*(ys3[0]);
	for (int i = 1; i < 50; ++i) 
	{
		xs3[i] = (i * 1/19.0f) ;
		ys3[i] = ys3[i-1] + m*(xs3[i] - xs3[i-1]);
		m = 3 - 2*xs3[i] - 0.5*ys3[i]; // y' = 3 - 2t - 0.5y
	}
	xs4[0] = 0;
	ys4[0] = -1.42;
	m = 3 - 2*(xs4[0]) - 0.5*(ys4[0]);
	for (int i = 1; i < 50; ++i) 
	{
		xs4[i] = (i * 1/19.0f) ;
		ys4[i] = ys4[i-1] + m*(xs4[i] - xs4[i-1]);
		m = 3 - 2*xs4[i] - 0.5*ys4[i]; // y' = 3 - 2t - 0.5y
	}
	xs5[0] = 0;
	ys5[0] = -1.8;
	m = 3 - 2*(xs5[0]) - 0.5*(ys5[0]);
	for (int i = 1; i < 50; ++i) 
	{
		xs5[i] = (i * 1/19.0f) ;
		ys5[i] = ys5[i-1] + m*(xs5[i] - xs5[i-1]);
		m = 3 - 2*xs5[i] - 0.5*ys5[i]; // y' = 3 - 2t - 0.5y
	}
	xs6[0] = 0;
	ys6[0] = -2.1;
	m = 3 - 2*(xs6[0]) - 0.5*(ys6[0]);
	for (int i = 1; i < 50; ++i) 
	{
		xs6[i] = (i * 1/19.0f) ;
		ys6[i] = ys6[i-1] + m*(xs6[i] - xs6[i-1]);
		m = 3 - 2*xs6[i] - 0.5*ys6[i]; // y' = 3 - 2t - 0.5y
	}
	if (ImPlot::BeginPlot("Direction Fields")) 
	{
	ImPlot::SetupAxes("x","y");
	ImPlot::SetupAxesLimits(0,2,0,2.3);
	//ImPlot::SetNextMarkerStyle(ImPlotMarker_Right, 4, ImPlot::GetColormapColor(2), IMPLOT_AUTO, ImPlot::GetColormapColor(2));
	ImPlot::PlotLine("", xs, ys, 50,ImPlotLineFlags_Segments);
	//ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(0.6f,0.6f,0.3f,0.8f));
        
	ImPlot::PlotLine("", xs2, ys2, 50,ImPlotLineFlags_Segments);
	ImPlot::PlotLine("", xs3, ys3, 50,ImPlotLineFlags_Segments);
	ImPlot::PlotLine("", xs4, ys4, 50,ImPlotLineFlags_Segments);
	ImPlot::PlotLine("", xs5, ys5, 50,ImPlotLineFlags_Segments);
	ImPlot::PlotLine("", xs6, ys6, 50,ImPlotLineFlags_Segments);
	ImPlot::EndPlot();
	}
}

//-----------------------------------------------------------------------------
double f(double x, double y) {
	return 3 - 2*x - 0.5*y ;// Example : y' = 3 - 2t - 0.5y
	//return -x / y; // Example: dy/dx = -x/y
}
void Demo_DirectionFields2() {
	// y' = 3 - 2t - 0.5y
	// Define your differential equation

	static double xs[500], ys[500];
	int i = 1;
	double x_min = -3.0, x_max = 3.0;
	double y_min = -3.0, y_max = 3.0;
	double step_size = 0.5;
	double dx, dy, magnitude;
	xs[0] = x_min;
	ys[0] = y_min;
	for (double y = y_min; y <= y_max; y += step_size) {
	for (double x = x_min; x <= x_max; x += step_size) {
		if (std::abs(y) > 0.001) { // Avoid division by zero for this example
		double slope = f(x, y);
		// Calculate components of a unit vector in the direction of the slope
		dx = 1.0;
		dy = slope;
		magnitude = sqrt(dx * dx + dy * dy);
		dx = dx / magnitude;
		dy = dy / magnitude;

		}
		xs[i] = x;
		ys[i] = y;
		i = i+1;
		xs[i] = x+dx;
		ys[i] = y+dy;
		i = i+1;
        }
	
	}

	if (ImPlot::BeginPlot("Direction Fields 2")) 
	{
	ImPlot::SetupAxes("x","y");
	ImPlot::SetupAxesLimits(-3,3,-3,3);
	//ImPlot::SetNextMarkerStyle(ImPlotMarker_Right, 4, ImPlot::GetColormapColor(2), IMPLOT_AUTO, ImPlot::GetColormapColor(2));
	ImPlot::PlotLine(" ", xs, ys, 500,ImPlotLineFlags_Segments);
	//ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(0.6f,0.6f,0.3f,0.8f));
	ImPlot::EndPlot();
	}
}


//-----------------------------------------------------------------------------

void Demo_CompoundInterest() {
	static float xs1[1001], ys1[1001];
	static int S0 = 0;
	static float r = 0.08;
	static int k = 2000;
	for (int i = 0; i < 1001; ++i) {
	xs1[i] = i;
	ys1[i] = S0 * exp(r*i) + (k/r)*(exp(r*i) - 1) ;
	}

	static bool range = false;
	ImGui::Checkbox("Change parameters", &range);
    
	if (range) {
	ImGui::SameLine();
	ImGui::SetNextItemWidth(200);
	ImGui::BulletText("Initial Deposit");
	ImGui::SliderInt("##Initial Deposit", &S0, 0, 10000);
	ImGui::SetNextItemWidth(200);
	ImGui::BulletText("Annual Deposit");
	ImGui::SliderInt("##Annual Deposit", &k, 1, 10000);
	ImGui::SetNextItemWidth(200);
	ImGui::BulletText("Return Rate");
	ImGui::DragFloat("##Return rate", &r, 0.01f, 0.2f);
	ImGui::SetNextItemWidth(200);
	}
	if (ImPlot::BeginPlot("How Long to Make USD 1 million with Annual Deposit?")) {
	ImPlot::SetupAxes("t","S(t)");
	ImPlot::SetupAxesLimits(0, 60, 0, 1000000);
	ImPlot::SetupLegend(ImPlotLocation_East, ImPlotLegendFlags_Outside);
        
	ImPlot::PlotLine("S(t) = S_{0} exp(rt) + (k/r) (exp(rt) - 1)", xs1, ys1, 1001);
	ImPlot::EndPlot();
    }
}

//-----------------------------------------------------------------------------

void Demo_LogisticGrowth() {
	static float xs1[1001], ys1[1001],ys2[1001],ys3[1001],ys4[1001],ys5[1001];
	static int y01 = 1, y02 = 2, y03 = 3, y04 = 6, y05 = 7;
	static float r = 0.8;
	static int K = 5;
	for (int i = 0; i < 1001; ++i) {
	xs1[i] = i;
	ys1[i] = (y01*K) / (y01 + (K - y01)*(exp(-r*i)) ) ;
	ys2[i] = (y02*K) / (y02 + (K - y02)*(exp(-r*i)) ) ;
	ys3[i] = (y03*K) / (y03 + (K - y03)*(exp(-r*i)) ) ;
	ys4[i] = (y04*K) / (y04 + (K - y04)*(exp(-r*i)) ) ;
	ys5[i] = (y05*K) / (y05 + (K - y05)*(exp(-r*i)) ) ;
	
	}

	static bool range = false;
	ImGui::Checkbox("Change parameters", &range);
    
	if (range) {
	ImGui::SameLine();
	ImGui::SetNextItemWidth(200);
	ImGui::BulletText("K");
	ImGui::SliderInt("##K", &K, 1, 100);
	ImGui::SetNextItemWidth(200);
	ImGui::BulletText("Rate");
	ImGui::DragFloat("##Rate", &r, 0.01f, 0.99f);
	ImGui::SetNextItemWidth(200);
	}
	if (ImPlot::BeginPlot("Logistic Growth")) {
	ImPlot::SetupAxes("t","y(t)");
	ImPlot::SetupAxesLimits(0, 10, 0, 10);
	ImPlot::SetupLegend(ImPlotLocation_East, ImPlotLegendFlags_Outside);
        
	ImPlot::PlotLine("y(t) = ( y_{0} * K ) / ( y_{0} + (K - y_{0})exp(-rt) )", xs1, ys1, 1001);
	ImPlot::PlotLine("y(t) = ( y_{0} * K ) / ( y_{0} + (K - y_{0})exp(-rt) )", xs1, ys2, 1001);
	ImPlot::PlotLine("y(t) = ( y_{0} * K ) / ( y_{0} + (K - y_{0})exp(-rt) )", xs1, ys3, 1001);
	ImPlot::PlotLine("y(t) = ( y_{0} * K ) / ( y_{0} + (K - y_{0})exp(-rt) )", xs1, ys4, 1001);
	ImPlot::PlotLine("y(t) = ( y_{0} * K ) / ( y_{0} + (K - y_{0})exp(-rt) )", xs1, ys5, 1001);
	ImPlot::EndPlot();
    }
}
//-----------------------------------------------------------------------------


void Demo_ScatterPlots() {
    srand(0);
    static float xs1[100], ys1[100];
    for (int i = 0; i < 100; ++i) {
        xs1[i] = i * 0.01f;
        ys1[i] = xs1[i] + 0.1f * ((float)rand() / (float)RAND_MAX);
    }
    static float xs2[50], ys2[50];
    for (int i = 0; i < 50; i++) {
        xs2[i] = 0.25f + 0.2f * ((float)rand() / (float)RAND_MAX);
        ys2[i] = 0.75f + 0.2f * ((float)rand() / (float)RAND_MAX);
    }

    if (ImPlot::BeginPlot("Scatter Plot")) {
        ImPlot::PlotScatter("Data 1", xs1, ys1, 100);
        ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
        ImPlot::SetNextMarkerStyle(ImPlotMarker_Square, 6, ImPlot::GetColormapColor(1), IMPLOT_AUTO, ImPlot::GetColormapColor(1));
        ImPlot::PlotScatter("Data 2", xs2, ys2, 50);
        ImPlot::PopStyleVar();
        ImPlot::EndPlot();
    }
}

//-----------------------------------------------------------------------------

void Demo_LogScale() {
    static double xs[1001], ys1[1001], ys2[1001], ys3[1001];
    for (int i = 0; i < 1001; ++i) {
        xs[i]  = i*0.1f;
        ys1[i] = sin(xs[i]) + 1;
        ys2[i] = log(xs[i]);
        ys3[i] = pow(10.0, xs[i]);
    }
    if (ImPlot::BeginPlot("Log Plot", ImVec2(-1,0))) {
        ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Log10);
        ImPlot::SetupAxesLimits(0.1, 100, 0, 10);
        ImPlot::PlotLine("f(x) = x",        xs, xs,  1001);
        ImPlot::PlotLine("f(x) = sin(x)+1", xs, ys1, 1001);
        ImPlot::PlotLine("f(x) = log(x)",   xs, ys2, 1001);
        ImPlot::PlotLine("f(x) = 10^x",     xs, ys3, 21);
	ImPlot::EndPlot();
    }
}

//-----------------------------------------------------------------------------

void Demo_CustomDataAndGetters() {
    ImGui::BulletText("You can plot custom structs using the stride feature.");
    ImGui::BulletText("Most plotters can also be passed a function pointer for getting data.");
    ImGui::Indent();
        ImGui::BulletText("You can optionally pass user data to be given to your getter function.");
        ImGui::BulletText("C++ lambdas can be passed as function pointers as well!");
    ImGui::Unindent();

    MyImPlot::Vector2f vec2_data[2] = { MyImPlot::Vector2f(0,0), MyImPlot::Vector2f(1,1) };

    if (ImPlot::BeginPlot("##Custom Data")) {

        // custom structs using stride example:
        ImPlot::PlotLine("Vector2f", &vec2_data[0].x, &vec2_data[0].y, 2, 0, 0, sizeof(MyImPlot::Vector2f) /* or sizeof(float) * 2 */);

        // custom getter example 1:
        ImPlot::PlotLineG("Spiral", MyImPlot::Spiral, nullptr, 1000);

        // custom getter example 2:
        static MyImPlot::WaveData data1(0.001, 0.2, 2, 0.75);
        static MyImPlot::WaveData data2(0.001, 0.2, 4, 0.25);
        ImPlot::PlotLineG("Waves", MyImPlot::SineWave, &data1, 1000);
        ImPlot::PlotLineG("Waves", MyImPlot::SawWave, &data2, 1000);
        ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
        ImPlot::PlotShadedG("Waves", MyImPlot::SineWave, &data1, MyImPlot::SawWave, &data2, 1000);
        ImPlot::PopStyleVar();

        // you can also pass C++ lambdas:
        // auto lamda = [](void* data, int idx) { ... return ImPlotPoint(x,y); };
        // ImPlot::PlotLine("My Lambda", lambda, data, 1000);

        ImPlot::EndPlot();
    }
}

//-----------------------------------------------------------------------------

void Demo_CustomStyles() {
    ImPlot::PushColormap(ImPlotColormap_Deep);
    // normally you wouldn't change the entire style each frame
    ImPlotStyle backup = ImPlot::GetStyle();
    MyImPlot::StyleSeaborn();
    if (ImPlot::BeginPlot("seaborn style")) {
        ImPlot::SetupAxes( "x-axis", "y-axis");
        ImPlot::SetupAxesLimits(-0.5f, 9.5f, 0, 10);
        unsigned int lin[10] = {8,8,9,7,8,8,8,9,7,8};
        unsigned int bar[10] = {1,2,5,3,4,1,2,5,3,4};
        unsigned int dot[10] = {7,6,6,7,8,5,6,5,8,7};
        ImPlot::PlotBars("Bars", bar, 10, 0.5f);
        ImPlot::PlotLine("Line", lin, 10);
        ImPlot::NextColormapColor(); // skip green
        ImPlot::PlotScatter("Scatter", dot, 10);
        ImPlot::EndPlot();
    }
    ImPlot::GetStyle() = backup;
    ImPlot::PopColormap();
}

//-----------------------------------------------------------------------------

void Demo_CustomRendering() {
    if (ImPlot::BeginPlot("##CustomRend")) {
        ImVec2 cntr = ImPlot::PlotToPixels(ImPlotPoint(0.5f,  0.5f));
        ImVec2 rmin = ImPlot::PlotToPixels(ImPlotPoint(0.25f, 0.75f));
        ImVec2 rmax = ImPlot::PlotToPixels(ImPlotPoint(0.75f, 0.25f));
        ImPlot::PushPlotClipRect();
        ImPlot::GetPlotDrawList()->AddCircleFilled(cntr,20,IM_COL32(255,255,0,255),20);
        ImPlot::GetPlotDrawList()->AddRect(rmin, rmax, IM_COL32(128,0,255,255));
        ImPlot::PopPlotClipRect();
        ImPlot::EndPlot();
    }
}

//-----------------------------------------------------------------------------

void Demo_CustomPlottersAndTooltips()  {
    ImGui::BulletText("You can create custom plotters or extend ImPlot using implot_internal.h.");
    double dates[]  = {1546300800,1546387200,1546473600,1546560000,1546819200,1546905600,1546992000,1547078400,1547164800,1547424000,1547510400,1547596800,1547683200,1547769600,1547942400,1548028800,1548115200,1548201600,1548288000,1548374400,1548633600,1548720000,1548806400,1548892800,1548979200,1549238400,1549324800,1549411200,1549497600,1549584000,1549843200,1549929600,1550016000,1550102400,1550188800,1550361600,1550448000,1550534400,1550620800,1550707200,1550793600,1551052800,1551139200,1551225600,1551312000,1551398400,1551657600,1551744000,1551830400,1551916800,1552003200,1552262400,1552348800,1552435200,1552521600,1552608000,1552867200,1552953600,1553040000,1553126400,1553212800,1553472000,1553558400,1553644800,1553731200,1553817600,1554076800,1554163200,1554249600,1554336000,1554422400,1554681600,1554768000,1554854400,1554940800,1555027200,1555286400,1555372800,1555459200,1555545600,1555632000,1555891200,1555977600,1556064000,1556150400,1556236800,1556496000,1556582400,1556668800,1556755200,1556841600,1557100800,1557187200,1557273600,1557360000,1557446400,1557705600,1557792000,1557878400,1557964800,1558051200,1558310400,1558396800,1558483200,1558569600,1558656000,1558828800,1558915200,1559001600,1559088000,1559174400,1559260800,1559520000,1559606400,1559692800,1559779200,1559865600,1560124800,1560211200,1560297600,1560384000,1560470400,1560729600,1560816000,1560902400,1560988800,1561075200,1561334400,1561420800,1561507200,1561593600,1561680000,1561939200,1562025600,1562112000,1562198400,1562284800,1562544000,1562630400,1562716800,1562803200,1562889600,1563148800,1563235200,1563321600,1563408000,1563494400,1563753600,1563840000,1563926400,1564012800,1564099200,1564358400,1564444800,1564531200,1564617600,1564704000,1564963200,1565049600,1565136000,1565222400,1565308800,1565568000,1565654400,1565740800,1565827200,1565913600,1566172800,1566259200,1566345600,1566432000,1566518400,1566777600,1566864000,1566950400,1567036800,1567123200,1567296000,1567382400,1567468800,1567555200,1567641600,1567728000,1567987200,1568073600,1568160000,1568246400,1568332800,1568592000,1568678400,1568764800,1568851200,1568937600,1569196800,1569283200,1569369600,1569456000,1569542400,1569801600,1569888000,1569974400,1570060800,1570147200,1570406400,1570492800,1570579200,1570665600,1570752000,1571011200,1571097600,1571184000,1571270400,1571356800,1571616000,1571702400,1571788800,1571875200,1571961600};
    double opens[]  = {1284.7,1319.9,1318.7,1328,1317.6,1321.6,1314.3,1325,1319.3,1323.1,1324.7,1321.3,1323.5,1322,1281.3,1281.95,1311.1,1315,1314,1313.1,1331.9,1334.2,1341.3,1350.6,1349.8,1346.4,1343.4,1344.9,1335.6,1337.9,1342.5,1337,1338.6,1337,1340.4,1324.65,1324.35,1349.5,1371.3,1367.9,1351.3,1357.8,1356.1,1356,1347.6,1339.1,1320.6,1311.8,1314,1312.4,1312.3,1323.5,1319.1,1327.2,1332.1,1320.3,1323.1,1328,1330.9,1338,1333,1335.3,1345.2,1341.1,1332.5,1314,1314.4,1310.7,1314,1313.1,1315,1313.7,1320,1326.5,1329.2,1314.2,1312.3,1309.5,1297.4,1293.7,1277.9,1295.8,1295.2,1290.3,1294.2,1298,1306.4,1299.8,1302.3,1297,1289.6,1302,1300.7,1303.5,1300.5,1303.2,1306,1318.7,1315,1314.5,1304.1,1294.7,1293.7,1291.2,1290.2,1300.4,1284.2,1284.25,1301.8,1295.9,1296.2,1304.4,1323.1,1340.9,1341,1348,1351.4,1351.4,1343.5,1342.3,1349,1357.6,1357.1,1354.7,1361.4,1375.2,1403.5,1414.7,1433.2,1438,1423.6,1424.4,1418,1399.5,1435.5,1421.25,1434.1,1412.4,1409.8,1412.2,1433.4,1418.4,1429,1428.8,1420.6,1441,1460.4,1441.7,1438.4,1431,1439.3,1427.4,1431.9,1439.5,1443.7,1425.6,1457.5,1451.2,1481.1,1486.7,1512.1,1515.9,1509.2,1522.3,1513,1526.6,1533.9,1523,1506.3,1518.4,1512.4,1508.8,1545.4,1537.3,1551.8,1549.4,1536.9,1535.25,1537.95,1535.2,1556,1561.4,1525.6,1516.4,1507,1493.9,1504.9,1506.5,1513.1,1506.5,1509.7,1502,1506.8,1521.5,1529.8,1539.8,1510.9,1511.8,1501.7,1478,1485.4,1505.6,1511.6,1518.6,1498.7,1510.9,1510.8,1498.3,1492,1497.7,1484.8,1494.2,1495.6,1495.6,1487.5,1491.1,1495.1,1506.4};
    double highs[]  = {1284.75,1320.6,1327,1330.8,1326.8,1321.6,1326,1328,1325.8,1327.1,1326,1326,1323.5,1322.1,1282.7,1282.95,1315.8,1316.3,1314,1333.2,1334.7,1341.7,1353.2,1354.6,1352.2,1346.4,1345.7,1344.9,1340.7,1344.2,1342.7,1342.1,1345.2,1342,1350,1324.95,1330.75,1369.6,1374.3,1368.4,1359.8,1359,1357,1356,1353.4,1340.6,1322.3,1314.1,1316.1,1312.9,1325.7,1323.5,1326.3,1336,1332.1,1330.1,1330.4,1334.7,1341.1,1344.2,1338.8,1348.4,1345.6,1342.8,1334.7,1322.3,1319.3,1314.7,1316.6,1316.4,1315,1325.4,1328.3,1332.2,1329.2,1316.9,1312.3,1309.5,1299.6,1296.9,1277.9,1299.5,1296.2,1298.4,1302.5,1308.7,1306.4,1305.9,1307,1297.2,1301.7,1305,1305.3,1310.2,1307,1308,1319.8,1321.7,1318.7,1316.2,1305.9,1295.8,1293.8,1293.7,1304.2,1302,1285.15,1286.85,1304,1302,1305.2,1323,1344.1,1345.2,1360.1,1355.3,1363.8,1353,1344.7,1353.6,1358,1373.6,1358.2,1369.6,1377.6,1408.9,1425.5,1435.9,1453.7,1438,1426,1439.1,1418,1435,1452.6,1426.65,1437.5,1421.5,1414.1,1433.3,1441.3,1431.4,1433.9,1432.4,1440.8,1462.3,1467,1443.5,1444,1442.9,1447,1437.6,1440.8,1445.7,1447.8,1458.2,1461.9,1481.8,1486.8,1522.7,1521.3,1521.1,1531.5,1546.1,1534.9,1537.7,1538.6,1523.6,1518.8,1518.4,1514.6,1540.3,1565,1554.5,1556.6,1559.8,1541.9,1542.9,1540.05,1558.9,1566.2,1561.9,1536.2,1523.8,1509.1,1506.2,1532.2,1516.6,1519.7,1515,1519.5,1512.1,1524.5,1534.4,1543.3,1543.3,1542.8,1519.5,1507.2,1493.5,1511.4,1525.8,1522.2,1518.8,1515.3,1518,1522.3,1508,1501.5,1503,1495.5,1501.1,1497.9,1498.7,1492.1,1499.4,1506.9,1520.9};
    double lows[]   = {1282.85,1315,1318.7,1309.6,1317.6,1312.9,1312.4,1319.1,1319,1321,1318.1,1321.3,1319.9,1312,1280.5,1276.15,1308,1309.9,1308.5,1312.3,1329.3,1333.1,1340.2,1347,1345.9,1338,1340.8,1335,1332,1337.9,1333,1336.8,1333.2,1329.9,1340.4,1323.85,1324.05,1349,1366.3,1351.2,1349.1,1352.4,1350.7,1344.3,1338.9,1316.3,1308.4,1306.9,1309.6,1306.7,1312.3,1315.4,1319,1327.2,1317.2,1320,1323,1328,1323,1327.8,1331.7,1335.3,1336.6,1331.8,1311.4,1310,1309.5,1308,1310.6,1302.8,1306.6,1313.7,1320,1322.8,1311,1312.1,1303.6,1293.9,1293.5,1291,1277.9,1294.1,1286,1289.1,1293.5,1296.9,1298,1299.6,1292.9,1285.1,1288.5,1296.3,1297.2,1298.4,1298.6,1302,1300.3,1312,1310.8,1301.9,1292,1291.1,1286.3,1289.2,1289.9,1297.4,1283.65,1283.25,1292.9,1295.9,1290.8,1304.2,1322.7,1336.1,1341,1343.5,1345.8,1340.3,1335.1,1341.5,1347.6,1352.8,1348.2,1353.7,1356.5,1373.3,1398,1414.7,1427,1416.4,1412.7,1420.1,1396.4,1398.8,1426.6,1412.85,1400.7,1406,1399.8,1404.4,1415.5,1417.2,1421.9,1415,1413.7,1428.1,1434,1435.7,1427.5,1429.4,1423.9,1425.6,1427.5,1434.8,1422.3,1412.1,1442.5,1448.8,1468.2,1484.3,1501.6,1506.2,1498.6,1488.9,1504.5,1518.3,1513.9,1503.3,1503,1506.5,1502.1,1503,1534.8,1535.3,1541.4,1528.6,1525.6,1535.25,1528.15,1528,1542.6,1514.3,1510.7,1505.5,1492.1,1492.9,1496.8,1493.1,1503.4,1500.9,1490.7,1496.3,1505.3,1505.3,1517.9,1507.4,1507.1,1493.3,1470.5,1465,1480.5,1501.7,1501.4,1493.3,1492.1,1505.1,1495.7,1478,1487.1,1480.8,1480.6,1487,1488.3,1484.8,1484,1490.7,1490.4,1503.1};
    double closes[] = {1283.35,1315.3,1326.1,1317.4,1321.5,1317.4,1323.5,1319.2,1321.3,1323.3,1319.7,1325.1,1323.6,1313.8,1282.05,1279.05,1314.2,1315.2,1310.8,1329.1,1334.5,1340.2,1340.5,1350,1347.1,1344.3,1344.6,1339.7,1339.4,1343.7,1337,1338.9,1340.1,1338.7,1346.8,1324.25,1329.55,1369.6,1372.5,1352.4,1357.6,1354.2,1353.4,1346,1341,1323.8,1311.9,1309.1,1312.2,1310.7,1324.3,1315.7,1322.4,1333.8,1319.4,1327.1,1325.8,1330.9,1325.8,1331.6,1336.5,1346.7,1339.2,1334.7,1313.3,1316.5,1312.4,1313.4,1313.3,1312.2,1313.7,1319.9,1326.3,1331.9,1311.3,1313.4,1309.4,1295.2,1294.7,1294.1,1277.9,1295.8,1291.2,1297.4,1297.7,1306.8,1299.4,1303.6,1302.2,1289.9,1299.2,1301.8,1303.6,1299.5,1303.2,1305.3,1319.5,1313.6,1315.1,1303.5,1293,1294.6,1290.4,1291.4,1302.7,1301,1284.15,1284.95,1294.3,1297.9,1304.1,1322.6,1339.3,1340.1,1344.9,1354,1357.4,1340.7,1342.7,1348.2,1355.1,1355.9,1354.2,1362.1,1360.1,1408.3,1411.2,1429.5,1430.1,1426.8,1423.4,1425.1,1400.8,1419.8,1432.9,1423.55,1412.1,1412.2,1412.8,1424.9,1419.3,1424.8,1426.1,1423.6,1435.9,1440.8,1439.4,1439.7,1434.5,1436.5,1427.5,1432.2,1433.3,1441.8,1437.8,1432.4,1457.5,1476.5,1484.2,1519.6,1509.5,1508.5,1517.2,1514.1,1527.8,1531.2,1523.6,1511.6,1515.7,1515.7,1508.5,1537.6,1537.2,1551.8,1549.1,1536.9,1529.4,1538.05,1535.15,1555.9,1560.4,1525.5,1515.5,1511.1,1499.2,1503.2,1507.4,1499.5,1511.5,1513.4,1515.8,1506.2,1515.1,1531.5,1540.2,1512.3,1515.2,1506.4,1472.9,1489,1507.9,1513.8,1512.9,1504.4,1503.9,1512.8,1500.9,1488.7,1497.6,1483.5,1494,1498.3,1494.1,1488.1,1487.5,1495.7,1504.7,1505.3};
    static bool tooltip = true;
    ImGui::Checkbox("Show Tooltip", &tooltip);
    ImGui::SameLine();
    static ImVec4 bullCol = ImVec4(0.000f, 1.000f, 0.441f, 1.000f);
    static ImVec4 bearCol = ImVec4(0.853f, 0.050f, 0.310f, 1.000f);
    ImGui::SameLine(); ImGui::ColorEdit4("##Bull", &bullCol.x, ImGuiColorEditFlags_NoInputs);
    ImGui::SameLine(); ImGui::ColorEdit4("##Bear", &bearCol.x, ImGuiColorEditFlags_NoInputs);
    ImPlot::GetStyle().UseLocalTime = false;

    if (ImPlot::BeginPlot("Candlestick Chart",ImVec2(-1,0))) {
        ImPlot::SetupAxes(nullptr,nullptr,0,ImPlotAxisFlags_AutoFit|ImPlotAxisFlags_RangeFit);
        ImPlot::SetupAxesLimits(1546300800, 1571961600, 1250, 1600);
        ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);
        ImPlot::SetupAxisLimitsConstraints(ImAxis_X1, 1546300800, 1571961600);
        ImPlot::SetupAxisZoomConstraints(ImAxis_X1, 60*60*24*14, 1571961600-1546300800);
        ImPlot::SetupAxisFormat(ImAxis_Y1, "$%.0f");
        MyImPlot::PlotCandlestick("GOOGL",dates, opens, closes, lows, highs, 218, tooltip, 0.25f, bullCol, bearCol);
        ImPlot::EndPlot();
    }
    }

//-----------------------------------------------------------------------------
// DEMO WINDOW
//-----------------------------------------------------------------------------

void DemoHeader(const char* label, void(*demo)()) {
    if (ImGui::TreeNodeEx(label)) {
        demo();
        ImGui::TreePop();
    }
}

void ShowFirstOrderDEWindow(bool* p_open) {
    static bool show_implot_metrics      = false;
    static bool show_implot_style_editor = false;
    static bool show_imgui_metrics       = false;
    static bool show_imgui_style_editor  = false;
    static bool show_imgui_demo          = false;

    if (show_implot_metrics) {
        ImPlot::ShowMetricsWindow(&show_implot_metrics);
    }
    if (show_implot_style_editor) {
        ImGui::SetNextWindowSize(ImVec2(415,762), ImGuiCond_Appearing);
        ImGui::Begin("Style Editor (ImPlot)", &show_implot_style_editor);
        ImPlot::ShowStyleEditor();
        ImGui::End();
    }
    if (show_imgui_style_editor) {
        ImGui::Begin("Style Editor (ImGui)", &show_imgui_style_editor);
        ImGui::ShowStyleEditor();
        ImGui::End();
    }
    if (show_imgui_metrics) {
        ImGui::ShowMetricsWindow(&show_imgui_metrics);
    }
    if (show_imgui_demo) {
        ImGui::ShowDemoWindow(&show_imgui_demo);
    }
    ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(600, 750), ImGuiCond_FirstUseEver);
    ImGui::Begin("First Order Differential Equations", p_open, ImGuiWindowFlags_MenuBar);
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Tools")) {
            ImGui::MenuItem("Metrics",      nullptr, &show_implot_metrics);
            ImGui::MenuItem("Style Editor", nullptr, &show_implot_style_editor);
            ImGui::Separator();
            ImGui::MenuItem("ImGui Metrics",       nullptr, &show_imgui_metrics);
            ImGui::MenuItem("ImGui Style Editor",  nullptr, &show_imgui_style_editor);
            ImGui::MenuItem("ImGui Demo",          nullptr, &show_imgui_demo);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    //-------------------------------------------------------------------------
    ImGui::Text("with ImPlot (v%s)", IMPLOT_VERSION);
    // display warning about 16-bit indices
    static bool showWarning = sizeof(ImDrawIdx)*8 == 16 && (ImGui::GetIO().BackendFlags & ImGuiBackendFlags_RendererHasVtxOffset) == false;
    if (showWarning) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1,1,0,1));
        ImGui::TextWrapped("WARNING: ImDrawIdx is 16-bit and ImGuiBackendFlags_RendererHasVtxOffset is false. Expect visual glitches and artifacts! See README for more information.");
        ImGui::PopStyleColor();
    }

    ImGui::Spacing();

    if (ImGui::BeginTabBar("ImPlotDemoTabs")) {
        if (ImGui::BeginTabItem("Plots")) {
	    DemoHeader("Compound Interest", Demo_CompoundInterest);
            DemoHeader("Logistic Growth", Demo_LogisticGrowth);
            DemoHeader("Direction Fields", Demo_DirectionFields);
            DemoHeader("Direction Fields 2", Demo_DirectionFields2);
            DemoHeader("Euler Method's", Demo_LinePlots);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Custom")) {
            DemoHeader("Custom Styles", Demo_CustomStyles);
            DemoHeader("Custom Data and Getters", Demo_CustomDataAndGetters);
            DemoHeader("Custom Rendering", Demo_CustomRendering);
            DemoHeader("Custom Plotters and Tooltips", Demo_CustomPlottersAndTooltips);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Config")) {
            Demo_Config();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Help")) {
            Demo_Help();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();
}

} // namespace ImPlot

namespace MyImPlot {

ImPlotPoint SineWave(int idx, void* data) {
    WaveData* wd = (WaveData*)data;
    double x = idx * wd->X;
    return ImPlotPoint(x, wd->Offset + wd->Amp * sin(2 * 3.14 * wd->Freq * x));
}

ImPlotPoint SawWave(int idx, void* data) {
    WaveData* wd = (WaveData*)data;
    double x = idx * wd->X;
    return ImPlotPoint(x, wd->Offset + wd->Amp * (-2 / 3.14 * atan(cos(3.14 * wd->Freq * x) / sin(3.14 * wd->Freq * x))));
}

ImPlotPoint Spiral(int idx, void*) {
    float r = 0.9f;            // outer radius
    float a = 0;               // inner radius
    float b = 0.05f;           // increment per rev
    float n = (r - a) / b;     // number  of revolutions
    double th = 2 * n * 3.14;  // angle
    float Th = float(th * idx / (1000 - 1));
    return ImPlotPoint(0.5f+(a + b*Th / (2.0f * (float) 3.14))*cos(Th),
                       0.5f + (a + b*Th / (2.0f * (float)3.14))*sin(Th));
}

void Sparkline(const char* id, const float* values, int count, float min_v, float max_v, int offset, const ImVec4& col, const ImVec2& size) {
    ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, ImVec2(0,0));
    if (ImPlot::BeginPlot(id,size,ImPlotFlags_CanvasOnly)) {
        ImPlot::SetupAxes(nullptr,nullptr,ImPlotAxisFlags_NoDecorations,ImPlotAxisFlags_NoDecorations);
        ImPlot::SetupAxesLimits(0, count - 1, min_v, max_v, ImGuiCond_Always);
        ImPlot::SetNextLineStyle(col);
        ImPlot::SetNextFillStyle(col, 0.25);
        ImPlot::PlotLine(id, values, count, 1, 0, ImPlotLineFlags_Shaded, offset);
        ImPlot::EndPlot();
    }
    ImPlot::PopStyleVar();
}

void StyleSeaborn() {

    ImPlotStyle& style              = ImPlot::GetStyle();

    ImVec4* colors                  = style.Colors;
    colors[ImPlotCol_Line]          = IMPLOT_AUTO_COL;
    colors[ImPlotCol_Fill]          = IMPLOT_AUTO_COL;
    colors[ImPlotCol_MarkerOutline] = IMPLOT_AUTO_COL;
    colors[ImPlotCol_MarkerFill]    = IMPLOT_AUTO_COL;
    colors[ImPlotCol_ErrorBar]      = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImPlotCol_FrameBg]       = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImPlotCol_PlotBg]        = ImVec4(0.92f, 0.92f, 0.95f, 1.00f);
    colors[ImPlotCol_PlotBorder]    = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImPlotCol_LegendBg]      = ImVec4(0.92f, 0.92f, 0.95f, 1.00f);
    colors[ImPlotCol_LegendBorder]  = ImVec4(0.80f, 0.81f, 0.85f, 1.00f);
    colors[ImPlotCol_LegendText]    = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImPlotCol_TitleText]     = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImPlotCol_InlayText]     = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImPlotCol_AxisText]      = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImPlotCol_AxisGrid]      = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImPlotCol_AxisBgHovered]   = ImVec4(0.92f, 0.92f, 0.95f, 1.00f);
    colors[ImPlotCol_AxisBgActive]    = ImVec4(0.92f, 0.92f, 0.95f, 0.75f);
    colors[ImPlotCol_Selection]     = ImVec4(1.00f, 0.65f, 0.00f, 1.00f);
    colors[ImPlotCol_Crosshairs]    = ImVec4(0.23f, 0.10f, 0.64f, 0.50f);

    style.LineWeight       = 1.5;
    style.Marker           = ImPlotMarker_None;
    style.MarkerSize       = 4;
    style.MarkerWeight     = 1;
    style.FillAlpha        = 1.0f;
    style.ErrorBarSize     = 5;
    style.ErrorBarWeight   = 1.5f;
    style.DigitalBitHeight = 8;
    style.DigitalBitGap    = 4;
    style.PlotBorderSize   = 0;
    style.MinorAlpha       = 1.0f;
    style.MajorTickLen     = ImVec2(0,0);
    style.MinorTickLen     = ImVec2(0,0);
    style.MajorTickSize    = ImVec2(0,0);
    style.MinorTickSize    = ImVec2(0,0);
    style.MajorGridSize    = ImVec2(1.2f,1.2f);
    style.MinorGridSize    = ImVec2(1.2f,1.2f);
    style.PlotPadding      = ImVec2(12,12);
    style.LabelPadding     = ImVec2(5,5);
    style.LegendPadding    = ImVec2(5,5);
    style.MousePosPadding  = ImVec2(5,5);
    style.PlotMinSize      = ImVec2(300,225);
}

} // namespaece MyImPlot

// WARNING:
//
// You can use "implot_internal.h" to build custom plotting fuctions or extend ImPlot.
// However, note that forward compatibility of this file is not guaranteed and the
// internal API is subject to change. At some point we hope to bring more of this
// into the public API and expose the necessary building blocks to fully support
// custom plotters. For now, proceed at your own risk!

#include "implot_internal.h"

namespace MyImPlot {

template <typename T>
int BinarySearch(const T* arr, int l, int r, T x) {
    if (r >= l) {
        int mid = l + (r - l) / 2;
        if (arr[mid] == x)
            return mid;
        if (arr[mid] > x)
            return BinarySearch(arr, l, mid - 1, x);
        return BinarySearch(arr, mid + 1, r, x);
    }
    return -1;
}

void PlotCandlestick(const char* label_id, const double* xs, const double* opens, const double* closes, const double* lows, const double* highs, int count, bool tooltip, float width_percent, ImVec4 bullCol, ImVec4 bearCol) {

    // get ImGui window DrawList
    ImDrawList* draw_list = ImPlot::GetPlotDrawList();
    // calc real value width
    double half_width = count > 1 ? (xs[1] - xs[0]) * width_percent : width_percent;

    // custom tool
    if (ImPlot::IsPlotHovered() && tooltip) {
        ImPlotPoint mouse   = ImPlot::GetPlotMousePos();
        mouse.x             = ImPlot::RoundTime(ImPlotTime::FromDouble(mouse.x), ImPlotTimeUnit_Day).ToDouble();
        float  tool_l       = ImPlot::PlotToPixels(mouse.x - half_width * 1.5, mouse.y).x;
        float  tool_r       = ImPlot::PlotToPixels(mouse.x + half_width * 1.5, mouse.y).x;
        float  tool_t       = ImPlot::GetPlotPos().y;
        float  tool_b       = tool_t + ImPlot::GetPlotSize().y;
        ImPlot::PushPlotClipRect();
        draw_list->AddRectFilled(ImVec2(tool_l, tool_t), ImVec2(tool_r, tool_b), IM_COL32(128,128,128,64));
        ImPlot::PopPlotClipRect();
        // find mouse location index
        int idx = BinarySearch(xs, 0, count - 1, mouse.x);
        // render tool tip (won't be affected by plot clip rect)
        if (idx != -1) {
            ImGui::BeginTooltip();
            char buff[32];
            ImPlot::FormatDate(ImPlotTime::FromDouble(xs[idx]),buff,32,ImPlotDateFmt_DayMoYr,ImPlot::GetStyle().UseISO8601);
            ImGui::Text("Day:   %s",  buff);
            ImGui::Text("Open:  $%.2f", opens[idx]);
            ImGui::Text("Close: $%.2f", closes[idx]);
            ImGui::Text("Low:   $%.2f", lows[idx]);
            ImGui::Text("High:  $%.2f", highs[idx]);
            ImGui::EndTooltip();
        }
    }

    // begin plot item
    if (ImPlot::BeginItem(label_id)) {
        // override legend icon color
        ImPlot::GetCurrentItem()->Color = IM_COL32(64,64,64,255);
        // fit data if requested
        if (ImPlot::FitThisFrame()) {
            for (int i = 0; i < count; ++i) {
                ImPlot::FitPoint(ImPlotPoint(xs[i], lows[i]));
                ImPlot::FitPoint(ImPlotPoint(xs[i], highs[i]));
            }
        }
        // render data
        for (int i = 0; i < count; ++i) {
            ImVec2 open_pos  = ImPlot::PlotToPixels(xs[i] - half_width, opens[i]);
            ImVec2 close_pos = ImPlot::PlotToPixels(xs[i] + half_width, closes[i]);
            ImVec2 low_pos   = ImPlot::PlotToPixels(xs[i], lows[i]);
            ImVec2 high_pos  = ImPlot::PlotToPixels(xs[i], highs[i]);
            ImU32 color      = ImGui::GetColorU32(opens[i] > closes[i] ? bearCol : bullCol);
            draw_list->AddLine(low_pos, high_pos, color);
            draw_list->AddRectFilled(open_pos, close_pos, color);
        }

        // end plot item
        ImPlot::EndItem();
    }
}

} // namespace MyImplot

#else

void ImPlot::ShowFirstOrderDEWindow(bool* p_open) {}

#endif

#endif // #ifndef IMGUI_DISABLE

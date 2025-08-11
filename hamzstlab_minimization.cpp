//--------------------------------------------------
// ImPlot3D v0.3 WIP
// implot3d_demo.cpp
// Date: 2024-11-17
// Author: Breno Cunha Queiroz (brenocq.com)
//
// Acknowledgments:
//  ImPlot3D is heavily inspired by ImPlot
//  (https://github.com/epezent/implot) by Evan Pezent,
//  and follows a similar code style and structure to
//  maintain consistency with ImPlot's API.
//--------------------------------------------------

// Table of Contents:
// [SECTION] User Namespace
// [SECTION] Helpers
// [SECTION] Plots
// [SECTION] Axes
// [SECTION] Custom
// [SECTION] Demo Window
// [SECTION] Style Editor
// [SECTION] User Namespace Implementation

// We define this to avoid accidentally using the deprecated API
#ifndef IMPLOT_DISABLE_OBSOLETE_FUNCTIONS
#define IMPLOT_DISABLE_OBSOLETE_FUNCTIONS
#endif

#include <iostream>
#include <iomanip> // to declare the manipulator of setprecision()
#include <fstream>
#include <bits/stdc++.h> //for setw(6) at display() function
#include <vector> // For std::vector (example container)
#include <armadillo>
#include "symintegrationc++.h"
#include <algorithm> // For std::sort

#include "implot3d.h"
#include "implot3d_internal.h"

using namespace std;

//-----------------------------------------------------------------------------
// [SECTION] User Namespace
//-----------------------------------------------------------------------------

// Encapsulates examples for customizing ImPlot3D
namespace MyImPlot3D {

// Example for Custom Styles section
void StyleSeaborn();

} // namespace MyImPlot3D

namespace ImPlot3D {

//-----------------------------------------------------------------------------
// [SECTION] Helpers
//-----------------------------------------------------------------------------

static void HelpMarker(const char* desc) {
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip()) {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

// Utility structure for realtime plot
struct ScrollingBuffer {
    int MaxSize;
    int Offset;
    ImVector<float> Data;
    ScrollingBuffer(int max_size = 2000) {
        MaxSize = max_size;
        Offset = 0;
        Data.reserve(MaxSize);
    }
    void AddPoint(float x) {
        if (Data.size() < MaxSize)
            Data.push_back(x);
        else {
            Data[Offset] = x;
            Offset = (Offset + 1) % MaxSize;
        }
    }
    void Erase() {
        if (Data.size() > 0) {
            Data.shrink(0);
            Offset = 0;
        }
    }
};

//-----------------------------------------------------------------------------
// [SECTION] Plots
//-----------------------------------------------------------------------------

void DemoSurfacePlots() {
    constexpr int N = 20;
    static float xs[N * N], ys[N * N], zs[N * N];
    static float t = 0.0f;
    t += ImGui::GetIO().DeltaTime;

    // Define the range for X and Y
    constexpr float min_val = -1.0f;
    constexpr float max_val = 1.0f;
    constexpr float step = (max_val - min_val) / (N - 1);

    // Populate the xs, ys, and zs arrays
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
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
        if (selected_fill == 0) {
            ImGui::SameLine();
            ImGui::ColorEdit4("##SurfaceSolidColor", (float*)&solid_color);
        }

        // Choose colormap
        ImGui::RadioButton("Colormap", &selected_fill, 1);
        if (selected_fill == 1) {
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
            ImGui::BeginDisabled();
        ImGui::SliderFloat("Range min", &range_min, -1.0f, range_max - 0.01f);
        ImGui::SliderFloat("Range max", &range_max, range_min + 0.01f, 1.0f);
        if (!custom_range)
            ImGui::EndDisabled();

        ImGui::Unindent();
    }

    // Begin the plot
    if (selected_fill == 1)
        ImPlot3D::PushColormap(colormaps[sel_colormap]);
    if (ImPlot3D::BeginPlot("Surface Plots", ImVec2(-1, 400), ImPlot3DFlags_NoClip)) {
        // Set styles
        ImPlot3D::SetupAxesLimits(-1, 1, -1, 1, -1.5, 1.5);
        ImPlot3D::PushStyleVar(ImPlot3DStyleVar_FillAlpha, 0.8f);
        if (selected_fill == 0)
            ImPlot3D::SetNextFillStyle(solid_color);
        ImPlot3D::SetNextLineStyle(ImPlot3D::GetColormapColor(1));

        // Plot the surface
        if (custom_range)
            ImPlot3D::PlotSurface("Wave Surface", xs, ys, zs, N, N, (double)range_min, (double)range_max);
        else
            ImPlot3D::PlotSurface("Wave Surface", xs, ys, zs, N, N);

        // End the plot
        ImPlot3D::PopStyleVar();
        ImPlot3D::EndPlot();
    }
    if (selected_fill == 1)
        ImPlot3D::PopColormap();
}

//-----------------------------------------------------------------------------

double division(double x, double y)
{
	return x/y;
}
Symbolic divisionsym(Symbolic x, Symbolic y)
{
	return x/y;
}

void DemoDownhillSimplexPlot() {
    		
		constexpr int N = 20;
		static float xs[N * N], ys[N * N], zs[N * N];
		
		// Define the range for X and Y
		constexpr float min_val = -2.0f;
		constexpr float max_val = 2.0f;
		constexpr float step = (max_val - min_val) / (N - 1);
		static float s = 0.75;
		static float t = 1;
		// Populate the xs, ys, and zs arrays
		for (int i = 0; i < N; i++) 
		{
			for (int j = 0; j < N; j++) 
			{
				int idx = i * N + j;
				xs[idx] = min_val + j * step;                                             // X values are constant along rows
				ys[idx] = min_val + i * step;                                             // Y values are constant along columns
				zs[idx] =  exp(-(xs[idx]*xs[idx] + ys[idx]*ys[idx])/(2*(s*s)))  - exp(-(xs[idx]*xs[idx] + ys[idx]*ys[idx])/(2*(t*t))) + 0.1*xs[idx] * xs[idx] + 0.1*ys[idx] * ys[idx]; // z = sin(2t + sqrt(x^2 + y^2))
			}
		}

		// Code for computation with Armadillo + SymIntegration starts here
		static float μ = 0.05;
		arma::vec x0 = {0.1, 0.5} ;
		arma::vec x1 = {x0[0]+μ, x0[1]+0} ;
		arma::vec x2 = {x0[0]+0, x0[1]+μ} ;

		static float α = 1, β = 0.5, γ = 2, δ = 0.5;
		arma::vec c;
		arma::vec xr;
		arma::vec xe;
		arma::vec xd;
		arma::vec xc;
		arma::vec s0;
		arma::vec s1;	

		int num_insidecontraction = 0, num_outsidecontraction = 0, num_expansion = 0, num_reflection = 0, num_shrink = 0;
		Symbolic x("x"), y("y");
		Symbolic f_x0, f_x1, f_x2, f_xr, f_xe, f_xc;
		double fx0, fx1, fx2, fxr, fxe, fxc;
		
		Symbolic f = exp(-divisionsym(x*x + y*y,2*s*s)) - exp(-divisionsym(x*x + y*y,2*t*t)) + division(1,10)*x*x + division(1,10)*y*y;
		
		int Ni = 6; // Number of iteration
		int n = 2; // number of  simplex points

		for (int i = 1; i <= Ni; i++) 
		{
			c = division(1,n)*(x0+x1+x2);
			
			xr = c+ α*(c-x2);

			Equations rules_xr = (x== xr[0],
		                		y == xr[1]);
			Equations rules_x0 = (x == x0[0],
		                		y == x0[1]);
			Equations rules_x1 = (x == x1[0],
		                		y == x1[1]);
			Equations rules_x2 = (x == x2[0],
		                		y == x2[1]);
			f_x0 = evalf(f.subst_all(rules_x0),1,1);
			f_x1 = evalf(f.subst_all(rules_x1),1,1);
			f_x2 = evalf(f.subst_all(rules_x2),1,1);
			f_xr = evalf(f.subst_all(rules_xr),1,1);
			
			fx0 = f_x0;
			fx1 = f_x1;
			fx2 = f_x2;
			fxr = f_xr;
			if (fxr < fx1 && fx0 <= fxr) // I. Reflection
			{
				//cout << "*** Reflection *** " << endl;
				x2 = xr;
				num_reflection += 1 ; 
			}
			else if (fxr < fx0) // continue to explore and do expansion
			{
				//cout << "*** Expansion *** " << endl;
				num_expansion += 1 ;
				xe = c + γ*(xr - c);
				Equations rules_xe = (x == xe[0],
		                		y == xe[1]);
				f_xe = evalf(f.subst_all(rules_xe),1,1);
				fxe = f_xe;
				
				// Greedy minimization
				if (fxe < fxr )
				{
					//cout << "*** Greedy minimization *** " << endl;
					x2 = xe;
				}
				else if (fxr <= fxe)
				{
					//cout << "*** Greedy minimization *** " << endl;
					x2 = xr;
				}
				// end of Greedy minimization
			}
			else if (fxr >= fx1 ) // Do contraction
			{
				if (fxr < fx0) // Outside contraction
				{
					xc = c + β*(xr-c);
					Equations rules_xc = (x == xc[0],
		                		y == xc[1]);
					f_xc = evalf(f.subst_all(rules_xc),1,1);
					fxc = f_xc;
					if (fxc <= fxr)
					{
						//cout << "*** Outside Contraction *** " << endl;
						x2 = xc; 
						num_outsidecontraction += 1 ;
					}
					else if (fxc > fx0)//  Peform shrink operation
					{
						//cout << "*** Shrink from outside contraction *** " << endl;
						
						s0 = x0 + δ*(x1-x0);
						s1 = x0 + δ*(x2-x0);	

						x1 = s0;
						x2 = s1;
						num_shrink += 1;
					}
				}
				else if (fxr >= fx0) // Inside contraction
				{
					xc = c + β*(x0-c);
					Equations rules_xc = (x == xc[0],
		                		y == xc[1]);
					f_xc = evalf(f.subst_all(rules_xc),1,1);
					fxc = f_xc;

					if (fxc <= fx0)
					{
						//cout << "*** Inside contraction *** " << endl;
						x2 =xc;
						num_insidecontraction += 1;
					}
					else if (fxc > fx0) //  Peform shrink operation
					{
						//cout << "*** Shrink from inside contraction *** " << endl;
						
						s0 = x0 + δ*(x1-x0);
						s1 = x0 + δ*(x2-x0);	
						
						x1 = s0;
						x2 = s1;
						num_shrink += 1;
					}
				}
			} // end of contraction
			
			
			// Sort in ascending order
			vector<double> v_fxy = {fx0, fx1, fx2};
			sort(v_fxy.begin(), v_fxy.end()); 
			
			if(v_fxy[0] == fx0) // sorting x0, x1, x2
			{
				x0 = x0;
				if (v_fxy[1] == fx1)
				{
					x1 = x1;
					x2 = x2;
				}
				else if (v_fxy[1] == fx2)
				{
					xd = x1;
					x1 = x2;
					x2 = xd;
				}
			}
			else if(v_fxy[0] == fx1)
			{
				xd = x0;
				x0 = x1;
				if (v_fxy[1] == fx0)
				{
					x1 = xd;
					x2 = x2;
				}
				else if (v_fxy[1] == fx2)
				{
					x1 = x2;
					x2 = xd;
				}			
			}
			else if(v_fxy[0] == fx2)
			{
				xd = x0;
				x0 = x2;
				if (v_fxy[1] == fx0)
				{
					x2 = x1;
					x1 = xd;
				}
				else if (v_fxy[1] == fx1)
				{
					x1 = x1;
					x2 = xd;
				}			
			}
		}	// end of for looping
		// end of Code for computation with Armadillo + SymIntegration

		// Choose fill color
		ImGui::Text("Fill color");
		static int selected_fill = 1; // Colormap by default
		static ImVec4 solid_color = ImVec4(0.8f, 0.8f, 0.2f, 0.6f);
		const char* colormaps[] = {"Viridis", "Plasma", "Hot", "Cool", "Pink", "Jet", "Twilight", "RdBu", "BrBG", "PiYG", "Spectral", "Greys"};
		static int sel_colormap = 10; // Spectral by default
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
		static bool range = false;
		ImGui::Checkbox("Change parameters", &range);
		if (range) {
			ImGui::SetNextItemWidth(200);
			ImGui::BulletText("s");
			ImGui::DragFloat("##s", &s, 0.1f, 10.0f);
			ImGui::SetNextItemWidth(200);
			ImGui::BulletText("t");
			ImGui::DragFloat("##t", &t, 0.1f, 10.0f);
			ImGui::SetNextItemWidth(200);
			/*ImGui::BulletText("alpha");
			ImGui::DragFloat("##alpha", &α, 0.1f, 10.0f);
			ImGui::SetNextItemWidth(200);
			ImGui::BulletText("beta");
			ImGui::DragFloat("##beta", &β, 0.1f, 10.0f);
			ImGui::SetNextItemWidth(200);
			ImGui::BulletText("gamma");
			ImGui::DragFloat("##gamma", &γ, 0.1f, 10.0f);
			ImGui::SetNextItemWidth(200);
			ImGui::BulletText("delta");
			ImGui::DragFloat("##delta", &δ, 0.1f, 10.0f);
			ImGui::SetNextItemWidth(200);*/
			
			
		}
		ImGui::Text("Number of iteration = %.0d ", Ni);
		ImGui::Text("Reflection = %.0d ", num_reflection);
		ImGui::Text("Expansion = %.0d ", num_expansion);
		ImGui::Text("Outside contraction = %.0d ", num_outsidecontraction);
		ImGui::Text("Inside contraction = %.0d ", num_insidecontraction);
		ImGui::Text("Shrink = %.0d ", num_shrink);
		ImGui::Text("f(x_{0}) = %.5f ", fx0);
		ImGui::Text("f(x_{1}) = %.5f ", fx1);
		ImGui::Text("f(x_{2}) = %.5f ", fx2);
		
		// Begin the plot
		if (selected_fill == 1)
		{
		ImPlot3D::PushColormap(colormaps[sel_colormap]);
		}
		if (ImPlot3D::BeginPlot("Surface Plot", ImVec2(-1, 400), ImPlot3DFlags_NoClip)) 
		{
		// Set styles
		ImPlot3D::SetupAxesLimits(-2, 2, -2, 2, -1.5, 1.5);
		ImPlot3D::PushStyleVar(ImPlot3DStyleVar_FillAlpha, 0.8f);
		if (selected_fill == 0)
		{
		ImPlot3D::SetNextFillStyle(solid_color);	
		ImPlot3D::SetNextLineStyle(ImPlot3D::GetColormapColor(1));}

		// Plot the surface
		ImPlot3D::PlotSurface("f(x,y) = exp(-(x^2+y^2)/(2s^2)) - exp(-(x^2+y^2)/(2t^2)) + (x^2)/10 + (y^2)/10", xs, ys, zs, N, N);

		// End the plot
		ImPlot3D::PopStyleVar();
		ImPlot3D::EndPlot();
		}
		if (selected_fill == 1)
		{
		ImPlot3D::PopColormap();
		}

}

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// [SECTION] Custom
//-----------------------------------------------------------------------------

void DemoCustomStyles() {
    ImPlot3D::PushColormap(ImPlot3DColormap_Deep);
    // normally you wouldn't change the entire style each frame
    ImPlot3DStyle backup = ImPlot3D::GetStyle();
    MyImPlot3D::StyleSeaborn();
    if (ImPlot3D::BeginPlot("Seaborn Style")) {
        ImPlot3D::SetupAxes("X-axis", "Y-axis", "Z-axis");
        ImPlot3D::SetupAxesLimits(-0.5f, 9.5f, -0.5f, 0.5f, 0, 10);
        unsigned int xs[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        unsigned int ys[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        unsigned int lin[10] = {8, 8, 9, 7, 8, 8, 8, 9, 7, 8};
        unsigned int dot[10] = {7, 6, 6, 7, 8, 5, 6, 5, 8, 7};
        ImPlot3D::NextColormapColor(); // Skip blue
        ImPlot3D::PlotLine("Line", xs, ys, lin, 10);
        ImPlot3D::NextColormapColor(); // Skip green
        ImPlot3D::PlotScatter("Scatter", xs, ys, dot, 10);
        ImPlot3D::EndPlot();
    }
    ImPlot3D::GetStyle() = backup;
    ImPlot3D::PopColormap();
}

void DemoCustomRendering() {
    if (ImPlot3D::BeginPlot("##CustomRend")) {
        ImPlot3D::SetupAxesLimits(-0.1f, 1.1f, -0.1f, 1.1f, -0.1f, 1.1f);

        // Draw circle
        ImVec2 cntr = ImPlot3D::PlotToPixels(ImPlot3DPoint(0.5f, 0.5f, 0.5f));
        ImPlot3D::GetPlotDrawList()->AddCircleFilled(cntr, 20, IM_COL32(255, 255, 0, 255), 20);

        // Draw box
        ImPlot3DPoint corners[8] = {
            ImPlot3DPoint(0, 0, 0), ImPlot3DPoint(1, 0, 0), ImPlot3DPoint(1, 1, 0), ImPlot3DPoint(0, 1, 0),
            ImPlot3DPoint(0, 0, 1), ImPlot3DPoint(1, 0, 1), ImPlot3DPoint(1, 1, 1), ImPlot3DPoint(0, 1, 1),
        };
        ImVec2 corners_px[8];
        for (int i = 0; i < 8; i++)
            corners_px[i] = ImPlot3D::PlotToPixels(corners[i]);

        ImU32 col = IM_COL32(128, 0, 255, 255);
        for (int i = 0; i < 4; i++) {
            ImPlot3D::GetPlotDrawList()->AddLine(corners_px[i], corners_px[(i + 1) % 4], col);
            ImPlot3D::GetPlotDrawList()->AddLine(corners_px[i + 4], corners_px[(i + 1) % 4 + 4], col);
            ImPlot3D::GetPlotDrawList()->AddLine(corners_px[i], corners_px[i + 4], col);
        }
        ImPlot3D::EndPlot();
    }
}

//-----------------------------------------------------------------------------
// [SECTION] Demo Window
//-----------------------------------------------------------------------------

void DemoHelp() {
    ImGui::SeparatorText("ABOUT THIS DEMO:");
    ImGui::BulletText("The other tabs are demonstrating many aspects of the library.");

    ImGui::SeparatorText("PROGRAMMER GUIDE:");
    ImGui::BulletText("See the ShowMinimizationWindow() code in hamzstlab_minimization.cpp. <- you are here!");

    ImGui::SeparatorText("USER GUIDE:");
    ImGui::BulletText("Translation");
    {
        ImGui::Indent();
        ImGui::BulletText("Left-click drag to translate.");
        ImGui::BulletText("If over axis, only that axis will translate.");
        ImGui::BulletText("If over plane, only that plane will translate.");
        ImGui::BulletText("If outside plot area, translate in the view plane.");
        ImGui::Unindent();
    }

    ImGui::BulletText("Zoom");
    {
        ImGui::Indent();
        ImGui::BulletText("Scroll or middle-click drag to zoom.");
        ImGui::BulletText("If over axis, only that axis will zoom.");
        ImGui::BulletText("If over plane, only that plane will zoom.");
        ImGui::BulletText("If outside plot area, zoom the entire plot.");
        ImGui::Unindent();
    }

    ImGui::BulletText("Rotation");
    {
        ImGui::Indent();
        ImGui::BulletText("Right-click drag to rotate.");
        ImGui::BulletText("To reset rotation, double right-click outside plot area.");
        ImGui::BulletText("To rotate to plane, double right-click when over the plane.");
        ImGui::Unindent();
    }

    ImGui::BulletText("Fit data");
    {
        ImGui::Indent();
        ImGui::BulletText("Double left-click to fit.");
        ImGui::BulletText("If over axis, fit data to axis.");
        ImGui::BulletText("If over plane, fit data to plane.");
        ImGui::BulletText("If outside plot area, fit data to plot.");
        ImGui::Unindent();
    }

    ImGui::BulletText("Context Menus");
    {
        ImGui::Indent();
        ImGui::BulletText("Right-click outside plot area to show full context menu.");
        ImGui::BulletText("Right-click over legend to show legend context menu.");
        ImGui::BulletText("Right-click over axis to show axis context menu.");
        ImGui::BulletText("Right-click over plane to show plane context menu.");
        ImGui::Unindent();
    }

    ImGui::BulletText("Click legend label icons to show/hide plot items.");
}

void DemoHeader(const char* label, void (*demo)()) {
    if (ImGui::TreeNodeEx(label)) {
        demo();
        ImGui::TreePop();
    }
}

void ShowAllDemos() {
    ImGui::Text("with ImPlot3D (%s)", IMPLOT3D_VERSION);
    ImGui::Spacing();
    if (ImGui::BeginTabBar("Minimization")) {
        if (ImGui::BeginTabItem("Plots")) {
            DemoHeader("Downhill Simplex Plot", DemoDownhillSimplexPlot);
            DemoHeader("Surface Plots", DemoSurfacePlots);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Custom")) {
            DemoHeader("Custom Styles", DemoCustomStyles);
            DemoHeader("Custom Rendering", DemoCustomRendering);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Help")) {
            DemoHelp();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

void ShowMinimizationWindow(bool* p_open) {
    static bool show_implot3d_style_editor = false;
    static bool show_imgui_metrics = false;
    static bool show_imgui_style_editor = false;
    static bool show_imgui_demo = false;

    if (show_implot3d_style_editor) {
        ImGui::Begin("Style Editor (ImPlot3D)", &show_implot3d_style_editor);
        ImPlot3D::ShowStyleEditor();
        ImGui::End();
    }

    if (show_imgui_style_editor) {
        ImGui::Begin("Style Editor (ImGui)", &show_imgui_style_editor);
        ImGui::ShowStyleEditor();
        ImGui::End();
    }
    if (show_imgui_metrics)
        ImGui::ShowMetricsWindow(&show_imgui_metrics);
    if (show_imgui_demo)
        ImGui::ShowDemoWindow(&show_imgui_demo);

    ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(600, 750), ImGuiCond_FirstUseEver);
    ImGui::Begin("Minimization Problem in 3D", p_open, ImGuiWindowFlags_MenuBar);
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Tools")) {
            ImGui::MenuItem("Style Editor", nullptr, &show_implot3d_style_editor);
            ImGui::Separator();
            ImGui::MenuItem("ImGui Metrics", nullptr, &show_imgui_metrics);
            ImGui::MenuItem("ImGui Style Editor", nullptr, &show_imgui_style_editor);
            ImGui::MenuItem("ImGui Demo", nullptr, &show_imgui_demo);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    ShowAllDemos();
    ImGui::End();
}

//-----------------------------------------------------------------------------
// [SECTION] Style Editor
//-----------------------------------------------------------------------------

bool ShowStyleSelector(const char* label) {
    static int style_idx = -1;
    if (ImGui::Combo(label, &style_idx, "Auto\0Classic\0Dark\0Light\0")) {
        switch (style_idx) {
            case 0: StyleColorsAuto(); break;
            case 1: StyleColorsClassic(); break;
            case 2: StyleColorsDark(); break;
            case 3: StyleColorsLight(); break;
        }
        return true;
    }
    return false;
}

void RenderColorBar(const ImU32* colors, int size, ImDrawList& DrawList, const ImRect& bounds, bool vert, bool reversed, bool continuous) {
    const int n = continuous ? size - 1 : size;
    ImU32 col1, col2;
    if (vert) {
        const float step = bounds.GetHeight() / n;
        ImRect rect(bounds.Min.x, bounds.Min.y, bounds.Max.x, bounds.Min.y + step);
        for (int i = 0; i < n; ++i) {
            if (reversed) {
                col1 = colors[size - i - 1];
                col2 = continuous ? colors[size - i - 2] : col1;
            } else {
                col1 = colors[i];
                col2 = continuous ? colors[i + 1] : col1;
            }
            DrawList.AddRectFilledMultiColor(rect.Min, rect.Max, col1, col1, col2, col2);
            rect.TranslateY(step);
        }
    } else {
        const float step = bounds.GetWidth() / n;
        ImRect rect(bounds.Min.x, bounds.Min.y, bounds.Min.x + step, bounds.Max.y);
        for (int i = 0; i < n; ++i) {
            if (reversed) {
                col1 = colors[size - i - 1];
                col2 = continuous ? colors[size - i - 2] : col1;
            } else {
                col1 = colors[i];
                col2 = continuous ? colors[i + 1] : col1;
            }
            DrawList.AddRectFilledMultiColor(rect.Min, rect.Max, col1, col2, col2, col1);
            rect.TranslateX(step);
        }
    }
}

static inline ImU32 CalcTextColor(const ImVec4& bg) {
    return (bg.x * 0.299f + bg.y * 0.587f + bg.z * 0.114f) > 0.5f ? IM_COL32_BLACK : IM_COL32_WHITE;
}
static inline ImU32 CalcTextColor(ImU32 bg) { return CalcTextColor(ImGui::ColorConvertU32ToFloat4(bg)); }

bool ColormapButton(const char* label, const ImVec2& size_arg, ImPlot3DColormap cmap) {
    ImGuiContext& G = *GImGui;
    const ImGuiStyle& style = G.Style;
    ImGuiWindow* Window = G.CurrentWindow;
    if (Window->SkipItems)
        return false;
    ImPlot3DContext& gp = *GImPlot3D;
    cmap = cmap == IMPLOT3D_AUTO ? gp.Style.Colormap : cmap;
    IM_ASSERT_USER_ERROR(cmap >= 0 && cmap < gp.ColormapData.Count, "Invalid colormap index!");
    const ImU32* keys = gp.ColormapData.GetKeys(cmap);
    const int count = gp.ColormapData.GetKeyCount(cmap);
    const bool qual = gp.ColormapData.IsQual(cmap);
    const ImVec2 pos = ImGui::GetCurrentWindow()->DC.CursorPos;
    const ImVec2 label_size = ImGui::CalcTextSize(label, nullptr, true);
    ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);
    const ImRect rect = ImRect(pos.x, pos.y, pos.x + size.x, pos.y + size.y);
    RenderColorBar(keys, count, *ImGui::GetWindowDrawList(), rect, false, false, !qual);
    const ImU32 text = CalcTextColor(gp.ColormapData.LerpTable(cmap, G.Style.ButtonTextAlign.x));
    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32_BLACK_TRANS);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1, 1, 1, 0.1f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1, 1, 1, 0.2f));
    ImGui::PushStyleColor(ImGuiCol_Text, text);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
    const bool pressed = ImGui::Button(label, size);
    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar(1);
    return pressed;
}

void ShowStyleEditor(ImPlot3DStyle* ref) {
    ImPlot3DContext& gp = *GImPlot3D;

    // Handle style internal storage
    ImPlot3DStyle& style = GetStyle();
    static ImPlot3DStyle ref_saved_style;
    static bool init = true;
    if (init && ref == nullptr)
        ref_saved_style = style;
    init = false;
    if (ref == nullptr)
        ref = &ref_saved_style;

    // Handle flash style color
    static float flash_color_time = 0.5f;
    static ImPlot3DCol flash_color_idx = ImPlot3DCol_COUNT;
    static ImVec4 flash_color_backup = ImVec4(0, 0, 0, 0);
    if (flash_color_idx != ImPlot3DCol_COUNT) {
        // Flash color
        ImVec4& color = style.Colors[flash_color_idx];
        ImGui::ColorConvertHSVtoRGB(ImCos(flash_color_time * 6.0f) * 0.5f + 0.5f, 0.5f, 0.5f, color.x, color.y, color.z);
        color.w = 1.0f;

        // Decrease timer until zero
        if ((flash_color_time -= ImGui::GetIO().DeltaTime) <= 0.0f) {
            // When timer reaches zero, restore the backup color
            style.Colors[flash_color_idx] = flash_color_backup;
            flash_color_idx = ImPlot3DCol_COUNT;
            flash_color_time = 0.5f;
        }
    }

    // Style selector
    if (ImPlot3D::ShowStyleSelector("Colors##Selector"))
        ref_saved_style = style;

    // Save/Revert button
    if (ImGui::Button("Save Ref"))
        *ref = ref_saved_style = style;
    ImGui::SameLine();
    if (ImGui::Button("Revert Ref"))
        style = *ref;
    ImGui::SameLine();
    HelpMarker("Save/Revert in local non-persistent storage. Default Colors definition are not affected. "
               "Use \"Export\" below to save them somewhere.");

    ImGui::Separator();

    if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None)) {
        if (ImGui::BeginTabItem("Variables")) {
            ImGui::Text("Item Styling");
            ImGui::SliderFloat("LineWeight", &style.LineWeight, 0.0f, 5.0f, "%.1f");
            ImGui::SliderFloat("MarkerSize", &style.MarkerSize, 2.0f, 10.0f, "%.1f");
            ImGui::SliderFloat("MarkerWeight", &style.MarkerWeight, 0.0f, 5.0f, "%.1f");
            ImGui::SliderFloat("FillAlpha", &style.FillAlpha, 0.0f, 1.0f, "%.2f");
            ImGui::Text("Plot Styling");
            ImGui::SliderFloat2("PlotDefaultSize", (float*)&style.PlotDefaultSize, 0.0f, 1000, "%.0f");
            ImGui::SliderFloat2("PlotMinSize", (float*)&style.PlotMinSize, 0.0f, 300, "%.0f");
            ImGui::SliderFloat2("PlotPadding", (float*)&style.PlotPadding, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("LabelPadding", (float*)&style.LabelPadding, 0.0f, 20.0f, "%.0f");
            ImGui::Text("Legend Styling");
            ImGui::SliderFloat2("LegendPadding", (float*)&style.LegendPadding, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("LegendInnerPadding", (float*)&style.LegendInnerPadding, 0.0f, 10.0f, "%.0f");
            ImGui::SliderFloat2("LegendSpacing", (float*)&style.LegendSpacing, 0.0f, 5.0f, "%.0f");
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Colors")) {
            static int output_dest = 0;
            static bool output_only_modified = true;
            if (ImGui::Button("Export")) {
                if (output_dest == 0)
                    ImGui::LogToClipboard();
                else
                    ImGui::LogToTTY();
                ImGui::LogText("ImVec4* colors = ImPlot3D::GetStyle().Colors;\n");
                for (int i = 0; i < ImPlot3DCol_COUNT; i++) {
                    const ImVec4& col = style.Colors[i];
                    const char* name = ImPlot3D::GetStyleColorName(i);
                    if (!output_only_modified || memcmp(&col, &ref->Colors[i], sizeof(ImVec4)) != 0)
                        ImGui::LogText("colors[ImPlot3DCol_%s]%*s= ImVec4(%.2ff, %.2ff, %.2ff, %.2ff);\n", name, 15 - (int)strlen(name), "", col.x,
                                       col.y, col.z, col.w);
                }
                ImGui::LogFinish();
            }
            ImGui::SameLine();
            ImGui::SetNextItemWidth(120);
            ImGui::Combo("##output_type", &output_dest, "To Clipboard\0To TTY\0");
            ImGui::SameLine();
            ImGui::Checkbox("Only Modified Colors", &output_only_modified);

            static ImGuiTextFilter filter;
            filter.Draw("Filter colors", ImGui::GetFontSize() * 16);

            static ImGuiColorEditFlags alpha_flags = ImGuiColorEditFlags_AlphaPreviewHalf;
#if IMGUI_VERSION_NUM < 19173
            if (ImGui::RadioButton("Opaque", alpha_flags == ImGuiColorEditFlags_None))
                alpha_flags = ImGuiColorEditFlags_None;
            ImGui::SameLine();
            if (ImGui::RadioButton("Alpha", alpha_flags == ImGuiColorEditFlags_AlphaPreview))
                alpha_flags = ImGuiColorEditFlags_AlphaPreview;
            ImGui::SameLine();
            if (ImGui::RadioButton("Both", alpha_flags == ImGuiColorEditFlags_AlphaPreviewHalf))
                alpha_flags = ImGuiColorEditFlags_AlphaPreviewHalf;
            ImGui::SameLine();
#else
            if (ImGui::RadioButton("Opaque", alpha_flags == ImGuiColorEditFlags_AlphaOpaque))
                alpha_flags = ImGuiColorEditFlags_AlphaOpaque;
            ImGui::SameLine();
            if (ImGui::RadioButton("Alpha", alpha_flags == ImGuiColorEditFlags_None))
                alpha_flags = ImGuiColorEditFlags_None;
            ImGui::SameLine();
            if (ImGui::RadioButton("Both", alpha_flags == ImGuiColorEditFlags_AlphaPreviewHalf))
                alpha_flags = ImGuiColorEditFlags_AlphaPreviewHalf;
            ImGui::SameLine();
#endif
            HelpMarker("In the color list:\n"
                       "Left-click on color square to open color picker,\n"
                       "Right-click to open edit options menu.");

            ImGui::Separator();

            for (int i = 0; i < ImPlot3DCol_COUNT; i++) {
                const char* name = ImPlot3D::GetStyleColorName(i);
                if (!filter.PassFilter(name))
                    continue;
                ImGui::PushID(i);

                // Flash color
                if (ImGui::Button("?")) {
                    if (flash_color_idx != ImPlot3DCol_COUNT)
                        style.Colors[flash_color_idx] = flash_color_backup;
                    flash_color_time = 0.5f;
                    flash_color_idx = (ImPlot3DCol)i;
                    flash_color_backup = style.Colors[i];
                }
                ImGui::SetItemTooltip("Flash given color to identify places where it is used.");
                ImGui::SameLine();

                // Handle auto color selection
                const bool is_auto = IsColorAuto(style.Colors[i]);
                if (is_auto)
                    ImGui::BeginDisabled();
                if (ImGui::Button("Auto"))
                    style.Colors[i] = IMPLOT3D_AUTO_COL;
                if (is_auto)
                    ImGui::EndDisabled();

                // Color selection
                ImGui::SameLine();
                if (ImGui::ColorEdit4("##Color", (float*)&style.Colors[i], ImGuiColorEditFlags_NoInputs | alpha_flags)) {
                    if (style.Colors[i].w == -1)
                        style.Colors[i].w = 1;
                }

                // Save/Revert buttons if color changed
                if (memcmp(&style.Colors[i], &ref->Colors[i], sizeof(ImVec4)) != 0) {
                    ImGui::SameLine();
                    if (ImGui::Button("Save"))
                        ref->Colors[i] = style.Colors[i];
                    ImGui::SameLine();
                    if (ImGui::Button("Revert"))
                        style.Colors[i] = ref->Colors[i];
                }
                ImGui::SameLine();
                ImGui::TextUnformatted(name);
                ImGui::PopID();
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Colormaps")) {
            static int output_dest = 0;
            if (ImGui::Button("Export", ImVec2(75, 0))) {
                if (output_dest == 0)
                    ImGui::LogToClipboard();
                else
                    ImGui::LogToTTY();
                int size = GetColormapSize();
                const char* name = GetColormapName(gp.Style.Colormap);
                ImGui::LogText("static const ImU32 %s_Data[%d] = {\n", name, size);
                for (int i = 0; i < size; ++i) {
                    ImU32 col = GetColormapColorU32(i, gp.Style.Colormap);
                    ImGui::LogText("    %u%s\n", col, i == size - 1 ? "" : ",");
                }
                ImGui::LogText("};\nImPlotColormap %s = ImPlot::AddColormap(\"%s\", %s_Data, %d);", name, name, name, size);
                ImGui::LogFinish();
            }
            ImGui::SameLine();
            ImGui::SetNextItemWidth(120);
            ImGui::Combo("##output_type", &output_dest, "To Clipboard\0To TTY\0");
            ImGui::SameLine();
            static bool edit = false;
            ImGui::Checkbox("Edit Mode", &edit);

            // built-in/added
            ImGui::Separator();
            for (int i = 0; i < gp.ColormapData.Count; ++i) {
                ImGui::PushID(i);
                int size = gp.ColormapData.GetKeyCount(i);
                bool selected = i == gp.Style.Colormap;

                const char* name = GetColormapName(i);
                if (!selected)
                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.25f);
                if (ImGui::Button(name, ImVec2(100, 0))) {
                    gp.Style.Colormap = i;
                    BustItemCache();
                }
                if (!selected)
                    ImGui::PopStyleVar();
                ImGui::SameLine();
                ImGui::BeginGroup();
                if (edit) {
                    for (int c = 0; c < size; ++c) {
                        ImGui::PushID(c);
                        ImVec4 col4 = ImGui::ColorConvertU32ToFloat4(gp.ColormapData.GetKeyColor(i, c));
                        if (ImGui::ColorEdit4("", &col4.x, ImGuiColorEditFlags_NoInputs)) {
                            ImU32 col32 = ImGui::ColorConvertFloat4ToU32(col4);
                            gp.ColormapData.SetKeyColor(i, c, col32);
                            BustItemCache();
                        }
                        if ((c + 1) % 12 != 0 && c != size - 1)
                            ImGui::SameLine();
                        ImGui::PopID();
                    }
                } else {
                    if (ColormapButton("##", ImVec2(-1, 0), i))
                        edit = true;
                }
                ImGui::EndGroup();
                ImGui::PopID();
            }

            static ImVector<ImVec4> custom;
            if (custom.Size == 0) {
                custom.push_back(ImVec4(1, 0, 0, 1));
                custom.push_back(ImVec4(0, 1, 0, 1));
                custom.push_back(ImVec4(0, 0, 1, 1));
            }
            ImGui::Separator();
            ImGui::BeginGroup();
            static char name[16] = "MyColormap";

            if (ImGui::Button("+", ImVec2((100 - ImGui::GetStyle().ItemSpacing.x) / 2, 0)))
                custom.push_back(ImVec4(0, 0, 0, 1));
            ImGui::SameLine();
            if (ImGui::Button("-", ImVec2((100 - ImGui::GetStyle().ItemSpacing.x) / 2, 0)) && custom.Size > 2)
                custom.pop_back();
            ImGui::SetNextItemWidth(100);
            ImGui::InputText("##Name", name, 16, ImGuiInputTextFlags_CharsNoBlank);
            static bool qual = true;
            ImGui::Checkbox("Qualitative", &qual);
            if (ImGui::Button("Add", ImVec2(100, 0)) && gp.ColormapData.GetIndex(name) == -1)
                AddColormap(name, custom.Data, custom.Size, qual);

            ImGui::EndGroup();
            ImGui::SameLine();
            ImGui::BeginGroup();
            for (int c = 0; c < custom.Size; ++c) {
                ImGui::PushID(c);
                if (ImGui::ColorEdit4("##Col1", &custom[c].x, ImGuiColorEditFlags_NoInputs)) {
                }
                if ((c + 1) % 12 != 0)
                    ImGui::SameLine();
                ImGui::PopID();
            }
            ImGui::EndGroup();

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
}

} // namespace ImPlot3D

//-----------------------------------------------------------------------------
// [SECTION] User Namespace Implementation
//-----------------------------------------------------------------------------

namespace MyImPlot3D {

void StyleSeaborn() {

    ImPlot3DStyle& style = ImPlot3D::GetStyle();

    ImVec4* colors = style.Colors;
    colors[ImPlot3DCol_Line] = IMPLOT3D_AUTO_COL;
    colors[ImPlot3DCol_Fill] = IMPLOT3D_AUTO_COL;
    colors[ImPlot3DCol_MarkerOutline] = IMPLOT3D_AUTO_COL;
    colors[ImPlot3DCol_MarkerFill] = IMPLOT3D_AUTO_COL;
    colors[ImPlot3DCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImPlot3DCol_PlotBg] = ImVec4(0.92f, 0.92f, 0.95f, 1.00f);
    colors[ImPlot3DCol_PlotBorder] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImPlot3DCol_LegendBg] = ImVec4(0.92f, 0.92f, 0.95f, 1.00f);
    colors[ImPlot3DCol_LegendBorder] = ImVec4(0.80f, 0.81f, 0.85f, 1.00f);
    colors[ImPlot3DCol_LegendText] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImPlot3DCol_TitleText] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImPlot3DCol_InlayText] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImPlot3DCol_AxisText] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImPlot3DCol_AxisGrid] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);

    style.LineWeight = 1.5;
    style.Marker = ImPlot3DMarker_None;
    style.MarkerSize = 4;
    style.MarkerWeight = 1;
    style.FillAlpha = 1.0f;
    style.PlotPadding = ImVec2(12, 12);
    style.LabelPadding = ImVec2(5, 5);
    style.LegendPadding = ImVec2(5, 5);
    style.PlotMinSize = ImVec2(300, 225);
}

} // namespace MyImPlot3D

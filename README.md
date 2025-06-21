# Hamzstlab-Mathematics
Interactive plotting library for Mathematics learning.

# Status

:sunflower: = Done

:writing_hand: = On Progress

| Status | Name | Details |
| -------------     | ------------- | ------------- | 
|:sunflower:   | Add basic demo for implot, imnodes, implot 3D				| Done
|:sunflower:   | Root Finding: Bisection and Newton' Method    				| Done
|:sunflower:   | First order differential equation: Euler' method      			| Done
|:writing_hand:| Minimization with gradient descent and Newton' Method			| Not yet
|:writing_hand:| Approximation with Lagrange interpolation, cubic-spline, Fourier series| Not yet
|:writing_hand:| Numerical integration with Simpson's rule, Romberg integration		| Not yet
|:writing_hand:| Boundary value problems						| Not yet
|:writing_hand:| PDE: Heat equation							| Not yet
|:writing_hand:| PDE: Wave equation							| Not yet
|:writing_hand:| Gene Sequence								| Not yet
|:writing_hand:| Genetic Algorithms							| Not yet
|:writing_hand:| Statistics								| Not yet


# Manual Book

<a href="https://github.com/glanzkaiser/Hamzstplot/blob/main/Hamzstlab%20Mathematics%20and%20Hamzstplot.pdf">Click</a>

| Status | Name | Details |
| -------------     | ------------- | ------------- | 
|:writing_hand:| Intermediate Calculus	        					| Not yet
|:writing_hand:| Differential Equations		       					| Not yet
|:writing_hand:| Partial Differential Equations	     					| Not yet
|:writing_hand:| Probability and Data Analysis        					| Not yet
|:writing_hand:| Linear and Nonlinear Programming    					| Not yet
|:writing_hand:| Introduction to Theory of Interest and Financial Mathematics		| Not yet
|:writing_hand:| Introduction to Stochastic Processes					| Not yet
|:writing_hand:| Numerical Analysis		     					| Not yet
|:writing_hand:| Complex Numbers		      					| Not yet

## Dependencies
[imgui v 1.92.0 WIP](https://github.com/ocornut/imgui)
[implot](https://github.com/epezent/implot)
[implot3d](https://github.com/brenocq/implot3d)
[implot_demos](https://github.com/epezent/implot_demos)

In this repository we already have the dependencies downloaded so no need for downloading again, but it is encouraged to read the repositories above to learn more.

# Example

We have created an example that can be easily compiled and executed at `examples/PilotProject/`, from this directory open terminal and type

```
make
./main
```

This `PilotProject` will plot stairstep function, sine function, and a histogram that we can interact with.

You can see that it is really fun, compared to the an image library like gnuplot, you can do more here with Imgui and Implot.

Another example is at `examples/First Order Differential Equation/`, from this directory open terminal and type

```
make
./main
```

This will plot an analytic solution of $`\frac{dy}{dt} = 3 - 2t - \frac{1}{2} y`$ with initial value of $`y(0)=1`$ and the scatter plot of the Euler's method approximation.


# ImPlot Demos

In the `/implot-demos` directory we have modified the implot_demos that has lengthier code to build an application or simulation like signal processing, perlin noise, realtime audio spectogram and visualizer.

We put all the source of Imgui, Implot, Implot 3D at the main directory: `Hamzstlab Mathematics/`, then we modify the `/implot-demos/CMakeLists.txt` to look for the source at the right directory.

We are only building few examples/demos from the original one.

If you want to create the demos, learn from the source code in `/implot-demos/demos` and you can create your own, don't forget to adjust the `/implot-demos/CMakeLists.txt` as well.

Once you are done, then open the terminal from `implot-demos` then type

```
mkdir build
cd build
cmake ..
make

./demo 
./graph
./spectogram
./perlin
```

# Current Plot and Simulation

| Name | Simulation |
| -------------     | ------------- | 
| PilotProject      | <img src="https://github.com/glanzkaiser/Hamzstlab-Mathematics/blob/main/images/HamzstlabMath.gif" width="83%">
| 3D Test 	    | <img src="https://github.com/glanzkaiser/Hamzstlab-Mathematics/blob/main/images/HamzstlabMath3d.gif" width="83%">
| Imnodes Test	    | <img src="https://github.com/glanzkaiser/Hamzstlab-Mathematics/blob/main/images/imnodestest.png" width="83%">
| Implot Demo 	    | <img src="https://github.com/glanzkaiser/Hamzstlab-Mathematics/blob/main/images/ImplotDemo.gif" width="83%">
| Spectogram 	    | <img src="https://github.com/glanzkaiser/Hamzstlab-Mathematics/blob/main/images/implotdemos.gif" width="83%">
| Euler's Method    | <img src="https://github.com/glanzkaiser/Hamzstlab-Mathematics/blob/main/images/1storderDE.png" width="83%">
| Root Finding	    | <img src="https://github.com/glanzkaiser/Hamzstlab-Mathematics/blob/main/images/rootfinding.png" width="83%">

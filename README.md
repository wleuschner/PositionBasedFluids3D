The code in this repository implements the Position Based Fluids algorithm from Miles Macklin andMatthias Müller using Qt, OpenGL 4.4. To run the application, an OpenGL 4.4 capable graphics card is needed. Further the following packages need to be installed:

    Qt (>=5.0)
    OpenGL (>=4.4)
    GLM

The repository is build by issuing qmake inside of the root of the repository followed by a make. Make sure the Shaders directory is in the same directory as the executable to get a rendering. To move in the scene, the WASD keys are used. To rotate the camera, the arrow keys are used. To start a simulation, load an Object in .obj format via the Load Model option and let the Solid Checkbox unchecked. To increase the particle count, load more models into the simulation. The simulation can be startet and stopped with a press on space. a press on Numpad+ executes a single timestep. Have fun and playwith the options given by the GUI.

A video of the application can be found at https://www.youtube.com/watch?v=znnvtUs5be8
###  Animation
https://github.com/user-attachments/assets/deba8abe-07e3-4013-8b48-7f4a188375f1

###  Description
This project is for a computer graphics class. It shows an animation produced using rendering techniques with ray tracing. The animation shows the following features:
1) Texture mapping on the buildings and road to achieve a realistic look.
2) Shadows cast from the buildings depending on lighting direction and intensity.
3) Reflection on the windows computed using the surface normal. 
5) Motion blur on the cars to show movement by taking samples at multiple time periods with the set velocity. 
6) Sound is used to bring the scene to life to show how fast the cars are moving.

The frame rate of the animation is 24 frames per second with over 240 frames produced in /Assets/scene. 

###  How to run code
make

###  How to create image
cd Assets  
../A4 scene0.lua 

###  How to create animation
ffmpeg -r 24 -i prefix_%4d.png -c:v libx264 -vf fps=24 -pix_fmt yuv420p animation.mp4




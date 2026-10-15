
# Milestone Two: Software Design and Engineering Enhancement

Liam Jones  
CS-499  
09/15/2026

## Artifact Description

The artifact I selected for this enhancement is my 3D arcade scene that I originally created in CS 330: Computational Graphics and Visualization. The project was created using C++ and OpenGL and includes an arcade cabinet made from different 3D shapes. I also used textures, lighting, materials, and camera controls to make the scene more complete and allow the user to move around and view the arcade machine.

I chose this artifact because it was one of the projects I enjoyed working on the most during the Computer Science program. I also thought it was a good project to come back to because it already worked, but there were still ways I could improve the code and add more functionality to it.

## Justification and Enhancement

I think this artifact is a good addition to my ePortfolio because it shows several of the skills I learned during the program. The project uses C++, OpenGL, textures, transformations, user input, and camera movement. One issue with the original version was that most of the code used to build and render the arcade machine was inside one large RenderScene() method. The program worked, but having so much code in one method made it harder to read and would make it more difficult to change later.

For my first enhancement, I reorganized this code into smaller methods. I created separate methods for the environment, arcade cabinet, screen, and controls. The main RenderScene() method now calls these methods instead of containing all of the code for every part of the arcade machine. This did not change how the arcade looked, but it made the code more organized and easier to follow.

I also added more interaction to the arcade machine. I created a new Arcade Ready screen that tells the user to press F to play. The program checks the position of the camera to determine how close the user is to the arcade machine. If the user is close enough and presses F, the screen changes from Arcade Ready to Game Over. Pressing F again changes it back. If the user is too far away, pressing F does not change the screen. I also added logic so holding down the F key does not cause the screen to constantly switch back and forth.

These were the main changes I planned for this artifact in Module One, so I was able to complete the enhancement without making any major changes to my original plan.

## Course Outcomes

The course outcomes I originally selected for this enhancement were Outcomes 3 and 4. Outcome 3 focuses on designing and evaluating solutions to problems using computer science practices and considering the choices made during the design process. Outcome 4 focuses on using computing skills and tools to create solutions that provide value.

I think I made progress toward both outcomes with this enhancement. I had to look at the way my original code was organized and decide how I could improve it without changing something that already worked. Adding the arcade interaction also required me to work with user input, camera position, distance calculations, and the current state of the screen. I had to make sure these new features worked with the controls and rendering that were already part of the project. At this point, I do not think I need to make any changes to my original outcome plan.

## Reflection

One of the biggest things I learned while working on this enhancement was how useful it is to make changes a little at a time. Before changing anything, I made sure my original arcade project still ran correctly. I then made a separate copy for the enhancement so I would still have the original version if something went wrong. As I reorganized the rendering code, I built and ran the project after each change to make sure the arcade still looked the same. This made it much easier to tell if something I changed caused a problem.

I did run into an issue before I could start the enhancement. At first, I tried using a copy of the project that I downloaded from my GitHub repository. When I tried to run it, Visual Studio showed a large number of errors because some of the supporting files and libraries from the original CS 330 project were missing. I was able to find my original project on my computer, and that version ran correctly. I made my enhancement copy from that version instead.

Adding the interaction was another part that took some thought because the camera controls and the arcade screen were handled in different parts of the program. I had to use the camera position and keyboard input to determine when the user was close enough to interact with the machine and then use that information to change the screen. I also had to make sure I did not break any of the controls that were already working.

Overall, I think the enhancement improved the project without changing what I liked about the original version. The arcade still looks and works like my original project, but the code is more organized and the user can now actually interact with the machine instead of only moving around and looking at it. Working on the enhancement also gave me more experience going back into an older project, understanding how it works, and finding ways to improve it.

## AI Use Statement

I used Grammarly to assist with grammar and spelling. I also used ChatGPT to generate the Arcade Ready image used as part of the enhanced artifact.
D

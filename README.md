# Lumi Engine
------

# Install
Add **compile_commands.json** file with command `xmake project -k compile_commands`

Generate **.sln** files with `xmake project -k vsxmake`

To compile the program just run `xmake` and then to run the program `xmake run`
Or run the `compile.sh` file to compile and run the program.

# Week 1
**Tag Version:** `Week1/2dHouse` and `Week1/2dHouseRefactor`

This week I implemented render of a house with triangles and quads. With the refactor I started with abstraction of the objects to a mesh class and child classes of the mesh class, so I can easily create more objects such as circle, cubes, or planes easier. 

To change the rooftop color use the space key

This is the house progress.

![Week1Progress.](media/Week1House.png)

---




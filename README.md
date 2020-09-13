# RRT on Turtle-Sim

### The Turtle And the Hare (Run the turtle in ROS simulator using a sample maze image)
1. "rrt.png" is used as the maze for the planning task. The turtle is moved from the top left corner of the image to the bottom right corner.
2.  rrtclient.cpp generates the path and send information about the path to turtleserver.py which forwards it to turtle.cpp (which runs our turtle simulator. Socket programming has been used for communication between client and server)
3.  turtle.cpp connects to the turtlesim_node using "cmd_vel" and "pose" topics
4.  COMPILATION INSTRUCTIONS :
a. RUN turtleserver.py FIRST as follows - 
	Input Local Host IP ("127.0.0.1") and Port No. while compiling turtleserver.py (eg - python turtleserver.py "127.0.0.1" 2729)
b. RUN rrtclient.cpp using make
c. RUN turtle.cpp inside beginner_tutorials package as - rosrun beginner_tutorials pubTsim						

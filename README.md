# KRSSG
Task Round Projects

FIRST TASK - FINITE STATE MACHINE (.....and the awkward lift encounters)
My first implementation was in c++ using switch cases to simulate a finite statemachine. Code presented in fsm.cpp
In order to build a robust FSM , I switched to a python codebase. In this , two codes are being run -
	1) statemachine.py - Provides the framework for states to be initialised into our machine > controls machine
	2) fsm1.py - Defines the different state functions and possible transition functions.

SECOND TASK - THE TURTLE AND THE HARE
	1)"rrt.png" is used for the task. Me move from the top left corner of the image to the bottom right corner.
	2) rrtclient.cpp generates the path and send information about the path to turtleserver.py which forwards it to turtle.cpp (which 			runs our turtle simulator. Socket programming has been used for communication between client and 
	3) turtle.cpp connects to the turtlesim_node using "cmd_vel" and "pose" topics
	4) COMPILATION INSTRUCTIONS :
						a)RUN turtleserver.py FIRST as follows - 
							Input Local Host IP ("127.0.0.1") and Port No. while compiling turtleserver.py
										eg - python turtleserver.py "127.0.0.1" 2729
						b)RUN rrtclient.cpp using make
						c)RUN turtle.cpp inside beginner_tutorials package as -
								rosrun beginner_tutorials pubTsim
	
THIRD TASK - HUMANOID TASK
	1)Appropriate changes made to strategy.cc. MAIN goal is to use KICK_DRIBBLE efficiently and try to avoid opponents in its path 
		to goal. 
										

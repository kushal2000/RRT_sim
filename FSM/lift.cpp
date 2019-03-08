#include <iostream>
#include <string>

using namespace std; 

enum Lift_State
{
	Idle, MovingUp , MovingDown , Stop
};

struct button
{
	int floor;
	string state;
};

int main()
{
	//Take input of total no. of floors , starting floor
	cout << "Enter total no. of floors\n";
	int total_floors;
	cin >> total_floors;
	cout << "Enter starting floor\n";
	int current_floor;
	cin >> current_floor;
	
	//Create a 2-D array soring order of lift button pushes and "UP" or "DOWN"
	button input[total_floors];
	cout << "Enter no. of buttons pushed from inside\n";
	int buttons;
	cin >> buttons;
	for(int i=0;i<buttons;i++)
	{
		cout << "Enter floor number : ";
		cin >> input[i].floor;
		cout << "Enter state required : ";
		cin >> input[i].state;
	}

	//Store the topmost and bottommost floor to be visited
	int bottom_floor= input[0].floor;
	int top_floor= input[0].floor;
	for(int i=1;i<buttons;i++)
	{
		if(input[i].floor>top_floor)
			top_floor=input[i].floor;
		if(input[i].floor<bottom_floor)
			bottom_floor=input[i].floor;
	}
	
	//Set initial state 
	Lift_State current_state = Idle;

	//Checks whether or not lift is to stop
	int flag = 0;

	while(1)
	{
		switch(current_state)
		{
			case Idle:
				cout << "Floor " << current_floor;
				if((top_floor-current_floor)<0)
					current_state=MovingDown;
				else if((current_floor-top_floor)>0)
					current_state=MovingUp;
				else if((top_floor-current_floor)>(current_floor-top_floor))
					current_state=MovingDown;
				else
					current_state=MovingUp;
				break;

			case MovingUp:
				current_floor++;
				for(int i = 0 ; i<buttons;i++)
				{
					if(input[i].floor==current_floor)
					{	
						if(current_floor==top_floor)						//condition to change state
						{	
							if(flag==0){
								cout << " >> " << "Floor " << current_floor;
								flag++;
								current_state=MovingDown;}
							else{
								if(input[i].state!="SATISFIED"){
									cout << " >> "<< "Floor " << current_floor;}
								cout << endl;
								current_state=Stop; }
						}
						else if(input[i].state=="UP")						//satisfies button push
						{
							cout << " >> " << "Floor " << current_floor;
							input[i].state="SATISFIED";
						}
	
						else if(input[i].state=="DOWN")						//doesnt satisfy button push but lift still stops here
							cout << " >> " << "Floor " << current_floor;
					}
				}

				break;

			case MovingDown:
				current_floor--;
				for(int i = 0 ; i<buttons;i++)
				{
					if(input[i].floor==current_floor)
					{	
						if(current_floor==bottom_floor)							//condition to change state
						{	
							if(flag==0){
								cout << " >> " << "Floor " << current_floor;
								flag++;
								current_state=MovingUp;}
							else{
								if(input[i].state!="SATISFIED"){
									cout << " >> "<< "Floor " << current_floor;}
								cout << endl;
								current_state=Stop; }
						}
						else if(input[i].state=="DOWN")							//satisfies button push
						{
							cout << " >> " << "Floor " << current_floor;
							input[i].state="SATISFIED";
						}
	
						else if(input[i].state=="UP")
							cout << " >> " << "Floor " << current_floor;		//doesnt satisfy button push but lift still stops here
					}
				}
				break;

			case Stop:
				return 0;

			default:
				cout << "Error! Invalid State Reached!\n";
				break;
		}
	}
	return 0;
}
		

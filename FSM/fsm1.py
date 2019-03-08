from statemachine import StateMachine

L1=[]
L2=[]
L2_button=[]
bottom_floor=0
top_floor=0
currentState=""

def moving_up(floor):
    floor=floor+1
    result=0
    if floor in L1:
        L1.remove(floor)
        result=floor
    if floor in L2:
        if L2_button[L2.index(floor)]=="UP" or floor==top_floor:
            del L2_button[L2.index(floor)]
            L2.remove(floor)
            result=floor
    if result:
        print("Stop at "+str(floor))

    return (transition("Up_state", floor),floor)

def moving_down(floor):
    floor=floor-1
    result=0
    if floor in L1:
        L1.remove(floor)
        result=floor
    if floor in L2:
        if L2_button[L2.index(floor)]=="DOWN" or floor==bottom_floor:
            del L2_button[L2.index(floor)]
            L2.remove(floor)
            result=floor
    if result:
        print("Stop at "+str(floor))

    return (transition("Down_state", floor),floor)

def idle(floor):
    return (transition("Idle_state", floor),floor)

def upToDown(floor):
    if len(L1)==0 and len(L2)==0:
        return False
    if floor==top_floor:
        if len(L1)!=0 or len(L2)!=0:
            return True
    return False
def upToStop(floor):
    if len(L1)==0 and len(L2)==0:
        return True
    return False
def downToUp(floor):
    if len(L1)==0 and len(L2)==0:
        return False
    if floor==bottom_floor:
        if len(L1)!=0 or len(L2)!=0:
            return True
    return False
def downToStop(floor):
    if len(L1)==0 and len(L2)==0:
        return True
    return False
def idleToUp(floor):
    if len(L1)==0 and len(L2)==0:
        return False
    if top_floor<floor:
        return False
    if bottom_floor>floor:
        return True
    if top_floor-floor < floor-bottom_floor:
        return True
    return False
def idleToDown(floor):
    if len(L1)==0 and len(L2)==0:
        return False
    if top_floor<floor:
        return True
    if bottom_floor>floor:
        return False
    if top_floor-floor < floor-bottom_floor:
        return False
    return True
def idleToStop(floor):
    if len(L1)==0 and len(L2)==0:
        return True
    return False

def transition(currentState,floor):
    if currentState == "Up_state":
        if(upToDown(floor)):
            return "Down_state"
        if(upToStop(floor)):
            return "Stop_state"
        return "Up_state"
    if currentState == "Down_state":
        if(downToUp(floor)):
            return "Up_state"
        if(downToStop(floor)):
            return "Stop_state"
        return "Down_state"
    if currentState == "Idle_state":
        if(idleToUp(floor)):
            return "Up_state"
        if(idleToStop(floor)):
            return "Stop_state"
        if(idleToDown(floor)):
            return "Down_state"

if __name__== "__main__":
    floor=input("Enter current floor: ")
    tot_floors=input("Enter total floors: ")

    s=raw_input("Enter lift buttons pressed inside: ")
    L1= list(map(int,s.split()))
    j=0
    for i in range(len(L1)):
        if(L1[j]>tot_floors):
            print(str(L1[j])+" is an invalid floor")
            del L1[j]
            j=j-1
        j=j+1
    if floor in L1:
        print("Lift is already at "+str(floor))
        L1.remove(floor)


    s=raw_input("Enter buttons pressed outside: ")
    L2= list(map(int,s.split()))

    j=0
    for i in range(len(L2)):
        if(L2[j]>tot_floors):
            print(str(L2[j])+" is an invalid floor")
            del L2[j]
            j=j-1
        j=j+1
    if floor in L2:
        print("Lift is already at "+str(floor))
        L2.remove(floor)

    if(len(L2)>0):   
        print("Enter UP or DOWN for each floor")
        for i in range(len(L2)):
            L2_button.append(raw_input(str(L2[i])+" = "))

    top_floor=0
    bottom_floor=tot_floors
    for i in range(len(L1)):
        if top_floor<L1[i]:
            top_floor=L1[i]
        if bottom_floor>L1[i]:
            bottom_floor=L1[i]
    for i in range(len(L2)):
        if top_floor<L2[i]:
            top_floor=L2[i]
        if bottom_floor>L2[i]:
            bottom_floor=L2[i]
    print("--------------------------------------")
    lift = StateMachine()
    lift.add_state("Idle_state", idle)
    lift.add_state("Up_state", moving_up)
    lift.add_state("Down_state", moving_down)
    lift.add_state("Stop_state", None, end_state=1)
    currentState="Idle"
    lift.set_start("Idle_state")
    lift.run(floor)
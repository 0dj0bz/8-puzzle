//
//  main.cpp
//  ITCS 8150 Programming Assigment1
//
//  Created by Rob Abbott - 800835197 on 9/18/16.
//  University of North Carolina at Charlotte
//


// includes
#include <iostream>
#include <vector>
#include <queue>

// namespaces
using namespace std;


// State class
// This is the class that we use to represent a state in the 8-puzzle problem
class State {
public:
    int values[3][3] = { {0,0,0}, {0,0,0}, {0,0,0} };
    int cols[3][3] = { {0,0,0}, {0,0,0}, {0,0,0} };
    int rows[3][3] = { {0,0,0}, {0,0,0}, {0,0,0} };
    int op = 0;
    int f_n = {0};
    int g_n = {0};
    int h_n = {0};
    int zero[2] = {0,0};  // 0 - row, 1 - col
    State *parent = NULL;

    State () {};
    State (int state[3][3]);
    State (State const &);
    void print();
    void calculateRelativeRowColPositions (int targetState[3][3]);
    void calculateManhattanDist ();
    void changeState (int op);
    
    vector<State> createChildStates();

};


// create a new Node with a provided state
State::State(int state[3][3]) {
    for (int i=0;i<3;i++)
        for (int j=0;j<3;j++) {
            values[i][j] = state[i][j];
            
            // while we're initializing the state, if we find the zero, save that info
            if (values[i][j] == 0) {
                zero[0] = i;
                zero[1] = j;
            }
        }
    calculateManhattanDist();
}

// create a new state as a replica of an existing state

State::State(State const &pstate) {
    
    for (int i=0;i<3;i++)
        for (int j=0;j<3;j++) {
            values[i][j] = pstate.values[i][j];
            cols[i][j] = pstate.cols[i][j];
            rows[i][j] = pstate.rows[i][j];
            op = pstate.op;
            f_n = pstate.f_n;
            g_n = pstate.g_n;
            h_n = pstate.h_n;
            zero[0] = pstate.zero[0];
            zero[1] = pstate.zero[1];
            parent = pstate.parent;
            
        }
}

void State::changeState (int op) {
    
    switch (op) {
        case (1): { //right
            // relocate value
            int tempStor = values[zero[0]][zero[1]+1];
            values[zero[0]][zero[1]+1] = values[zero[0]][zero[1]];
            values[zero[0]][zero[1]] = tempStor;
            
            //relocate relative column position
            tempStor = cols[zero[0]][zero[1]+1];
            cols[zero[0]][zero[1]+1] = cols[zero[0]][zero[1]];
            cols[zero[0]][zero[1]] = tempStor;
            
            //relocate relative row position
            tempStor = rows[zero[0]][zero[1]+1];
            rows[zero[0]][zero[1]+1] = rows[zero[0]][zero[1]];
            rows[zero[0]][zero[1]] = tempStor;
            
            //now move zero
            zero[1]++;
            break;
        }
        case (-1): { //left
            // relocate value
            int tempStor = values[zero[0]][zero[1]-1];
            values[zero[0]][zero[1]-1] = values[zero[0]][zero[1]];
            values[zero[0]][zero[1]] = tempStor;
            
            //relocate relative column position
            tempStor = cols[zero[0]][zero[1]-1];
            cols[zero[0]][zero[1]-1] = cols[zero[0]][zero[1]];
            cols[zero[0]][zero[1]] = tempStor;
            
            //relocate relative row position
            tempStor = rows[zero[0]][zero[1]-1];
            rows[zero[0]][zero[1]-1] = rows[zero[0]][zero[1]];
            rows[zero[0]][zero[1]] = tempStor;
            
            //now move zero
            zero[1]--;
            break;
        }
        case (2): { //up
            // relocate value
            int tempStor = values[zero[0]-1][zero[1]];
            values[zero[0]-1][zero[1]] = values[zero[0]][zero[1]];
            values[zero[0]][zero[1]] = tempStor;
            
            //relocate relative column position
            tempStor = cols[zero[0]-1][zero[1]];
            cols[zero[0]-1][zero[1]] = cols[zero[0]][zero[1]];
            cols[zero[0]][zero[1]] = tempStor;
            
            //relocate relative row position
            tempStor = rows[zero[0]-1][zero[1]];
            rows[zero[0]-1][zero[1]] = rows[zero[0]][zero[1]];
            rows[zero[0]][zero[1]] = tempStor;
            
            //now move zero
            zero[0]--;
            break;
        }
        case (-2): { //down
            // relocate value
            int tempStor = values[zero[0]+1][zero[1]];
            values[zero[0]+1][zero[1]] = values[zero[0]][zero[1]];
            values[zero[0]][zero[1]] = tempStor;
            
            //relocate relative column position
            tempStor = cols[zero[0]+1][zero[1]];
            cols[zero[0]+1][zero[1]] = cols[zero[0]][zero[1]];
            cols[zero[0]][zero[1]] = tempStor;
            
            //relocate relative row position
            tempStor = rows[zero[0]+1][zero[1]];
            rows[zero[0]+1][zero[1]] = rows[zero[0]][zero[1]];
            rows[zero[0]][zero[1]] = tempStor;
            
            //now move zero
            zero[0]++;
            break;
        }
    }

}

// returns a vector<> of child nodes derived from the parent
vector<State> State::createChildStates() {
    
    vector<int> ops;

    // we use the following encodings for allowable transforms:
    // -1 - down, 1 - up, 2 - right, -2 - left
    // zero[0] refers to the row location of the "blank" space.
    // zero[1] refers to the column location of the "blank" space.
    
    if (zero[0] != 2) //allow down
        ops.push_back(-2);
    
    if (zero[0] != 0) //allow up
        ops.push_back(2);
    
    if (zero[1] != 2) //allow right
        ops.push_back(1);
    
    if (zero[1] != 0) //allow left
        ops.push_back(-1);
    
    // now generate the children
    vector<State> childStates;
    
    for (int i=0;i<ops.size();i++) {

        if (ops[i] == -(this->op) ) // if this op undoes the parent of the new node, skip it
            continue;
        
        State childState (*this);  // initialize the new child's values with the values in this node
        childState.parent = this; // set the new child's parent to the current node
        
        childState.op = ops[i];
        childState.changeState(ops[i]);
        childState.calculateManhattanDist();
        childState.g_n++; // increase the depth by one
        childState.f_n = childState.g_n + childState.h_n;
        
        childStates.push_back(childState);
    }
    
    return childStates;
    
}

void State::calculateRelativeRowColPositions(int targetState[3][3]) {
    for (int i=0;i<3;i++)
        for (int j=0;j<3;j++)
            for (int k=0;k<3;k++)
                for (int l=0;l<3;l++) {
                    if (values[i][j] == targetState[k][l]) {
                        rows[i][j] = k;
                        cols[i][j] = l;
                        
                        // put in logic to break once found
                    }
                }
}

void State::calculateManhattanDist() {
    
    h_n = 0;
    
    for (int i=0;i<3;i++)
        for (int j=0;j<3;j++) {
            // don't count the empty (zero) space in the L1 distance
            if (i == zero[0] && j == zero[1])
                continue;
    
            h_n += abs(rows[i][j] - i) + abs(cols[i][j] - j);
        }
}



void State::print() {
    
    string outBuf = "{";
    for (int i=0;i<3;i++) {
        outBuf += "{ ";
        
        for (int j=0;j<3;j++) {
            outBuf += to_string(values[i][j]);
            outBuf += " ";
    
        }
        outBuf += "}";
    }
    outBuf += "}";
    
    cout << "state: " << outBuf << " : f(n): " << f_n << " g(n): " << g_n << " h(n): " << h_n << endl;
    
    return;
}


int main(int argc, const char * argv[]) {

    // declarations
    
    // define our initial and final states
    // these should be dynamic
    
    int _initialState [3][3] = {{2,8,3},{1,6,4},{7,0,5}};
    int _finalState [3][3] =   {{1,2,3},{8,6,4},{7,5,0}};

    // this is the lambda function used by our priority queue to order the elements
    auto compare = [](State state1, State state2) -> bool { return state1.h_n > state2.h_n; };
    
    // the priority queue that contains the states of the 8-puzzle
    priority_queue<State, vector<State>, decltype(compare)> workingPath {compare};

    // indicator that we've reached the goal state
    bool goalAchieved = false;
    
    // save the goalState so that we can print the shortest path
    State *goalState;
    
    // generate the initialState for the 8-puzzle
    State initialState(_initialState);
    
    // calculate the positions of each of the tiles relative to the target state
    initialState.calculateRelativeRowColPositions(_finalState);
    
    // calculate the L1 distance for the initial state
    initialState.calculateManhattanDist();
    initialState.f_n = initialState.g_n + initialState.h_n;
    
    // push this onto the priority queue as the root
    workingPath.push(initialState);
    
    // now process the state space until we're out of states or we reach the goal state
    
    cout << "Search space to find goal:" << endl;
    while(!workingPath.empty()) {

        // retrieve the lowest-cost state from the priority queue
        State *currentState = new State(workingPath.top());
        workingPath.pop();
        
        currentState->print();
        
        // check to see if we've reached the goal state
        // if so, break out
        if (currentState->h_n == 0) {
            goalState = currentState;
            goalAchieved = true;
            break;
        }
        
        // generate the child states for the current state
        vector<State> childStates = currentState->createChildStates();
    
        // push the child states into the priority queue
        for (int i=0;i<childStates.size();i++)
            workingPath.push(childStates[i]);
          
    }
    
    if (goalAchieved)
        cout << "goal reached" << endl;
    else
        cout << "goal not reached" << endl;
    
    // print out shortest path
    
    State * aState;
    
    aState = goalState;
    
    // walk the path from the goal state back to the initial state
    cout << "Optimal path from goal state to initial state:" << endl;
    while (aState != NULL) {
        aState->print();
        aState = aState->parent;
    }
    
    return 0;
}

//
//  main.cpp
//  Assigment1
//
//  Created by Rob Abbott on 9/18/16.
//  Copyright © 2016 Rob Abbott. All rights reserved.
//

#include <iostream>
#include <vector>

using namespace std;





class Node {
public:
    int values[3][3] = { {0,0,0}, {0,0,0}, {0,0,0} };
    int cols[3][3] = { {0,0,0}, {0,0,0}, {0,0,0} };
    int rows[3][3] = { {0,0,0}, {0,0,0}, {0,0,0} };
    int op = 0;
    int f_n = {0};
    int g_n = {0};
    int h_n = {0};
    int zero[2] = {0,0};  // 0 - row, 1 - col
    Node *parent = NULL;

    
    Node ();
    Node (int state[3][3]);
    Node (Node const &);
    string print();
    void calculateRelativeRowColPositions (int targetState[3][3]);
    void calculateManhattanDist ();
    void changeState (int op);
    
    vector<Node> createChildren();

};

// create a new Node with a provided state
Node::Node(int state[3][3]) {
    for (int i=0;i<3;i++)
        for (int j=0;j<3;j++)
            values[i][j] = state[i][j];
}

// create a new node as a replica of an existing node

Node::Node(Node const &pnode) {
    
    for (int i=0;i<3;i++)
        for (int j=0;j<3;j++) {
            values[i][j] = pnode.values[i][j];
            cols[i][j] = pnode.cols[i][j];
            rows[i][j] = pnode.rows[i][j];
            op = pnode.op;
            f_n = pnode.f_n;
            g_n = pnode.g_n;
            h_n = pnode.h_n;
            zero[0] = pnode.zero[0];
            zero[1] = pnode.zero[1];
            parent = pnode.parent;
            
        }
}

void Node::changeState (int op) {
    
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
vector<Node> Node::createChildren() {
    
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
    vector<Node> childNodes;
    
    for (int i=0;i<ops.size();i++) {

        if (ops[i] == -(this->op) ) // if this op undoes the parent of the new node, skip it
            continue;
        
        Node childNode (*this);  // initialize the new child's values with the values in this node
        childNode.parent = this; // set the new child's parent to the current node
        
        childNode.op = ops[i];
        childNode.changeState(ops[i]);
        
        childNodes.push_back(childNode);
    }
    
    return childNodes;
    
}

void Node::calculateRelativeRowColPositions(int targetState[3][3]) {
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

void Node::calculateManhattanDist() {
    
    h_n = 0;
    
    for (int i=0;i<3;i++)
        for (int j=0;j<3;j++) {
            // don't count the empty (zero) space in the L1 distance
            if (i == zero[0] && j == zero[1])
                continue;
    
            h_n += abs(rows[i][j] - i) + abs(cols[i][j] - j);
        }
}



string Node::print() {
    
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
    
    return outBuf;
}


int main(int argc, const char * argv[]) {


    
    int initialState [3][3] = {{2,8,3},{1,6,4},{7,0,5}};
    int finalState [3][3] =   {{1,2,3},{8,6,4},{7,5,0}};
    
    Node myNode( initialState);
    myNode.zero[0] = 2; myNode.zero[1] = 1;
    
    myNode.calculateRelativeRowColPositions(finalState);
    myNode.calculateManhattanDist();
    
    vector<Node> childNodes = myNode.createChildren();
    
    cout << myNode.h_n << endl;
    cout << myNode.print() << endl;
    
    return 0;
}

#include "screen.h"
#include <numeric>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>

using namespace std;


/**
 * 
 * vec3 represents a 3 dimensional coordinate with members x, y, z
 * 
*/
struct vec3{
    float x,y,z;
};

/**
 * 
 * vec2 represents a 2 dimensional coordinate with members u and v
 * 
*/
struct vec2{
    float u,v;
};

/**
 * 
 * connection is a struct with members a, b, and c representing the group of 3 vertices that represent each face
 * 
 */ 
struct connection{
    int a,b,c;
};

/**
 * 
 * Converts 3D world coordinates to 2D screen space (pixel)
 * Input is a vector of vec3 (3 dimensional coordinates) and returns a vector of vec2 (2 dimensional coordinates)
 * 
 */ 
vector<vec2> worldSpaceToScreenSpace(vector<vec3> v){
    float e = 1000.0;
    vector<vec2> sSpace;
   
   for(auto& a : v){
       vec3 d;
       d.x = a.x;
       d.y = a.y;
       d.z = a.z - e;

       vec2 b;
       b.u = (d.x*(-e/d.z));
       b.v = (d.y*(-e/d.z));
       sSpace.push_back(b);
   }
   
    return sSpace;

}

/**
 * 
 * Shifts coordinate system so that origin is at the center of the window rather than the top left (default)
 * Additionally, y coordinate is flipped such that the Y-axis is pointing vertically upwards
 * 
 */ 
void shiftCoordFrame(vector<vec2>& v){
    for(auto&a : v){
        a.u += 160;
        a.v += 120;
        //dy represents distance from y axis
        float dy = a.v - 120;
        a.v = 120 - dy;
    }

}

/**
 * 
 * Rotates 3D coordinates around X-axis, passes by reference so that v is updated
 * Inputs are a vector of 3 dimensional coordinates (vec3) and theta, the rotation angle
 * 
 */ 
void rotateX(vector<vec3>& v, float theta){
    for(auto& p : v){
        float ax, ay, az;

        ax = (p.x * 1) + (p.y * 0) + (p.z * 0);
        ay = (p.x * 0) + (p.y * cos(theta)) + (p.z * -sin(theta));
        az = (p.x * 0) + (p.y * sin(theta)) + (p.z * cos(theta));

        p.x = ax;
        p.y = ay;
        p.z = az;
    }

}

/**
 * 
 * Rotates 3D coordinates around Y-axis, passes by reference so that v is updated
 * Inputs are a vector of 3 dimensional coordinates (vec3) and theta, the rotation angle
 * 
 */ 
void rotateY(vector<vec3>& v, float theta){
    for(auto& p : v){
        float ax, ay, az;

        ax = (p.x * cos(theta)) + (p.y * 0) + (p.z * sin(theta));
        ay = (p.x * 0) + (p.y * 1) + (p.z * 0);
        az = (p.x * -sin(theta)) + (p.y * 0) + (p.z * cos(theta));

        p.x = ax;
        p.y = ay;
        p.z = az;
    }

}

/**
 * 
 * Draws a line between two points (x1, y1) and (x2, y2) given as inputs
 * 
 */ 
void drawLine(Screen& screen, float x1, float y1, float x2, float y2){
    float dx = x2 - x1;
    float dy = y2 - y1;

    float length = sqrt(dx * dx + dy * dy);
    float angle = atan2(dy, dx);

    for(float i = 0; i < length; i++){
        screen.pixel(x1 + cos(angle) * i, y1+ sin(angle) * i);
    }
    
}

/**
 * 
 * function to draw a circle
 * Inputs are a Screen object, the x location of the center, the y location of the center,
 * and the desired radius of the circle
 */ 
void draw_circle(Screen& screen, int x, int y, int radius){
    for (int w = 0; w < radius * 2; w++)
    {
        for (int h = 0; h < radius * 2; h++)
        {
            int dx = radius - w; // horizontal offset
            int dy = radius - h; // vertical offset
            if ((dx*dx + dy*dy) <= (radius * radius))
            {
                screen.pixel(x + dx, y + dy);
            }
        }
    }
}

/**
 * 
 * function to find the angle between the normal of the first surface and Z-axis
 * used to compute the color used for the shape
 * 
 */ 
float angBetweenNormalandZ(vector<vec3> v){
    vec3 c_P;
    

    
    c_P.x = (v[0].y-120) * v[1].z - v[0].z * (v[1].y-120);
    c_P.y = -((v[0].x-160) * v[1].z - v[0].z * (v[1].x-160));
    c_P.z = (v[0].x-160) * (v[1].y-120) - (v[0].y-120) * (v[1].x-160);

    cout << c_P.x << endl;
    

    float x1 = c_P.x;
    float y1 = c_P.y;
    float z1 = c_P.z;

    float x2 = 0;
    float y2 = 0;
    float z2 = 1;

    float dot = z1*z2;    //between [x1, y1, z1] and [x2, y2, z2] x2 and y2 are 0 so only z component matters
    float lenSq1 = z1*z1;
    float lenSq2 = x2*x2 + y2*y2 + z2*z2;
    float angle = acos(dot/sqrt(lenSq1 * lenSq2));
    return angle;
}


int main(){
    //Reading data from text file
    ifstream file("object.txt"); 
    string line;
    string str;

    getline(file, line);
    stringstream s(line);
    getline(s, str, ',');
    int vertices = stoi(str);
    getline(s, str);
    int faces = stoi(str);
    
    string val;

    //v stores coordinates represented through vec3 struct
    vector<vec3> v; 

    //storing vertices x,y,z coordinates from text file
    for(int i = 0; i < vertices; i++){
        float d;
        vec3 point;

        getline(file, line);
        stringstream s(line);
        getline(s, val, ','); //id of vertex

        getline(s, val, ','); //x value
        d = stod(val);
        point.x = d;
        getline(s, val, ','); //y value
        d = stod(val);
        point.y = d;
        getline(s, val, ','); //z value
        d = stod(val);
        point.z = d;

        v.push_back(point); //adding coordinates to vector v
    }

    

    vector<connection> f;
    for(int i = 0; i < faces; i++){
        
        connection con;
        int j;

        getline(file, line);
        stringstream s(line);

        getline(s, val, ','); //1st index of vertex in v
        j = stoi(val);
        con.a = j;
        getline(s, val, ','); //2nd index of vertex in v
        j = stoi(val);
        con.b = j;
        getline(s, val, ','); ///3rd index of vertex in v
        j = stoi(val);
        con.c = j;

        f.push_back(con); //adding vector of face ids to vector f
        
    }

    //converting from 3D to 2D
    vector<vec2> v2 = worldSpaceToScreenSpace(v);

    //shifting so that origin is at the center of the window
    shiftCoordFrame(v2);

    Screen screen;
    bool isRunning = true;
    
   

   int x,y;
   SDL_GetGlobalMouseState(&x,&y);
   

    
    
    while(isRunning){
        
        int x_t = x;
        int y_t = y;
        Uint32 i = SDL_GetGlobalMouseState(&x,&y);
        if(i == 1 && x != x_t){
            int dx = x - x_t;

            //angle to rotate in the X-direction by
            //3 is used such that a 360 degree rotation can be done by moving the mouse across half of the screen
            //i.e. 360/120 = 3
            int angle = 3 * dx;
            rotateX(v, angle);
        }
        if(i == 1 && y != y_t){
            int dy = y - y_t;

            //angle to rotate in the Y-direction by
            //3 is used such that a 360 degree rotation can be done by moving the mouse across half of the screen
            //i.e. 240/120 = 2
            int angle = 2 * dy;
            rotateY(v, angle);
        }

        //converting from 3D to 2D
        vector<vec2> v2 = worldSpaceToScreenSpace(v);
        //shifting so that origin is at the center of the window
        shiftCoordFrame(v2);
        
        for(auto& p : v2){  
            draw_circle(screen, p.u, p.v, 2);                     
            
        }
        vector<vector< SDL_Vertex> > verts;
        

        
        //drawing the edges connecting each group of vertices in vector<connections> to form each face
        for(auto& conn : f){
            drawLine(screen, v2[conn.a-1].u, v2[conn.a-1].v, v2[conn.b-1].u, v2[conn.b-1].v);
            drawLine(screen, v2[conn.b-1].u, v2[conn.b-1].v, v2[conn.c-1].u, v2[conn.c-1].v);
            drawLine(screen, v2[conn.c-1].u, v2[conn.c-1].v, v2[conn.a-1].u, v2[conn.a-1].v);
            
        }

        float ang = angBetweenNormalandZ(v);
        if(ang < 0){
            ang = -ang;
        }
        while(ang > 90){
            ang -= 90;
        }
        //B value varying color from (#0000ff {0,0,255}) to #00005f {0,0,95}), if angle made between normal of
        //surface and Z-axis is 90 degrees, then color will be #0000ff; if angle is 0 (i.e., orthogonal)
        //then color will be #00005f {0,0,95}
        Uint8 color = ang * (160/90) + 95;
        //cout << unsigned(color) << endl;
        
        for(auto& conn : f){
            vector<SDL_Vertex> ve;
            ve.push_back({ SDL_FPoint{ v2[conn.a-1].u, v2[conn.a-1].v }, SDL_Color{ 0, 0, color, 0 }, SDL_FPoint{ 0 }});
            ve.push_back({ SDL_FPoint{ v2[conn.b-1].u, v2[conn.b-1].v }, SDL_Color{ 0, 0, color, 0 }, SDL_FPoint{ 0 }});
            ve.push_back({ SDL_FPoint{ v2[conn.c-1].u, v2[conn.c-1].v }, SDL_Color{ 0, 0, color, 0 }, SDL_FPoint{ 0 }});
            verts.push_back(ve);
        }

        screen.show(verts);
        screen.clear();
        screen.input();
        SDL_Delay(30);
    }
    

    
    
    return 0;
}
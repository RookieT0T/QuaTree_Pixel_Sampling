#include <iostream>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <algorithm>
using namespace std;

// color
class RGB {
    public:
        uint8_t red;
        uint8_t green;
        uint8_t blue;
        RGB(uint8_t r, uint8_t g, uint8_t b) : red(r), green(g), blue(b){}

};

// supersampling
class SUPER{
    public:
        bool on;
        RGB color;
        SUPER(bool signal, RGB rgb) : on(signal), color(rgb){}
};

// pixel
class PIXEL{
    public:
        bool on;
        SUPER s0;
        SUPER s1;
        SUPER s2;
        SUPER s3;
        PIXEL(bool signal, SUPER sam0, SUPER sam1, SUPER sam2, SUPER sam3) 
        : on(signal), s0(sam0), s1(sam1), s2(sam2), s3(sam3){}
};

// * mag^2
int calArea(vector<vector<int>> tri){
    vector<int> A = tri[0];
    vector<int> B = tri[1];
    vector<int> C = tri[2];
    vector<int> AB = {B[0] - A[0], B[1] - A[1]};
    vector<int> AC = {C[0] - A[0], C[1] - A[1]};
    int triArea = abs(AB[0] * AC[1] - AC[0] * AB[1]);
    return triArea;
}   

int calDet(int endPoint0x, int endPoint0y, int endPoint1x, int endPoint1y, int x, int y){
    int result = (endPoint0x - x) * (endPoint1y - y) - (endPoint1x - x) * (endPoint0y - y); 
    return result;
}

bool intersect(vector<vector<int>> triEdge, vector<vector<int>> sqEdge){
    // triangle edge endpoints against square edge
    int det0 = calDet(triEdge[0][0], triEdge[0][1], triEdge[1][0], triEdge[1][1], sqEdge[0][0], sqEdge[0][1]);
    int det1 = calDet(triEdge[0][0], triEdge[0][1], triEdge[1][0], triEdge[1][1], sqEdge[1][0], sqEdge[1][1]);
    
    // square edge endpoints against triangle edge
    int det2 = calDet(sqEdge[0][0], sqEdge[0][1], sqEdge[1][0], sqEdge[1][1], triEdge[0][0], triEdge[0][1]);
    int det3 = calDet(sqEdge[0][0], sqEdge[0][1], sqEdge[1][0], sqEdge[1][1], triEdge[1][0], triEdge[1][1]);

    if(((det0 ^ det1) < 0) && ((det2 ^ det3) < 0)){
        return true;
    }
    return false;
}

// check the specified square overlapps with the triangle
bool overlap(vector<vector<int>> triOld, float rowP, float colP, float space){
    // scale the triangle by 2
    vector<vector<int>> tri(triOld);
    for(auto &e : tri){
        for(auto &n : e){
            n *= 2;
        }
    }
    // triangle area
    int triArea = calArea(tri);
    // triangle edges
    vector<vector<vector<int>>> triEdges = {{tri[0], tri[1]}, {tri[1], tri[2]}, {tri[2], tri[0]}};

    // square coordinates
    vector<int> topLeft = {int(colP * 2), int(rowP * 2)};
    vector<int> topRight = {int((colP + space) * 2), int(rowP * 2)};
    vector<int> botLeft = {int(colP * 2), int((rowP + space) * 2)};
    vector<int> botRight = {int((colP + space) * 2), int((rowP + space) * 2)};
    vector<vector<int>> square = {topLeft, topRight, botLeft, botRight};
    // square egdes
    vector<vector<int>> left = {botLeft, topLeft};
    vector<vector<int>> upper = {topLeft, topRight};
    vector<vector<int>> right = {topRight, botRight};
    vector<vector<int>> bottom = {botRight, botLeft};
    vector<vector<vector<int>>> sqEdges = {left, upper, right, bottom};

    // S0: check if square vertices are within the triangle
    for(auto i : square){
        int total = 0;
        for(int k = 0; k < tri.size(); ++k){
            vector<vector<int>> temp(tri);
            temp[k] = i;
            total += calArea(temp);
        }
        if(triArea == total){
            return true;
        }
    }

    // S1: check if triangle vertices are within the square
    for(auto i : tri){
        if(i[0] >= colP && i[0] <= colP + 1 && i[1] >= rowP && i[1] <= rowP + 1){
            return true;
        }
    }

    // S2: check if triangle edges intersect with square edges
    for(auto i : triEdges){
        for(auto k : sqEdges){
            if(intersect(i, k)){
                return true;
            }
        }
    }

    return false;
}

/*
void traverseRe(vector<vector<bool>> &pixels, int rowS, int rowE, int colS, int colE, vector<vector<int>> &tri){
    if(rowS > rowE || colS > colE){
        return;
    }
    // base case 1
    if(rowS == rowE && colS == colE){
        pixels[rowS][colS] = overlap(tri, rowS, rowE, colS, colE) || pixels[rowS][colS];
        return;
    }
    // base case 2
    if(overlap(tri, rowS, rowE, colS, colE) == false){
        return;
    }

    int rowMid = rowS + (rowE -  rowS) / 2;
    int colMid = colS + (colE - colS) / 2;
    traverseRe(pixels, rowS, rowMid, colS, colMid, tri);      // top left
    traverseRe(pixels, rowS, rowMid, colMid + 1, colE, tri);  // top right
    traverseRe(pixels, rowMid + 1, rowE, colS, colMid, tri);  // bottom left
    traverseRe(pixels, rowMid + 1, rowE, colMid + 1, colE, tri);  // bottom right
}
*/

void traverseTi(vector<vector<PIXEL>> &pixels, int rowS, int rowE, int colS, int colE, vector<vector<int>> &tri){
    for(int i = rowS; i < rowE; ++i){
        for(int k = colS; k < colE; ++k){
            if(overlap(tri, i, k, 1.0)){
                pixels[i][k].on = true;

                // check four super samplings
                // first super sampling block
                if(overlap(tri, i, k, 0.5)){
                    pixels[i][k].s0.on = true;
                }
                // second super sampling block
                if(overlap(tri, i, k + 0.5, 0.5)){
                    pixels[i][k].s1.on = true;
                }
                // third super sampling block
                if(overlap(tri, i + 0.5, k, 0.5)){
                    pixels[i][k].s2.on = true;
                }
                // forth super sampling block
                if(overlap(tri, i + 0.5, k + 0.5, 0.5)){
                    pixels[i][k].s3.on = true;
                }
            }
        }
    }
}

int main(){

    int times = 5;
    int len = 10 * times;
    int wid = 10 * times;
    vector<vector<PIXEL>> pixels(wid, vector<PIXEL>(len, PIXEL(false, SUPER(false, RGB(0, 0, 0)), SUPER(false, RGB(0, 0, 0)), SUPER(false, RGB(0, 0, 0)), SUPER(false, RGB(0, 0, 0)))));

    // triangle coordinates & attributes
    vector<vector<int>> tri0 = {{0 * times, 0 * times}, {8 * times, 0 * times}, {0 * times, 8 * times}};
    vector<vector<vector<int>>> tri = {tri0};
    RGB v1_color = {255, 0, 0};
    RGB v2_color = {0, 255, 0};
    RGB v3_color = {0, 0, 255};
    
    // call rasterization process
    for(auto t : tri){
        int boxRowS = min({t[0][1], t[1][1], t[2][1]});
        int boxRowE = max({t[0][1], t[1][1], t[2][1]});
        int boxColS = min({t[0][0], t[1][0], t[2][0]});
        int boxColE = max({t[0][0], t[1][0], t[2][0]});
        //traverseRe(pixels, 0, wid - 1, 0, len - 1, t);
        traverseTi(pixels, boxRowS, boxRowE, boxColS, boxColE, t);
    }
    
    // show all chosen pixles
    /*
    cout << "  ";
    for(int i = 0; i < len; ++i){
        cout << i << " ";
    }
    cout << endl;
    */
    
    // vertical axis & traverse
    //int temp = 0;
    for(auto i : pixels){
        //cout << temp << " ";
        //temp++;
        for(auto k : i){
            cout << k.on << " ";
        }
        cout << endl;
    }
    return 0;
}
#include <iostream>
#include <vector>
#include <cstdlib>
#include <cmath>
using namespace std;

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
bool overlap(vector<vector<int>> tri, int rowS, int rowE, int colS, int colE){
    // triangle area
    int triArea = calArea(tri);

    // triangle edges
     vector<vector<vector<int>>> triEdges = {{tri[0], tri[1]}, {tri[1], tri[2]}, {tri[2], tri[0]}};

    // square coordinates
    vector<int> topLeft = {colS, rowS};
    vector<int> topRight = {colE + 1, rowS};
    vector<int> botLeft = {colS, rowE + 1};
    vector<int> botRight = {colE + 1, rowE + 1};
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
        if(i[0] >= colS && i[0] <= colE + 1 && i[1] >= rowS && i[1] <= rowE + 1){
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

void traverse(vector<vector<bool>> &pixels, int rowS, int rowE, int colS, int colE, vector<vector<int>> &tri){
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
    traverse(pixels, rowS, rowMid, colS, colMid, tri);      // top left
    traverse(pixels, rowS, rowMid, colMid + 1, colE, tri);  // top right
    traverse(pixels, rowMid + 1, rowE, colS, colMid, tri);  // bottom left
    traverse(pixels, rowMid + 1, rowE, colMid + 1, colE, tri);  // bottom right
}

int main(){
    int times = 10;
    int len = 10 * times;
    int wid = 7 * times;
    vector<vector<bool>> pixels(wid, vector<bool>(len, false));

    // triangle coordinates
    vector<vector<int>> tri0 = {{3 * times, 2 * times}, {5 * times, 0 * times}, {5 * times, 4 * times}};
    vector<vector<int>> tri1 = {{5 * times, 4 * times}, {7 * times, 2 * times}, {9 * times, 4 * times}};
    vector<vector<int>> tri2 = {{5 * times, 4 * times}, {7 * times, 6 * times}, {5 * times, 7 * times}}; 
    vector<vector<int>> tri3 = {{1 * times, 4 * times}, {5 * times, 4 * times}, {3 * times, 6 * times}}; 
    vector<vector<vector<int>>> tri = {tri0, tri1, tri2, tri3};
    for(auto t : tri){
        traverse(pixels, 0, wid - 1, 0, len - 1, t);
    }
    
    // horizontal axis
    cout << "  ";
    for(int i = 0; i < len; ++i){
        cout << i << " ";
    }
    cout << endl;
    // vertical axis
    // traverse
    int temp = 0;
    for(auto i : pixels){
        cout << temp << " ";
        temp++;
        for(auto k : i){
            cout << k << " ";
        }
        cout << endl;
    }
    return 0;
}
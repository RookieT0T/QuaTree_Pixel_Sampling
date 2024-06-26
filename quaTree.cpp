#include <iostream>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <tuple>
#include <algorithm>
#include <iomanip>
#include <map>
using namespace std;

// color
class RGB {
    public:
        float red;
        float green;
        float blue;
        RGB(float r, float g, float b) : red(r), green(g), blue(b){}
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

// triangle vertex & its color attributes
class TRIVER{
    public:
        int x;
        int y;
        RGB color;
        TRIVER(int xcoord, int ycoord, RGB col)
        : x(xcoord), y(ycoord), color(col){}
};

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
bool overlap(vector<TRIVER> triOld, float rowP, float colP, float space){
    vector<vector<int>> tri = {{triOld[0].x, triOld[0].y}, {triOld[1].x, triOld[1].y}, {triOld[2].x, triOld[2].y}};
    // scale the triangle by 2
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

tuple<float, float, float> interpolate(vector<TRIVER> &triOld, float rowP, float colP, map<tuple<int, int, int>, tuple<float, float, float>> &cache){
    vector<vector<int>> tri = {{triOld[0].x, triOld[0].y}, {triOld[1].x, triOld[1].y}, {triOld[2].x, triOld[2].y}};
    // scale the triangle by 2
    for(auto &e : tri){
        for(auto &n : e){
            n *= 2;
        }
    }
    // triangle area
    float triArea = (float)calArea(tri);

    // sub-triangle PBC
    int newRowP = int(rowP * 2);
    int newColP = int(colP * 2);
    vector<int> temp0 = tri[0];
    tri[0] = {newColP, newRowP};
    float areaPBC = (float)calArea(tri);
    tri[0] = temp0;

    // sub-triangle PAC
    vector<int> temp1 = tri[1];
    tri[1] = {newColP, newRowP};
    float areaPAC = (float)calArea(tri);
    tri[1] = temp1;

    // sub-triangle PAB
    vector<int> temp2 = tri[2];
    tri[2] = {newColP, newRowP};
    float areaPAB = (float)calArea(tri);
    tri[2] = temp2;

    // triOld[0].color -> color of vertex A
    // triOld[1].color -> color of vertex B
    // triOld[2].color -> color of vertex C
    int threshold1 = 100;
    int threshold2 = 100;
    float cPBC = areaPBC / triArea;
    int key1 = round(cPBC * threshold1);
    float cPAC = areaPAC / triArea;
    int key2 = round(cPAC * threshold1);
    float cPAB = areaPAB / triArea;
    int key3 = round(cPAB * threshold1);

    auto key = make_tuple(key1, key2, key3);
    //cache.find(key) != cache.end()
    if(false){
        return cache[key];
    }
    else{
        float red, green, blue = 0;
        red = cPBC * triOld[0].color.red + cPAC * triOld[1].color.red + cPAB * triOld[2].color.red;
        red = round(red * threshold2) / threshold2; // precision - two digits behind the decimal point

        green = cPBC * triOld[0].color.green + cPAC * triOld[1].color.green + cPAB * triOld[2].color.green;
        green = round(green * threshold2) / threshold2;

        blue = cPBC * triOld[0].color.blue + cPAC * triOld[1].color.blue + cPAB * triOld[2].color.blue;
        blue = round(blue * threshold2) / threshold2;
        
        auto ans = make_tuple(red, green, blue);
        cache[key] = ans;
        return ans;
    }
}

void traverseTi(vector<vector<PIXEL>> &pixels, int rowS, int rowE, int colS, int colE, vector<TRIVER> &tri){
    // hashmap
    map<tuple<int, int, int>, tuple<float, float, float>> approximate_cache;
    for(int i = rowS; i < rowE; ++i){
        for(int k = colS; k < colE; ++k){
            if(overlap(tri, i, k, 1.0)){
                pixels[i][k].on = true;

                // check four super samplings and update their color if they should intersect the triangle
                // first super sampling block
                if(overlap(tri, i, k, 0.5)){
                    pixels[i][k].s0.on = true;
                    auto ans0 = interpolate(tri, i, k, approximate_cache);
                    pixels[i][k].s0.color.red = get<0>(ans0);
                    pixels[i][k].s0.color.green = get<1>(ans0);
                    pixels[i][k].s0.color.blue = get<2>(ans0);
                }
                // second super sampling block
                if(overlap(tri, i, k + 0.5, 0.5)){
                    pixels[i][k].s1.on = true;
                    auto ans1 = interpolate(tri, i, k + 0.5, approximate_cache);
                    pixels[i][k].s1.color.red = get<0>(ans1);
                    pixels[i][k].s1.color.green = get<1>(ans1);
                    pixels[i][k].s1.color.blue = get<2>(ans1);
                }
                // third super sampling block
                if(overlap(tri, i + 0.5, k, 0.5)){
                    pixels[i][k].s2.on = true;
                    auto ans2 = interpolate(tri, i + 0.5, k, approximate_cache);
                    pixels[i][k].s2.color.red = get<0>(ans2);
                    pixels[i][k].s2.color.green = get<1>(ans2);
                    pixels[i][k].s2.color.blue = get<2>(ans2);
                }
                // forth super sampling block
                if(overlap(tri, i + 0.5, k + 0.5, 0.5)){
                    pixels[i][k].s3.on = true;
                    auto ans3 = interpolate(tri, i + 0.5, k + 0.5, approximate_cache);
                    pixels[i][k].s3.color.red = get<0>(ans3);
                    pixels[i][k].s3.color.green = get<1>(ans3);
                    pixels[i][k].s3.color.blue = get<2>(ans3);
                }
            }
        }
    }
}

int main(){
    int times = 100;
    int len = 10 * times;
    int wid = 10 * times;
    vector<vector<PIXEL>> pixels(wid, vector<PIXEL>(len, PIXEL(false, SUPER(false, RGB(0, 0, 0)), SUPER(false, RGB(0, 0, 0)), SUPER(false, RGB(0, 0, 0)), SUPER(false, RGB(0, 0, 0)))));

    // triangle coordinates & attributes
    vector<TRIVER> tri = {TRIVER(0 * times, 0 * times, RGB(1, 0, 0)), TRIVER(8 * times, 0 * times, RGB(0, 1, 0)), TRIVER(0 * times, 8 * times, RGB(0, 0, 1))};
    
    // use bounding box and then call rasterization + interpolation process
    int boxRowS = min({tri[0].y, tri[1].y, tri[2].y});
    int boxRowE = max({tri[0].y, tri[1].y, tri[2].y});
    int boxColS = min({tri[0].x, tri[1].x, tri[2].x});
    int boxColE = max({tri[0].x, tri[1].x, tri[2].x});
    traverseTi(pixels, boxRowS, boxRowE, boxColS, boxColE, tri);

    // need code to visualize each sampling point of each pixel in vector named pixels
    for(int i = 0; i < pixels.size(); ++i){
        for(int times = 0; times < 2; ++times){
            for (int j = 0; j < pixels[i].size(); ++j){
                if(times == 0){
                    cout << setprecision(2) << "(" << pixels[i][j].s0.color.red << ", " << pixels[i][j].s0.color.green << ", " << pixels[i][j].s0.color.blue << ")  ";
                    cout << "(" << pixels[i][j].s1.color.red << ", " << pixels[i][j].s1.color.green << ", " << pixels[i][j].s1.color.blue << ")  ";
                }
                else{
                    cout << "(" << pixels[i][j].s2.color.red << ", " << pixels[i][j].s2.color.green << ", " << pixels[i][j].s2.color.blue << ")  ";
                    cout << "(" << pixels[i][j].s3.color.red << ", " << pixels[i][j].s3.color.green << ", " << pixels[i][j].s3.color.blue << ")  "; 
                }
            }
            cout << endl;
        }
    }
    return 0;
}
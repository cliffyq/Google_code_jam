#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cctype>
#include <algorithm>
#include <cmath>
#include <cassert>
#include <vector>
#include <algorithm>

using namespace std;

int double_comp(const double &a, const double &b);
struct position {
    double x;
    double y;
    bool operator==(const position& p) const {
        return (double_comp(p.x,x)==0 && double_comp(p.y,y)==0 );
    }
};
enum direction { LEFT, RIGHT, UP, DOWN };
enum reflection_type { HORIZONAL, VERTICAL, OPPOSITE, STOP, THROUGH };
void read_line( string& s, int &H, int &W, int &D);
void read_line( string& s, bool * row, int n, int row_y);
bool notspace(char c);
bool space(char c);
int reflect_once(bool** has_mirror, const  int& W,const  int& H);
enum reflection_type check_reflection_type(bool** has_mirror, int x, int y, enum direction x_direction ,enum direction y_direction );
bool x_in_range(double x,int W);
bool y_in_range(double y,int H);
double get_x(double y);
double get_y(double x);
void mirror_image(enum reflection_type rt);
vector<struct position> get_corner_ponits();
vector<struct position>::const_iterator find_by_x(const vector<struct position>& c,int xval);
vector<struct position>::const_iterator find_by_y(const vector<struct position>& c,int yval);
bool collinear(const struct position& p,const struct position& a,const struct position& b);
int gcd(int a, int b);
const char* infile = "D-large-practice.in";
const double EPS =0.000000001;
static int IMAGE_COUNT;
static struct position IMAGE;
static struct position PAIRED_IMAGE;
static struct position ORIGIN;
static struct position LAST_INTERSEC;
bool print_image = false;
int main()
{
    FILE *fp = fopen("results.txt","w");
    int T,H,W,D;
    ifstream fin(infile);
    string s;
    getline(fin,s);
    T = atoi(s.c_str());
    for(int t = 0; t < T; ++t) {
        IMAGE_COUNT = 0;
        getline(fin,s);
        read_line(s,H,W,D);
        bool **has_mirror = new bool*[H];
        bool **counted = new bool*[2*D+1];
        for(int i = 0; i< 2*D+1 ; ++i) {
            counted[i] = new bool[2*D+1];
            for(int j =0; j< 2*D+1 ; ++j)
                counted[i][j] = false;
        }
        for(int y = 0; y<H ; ++y ) {
            has_mirror[y] = new bool[W];
            getline(fin,s);
            read_line(s,has_mirror[y],W,y);
        }
        for (int x = 0; x<=2*D ; ++x ) {
            for (int y = 0; y <= 2*D  ; ++y ) {
                if(x==D && y==D)
                    continue;
                if(counted[x][y]) {
                    IMAGE_COUNT++;
                    continue;
                }
                if((x-D)*(x-D)+(y-D)*(y-D)>D*D) continue;
                IMAGE.x = ORIGIN.x+x-D;
                IMAGE.y = ORIGIN.y+y-D;
                PAIRED_IMAGE = ORIGIN;
                LAST_INTERSEC = ORIGIN;
                while(1) {
                    int flag = reflect_once(has_mirror,W,H);
                    if(flag==2) {
                        IMAGE_COUNT++;
                        int paired_x = PAIRED_IMAGE.x - ORIGIN.x + D ;
                        int paired_y = PAIRED_IMAGE.y - ORIGIN.y + D ;
                        counted[x][y] = true;
                        counted[paired_x][paired_y] = true;
                        break;
                    } else if(flag==1) {
                        continue;
                    } else if(flag==0) break;
                }
            }
        }
        printf("Case #%d: %d\n",t+1,IMAGE_COUNT);
        fprintf(fp,"Case #%d: %d\n",t+1,IMAGE_COUNT);
        if(print_image) {
            for(int y = min(ORIGIN.y-D+0.5,0.0); y<=max(H-1.0,D+ORIGIN.y+0.5); ++y) {
                for(int x=min(ORIGIN.x-D+0.5,0.0); x<=max(W-1.0,D+ORIGIN.x+0.5); ++x) {
                    int c_x = D+x-ORIGIN.x-0.5;
                    int c_y = D+y-ORIGIN.y-0.5;
                    int c=0,h=0;
                    if(c_x>=0&&c_x<=2*D&&c_y>=0&&c_y<=2*D)
                        c = (int)counted[c_x][c_y];
                    if(x>=0&&x<W&&y>=0&&y<H)
                        h = 2*(int)has_mirror[y][x];
                    if(double_comp(x-0.5,ORIGIN.x)==0&&double_comp(y-0.5,ORIGIN.y)==0) {
                        fprintf(fp,"X");
                    } else {
                        if(c+h==0) {
                            fprintf(fp,".");
                        }
                        if(c+h==1) {
                            fprintf(fp,"x");
                        }

                        if(c+h==2) {
                            fprintf(fp,"#");
                        }

                        if(c+h==3) {
                            fprintf(fp,"*");
                        }
                    }
                }
                fprintf(fp,"\n");
            }
        }
        for(int y = 0; y<H ; ++y ) {
            delete[] has_mirror[y];
        }
        delete[] has_mirror;
        for(int i = 0; i< 2*D+1 ; ++i) {
            delete[] counted[i];
        }
        delete[] counted;
    }
    fclose(fp);
    return 0;
}

bool notspace(char c)
{
    return !isspace(c);
}
bool space(char c)
{
    return isspace(c);
}
void read_line( string& s, int &H, int &W, int &D)
{
    string::iterator l,r;
    l=s.begin();
    int i = 0;
    while(i < 3) {
        l = find_if(l,s.end(),notspace);
        r = find_if(l,s.end(),space);
        if(i == 0) {
            H = atoi(string(l,r).c_str());
        } else if(i == 1) {
            W = atoi(string(l,r).c_str());
        } else if(i == 2) {
            D = atoi(string(l,r).c_str());
        }
        l=r;
        i++;
    }
}
void read_line( string& s, bool * row, int n,int row_y)
{
    for(int x = 0; x<n; ++x) {
        if(s[x]=='#') {
            row[x]=true;
        } else if(s[x]=='.') {
            row[x]=false;
        } else if(s[x]=='X') {
            row[x]=false;
            ORIGIN.x = x-0.5;
            ORIGIN.y = row_y-0.5;
        }
    }
}
int reflect_once(bool** has_mirror,const int &W, const int &H)
{
    //0-stop 1-go on 2-found
    enum direction x_direction;
    enum direction y_direction;
    x_direction=IMAGE.x<PAIRED_IMAGE.x?LEFT:RIGHT;
    y_direction=IMAGE.y<PAIRED_IMAGE.y?UP:DOWN;
    double next_x=IMAGE.x,next_y=IMAGE.y;
    enum reflection_type rt=THROUGH;
    int temp_x = x_direction == LEFT ? floor(LAST_INTERSEC.x):ceil(LAST_INTERSEC.x);
    vector<struct position> corners = get_corner_ponits();
    for(; temp_x>=0 && temp_x<=W-2; temp_x= x_direction==LEFT?temp_x-1:temp_x+1) {
        if((x_direction==LEFT && temp_x<=next_x)||(x_direction==RIGHT && temp_x>=next_x))
            break;
        if(double_comp(temp_x,LAST_INTERSEC.x)!=0) {
            vector<struct position>::const_iterator it = find_by_x(corners,temp_x);
            if(it!=corners.end()) {
                if(it->y<0||it->y>H-2)
                    break;
                rt = check_reflection_type(has_mirror,temp_x,it->y,x_direction,y_direction);
                if(rt==STOP) {
                    next_y = it->y;
                    break;
                } else if(rt!=THROUGH) {
                    next_x = temp_x;
                    next_y = it->y;
                    break;
                } else {
                    continue;
                }
            }
            double temp_y = get_y(temp_x);
            int y_idx = ceil(temp_y);
            int x_idx = x_direction == LEFT ?temp_x:temp_x+1;
            if(x_idx<0||x_idx>=W||y_idx<0||y_idx>=H) break;
            if(has_mirror[y_idx][x_idx]) {
                rt=HORIZONAL;
                next_x = temp_x;
                next_y = temp_y;
                break;
            }
        }
    }
    int temp_y = y_direction == UP?floor(LAST_INTERSEC.y):ceil(LAST_INTERSEC.y);
    for(; temp_y>=0 && temp_y<=H-2; temp_y= y_direction==UP?temp_y-1:temp_y+1) {
        if((y_direction==UP && temp_y<=next_y)||(y_direction==DOWN && temp_y>=next_y))
            break;
        if(double_comp(temp_y,LAST_INTERSEC.y)!=0) {
            vector<struct position>::const_iterator it = find_by_y(corners,temp_y);
            if(it!=corners.end()) continue;
            double temp_x = get_x(temp_y);
            int y_idx = y_direction == UP?temp_y:temp_y+1;
            int x_idx =ceil(temp_x);
            if(x_idx<0||x_idx>=W||y_idx<0||y_idx>=H) break;
            if(has_mirror[y_idx][x_idx]) {
                rt=VERTICAL;
                next_x = temp_x;
                next_y = temp_y;
                break;
            }
        }
    }

    if(rt==STOP) {
        return 0;
    }
    if(rt==THROUGH) {
        return IMAGE==ORIGIN?2:0;
    }
    if(rt==VERTICAL&&fabs(next_y-ORIGIN.y)>fabs(IMAGE.y-next_y)) {
        return 0;
    }
    if(rt==HORIZONAL && fabs(next_x-ORIGIN.x)>fabs(IMAGE.x-next_x)) {
        return 0;
    }
    if(rt==OPPOSITE&&(fabs(next_y-ORIGIN.y)>fabs(IMAGE.y-next_y)||fabs(next_x-ORIGIN.x)>fabs(IMAGE.x-next_x))) {
        return 0;
    }
    struct position next = {next_x,next_y};
    if(collinear(ORIGIN,LAST_INTERSEC,next)&&collinear(ORIGIN,PAIRED_IMAGE,IMAGE)) return 0;
    LAST_INTERSEC = next;
    mirror_image(rt);
    return 1;
}
inline int double_comp(const double &a, const double &b)
{
    if(fabs(a-b)<EPS) return 0;//==
    if(a>b) return 1;//>
    return -1;//<
}
inline double get_x(double y)
{
    return (IMAGE.x-PAIRED_IMAGE.x)*(y-PAIRED_IMAGE.y)/(IMAGE.y-PAIRED_IMAGE.y)+PAIRED_IMAGE.x;
}
inline double get_y(double x)
{
    return PAIRED_IMAGE.y+(x-PAIRED_IMAGE.x)*(IMAGE.y-PAIRED_IMAGE.y)/(IMAGE.x-PAIRED_IMAGE.x);
}
enum reflection_type check_reflection_type(bool** has_mirror, int x, int y, enum direction x_direction ,enum direction y_direction )
{
    enum reflection_type rt;
    bool i = has_mirror[y][x+1];
    bool ii = has_mirror[y][x];
    bool iii = has_mirror[y+1][x];
    bool iv = has_mirror[y+1][x+1];
    if(x_direction==LEFT&&y_direction==UP) {
        if(!ii) rt=THROUGH;
        else if(!iii&&i) rt = VERTICAL;
        else if(!i&&iii) rt = HORIZONAL;
        else if(!i&&!iii) rt = STOP;
        else rt = OPPOSITE;
    }
    if(x_direction==RIGHT&&y_direction==UP) {
        if(!i) rt=THROUGH;
        else if(!iv&&ii) rt = VERTICAL;
        else if(!ii&&iv) rt = HORIZONAL;
        else if(!ii&&!iv) rt = STOP;
        else rt = OPPOSITE;
    }
    if(x_direction==LEFT&&y_direction==DOWN) {
        if(!iii) rt=THROUGH;
        else if(!ii&&iv) rt = VERTICAL;
        else if(!iv&&ii) rt = HORIZONAL;
        else if(!ii&&!iv) rt = STOP;
        else rt = OPPOSITE;
    }
    if(x_direction==RIGHT&&y_direction==DOWN) {
        if(!iv) rt=THROUGH;
        else if(!i&&iii) rt = VERTICAL;
        else if(!iii&&i) rt = HORIZONAL;
        else if(!i&&!iii) rt = STOP;
        else rt = OPPOSITE;
    }
    return rt;
}
void mirror_image(enum reflection_type rt)
{
    if(rt==VERTICAL) {
        IMAGE.y = 2*LAST_INTERSEC.y-IMAGE.y;
        PAIRED_IMAGE.y = 2*LAST_INTERSEC.y-PAIRED_IMAGE.y;
    }
    if(rt==HORIZONAL) {
        IMAGE.x = 2*LAST_INTERSEC.x-IMAGE.x;
        PAIRED_IMAGE.x = 2*LAST_INTERSEC.x-PAIRED_IMAGE.x;
    }
    if(rt==OPPOSITE) {
        IMAGE.x = 2*LAST_INTERSEC.x-IMAGE.x;
        IMAGE.y = 2*LAST_INTERSEC.y-IMAGE.y;
        PAIRED_IMAGE.x = 2*LAST_INTERSEC.x-PAIRED_IMAGE.x;
        PAIRED_IMAGE.y = 2*LAST_INTERSEC.y-PAIRED_IMAGE.y;
    }
}
bool collinear(const struct position& p,const struct position& a,const struct position& b)
{
    //return true if p is on (a,b)
    int y_diff1 = double_comp(p.y,a.y), y_diff2 = double_comp(p.y,b.y);
    if(y_diff1==0 || y_diff2==0) {
        if(y_diff1==0 && y_diff2==0)
            return (a.x-p.x)*(p.x-b.x)>0?true:false;
        else
            return false;
    }
    if(double_comp((p.x-b.x)*(a.y-b.y),(p.y-b.y)*(a.x-b.x))==0) {
        return (a.y-p.y)*(p.y-b.y)>0?true:false;
    }
    return false;
}
int gcd(int a, int b)
{
    if(b==0) return a;
    int c = a % b;
    while(c != 0) {
        a = b;
        b = c;
        c = a % b;
    }
    return b;
}
vector<struct position>::const_iterator find_by_x(const vector<struct position>& c,int xval)
{
    vector<struct position>::const_iterator it=c.begin();
    for(; it!=c.end(); ++it)
        if(it->x==xval) break;
    return it;
}
vector<struct position>::const_iterator find_by_y(const vector<struct position>& c,int yval)
{
    vector<struct position>::const_iterator it=c.begin();
    for(; it!=c.end(); ++it)
        if(it->y==yval) break;
    return it;
}
vector<struct position> get_corner_ponits()
{
    vector<struct position> corners;
    int diff_x = IMAGE.x-PAIRED_IMAGE.x;
    int diff_y = IMAGE.y-PAIRED_IMAGE.y;
    if(diff_x!=0&&diff_y!=0) {
        int gcdxy = gcd(abs(diff_x),abs(diff_y));
        int step_x = diff_x/gcdxy, step_y = diff_y/gcdxy;
        if(step_x%2!=0 && step_y%2!=0 ) {
            for (int i = 0; (i+0.5)*abs(step_x)<abs(diff_x) ; ++i) {
                struct position p= {PAIRED_IMAGE.x+(i+0.5)*step_x,PAIRED_IMAGE.y+(i+0.5)*step_y};
                corners.push_back(p);
            }
        }
    }
    return corners;
}

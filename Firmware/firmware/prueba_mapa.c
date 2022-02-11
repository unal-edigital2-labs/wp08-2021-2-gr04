#include <math.h>
#define _USE_MATH_DEFINES
using namespace std;
int main() {
  //def matriz(direccion, fin, take)
    //take dice cuándo se agrega un punto a la matriz
    //Dir puede ser adelante, izquierda, derecha
    //dir = 3 ----> take = 1
    int ang = 90;
    int angulos [20][2];
    int coor[20][2]={{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}};
    int dir = 2;
    int take = 0;
    
    int inde = 0;
    int fin = 0;
    int angu = 0;
    
    while (inde!=20){
      if(fin==0){
      while(take==1){
      //Derecha
      if(dir == 1){
        ang = -90;
      }
      //Izquierda
      if(dir == 2){
        ang = 90;
      }
      //Avanza
      if(dir==0){
        ang = 0;
      }
      angulos[inde][1]=ang;
      angulos[inde][0]=1;
      inde++;
    }}
      angulos[inde][1]=ang;
      angulos[inde][0]=1;
      inde++;
      }

  
  int minx = 0;
  int maxx = 0;
  int miny = 0;
  int maxy = 0;
for (int i = 1; i <= 19; i++){
      angu = angu + angulos[i][1];
      coor[i][0] = coor[i-1][0] + angulos[i][0]cos(angu M_PI / 180);
      coor[i][1] = coor[i-1][1] + angulos[i][0]sin(angu M_PI / 180);
    cout<<coor[i][0]<<coor[i][1]<<'\n';

    if(coor[i][0]>maxx){
      maxx=coor[i][0];
    }
    if(coor[i][0]<minx){
      minx=coor[i][0];
    }
    if(coor[i][1]>maxy){
      maxy=coor[i][1];
    }
    if(coor[i][1]<miny){
      miny=coor[i][1];
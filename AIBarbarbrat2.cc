#include "Player.hh"
#include <bits/stdc++.h>


/**
* Write the name of your player and save this file
* with the same name and .cc extension.
*/
#define PLAYER_NAME Barbarbrat2


struct PLAYER_NAME : public Player {

/**
* Factory: returns a new instance of this class.
* Do not modify this function.
*/
static Player* factory () {
return new PLAYER_NAME;
}

/**
* Types and attributes for your player can be defined here.
*/

typedef vector<int> VI;
typedef vector<VI>  VVI;

vector<vector<Cell> > mapa;
vector<pair<int, int> > ent;
vector<pair<int, int> > orc;

int dirx[4]={1, 0, -1, 0};
int diry[4]={0, 1, 0, -1};

void pinta_mapa() {
    mapa=vector<vector<Cell> > (rows(), vector<Cell> (cols()));
    for (int i=0; i<rows(); ++i)
        for (int j=0; j<cols(); ++j)
            mapa[i][j]=cell(i, j);
}

/**
* Play method, invoked once per each round.
*/
virtual void play () {
    /*
    if (round()==0) {
        pinta_mapa();
    }
    */
    
    ent=vector<pair<int, int> > ();
    orc=vector<pair<int, int> > ();
    
    for (int i=0; i<rows(); ++i)
        for (int j=0; j<cols(); ++j) {
            Cell c=cell(i, j);
            if (c.unit_id != -1 and unit(c.unit_id).player != me()) orc.push_back(make_pair(i, j));
            if (c.unit_id != -1 and unit(c.unit_id).player == me()) ent.push_back(make_pair(i, j));
        }
    
    for (int i=0; i<int(ent.size()); ++i) {
        int vida=unit(cell(ent[i].first, ent[i].second).unit_id).health;
        int posx=ent[i].first;
        int posy=ent[i].second;
        vector<vector<pair<pair<int, int>, pair<int, int> > > > dist1(rows(), vector<pair<pair<int, int>, pair<int, int> > > (cols(), make_pair(make_pair(-1,-1), make_pair(-1, -1))));
        priority_queue<pair<int, pair<pair<int, int>, pair<int, int> > > > q;
        pair<int, int> ciutat=make_pair(-1, -1);
        pair<int, int> cami=make_pair(-1, -1);
        pair<int, int> presa=make_pair(-1, -1);
        pair<int, int> depredador=make_pair(-1, -1);
        q.push(make_pair(0, make_pair(make_pair(posx, posy), make_pair(posx, posy))));
        bool seguir=true;
        while(!q.empty() and seguir) {
            int dist=-q.top().first;
            pair<int, int> to=q.top().second.first;
            pair<int, int> from=q.top().second.second;
            q.pop();
            if (dist1[to.first][to.second].first.first==-1) {
                dist1[to.first][to.second].first.first=dist;
                dist1[to.first][to.second].first.second=dist1[from.first][from.second].first.second+1;
                dist1[to.first][to.second].second=from;
                Cell c=cell(to.first, to.second);
                if (c.type==CITY and ciutat.first==-1 and city_owner(c.city_id)!=me()) ciutat=to;
                else if (c.type==PATH and cami.first==-1 and city_owner(c.path_id)!=me()) cami=to;
                if (c.unit_id != -1 and unit(c.unit_id).player != me()) {
                    if (presa.first==-1 and unit(c.unit_id).health<vida-dist)
                        presa=to;
                }
                for (int k=0; k<4; ++k) {
                    if (to.first+dirx[k]>=0 and to.first+dirx[k]<rows() and to.second+diry[k]>=0 and to.second+diry[k]<cols() and dist1[to.first+dirx[k]][to.second+diry[k]].first.first==-1 and cell(to.first+dirx[k], to.second+diry[k]).type!=WATER) {
                        q.push(make_pair(-(dist+cost(cell(to.first+dirx[k], to.second+diry[k]).type)), make_pair(make_pair(to.first+dirx[k], to.second+diry[k]), to)));
                    }
                }
            }
            if (ciutat.first!=-1 and cami.first!=-1 and (presa.first!=-1 or dist1[to.first][to.second].first.second>10)) seguir=false;
        }
        while (!q.empty()) q.pop();
        
        vector<vector<int> > dist2(rows(), vector<int> (cols(), -1));
        priority_queue<pair<int, pair<int, int> > > q2;
        q2.push(make_pair(0, make_pair(posx, posy)));
        while (!q2.empty()) {
            int dist=-q2.top().first;
            pair<int, int> to=q2.top().second;
            q2.pop();
            if (dist2[to.first][to.second]==-1) {
                dist2[to.first][to.second]=dist;
                Cell c=cell(to.first, to.second);
                if (c.unit_id != -1 and unit(c.unit_id).player != me()) {
                    if (depredador.first==-1 and unit(c.unit_id).health>=vida+dist)
                        depredador=to;
                }
                for (int k=0; k<4; ++k) {
                    if (abs(to.first+dirx[k]-posx)+abs(to.second+diry[k]-posy)<=3 and to.first+dirx[k]>=0 and to.first+dirx[k]<rows() and to.second+diry[k]>=0 and to.second+diry[k]<cols() and dist2[to.first+dirx[k]][to.second+diry[k]]==-1 and cell(to.first+dirx[k], to.second+diry[k]).type!=WATER) {
                        q2.push(make_pair(-(dist+cost(cell(to.first+dirx[k], to.second+diry[k]).type)), make_pair(to.first+dirx[k], to.second+diry[k])));
                    }
                }
            }
        }
        
        if (depredador.first!=-1) {
            if (posx>depredador.first and cell(posx+1, posy).type!=WATER and (cell(posx+1, posy).unit_id == -1 or unit(cell(posx+1, posy).unit_id).player != me()))
                execute(Command(cell(posx, posy).unit_id, BOTTOM));
            else if (posx<depredador.first and cell(posx-1, posy).type!=WATER and (cell(posx-1, posy).unit_id == -1 or unit(cell(posx-1, posy).unit_id).player != me()))
                execute(Command(cell(posx, posy).unit_id, TOP));
            else if (posy>depredador.second and cell(posx, posy+1).type!=WATER and (cell(posx, posy+1).unit_id == -1 or unit(cell(posx, posy+1).unit_id).player != me()))
                execute(Command(cell(posx, posy).unit_id, RIGHT));
            else if (posy<depredador.second and cell(posx, posy-1).type!=WATER and (cell(posx, posy-1).unit_id == -1 or unit(cell(posx, posy-1).unit_id).player != me()))
                execute(Command(cell(posx, posy).unit_id, LEFT));
            else {
                if (posx>=depredador.first and cell(posx+1, posy).type!=WATER and (cell(posx+1, posy).unit_id == -1 or unit(cell(posx+1, posy).unit_id).player != me()))
                    execute(Command(cell(posx, posy).unit_id, BOTTOM));
                else if (posx<=depredador.first and cell(posx-1, posy).type!=WATER and (cell(posx-1, posy).unit_id == -1 or unit(cell(posx-1, posy).unit_id).player != me()))
                    execute(Command(cell(posx, posy).unit_id, TOP));
                else if (posy>=depredador.second and cell(posx, posy+1).type!=WATER and (cell(posx, posy+1).unit_id == -1 or unit(cell(posx, posy+1).unit_id).player != me()))
                    execute(Command(cell(posx, posy).unit_id, RIGHT));
                else if (posy<=depredador.second and cell(posx, posy-1).type!=WATER and (cell(posx, posy-1).unit_id == -1 or unit(cell(posx, posy-1).unit_id).player != me()))
                    execute(Command(cell(posx, posy).unit_id, LEFT));
            }
        }
        else if (presa.first!=-1) {
            if (posx<presa.first and cell(posx+1, posy).type!=WATER and (cell(posx+1, posy).unit_id == -1 or unit(cell(posx+1, posy).unit_id).player != me()))
                execute(Command(cell(posx, posy).unit_id, BOTTOM));
            else if (posx>presa.first and cell(posx-1, posy).type!=WATER and (cell(posx-1, posy).unit_id == -1 or unit(cell(posx-1, posy).unit_id).player != me()))
                execute(Command(cell(posx, posy).unit_id, TOP));
            else if (posy<presa.second and cell(posx, posy+1).type!=WATER and (cell(posx, posy+1).unit_id == -1 or unit(cell(posx, posy+1).unit_id).player != me()))
                execute(Command(cell(posx, posy).unit_id, RIGHT));
            else if (posy>presa.second and cell(posx, posy-1).type!=WATER and (cell(posx, posy-1).unit_id == -1 or unit(cell(posx, posy-1).unit_id).player != me()))
                execute(Command(cell(posx, posy).unit_id, LEFT));
            else {
                if (posx<=presa.first and cell(posx+1, posy).type!=WATER and (cell(posx+1, posy).unit_id == -1 or unit(cell(posx+1, posy).unit_id).player != me()))
                    execute(Command(cell(posx, posy).unit_id, BOTTOM));
                else if (posx>=presa.first and cell(posx-1, posy).type!=WATER and (cell(posx-1, posy).unit_id == -1 or unit(cell(posx-1, posy).unit_id).player != me()))
                    execute(Command(cell(posx, posy).unit_id, TOP));
                else if (posy<=presa.second and cell(posx, posy+1).type!=WATER and (cell(posx, posy+1).unit_id == -1 or unit(cell(posx, posy+1).unit_id).player != me()))
                    execute(Command(cell(posx, posy).unit_id, RIGHT));
                else if (posy>=presa.second and cell(posx, posy-1).type!=WATER and (cell(posx, posy-1).unit_id == -1 or unit(cell(posx, posy-1).unit_id).player != me()))
                    execute(Command(cell(posx, posy).unit_id, LEFT));
            }
        }
        else if (dist1[ciutat.first][ciutat.second].first.first<=dist1[cami.first][cami.second].first.first) {
            pair<int, int> posicio=ciutat;
            pair<int, int> ant=ciutat;
            while (dist1[posicio.first][posicio.second].second!=posicio) {
                ant=posicio;
                posicio=dist1[posicio.first][posicio.second].second;
            }
            if (posx<ant.first)
                execute(Command(cell(posx, posy).unit_id, BOTTOM));
            else if (posx>ant.first)
                execute(Command(cell(posx, posy).unit_id, TOP));
            else if (posy<ant.second)
                execute(Command(cell(posx, posy).unit_id, RIGHT));
            else if (posy>ant.second)
                execute(Command(cell(posx, posy).unit_id, LEFT));
        }
        else {
            pair<int, int> posicio=cami;
            pair<int, int> ant=cami;
            while (dist1[posicio.first][posicio.second].second!=posicio) {
                ant=posicio;
                posicio=dist1[posicio.first][posicio.second].second;
            }
            if (posx<ant.first)
                execute(Command(cell(posx, posy).unit_id, BOTTOM));
            else if (posx>ant.first)
                execute(Command(cell(posx, posy).unit_id, TOP));
            else if (posy<ant.second)
                execute(Command(cell(posx, posy).unit_id, RIGHT));
            else if (posy>ant.second)
                execute(Command(cell(posx, posy).unit_id, LEFT));
        }
    }
    /*
    if (round() == 1) {
        cerr << status(me()) << endl;
        int x;
        cin>>x;
    }
    */
    
}

};


/**
* Do not modify the following line.
*/
RegisterPlayer(PLAYER_NAME);

#include "Player.hh"
#include <bits/stdc++.h>


/**
* Write the name of your player and save this file
* with the same name and .cc extension.
*/
#define PLAYER_NAME Barbarbrat


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
    if (round()==0) {
        pinta_mapa();
    }
    
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
        vector<vector<pair<int, pair<int, int> > > > dist1(rows(), vector<pair<int, pair<int, int> > > (cols(), make_pair(-1, make_pair(-1, -1))));
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
            if (dist1[to.first][to.second].first==-1) {
                dist1[to.first][to.second].first=dist;
                dist1[to.first][to.second].second=from;
                Cell c=cell(to.first, to.second);
                if (c.type==CITY and ciutat.first==-1) ciutat=make_pair(to.first, to.second);
                else if (c.type==PATH and cami.first==-1) cami=make_pair(to.first, to.second);
                if (c.unit_id != -1 and unit(c.unit_id).player != me()) {
                    if (depredador.first==-1 and unit(c.unit_id).health>vida+dist+cost(cell(ent[i].first, ent[i].second).type)-cost(c.type))
                        depredador=make_pair(to.first, to.second);
                    else if (presa.first==-1 and unit(c.unit_id).health<vida-dist)
                        presa=make_pair(to.first, to.second);
                }
                for (int k=0; k<4; ++k) {
                    if (to.first+dirx[k]>=0 and to.first+dirx[k]<rows() and to.second+diry[k]>=0 and to.second+diry[k]<cols() and dist1[to.first+dirx[k]][to.second+diry[k]].first==-1 and cell(to.first+dirx[k], to.second+diry[k]).type!=WATER) {
                        q.push(make_pair(-(dist+cost(cell(to.first+dirx[k], to.second+diry[k]).type)), make_pair(make_pair(to.first+dirx[k], to.second+diry[k]), to)));
                    }
                }
            }
            if (ciutat.first!=-1 and cami.first!=-1 and ((presa.first!=-1 or dist>15) and (depredador.first!=-1 or dist>8))) seguir=false;
        }
        while (!q.empty()) q.pop();
            
        vector<vector<pair<int, pair<int, int> > > > dist2(rows(), vector<pair<int, pair<int, int> > > (cols(), make_pair(-1, make_pair(-1, -1))));
        q.push(make_pair(0, make_pair(make_pair(posx, posy), make_pair(posx, posy))));
        q.push(make_pair(0, make_pair(make_pair(posx, posy), make_pair(posx, posy))));
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

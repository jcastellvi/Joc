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

Dir cap[4]={BOTTOM, RIGHT, TOP, LEFT};

bool puc(int x, int y) {
    if (x<0 or y<0 or x>rows() or y>cols())
        return false;
    if (cell(x, y).type==WATER)
        return false;
    return true;
}


/**
* Play method, invoked once per each round.
*/
virtual void play () {
    
    ent=vector<pair<int, int> > ();
    orc=vector<pair<int, int> > ();
    
    bool tot_meu=true;
    
    for (int i=0; i<rows(); ++i)
        for (int j=0; j<cols(); ++j) {
            Cell c=cell(i, j);
            if (c.unit_id != -1 and unit(c.unit_id).player != me()) orc.push_back(make_pair(i, j));
            if (c.unit_id != -1 and unit(c.unit_id).player == me()) ent.push_back(make_pair(i, j));
            if (c.type==CITY and city_owner(c.city_id)!=me()) tot_meu=false;
            if (c.type==PATH and path_owner(c.path_id)!=me()) tot_meu=false;
        }
    
    for (int i=0; i<int(ent.size()); ++i) {
        int posx=ent[i].first;
        int posy=ent[i].second;
        int vida=unit(cell(posx, posy).unit_id).health;
        vector<vector<pair<pair<int, int>, pair<int, int> > > > dist1(rows(), vector<pair<pair<int, int>, pair<int, int> > > (cols(), make_pair(make_pair(-1,-1), make_pair(-1, -1))));
        priority_queue<pair<pair<int, int>, pair<pair<int, int>, pair<int, int> > > > q;
        pair<int, int> ciutat=make_pair(-1, -1);
        pair<int, int> cami=make_pair(-1, -1);
        pair<int, int> presa=make_pair(-1, -1);
        pair<int, int> depredador=make_pair(-1, -1);
        q.push(make_pair(make_pair(0, 0), make_pair(make_pair(posx, posy), make_pair(posx, posy))));
        bool seguir=true;
        if (tot_meu) seguir=false;
        while(!q.empty() and seguir) {
            int dist=-q.top().first.first;
            int pas=q.top().first.second;
            pair<int, int> to=q.top().second.first;
            pair<int, int> from=q.top().second.second;
            q.pop();
            if (dist1[to.first][to.second].first.first==-1) {
                dist1[to.first][to.second].first.first=dist;
                dist1[to.first][to.second].first.second=dist1[from.first][from.second].first.second+1;
                dist1[to.first][to.second].second=from;
                Cell c=cell(to.first, to.second);
                if (pas!=0 and c.type==CITY and ciutat.first==-1 and city_owner(c.city_id)!=me()) ciutat=to;
                if (pas!=0 and c.type==PATH and cami.first==-1 and path_owner(c.path_id)!=me()) cami=to;
                for (int k=0; k<4; ++k) {
                    if (puc(to.first+dirx[k], to.second+diry[k]) and dist1[to.first+dirx[k]][to.second+diry[k]].first.first==-1) {
                        q.push(make_pair(make_pair(-(dist+cost(cell(to.first+dirx[k], to.second+diry[k]).type)), pas-1), make_pair(make_pair(to.first+dirx[k], to.second+diry[k]), to)));
                    }
                }
            }
            if (ciutat.first!=-1 or cami.first!=-1) seguir=false;
        }
        while (!q.empty())
            q.pop();
        
        
        vector<vector<pair<int, pair<int, int> > > > dist2(rows(), vector<pair<int, pair<int, int> > > (cols(), make_pair(-1, make_pair(-1, -1))));
        priority_queue<pair<int, pair<pair<int, int>, pair<int, int> > > > q2;
        q2.push(make_pair(0, make_pair(make_pair(posx, posy), make_pair(posx, posy))));
        while(!q2.empty()) {
            int dist=-q2.top().first;
            pair<int, int> to=q2.top().second.first;
            pair<int, int> from=q2.top().second.second;
            q2.pop();
            if (dist2[to.first][to.second].first==-1) {
                dist2[to.first][to.second].first=dist;
                dist2[to.first][to.second].second=from;
                Cell c=cell(to.first, to.second);
                if (c.unit_id != -1 and unit(c.unit_id).player != me()) {
                    if (depredador.first==-1 and ((dist==3 and unit(c.unit_id).health-cost(cell(posx, posy).type)>vida) or (dist<3 and unit(c.unit_id).health>vida)))
                        depredador=to;
                }
                for (int k=0; k<4; ++k) {
                    if (puc(to.first+dirx[k], to.second+diry[k]) and dist<3 and dist2[to.first+dirx[k]][to.second+diry[k]].first==-1) {
                        q2.push(make_pair(-(dist+1), make_pair(make_pair(to.first+dirx[k], to.second+diry[k]), to)));
                    }
                }
            }
        }
        while (!q2.empty()) q2.pop();
        
        
        vector<vector<pair<int, pair<int, int> > > > dist3(rows(), vector<pair<int, pair<int, int> > > (cols(), make_pair(-1, make_pair(-1, -1))));
        priority_queue<pair<int, pair<pair<int, int>, pair<int, int> > > > q3;
        q3.push(make_pair(0, make_pair(make_pair(posx, posy), make_pair(posx, posy))));
        while(!q3.empty()) {
            int dist=-q3.top().first;
            pair<int, int> to=q3.top().second.first;
            pair<int, int> from=q3.top().second.second;
            q3.pop();
            if (dist3[to.first][to.second].first==-1) {
                dist3[to.first][to.second].first=dist;
                dist3[to.first][to.second].second=from;
                Cell c=cell(to.first, to.second);
                if (c.unit_id != -1 and unit(c.unit_id).player != me()) {
                    if (presa.first==-1 and unit(c.unit_id).health+cost(cell(to.first, to.second).type)<vida)
                        presa=to;
                }
                for (int k=0; k<4; ++k) {
                    if (puc(to.first+dirx[k], to.second+diry[k]) and dist<3 and dist3[to.first+dirx[k]][to.second+diry[k]].first==-1) {
                        q3.push(make_pair(-(dist+1), make_pair(make_pair(to.first+dirx[k], to.second+diry[k]), to)));
                    }
                }
            }
        }
        while (!q3.empty()) q3.pop();
        
        //ara es mou
        int direccio=-1; //direccio cap a on vull anar
        if (tot_meu) {
            
        }
        else {
            if (presa.first!=-1) {
                pair<int, int> pos=presa;
                pair<int, int> ant=pos;
                while (dist3[pos.first][pos.second].second!=pos) {
                    ant=pos;
                    pos=dist3[pos.first][pos.second].second;
                }
                for (int k=0; k<4; ++k)
                    if (ant==make_pair(posx+dirx[k], posy+diry[k]))
                        direccio=k;            
            }
            else {
                pair<int, int> pos=ciutat;
                if (ciutat.first==-1)
                    pos=cami;
                pair<int, int> ant=pos;
                while (dist1[pos.first][pos.second].second!=pos) {
                    ant=pos;
                    pos=dist1[pos.first][pos.second].second;
                }
                for (int k=0; k<4; ++k)
                    if (ant==make_pair(posx+dirx[k], posy+diry[k]))
                        direccio=k;
            }
            if (depredador.first==-1) {
                if (direccio!=-1)
                    execute(Command(cell(posx, posy).unit_id, cap[direccio]));
            }
            else if (direccio==-1 or abs(posx-depredador.first)+abs(posy-depredador.second)<=abs(posx+dirx[direccio]-depredador.first)+abs(posy+diry[direccio]-depredador.second)) {
                if (direccio != -1)
                    execute(Command(cell(posx, posy).unit_id, cap[direccio]));
            }
            else {
                if (direccio!=-1) {
                    int direccio2=-1;
                    int dir_cost=4;
                    for (int k=0; k<4; ++k) {
                        if (puc(posx+dirx[k], posy+diry[k]) and abs(posx-depredador.first)+abs(posy-depredador.second)<=abs(posx+dirx[k]-depredador.first)+abs(posy+diry[k]-depredador.second) and (cost(cell(posx+dirx[k], posy+diry[k]).type)<dir_cost or (cost(cell(posx+dirx[k], posy+diry[k]).type)==dir_cost and (k==(direccio+1)%4 or k==(direccio+3)%4)))) {
                            direccio2=k;
                            dir_cost=cost(cell(posx+dirx[k], posy+diry[k]).type);
                        }
                    }
                    if (direccio2!=-1)
                        execute(Command(cell(posx, posy).unit_id, cap[direccio2]));
                }
                else {
                    int dir_cost=4;
                    for (int k=0; k<4; ++k) {
                        if (puc(posx+dirx[k], posy+diry[k]) and abs(posx-depredador.first)+abs(posy-depredador.second)<=abs(posx+dirx[k]-depredador.first)+abs(posy+diry[k]-depredador.second) and cost(cell(posx+dirx[k], posy+diry[k]).type)<dir_cost) {
                            direccio=k;
                            dir_cost=cost(cell(posx+dirx[k], posy+diry[k]).type);
                        }
                    }
                    if (direccio!=-1)
                        execute(Command(cell(posx, posy).unit_id, cap[direccio]));
                }
                
            }
        }
        
    }
    
}

};


/**
* Do not modify the following line.
*/
RegisterPlayer(PLAYER_NAME);

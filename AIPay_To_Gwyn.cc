#include "Player.hh"
#include <bits/stdc++.h>


/**
* Write the name of your player and save this file
* with the same name and .cc extension.
*/
#define PLAYER_NAME Pay_To_Gwyn


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
vector<vector<bool> > mhi_moc;

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

int voltant(int x, int y) {
    int res=0;
    for (int i=0; i<4; ++i)
        if (puc(x+dirx[i], y+diry[i]) and !mhi_moc[x+dirx[i]][y+diry[i]] and (cell(x+dirx[i], y+diry[i]).unit_id == -1 or unit(cell(x+dirx[i], y+diry[i]).unit_id).player != me()))
            ++res;
    return res;
}

/*
int valor(int x, int y) {
    int ret=0;
    if (cell(x, y).unit_id == -1 or unit(cell(x, y).unit_id).player == me())
}
*/

/**
* Play method, invoked once per each round.
*/
virtual void play () {
    
    ent=vector<pair<int, int> > ();
    orc=vector<pair<int, int> > ();
    
    bool tot_meu=true;
    
    mhi_moc=vector<vector<bool> > (rows(), vector<bool> (cols(), false));
    
    for (int i=0; i<rows(); ++i)
        for (int j=0; j<cols(); ++j) {
            Cell c=cell(i, j);
            if (c.unit_id != -1 and unit(c.unit_id).player != me()) orc.push_back(make_pair(i, j));
            if (c.unit_id != -1 and unit(c.unit_id).player == me()) ent.push_back(make_pair(i, j));
            if (c.type==CITY and city_owner(c.city_id)!=me()) tot_meu=false;
            if (c.type==PATH and path_owner(c.path_id)!=me()) tot_meu=false;
        }
    
    priority_queue<pair<int, pair<int, int> > > lenin;
    
    for (int i=0; i<int(ent.size()); ++i) {
        int posx=ent[i].first;
        int posy=ent[i].second;
        int vida=unit(cell(posx, posy).unit_id).health;
        
        
        int cosa=0;
        
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
                    if (unit(c.unit_id).health>vida) {
                        if (dist==1 and cosa<3) cosa=3;
                        else if (dist==2 and cosa<2) cosa=2;
                        else if (dist==3 and cosa<1) cosa=1;
                    }
                    if (dist==1 and unit(c.unit_id).health+cost(c.type)>vida) {
                        if (cosa<3) cosa=3;
                    }
                }
                for (int k=0; k<4; ++k) {
                    if (puc(to.first+dirx[k], to.second+diry[k]) and dist<3 and dist2[to.first+dirx[k]][to.second+diry[k]].first==-1) {
                        q2.push(make_pair(-(dist+1), make_pair(make_pair(to.first+dirx[k], to.second+diry[k]), to)));
                    }
                }
            }
            
        }
        while (!q2.empty()) q2.pop();
        
        
        lenin.push(make_pair(cosa, make_pair(posx, posy)));
        
    }
    
    
    while(not lenin.empty()) {
        int perill=lenin.top().first;
        int posx=lenin.top().second.first;
        int posy=lenin.top().second.second;
        int vida=unit(cell(posx, posy).unit_id).health;
        lenin.pop();
        
        vector<vector<pair<pair<int, int>, pair<int, int> > > > dist1(rows(), vector<pair<pair<int, int>, pair<int, int> > > (cols(), make_pair(make_pair(-1,-1), make_pair(-1, -1))));
        priority_queue<pair<pair<int, pair<int, bool> >, pair<pair<int, int>, pair<int, int> > > > q;
        pair<int, int> ciutat=make_pair(-1, -1);
        pair<int, int> cami=make_pair(-1, -1);
        pair<int, int> presa=make_pair(-1, -1);
        q.push(make_pair(make_pair(0, make_pair(0, 0)), make_pair(make_pair(posx, posy), make_pair(posx, posy))));
        bool seguir=true;
        if (tot_meu) seguir=false;
        
        while(!q.empty() and seguir) {
            int dist=q.top().first.second.first;
            int pas=q.top().first.second.second;
            pair<int, int> to=q.top().second.first;
            pair<int, int> from=q.top().second.second;
            q.pop();
            if (dist1[to.first][to.second].first.first==-1) {
                dist1[to.first][to.second].first.first=dist;
                dist1[to.first][to.second].first.second=pas+1;
                dist1[to.first][to.second].second=from;
                Cell c=cell(to.first, to.second);
                if (pas!=0 and c.type==CITY and ciutat.first==-1 and city_owner(c.city_id)!=me()) ciutat=to;
                if (pas!=0 and c.type==PATH and cami.first==-1 and path_owner(c.path_id)!=me()) cami=to;
                for (int k=0; k<4; ++k) {
                    if (puc(to.first+dirx[k], to.second+diry[k]) and dist1[to.first+dirx[k]][to.second+diry[k]].first.first==-1) {
                        int ndist=dist+cost(cell(to.first+dirx[k], to.second+diry[k]).type);
                        int npas=pas+1;
                        int sumar=0;
                        if (pas==0) {
                            if (!mhi_moc[posx+dirx[k]][posy+diry[k]] and (cell(posx+dirx[k], posy+diry[k]).unit_id == -1 or unit(cell(posx+dirx[k], posy+diry[k]).unit_id).player != me()))
                                sumar+=5;
                            sumar+=4-voltant(posx+dirx[k], posy+diry[k]);
                        }
                        q.push(make_pair(make_pair(-(70*ndist+10*npas+sumar), make_pair(ndist, npas)), make_pair(make_pair(to.first+dirx[k], to.second+diry[k]), to)));
                    }
                }
            }
            if (ciutat.first!=-1 or cami.first!=-1) seguir=false;
        }
        while (!q.empty())
            q.pop();
        
        
        bool puc3[4]={1, 1, 1, 1};
        bool puc2[4]={1, 1, 1, 1};
        bool puc1[4]={1, 1, 1, 1};
        
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
                    if (unit(c.unit_id).health>vida) {
                        for (int k=0; k<4; ++k) {
                            if (abs(posx+dirx[k]-to.first)+abs(posy+diry[k]-to.second)<abs(posx-to.first)+abs(posy-to.second)){
                                puc3[k]=0;
                                if (dist<=2)
                                    puc2[k]=0;
                                if (dist<=1)
                                    puc1[k]=0;
                            }
                        }
                    }
                    if (dist==1 and unit(c.unit_id).health+cost(c.type)>vida) {
                        for (int k=0; k<4; ++k) {
                            if (posx+dirx[k]==to.first and posy+diry[k]==to.second){
                                puc3[k]=0;
                                puc2[k]=0;
                                puc1[k]=0;
                            }
                        }
                    }
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
        priority_queue<pair<pair<int, int>, pair<pair<int, int>, pair<int, int> > > > q3;
        q3.push(make_pair(make_pair(0, 0), make_pair(make_pair(posx, posy), make_pair(posx, posy))));
        while(!q3.empty()) {
            int dist=-q3.top().first.first;
            int pes=-q3.top().first.second;
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
                    if (puc(to.first+dirx[k], to.second+diry[k]) and dist<2 and dist3[to.first+dirx[k]][to.second+diry[k]].first==-1) {
                        q3.push(make_pair(make_pair(-(dist+1), -(pes+cost(cell(to.first+dirx[k], to.second+diry[k]).type))), make_pair(make_pair(to.first+dirx[k], to.second+diry[k]), to)));
                    }
                }
            }
        }
        while (!q3.empty()) q3.pop();
        
        
        
        //ara es mou
        int direccio=-1; //direccio cap a on vull anar
        if (not tot_meu) {
            int direccio=-1;
            if (presa.first!=-1) {
                pair<int, int> pos=presa;
                pair<int, int> ant=pos;
                while (dist3[pos.first][pos.second].second!=pos) {
                    ant=pos;
                    pos=dist3[pos.first][pos.second].second;
                }
                for (int k=0; k<4; ++k)
                    if (posx+dirx[k]==ant.first and posy+diry[k]==ant.second)
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
                    if (posx+dirx[k]==ant.first and posy+diry[k]==ant.second)
                        direccio=k;
            }
            if (perill==0 or (puc3[direccio] and (not mhi_moc[posx+dirx[direccio]][posy+diry[direccio]]) and (cell(posx+dirx[direccio], posy+diry[direccio]).unit_id == -1 or unit(cell(posx+dirx[direccio], posy+diry[direccio]).unit_id).player != me()))) {
                execute(Command(cell(posx, posy).unit_id, cap[direccio]));
                mhi_moc[posx+dirx[direccio]][posy+diry[direccio]]=true;
            }
            else {
                int triar[4]={0, 0, 0, 0};
                for (int k=0; k<4; ++k)
                    if (not puc(posx+dirx[k], posy+diry[k]))
                        triar[k]=-1;
                for (int k=0; k<4; ++k)
                    if (triar[k]!=-1) {
                        if (puc3[k])
                            triar[k]+=30000;
                        else if (puc2[k])
                            triar[k]+=20000;
                        else if (puc1[k])
                            triar[k]+=10000;
                    }
                for (int k=0; k<4; ++k) {
                    Cell c=cell(posx+dirx[k], posy+diry[k]);
                    if (triar[k]!=-1 and (c.unit_id == -1 or unit(c.unit_id).player != me()))
                        triar[k]+=1000;
                }
                for (int k=0; k<4; ++k)
                    if (triar[k]!=-1 and not mhi_moc[posx+dirx[k]][posy+diry[k]])
                        triar[k]+=200;

                for (int k=0; k<4; ++k)
                    if (triar[k]!=-1)
                        triar[k]+=25*voltant(posx+dirx[k], posy+diry[k]);
                
                for (int k=0; k<4; ++k) {
                    Cell c=cell(posx+dirx[k], posy+diry[k]);
                    if (triar[k]!=-1)
                        triar[k]+=50-10*cost(c.type);
                }
                
                for (int k=0; k<4; ++k) {
                    if (triar[k]!=-1 and (k==(direccio+1)%4 or k==(direccio+3)%4))
                        triar[k]+=1;
                    else if (triar[k]!=-1 and k==direccio)
                        triar[k]+=5;
                }
                
                int direccio=0;
                for (int k=1; k<4; ++k)
                    if (triar[k]>triar[direccio])
                        direccio=k;
                
                if (triar[direccio]!=-1) {
                    execute(Command(cell(posx, posy).unit_id, cap[direccio]));
                    mhi_moc[posx+dirx[direccio]][posy+diry[direccio]]=true;
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

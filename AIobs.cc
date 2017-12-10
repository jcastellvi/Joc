/*
 * Area CITY i PATH
 * grafs connexos
 */


#include "Player.hh"
#include <queue>
#include <functional>
#include <algorithm>

/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME obs


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
  
  struct Path_vertex {
    int dist;
    Pos pos, ant;
  };
  
  struct Urban_info {
    int graph, graph_size, area, paths;
  };
    
  struct Atraction_node {
    Pos pos;
    Dir dir;
    int dist;
  };
  
  typedef vector<int> VI;
  typedef vector<VI> VVI;
  typedef vector<VVI> VVVI;
  typedef vector<bool> VB;
  typedef vector<pair<int, int> > VPII;
  typedef vector<Urban_info> VUI;
  typedef vector<Path_vertex> VPV;
  typedef vector<VPV> VVPV;
  typedef priority_queue<Path_vertex, vector<Path_vertex>, function<bool(Path_vertex, Path_vertex)> > Dij_pq;
  typedef pair<Dir, int> PDI;
  typedef vector<PDI> VPDI;
  typedef vector<VPDI> VVPDI;
  typedef vector<VVPDI> VVVPDI;
  typedef vector<Atraction_node> VAN;
  typedef vector<VAN> VVAN;

  // Valors variables:
  const int CONNECTED_COMP = 20;
  const int PSEUDO_CONNECTED_COMP = 8;
  const int GRAPH_SIZE_BONUS = 1;
  const int AREA_BONUS = 1;
  const int ADJACENT_CITIES = 1;
  
  const int MAX_ATRACTIONS = 5;
  const int MIN_PRI_IMPORTANT = 25;
  const int ORKS_SENT = 2;
  const int MAX_ATRACTION_TURNS = 15;
  const int COST_OF_TIME = 5;
  
  VVI available_dir;
  VVPDI Dirs;
  VVVPDI Next_city;
  VVI Connexions;
  VUI U_info;                               // first = component connexa, second = # {ciutats que hi pertanyen}
  VVAN At;
  
  // Caldra borrar la seguent funcio:
  void print_paths() {
    for (int i1 = 0; i1 < 70; ++i1) {
      for (int j1 = 0; j1 < 70; ++j1) {
        if (cell(Pos(i1,j1)).type == CITY or cell(Pos(i1,j1)).type == PATH) {
          if (Dirs[i1][j1].first == NONE) cerr << '*';
          else if (Dirs[i1][j1].first == TOP) cerr << "^";
          else if (Dirs[i1][j1].first == BOTTOM) cerr << "v";
          else if (Dirs[i1][j1].first == LEFT) cerr << "<";
          else if (Dirs[i1][j1].first == RIGHT) cerr << ">";
        } 
        else if (Dirs[i1][j1].first == NONE) cerr << '.';
        else if (Dirs[i1][j1].first == TOP) cerr << "T";
        else if (Dirs[i1][j1].first == BOTTOM) cerr << "B";
        else if (Dirs[i1][j1].first == LEFT) cerr << "L";
        else if (Dirs[i1][j1].first == RIGHT) cerr << "R";
      }
      cerr << endl;
    }
  }
  
  // Caldra borrar la seguent funcio:
  void print_next_city() {
    for (int k = 0; k < 2; ++k) {
      for (int i1 = 0; i1 < 70; ++i1) {
        for (int j1 = 0; j1 < 70; ++j1) {
          if (cell(Pos(i1,j1)).type == CITY) cerr << '#';
          else if (Next_city[i1][j1][k].first == NONE) cerr << '.';
          else if (Next_city[i1][j1][k].first == TOP) cerr << "T";
          else if (Next_city[i1][j1][k].first == BOTTOM) cerr << "B";
          else if (Next_city[i1][j1][k].first == LEFT) cerr << "L";
          else if (Next_city[i1][j1][k].first == RIGHT) cerr << "R";
        }
        cerr << endl;
      }
      cerr << endl << endl;
    }
  }
  
  // Caldra borrar la seguent funcio:
  void print_map() {
    for (int i1 = 0; i1 < 70; ++i1) {
      for (int j1 = 0; j1 < 70; ++j1) {
//        if (cell(Pos(i1,j1)).unit_id != -1 and unit(cell(Pos(i1,j1)).unit_id).player == me()) cerr << cell(Pos(i1,j1)).unit_id/10;
        if (cell(Pos(i1,j1)).type == WATER) cerr << '.';
        else if (cell(Pos(i1,j1)).type == SAND) cerr << 'S';
        else if (cell(Pos(i1,j1)).type == FOREST) cerr << 'F';
        else if (cell(Pos(i1,j1)).type == GRASS) cerr << 'G';
        else if (cell(Pos(i1,j1)).type == CITY) cerr << '.';
        else if (cell(Pos(i1,j1)).type == PATH) cerr << '.';
      }
      cerr << endl;
    }
  }
  
  // Retorna la direccio per anar de "a" a "b" (cal que "a" i "b" siguin adjacents)
  Dir direction(Pos a, Pos b) {
    if (a.j == b.j+1) return LEFT;
    if (a.j == b.j-1) return RIGHT;
    if (a.i == b.i-1) return BOTTOM;
    if (a.i == b.i+1) return TOP;
    return NONE;
  }
  
  // Caldra borrar la seguent funcio:
  void pos_mes_dir(Pos &p, Dir a) {
    if (a == TOP) --p.i;
    if (a == RIGHT) ++p.j;
    if (a == BOTTOM) ++p.i;
    if (a == LEFT) --p.j;
  }
  
  // Defineix l'ordre d'ordenació de la priority_queue de Dijkstra
  static bool dij_comp(Path_vertex a, Path_vertex b) {
    if (a.dist == -1) return true;
    if (b.dist == -1) return false;
    return a.dist > b.dist;
  }
  
  // Inicialitza Dijkstra
  void dij_init(Pos &ori, VVPV &Dij_mat, Dij_pq &Q) {
    Path_vertex aux;
    aux.dist = -1;
    Dij_mat = VVPV (rows(), VPV(cols(), aux));
    aux.pos = ori;
    aux.dist = 0;
    Q.push(aux);
  }
  
  // Retorna si "pv" ha de ser afegit a la priority_queue de Dijkstra
  bool dij_include_pq(Path_vertex &pv, VVPV &Dij_mat) {
    if (cell(pv.pos).type == WATER) return false;
    if (Dij_mat[pv.pos.i][pv.pos.j].dist != -1) return false;
    return true;
  }
  
  // Es possible que calgui modificar aquesta funció per a que sigui mes eficient
  // Crea el node abans d'afegir-lo a la priority_queue de Dijkstra
  void dij_create_node(Path_vertex &pv, int dist) {
    /*if (Dirs[pv.pos.i][pv.pos.j].first != NONE) {
      Pos act = pv.pos;
      while (cell(act).type != CITY and cell(act).type != PATH) {
        if (Dirs[act.i][act.j].first == TOP) --act.i;
        else if (Dirs[act.i][act.j].first == BOTTOM) ++act.i;
        else if (Dirs[act.i][act.j].first == RIGHT) ++act.j;
        else if (Dirs[act.i][act.j].first == LEFT) --act.j;
        else {cerr << "Dijkstra retorna un cami erroni" << endl; print_paths(); break;}
      }
      pv.dist = dist+Dirs[pv.pos.i][pv.pos.j].second;
      pv.pos = act;
    }
    else*/ pv.dist = dist+cost(cell(pv.pos).type);
  }
  
  // Algorisme de Dijkstra des d'un punt a la zona urbana mes propera
  void dijkstra(Pos ori) {
    if (Dirs[ori.i][ori.j].second != -1) return;
    VVPV Dij_mat;
    Dij_pq Q(dij_comp);
    dij_init(ori, Dij_mat, Q);
    Cell act = cell(ori);
    int count = 0;
    while (act.type != CITY and act.type != PATH) {
//      cerr << count << "      " << Q.size() <<"   " << Q.top().dist << "    " << Q.top().pos.i << " " << Q.top().pos.j << "   " << Dij_mat[Q.top().pos.i][Q.top().pos.j].dist << "  " << cell(Q.top().pos).type << "      " << Q.top().ant.i << "," << Q.top().ant.j << "    " << cell(Q.top().pos).type << endl;
      Path_vertex top = Q.top();
      Q.pop();
      if (act.type != WATER and Dij_mat[top.pos.i][top.pos.j].dist == -1) {
        Path_vertex aux;
        aux.ant = aux.pos = top.pos;
        ++aux.pos.i;
        if (dij_include_pq(aux, Dij_mat)) {
          dij_create_node(aux, top.dist);
          Q.push(aux);
        }
        aux.pos.i -= 2;
        if (dij_include_pq(aux, Dij_mat)) {
          dij_create_node(aux, top.dist);
          Q.push(aux);
        }
        ++aux.pos.i;
        ++aux.pos.j;
        if (dij_include_pq(aux, Dij_mat)) {
          dij_create_node(aux, top.dist);
          Q.push(aux);
        }
        aux.pos.j -= 2;
        if (dij_include_pq(aux, Dij_mat)) {
          dij_create_node(aux, top.dist);
          Q.push(aux);
        }
        if (Q.empty()) cerr << "Q esta buida!!" << endl;
      }
      Dij_mat[top.pos.i][top.pos.j] = top;
      act = cell(Q.top().pos);
      ++count;
    }
//  cerr << count << "           " << ori.i << "   " << ori.j << endl;
    if (Q.top().pos == ori) Dirs[ori.i][ori.j] = make_pair(NONE, 0);
    Path_vertex pv = Q.top();
    while (pv.pos != ori) {
//        if (ori == Pos(10,7)) cerr << pv.ant.i << "," << pv.ant.j << "  --->  " << pv.pos.i << "," << pv.pos.j<<endl;
      Dirs[pv.ant.i][pv.ant.j] = make_pair(direction(pv.ant, pv.pos), Q.top().dist - Dij_mat[pv.ant.i][pv.ant.j].dist);
      pv = Dij_mat[pv.ant.i][pv.ant.j];
    }
  }
  
  bool BFS_include_q(Path_vertex &aux, VVPV &BFS_mat, VB &t, VI &c, int p_id) {
    return (BFS_mat[aux.pos.i][aux.pos.j].dist == -1 and (cell(aux.pos).path_id == p_id or (not t[0] and cell(aux.pos).city_id == c[0]) or (not t[1] and cell(aux.pos).city_id == c[1])));
  }
  
  // Inicialitza el BFS del camins
  void BFS_init(Pos &ori, VVPV &BFS_mat, queue<Path_vertex> &Q) {
    Path_vertex aux;
    aux.dist = -1;
    BFS_mat = VVPV (rows(), VPV(cols(), aux));
    aux.pos = ori;
    aux.dist = 0;
    Q.push(aux);
  }
  
  // BFS pels camins
  void BFS_camins(Pos &ori, int p_id, VI &c) {
    VVPV BFS_mat;
    queue<Path_vertex> Q;
    BFS_init(ori, BFS_mat, Q);
    VB t(2, false);
    Pos first_pos;
    int first_dist = 0;
    int count = 0;
    while ((not t[0] or not t[1])) {
//      cerr << count << "      " << Q.size() <<"   " << Q.front().dist << "    " << Q.front().pos.i << " " << Q.front().pos.j << "   " << BFS_mat[Q.front().pos.i][Q.front().pos.j].dist << "  " << cell(Q.front().pos).type << "      " << Q.front().ant.i << "," << Q.front().ant.j << endl;
      if (cell(Q.front().pos).type == CITY) {
        int i = 0;
        if (cell(Q.front().pos).city_id == c[1]) i = 1;
        if (not t[i]) {
          t[i] = true;
          Path_vertex pv = Q.front();
          while (pv.pos != ori) {
            Next_city[pv.ant.i][pv.ant.j][i] = make_pair(direction(pv.ant, pv.pos), Q.front().dist - BFS_mat[pv.ant.i][pv.ant.j].dist);
            pv = BFS_mat[pv.ant.i][pv.ant.j];
          }
          if (t[1-i]) {
            pv = BFS_mat[first_pos.i][first_pos.j];
            while (pv.pos != ori) {
              Next_city[pv.pos.i][pv.pos.j][i] = make_pair(direction(pv.pos, pv.ant), Q.front().dist+BFS_mat[pv.pos.i][pv.pos.j].dist);
              pv = BFS_mat[pv.ant.i][pv.ant.j];
            }
            Path_vertex pv = Q.front();
            while (pv.pos != ori) {
              Next_city[pv.pos.i][pv.pos.j][1-i] = make_pair(direction(pv.pos, pv.ant), first_dist+BFS_mat[pv.pos.i][pv.pos.j].dist);
              pv = BFS_mat[pv.ant.i][pv.ant.j];
            }
          }
          else {
            first_pos = Q.front().pos;
            first_dist = Q.front().dist;
          }
        }
        BFS_mat[Q.front().pos.i][Q.front().pos.j] = Q.front();
        Q.pop();
      }
      else {
        Path_vertex aux;
        aux.ant = aux.pos = Q.front().pos;
        aux.dist = Q.front().dist + 1;
        ++aux.pos.i;
        if (BFS_include_q(aux, BFS_mat, t, c, p_id)) {
          Q.push(aux);
        }
        aux.pos.i -= 2;
        if (BFS_include_q(aux, BFS_mat, t, c, p_id)) {
          Q.push(aux);
        }
        ++aux.pos.i;
        ++aux.pos.j;
        if (BFS_include_q(aux, BFS_mat, t, c, p_id)) {
          Q.push(aux);
        }
        aux.pos.j -= 2;
        if (BFS_include_q(aux, BFS_mat, t, c, p_id)) {
          Q.push(aux);
        }
        if (Q.empty()) cerr << "Q esta buida!!" << endl;
        BFS_mat[Q.front().pos.i][Q.front().pos.j] = Q.front();
        Q.front();
        Q.pop();
      }
      ++count;
    }
  }
  
  void BFS_urban_info(VUI &U_info) {
    VB investigat(nb_cities(), false);
    int i = 0, count = 0;
    VI comp_con(nb_cities(), 0);
    while (i < nb_cities()) {
      queue<int> Q;
      Q.push(i);
      while (not Q.empty()) {
        ++comp_con[count];
        U_info[Q.front()].graph = count;
        investigat[Q.front()] = true;
        for (int j = 0; j < nb_cities(); ++j) {
          if (Connexions[Q.front()][j] != -1 and not investigat[j]) {
            U_info[nb_cities() + Connexions[Q.front()][j]].graph = count;
            Q.push(j);
          }
        }
        Q.pop();
      }
      ++count;
      while (i < nb_cities() and investigat[i]) ++i;
    }
    for (int i = 0; comp_con[i] != 0; ++i)
      for (int j = 0; j < nb_cities() + nb_paths(); ++j)
        if (U_info[j].graph == i) U_info[j].graph_size = comp_con[i];
  }
  
  Pos dij_at_trobar_ori(int id) {
    for (int i = 0; i < rows(); ++i) {
      for (int j = 0; j < cols(); ++j) {
        if (cell(Pos(i,j)).city_id == id) {
          return Pos(i,j);
        }
      }
    }
    return Pos(0,0);
  }
  
  // Inicialitza Dijkstra
  void dij_at_init(Pos &ori, VVPV &Dij_mat, Dij_pq &Q, int &id) {
    bool is_path = (id >= nb_cities());
    if (is_path) ori = path(id-nb_cities()).second[0];
    else ori = dij_at_trobar_ori(id);
    Path_vertex aux;
    aux.dist = -1;
    Dij_mat = VVPV (rows(), VPV(cols(), aux));
    aux.pos = ori;
    aux.dist = 0;
    Q.push(aux);
  }
  
  // Crea el node abans d'afegir-lo a la priority_queue de Dijkstra
  void dij_at_create_node(Path_vertex &pv, int dist, int id) {
    bool is_path = (id >= nb_cities());
    if (is_path and cell(pv.pos).path_id == id) pv.dist = 0;
    else if ((not is_path) and cell(pv.pos).city_id == id) pv.dist = 0;
    else pv.dist = dist + COST_OF_TIME + cost(cell(pv.pos).type);
  }
  
  // Algorisme de Dijkstra des d'un punt a la zona urbana mes propera
  void dijkstra_atraccio(int id) {
    Pos ori;
    
    VVPV Dij_mat;
    Dij_pq Q(dij_comp);
    dij_at_init(ori, Dij_mat, Q, id);
    Cell act = cell(ori);
    int count = 0;
    while (Q.top().dist < MAX_ATRACTION_TURNS*COST_OF_TIME and not Q.empty()) {
//      cerr << count << "      " << Q.size() <<"   " << Q.top().dist << "    " << Q.top().pos.i << " " << Q.top().pos.j << "   " << Dij_mat[Q.top().pos.i][Q.top().pos.j].dist << "  " << cell(Q.top().pos).type << "      " << Q.top().ant.i << "," << Q.top().ant.j << "    " << cell(Q.top().pos).type << endl;
      Path_vertex top = Q.top();
      Q.pop();
      if (act.type != WATER and Dij_mat[top.pos.i][top.pos.j].dist == -1) {
        Path_vertex aux;
        aux.ant = aux.pos = top.pos;
        ++aux.pos.i;
        if (dij_include_pq(aux, Dij_mat)) {
          dij_at_create_node(aux, top.dist, id);
          Q.push(aux);
        }
        aux.pos.i -= 2;
        if (dij_include_pq(aux, Dij_mat)) {
          dij_at_create_node(aux, top.dist, id);
          Q.push(aux);
        }
        ++aux.pos.i;
        ++aux.pos.j;
        if (dij_include_pq(aux, Dij_mat)) {
          dij_at_create_node(aux, top.dist, id);
          Q.push(aux);
        }
        aux.pos.j -= 2;
        if (dij_include_pq(aux, Dij_mat)) {
          dij_at_create_node(aux, top.dist, id);
          Q.push(aux);
        }
        if (Q.empty()) cerr << "Q esta buida!!" << endl;
      }
      Dij_mat[top.pos.i][top.pos.j] = top;
      if (top.dist != 0) {
        Atraction_node at_n;
        at_n.pos = top.pos;
        at_n.dist = top.dist;
        at_n.dir = direction(top.pos, top.ant);
        At[id].push_back(at_n);
      }
      act = cell(Q.top().pos);
      ++count;
    }
  }
  
  bool prioritari(int a, int b) {
    if (a == 0) return false;
    if (b == 0) return true;
    if (a < 0 and b < 0) return a < b;
    if (a > 0 and b > 0) return a > b;
    return a < 0;
  }
  
  int interes(int i, int j, Unit &mine, Unit other) {
    int dist = abs(i) + abs(j);
    if ((cell(mine.pos).type == WATER or other.player == me()) and dist == 1) return -1;
    else if (other.player == me()) return 0;
    
    else if (dist == 1 and mine.health > other.health + cost(cell(other.pos).type)) return 5;
    else if (dist == 2 and abs(i-j) == 2 and mine.health > other.health + cost(cell(other.pos).type)) return 4;
    else if (dist == 2 and mine.health > other.health + cost(cell(other.pos).type)) return 3;
    else if (dist == 3 and abs(i-j) == 3 and mine.health > other.health + cost(cell(other.pos).type)) return 2;
    else if (dist == 3 and mine.health > other.health + cost(cell(other.pos).type)) return 1;
    
    else if (dist == 1 and mine.health < other.health + cost(cell(other.pos).type)) return -6;
    else if (dist == 2 and abs(i-j) == 2 and mine.health < other.health) return -5;
    else if (dist == 2 and mine.health < other.health) return -4;
    else if (dist == 3 and abs(i-j) == 3 and mine.health < other.health) return -3;
    else if (dist == 3 and mine.health < other.health) return -2;
    
    else return 0;
  }
  
  Dir best_dir(Unit &ork, VI &near_ork) {
    int i_max = -1, max = -1;
    for (int i = 0; i < 4; ++i) {
      if (cell(ork.pos + Dir(i)).type != WATER and (i_max == -1 or max < near_ork[i])) {
        i_max = i;
        max = near_ork[i];
      }
    }
    
    int count = 0;
    for (int i = 0; i < 4; ++i)
      if (cell(ork.pos + Dir(i)).type != WATER and max == near_ork[i]) {
        ++count;
        near_ork[i] = 1;
      }
      else near_ork[i] = 0;
    
    if (count > 1) return NONE;
    return Dir(i_max);
    
    /*
    int min = -1, in_min = i_max;
    if (count > 1) {
      for (int i = 0; i < 4; ++i) {
        if (cell(ork.pos + Dir(i)).type != WATER and max == near_ork[i]) {
          Pos pi = ork.pos + Dir(i);
          dijkstra(pi);
          if (min == -1 or cost(cell(pi).type) + Dirs[pi.i][pi.j].second < min) {
            min = cost(cell(pi).type) + Dirs[pi.i][pi.j].second;
            in_min = i;
          }
        }
      }
    }
    return Dir(in_min);*/
  }
  
  void look_around(Unit &ork, VI &near_ork) {
    for (int i = -3; i < 4; ++i) {
      for (int j = -3+abs(i); j <= 3-abs(i); ++j) {
        if (abs(i) + abs(j) != 0 and pos_ok(ork.pos.i + i, ork.pos.j + j) and cell(Pos(ork.pos.i+i, ork.pos.j+j)).unit_id != -1) {
          int prox = interes(i, j, ork, unit(cell(Pos(ork.pos.i+i, ork.pos.j+j)).unit_id));
          
          if (i > 0 and prioritari(prox, near_ork[2])) near_ork[0] = prox;
          if (i < 0 and prioritari(prox, near_ork[0])) near_ork[2] = prox;
          if (j > 0 and prioritari(prox, near_ork[1])) near_ork[1] = prox;
          if (j < 0 and prioritari(prox, near_ork[3])) near_ork[3] = prox;
        }
      }
    }
  }
  
  Dir out_of_urban_best_dir(Unit &ork, VI &near_ork) {
    int i_max = -1, max = 0;
    for (int i = 0; i < 4; ++i) {
      if (cell(ork.pos + Dir(i)).type != WATER and (i_max == -1 or max < near_ork[i])) {
        i_max = i;
        max = near_ork[i];
      }
    }
    
    int count = 0;
    for (int i = 0; i < 4; ++i)
      if (cell(ork.pos + Dir(i)).type != WATER and max == near_ork[i])
        ++count;
    int min = -1, in_min = i_max;
    if (count > 1) {
      for (int i = 0; i < 4; ++i) {
        if (cell(ork.pos + Dir(i)).type != WATER and max == near_ork[i]) {
          Pos pi = ork.pos + Dir(i);
          dijkstra(pi);
          if (min == -1 or cost(cell(pi).type) + Dirs[pi.i][pi.j].second < min) {
            min = cost(cell(pi).type) + Dirs[pi.i][pi.j].second;
            in_min = i;
          }
        }
      }
    }
    return Dir(in_min);
  }
  
  void create_urban_priorities(VPII &urban_priorities) {
    for (int i = 0; i < int(urban_priorities.size()); ++i) {
      urban_priorities[i].second = i;
      if (i < nb_cities()) {
        for (int j = 0; j < nb_cities(); ++j) {
          if (Connexions[i][j] != -1) {
            if (path_owner(Connexions[i][j]) == me() and city_owner(j) == me()) urban_priorities[i].first += CONNECTED_COMP;
            else if (path_owner(Connexions[i][j]) == me()) urban_priorities[i].first += PSEUDO_CONNECTED_COMP;
            else if (city_owner(j) == me()) urban_priorities[i].first += PSEUDO_CONNECTED_COMP;
          }
        }
      }
      else {
        if (city_owner(path(i-nb_cities()).first.first) == me() and city_owner(path(i-nb_cities()).first.second) == me()) urban_priorities[i].first += CONNECTED_COMP;
        else if (city_owner(path(i-nb_cities()).first.first) == me()) urban_priorities[i].first += PSEUDO_CONNECTED_COMP;
        else if (city_owner(path(i-nb_cities()).first.second) == me()) urban_priorities[i].first += PSEUDO_CONNECTED_COMP;
      }
      urban_priorities[i].first += ADJACENT_CITIES*U_info[i].paths;
      urban_priorities[i].first += GRAPH_SIZE_BONUS*U_info[i].graph_size;
      urban_priorities[i].first += AREA_BONUS*U_info[i].area;
    }
    sort(urban_priorities.begin(), urban_priorities.end());
  }
  
  void attract(int id) {
    int count = 0;
    for (int i = 0; i < int(At[id].size()) and count < ORKS_SENT; ++i) {
      int u_id = cell(At[id][i].pos).unit_id;
      if (u_id != -1 and unit(u_id).player == me() and available_dir[u_id][5] == 0 and available_dir[u_id][At[id][i].dir]) {
        execute(Command(u_id, At[id][i].dir));
        available_dir[u_id][5] = 1;
        ++count;
      }
    }
  }
  
  // Inicialitza les matrius a l'inici de cada partida
  void init_matrius() {
    Urban_info ui;
    ui.graph = ui.graph_size = -1;
    ui.area = 0;
    ui.paths = 0;
    U_info = VUI (nb_cities() + nb_paths(), ui);
    for (int i = nb_cities(); i < nb_cities() + nb_paths(); ++i) U_info[i].paths = 2;
    
    Dirs = VVPDI (rows(), VPDI (cols(), make_pair(NONE, -1)));
    Connexions = VVI (nb_cities(), VI(nb_cities(), -1));
    for (int i = 0; i < nb_paths(); ++i) {
      Connexions[path(i).first.first][path(i).first.second] = Connexions[path(i).first.second][path(i).first.first] = i;
      ++U_info[path(i).first.first].paths;
      ++U_info[path(i).first.second].paths;
    }
    Next_city = VVVPDI (rows(), VVPDI (cols(), VPDI(2, make_pair(NONE, -1))));
    for (int i = 0; i < nb_paths(); ++i) {
      Path pa = path(i);
      vector<int> vc(2);
      vc[0] = pa.first.first;
      vc[1] = pa.first.second;
      BFS_camins(pa.second[0], i, vc);
    }
    BFS_urban_info(U_info);
    for (int i = 0; i < rows(); ++i) {
      for (int j = 0; j < cols(); ++j) {
        if (cell(Pos(i, j)).city_id != -1) ++U_info[cell(Pos(i, j)).city_id].area;
        if (cell(Pos(i, j)).path_id != -1) ++U_info[nb_cities() + cell(Pos(i, j)).path_id].area;
      }
    }
    At = VVAN(nb_cities() + nb_paths(), VAN(0));
    for (int i = 0; i < int(At.size()); ++i) dijkstra_atraccio(i);
  }
  
  /**
   * Play method, invoked once per each round.
   */
  virtual void play () {
    if (round() == 0) init_matrius();
//    if (round() == 0) print_map();
    VI my_orks = orks(me());
//    for (int i = 0; i < nb_cities() + nb_paths(); ++i)
//      cerr << U_info[i].graph << "   " << U_info[i].graph_size << "                   " << U_info[i].area << "    " << U_info[i].paths << endl;
//    if (round() == 199) cerr << my_orks.size() << endl;                                                           // Eliminar
    available_dir = VVI(nb_units(), VI(6, 0));
    
    // Comprovem si estem en perill o podem menjar
    for (int i = 0; i < int(my_orks.size()); ++i) {
//      cerr << my_orks.size() << " " << i << " " << my_orks[i] << endl; 
      Unit ork = unit(my_orks[i]);
      look_around(ork, available_dir[my_orks[i]]);
      Dir d = best_dir(ork, available_dir[my_orks[i]]);
      if (d != NONE) {
        available_dir[my_orks[i]][5] = 1;
        execute(Command(my_orks[i], d));
      }
    }
    
    VPII urban_priorities(nb_cities() + nb_paths(), make_pair(0, -1));                      // first = prioritat de la zona, second = id de la zona
    create_urban_priorities(urban_priorities);
    
    // Atraiem els orcos a les zones urbanes importants
    for (int i = 0; i < MAX_ATRACTIONS and i < int(urban_priorities.size()); ++i) {
      if (urban_priorities[i].first > MIN_PRI_IMPORTANT) {
        attract(urban_priorities[i].second);
      }
    }
    
    // Els que han sobrat faran el seguent
    for (int i = 0; i < int(my_orks.size()); ++i) {
      if (available_dir[my_orks[i]][5] == 0) {
        Unit ork = unit(my_orks[i]);
        if (cell(ork.pos).type == PATH or cell(ork.pos).type == CITY) {
          if (available_dir[my_orks[i]][0] or available_dir[my_orks[i]][1] or available_dir[my_orks[i]][2] or available_dir[my_orks[i]][3]) {
            execute(Command(my_orks[i], Dir(out_of_urban_best_dir(ork, available_dir[my_orks[i]]))));
          }
        }
        
        // Si no estan a zona urbanitzada, anar-hi
        else if (cell(ork.pos).type != CITY and cell(ork.pos).type != PATH) {
          if (available_dir[my_orks[i]][0] or available_dir[my_orks[i]][1] or available_dir[my_orks[i]][2] or available_dir[my_orks[i]][3]) {
            execute(Command(my_orks[i], Dir(out_of_urban_best_dir(ork, available_dir[my_orks[i]]))));
          }
          else if (Dirs[ork.pos.i][ork.pos.j].second != -1) execute(Command(my_orks[i], Dirs[ork.pos.i][ork.pos.j].first));
          else { 
              dijkstra(ork.pos);
              execute(Command(my_orks[i], Dirs[ork.pos.i][ork.pos.j].first));
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



/*
  // Envia una direccio al numero associat
  int dir_num(Dir a) {
    if (a == TOP) return 0;
    if (a == RIGHT) return 1;
    if (a == BOTTOM) return 2;
    if (a == LEFT) return 3;
    if (a == NONE) return 4;
    return -1;
  }
  
  // Envia un numero a la direccio associada
  Dir num_dir(int a) {
    if (a == 0) return TOP;
    if (a == 1) return RIGHT;
    if (a == 2) return BOTTOM;
    if (a == 3) return LEFT;
    return NONE;
  }
  
  // Si alguna posicio es prohibida, retorna quina es la millor direccio a la que moure's
  Dir best_dir_in_danger(VB &near_ork, Unit &ork) {
    Pos p = ork.pos;
    int min = -1, in_min = -1;
    if (not near_ork[0]) {
      dijkstra(p + BOTTOM);
      min = cost(cell(p).type) + Dirs[p.i][p.j].second;
      in_min = 0;
    }
    if (not near_ork[1]) {
      dijkstra(p + RIGHT);
      if (min == -1 or cost(cell(p).type) + Dirs[p.i][p.j].second < min) {
        min = cost(cell(p).type) + Dirs[p.i][p.j].second;
        in_min = 1;
      }
    }
    if (not near_ork[2]) {
      ++p.i;
      dijkstra(p + );
      if (min == -1 or cost(cell(p).type) + Dirs[p.i][p.j].second < min) {
        min = cost(cell(p).type) + Dirs[p.i][p.j].second;
        in_min = 2;
      }
      --p.i;
    }
    if (not near_ork[3]) {
      --p.j;
      dijkstra(p);
      if (min == -1 or cost(cell(p).type) + Dirs[p.i][p.j].second < min) {
        min = cost(cell(p).type) + Dirs[p.i][p.j].second;
        in_min = 3;
      }
      ++p.j;
    }
    return num_dir(in_min);
  }
  
  // Falta fer algo amb aquesta funcio
  // Estableix en quina direccio anar en cas que totes estiguin prohibides
  void suicide(Unit &ork, VB &near_ork) {
    Pos p = ork.pos;
    --p.i;
    if (cell(p).type != WATER) {
      near_ork[0] = true;
      return;
    }
    ++p.i;
    ++p.j;
    if (cell(p).type != WATER) {
      near_ork[1] = true;
      return;
    }
    --p.j;
    ++p.i;
    if (cell(p).type != WATER) {
      near_ork[2] = true;
      return;
    }
    --p.i;
    --p.j;
    if (cell(p).type != WATER) {
      near_ork[3] = true;
      return;
    }
  }
  
  // Estableix si hi ha perill a un dels nodes adjacents i actualitza el vector de direccions accessibles
  bool update_danger_adjacent(VB &near_ork, Pos &p, Unit &ork, int i) {
    bool ret = false;
    if (cell(p).type == WATER) near_ork[i] = false;
    else if (cell(p).unit_id != -1) {
      if (unit(cell(p).unit_id).player == me()) near_ork[i] = false;
      else if (unit(cell(p).unit_id).health > ork.health - cost(cell(p).type)) {
        if (unit(cell(p).unit_id).health > ork.health + cost(cell(ork.pos).type)) ret = true;
        near_ork[i] = false;
      }
    }
    return ret;
  }
  
  // Retorna si una cel·la es perillosa
  bool dangerous_cell(Unit &ork, Pos &p) {
    return (cell(p).unit_id != -1 and (unit(cell(p).unit_id).player != me() and unit(cell(p).unit_id).health > ork.health));
  }
  
  // Retorna si hi ha perill en alguna direccio i actualitza el vector de direccions accessibles
  bool danger(VI &near_ork, Unit &ork) {
    bool ret = false;
    for (int i = -3; i < 4; ++i) {
      for (int j = -3; j < 4; ++j) {
        int dist  = abs(i-j);
        if (dist < 4 and dist != 0 and dangerous_cell(ork, Pos(ork.pos.i+i, ork.pos.j+j))) {
          int prox = proximitat(i, j);
          if (i > 0 and prox > near_ork[2]) near_ork[2] = prox;
          if (i < 0 and prox > near_ork[0]) near_ork[0] = prox;
          if (j > 0 and prox > near_ork[1]) near_ork[1] = prox;
          if (j < 0 and prox > near_ork[3]) near_ork[3] = prox;
        }
      }
    }
    
    if (near_ork[0] and near_ork[1] and near_ork[2] and near_ork[3]) suicide(ork, near_ork);
    return near_ork[0] or near_ork[1] or near_ork[2] or near_ork[3];
    
    
       
    --p.i;
    ret = update_danger_adjacent(near_ork, p, ork, 0) or ret;
    --p.i;
    if (p.i >= 0 and dangerous_cell(ork, p)) {
      ret = true;
      near_ork[0] = false;
    }
    ++p.i;
    ++p.j;
    if (dangerous_cell(ork, p)) {
      ret = true;
      near_ork[0] = near_ork[1] = false;
    }
    ++p.i;
    ret = update_danger_adjacent(near_ork, p, ork, 1) or ret;
    ++p.j;
    if (p.j < cols() and dangerous_cell(ork, p)) {
      ret = true;
      near_ork[1] = false;
    }
    --p.j;
    ++p.i;
    if (dangerous_cell(ork, p)) {
      ret = true;
      near_ork[1] = near_ork[2] = false;
    }
    --p.j;
    ret = update_danger_adjacent(near_ork, p, ork, 2) or ret;
    ++p.i;
    if (p.i < rows() and dangerous_cell(ork, p)) {
      ret = true;
      near_ork[2] = false;
    }
    --p.i;
    --p.j;
    if (dangerous_cell(ork, p)) {
      ret = true;
      near_ork[2] = near_ork[3] = false;
    }
    --p.i;
    ret = update_danger_adjacent(near_ork, p, ork, 3) or ret;
    --p.j;
    if (p.j >= 0 and dangerous_cell(ork, p)) {
      ret = true;
      near_ork[3] = false;
    }
    --p.i;
    ++p.j;
    if (dangerous_cell(ork, p)) {
      ret = true;
      near_ork[0] = near_ork[3] = false;
    }
  }
  
  // Si alguna posicio es prohibida, retorna quina es la millor direccio a la que moure's
  Dir best_dir_in_danger(VB &near_ork, Unit &ork) {
    Pos p = ork.pos;
    int min = -1, in_min = -1;
    if (not near_ork[0]) {
      --p.i;
      dijkstra(p);
      min = cost(cell(p).type) + Dirs[p.i][p.j].second;
      in_min = 0;
      ++p.i;
    }
    if (not near_ork[1]) {
      ++p.j;
      dijkstra(p);
      if (min == -1 or cost(cell(p).type) + Dirs[p.i][p.j].second < min) {
        min = cost(cell(p).type) + Dirs[p.i][p.j].second;
        in_min = 1;
      }
      --p.j;
    }
    if (not near_ork[2]) {
      ++p.i;
      dijkstra(p);
      if (min == -1 or cost(cell(p).type) + Dirs[p.i][p.j].second < min) {
        min = cost(cell(p).type) + Dirs[p.i][p.j].second;
        in_min = 2;
      }
      --p.i;
    }
    if (not near_ork[3]) {
      --p.j;
      dijkstra(p);
      if (min == -1 or cost(cell(p).type) + Dirs[p.i][p.j].second < min) {
        min = cost(cell(p).type) + Dirs[p.i][p.j].second;
        in_min = 3;
      }
      ++p.j;
    }
    return num_dir(in_min);
  }
  
  // Retorna si hi ha una victima a la posicio indicada
  bool victim(Unit &ork, Pos &p) {
    return cell(p).unit_id != -1 and unit(cell(p).unit_id).player != me() and unit(cell(p).unit_id).health < ork.health + cost(cell(p).type);
  }
  
  // Falta fer algo amb aquesta funcio
  // Estableix si "a" es millor direccio per anar en cas que totes dues continguin una victima
  bool better_kill(Unit &ork, Dir a, Dir b) {
    if (b == NONE) return true;
    if (a == NONE) return false;
    return random(0, 1);
  }
  
  // Retorna si hi ha alguna victima a una posicio adjacent i actualitza el vector per indicar quina
  Dir adjacent_victim(Unit &ork, VI &near_ork, Dir &d) {
    Dir ret = NONE;
    for (int i = -3; i < 4; ++i) {
      for (int j = -3; j < 4; ++j) {
        int dist  = abs(i-j);
        if (dist < 4 and dist != 0 and victim(ork, Pos(ork.pos.i+i, ork.pos.j+j))) {
          int prox = proximitat(i, j);
          if (i > 0 and prox > near_ork[2]) near_ork[2] = prox;
          if (i < 0 and prox > near_ork[0]) near_ork[0] = prox;
          if (j > 0 and prox > near_ork[1]) near_ork[1] = prox;
          if (j < 0 and prox > near_ork[3]) near_ork[3] = prox;
        }
      }
    }
    
    if ((int(near_ork[0]) + int(near_ork[1]) + int(near_ork[2]) + int(near_ork[3])) ) suicide(ork, near_ork);
    return d = ret;
  }
  
  void conquerir(Unit &ork) {
//    Dir d = near_priority(ork);
//    if (d != NONE) {}
  }*/


/*
      for (int i1 = 0; i1 < 70; ++i1) {
        for (int j1 = 0; j1 < 70; ++j1) {
          if (Dirs[i1][j1].first == NONE) cerr << '.';
          else if (Dirs[i1][j1].first == TOP) cerr << "T";
          else if (Dirs[i1][j1].first == BOTTOM) cerr << "B";
          else if (Dirs[i1][j1].first == LEFT) cerr << "L";
          else if (Dirs[i1][j1].first == RIGHT) cerr << "R";
        }
        cerr << endl;
      } */

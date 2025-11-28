#include "Player.hh"
#include <queue>

/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME pastori


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

  Dir vuelta (Pos p, Pos next, const vector< vector<Dir>>& visitados) {
    Pos act = next;
    Dir d = None;
    while (visitados[act.i][act.j] != None) {
      if (visitados[act.i][act.j] == Bottom) {
        d = Bottom;
        act += Top;
      }
      else if (visitados[act.i][act.j] == Top) {
        d = Top;
        act += Bottom;
      }
      else if (visitados[act.i][act.j] == Right) {
        d = Right;
        act += Left;
      }
      else if (visitados[act.i][act.j] == Left) {
        d = Left;
        act += Right;
      }
      else if (visitados[act.i][act.j] == BR) {
        d = BR;
        act += TL;
      }
      else if (visitados[act.i][act.j] == LB) {
        d = LB;
        act += RT;
      }
      else if (visitados[act.i][act.j] == TL) {
        d = TL;
        act += BR;
      }
      else if (visitados[act.i][act.j] == RT) {
        d = RT;
        act += LB;
      }
    }
    return d;
  }

  bool hellhound(Pos p) {
    vector<Dir> dir = {BR, Bottom, LB, RT, Top, TL, Left, Right};
    
    for (int i = 0; i < 8; ++i) {
      Pos o = p;
      for (int j = 0; j < 3; ++j) {
        o += dir[i];
        if (pos_ok(o)) {
          Cell c = cell(o);
          if (c.id != -1 and unit(c.id).type == Hellhound) return true;
        }

      }
    }
    return false;
  }

  bool furys (Pos p) {
    vector<Dir> dir = {BR, Bottom, LB, RT, Top, TL, Left, Right};
    
    for (int i = 0; i < 8; ++i) {
      Pos o = p;
      for (int j = 0; j < 2; ++j) {
        o += dir[i];
        if (pos_ok(o)) {
          Cell c = cell(o);
          if (c.id != -1 and unit(c.id).type == Furyan and unit(c.id).player != me()) return true;
        }

      }
    }
    return false;
  }
  
  bool sun (Pos p) {
    
    Pos o = p;
    for (int j = 0; j < 3; ++j) {
        o += Left;
        if (pos_ok(o) and daylight(o) ) return true;

    }
    return false;
  }
  
  bool necro(Pos p) {
    vector<Dir> dir = {BR, Bottom, LB, RT, Top, TL, Left, Right};
    
    for (int i = 0; i < 8; ++i) {
      Pos o = p;
      for (int j = 0; j < 3; ++j) {
        o += dir[i];
        if (pos_ok(o)) {
          Cell c = cell(o);
          if (c.id != -1 and unit(c.id).type == Necromonger) return true;
        }
      }
    }
    return false;
  }
  
  Dir bfs_gema (Pos p) {
    vector< vector<Dir>> visitados (40, vector<Dir> (80, DirSize));

    queue<Pos> Q;
    Q.push(p);
    visitados[p.i][p.j] = None;

    while (not Q.empty()) {
      Pos act = Q.front();
      Q.pop();
      for (int d = 0; d < 8; ++d) {
        Pos next = act + Dir(d);
        if (pos_ok(next) and visitados[next.i][next.j] == DirSize) {
          visitados[next.i][next.j] = Dir(d);
          Cell c = cell(next);
          if (sun(next)) return Right;
          if (not necro(next)) {
            if (c.gem) return vuelta(p,next,visitados);
            /*if (c.id == -1)*/ Q.push(next);
          }
        }
      }
    }
    return None;
  }

  Dir bfs_furyans (Pos p) {
    vector< vector<Dir>> visitados (40, vector<Dir> (80, DirSize));

    queue<Pos> Q;
    Q.push(p);
    visitados[p.i][p.j] = None;
    while (not Q.empty()) {
      Pos act = Q.front();
      Q.pop();
      for (int d = 0; d < 8; ++d) {
        Pos next = act + Dir(d);
        if (pos_ok(next) and visitados[next.i][next.j] == DirSize) {
          visitados[next.i][next.j] = Dir(d);
          Cell c = cell(next);

          if (c.type != Rock and not hellhound(next)) {
            if (c.id != -1 and unit(c.id).player != me()) {
              return vuelta(p,next,visitados);
            }

            if(c.id == -1) Q.push(next);
          }
        }
      }
    }
    return None;
  }

  Dir bfs_pioneers (Pos p, int i, int size) {
    vector < vector<Dir>> visitados (40, vector<Dir>(80, DirSize));

    queue<Pos> Q;
    Q.push(p);
    visitados[p.i][p.j] = None;
    while (not Q.empty()) {
      Pos act = Q.front();
      Q.pop();
      Cell a = cell(act);
      Pos sub (act.i, act.j, 1);
      if (a.type == Elevator) {
        if (act.k == 0 and not daylight(sub) and not hellhound(p) and round() < 100)
          return Up;
      }
      if (act.k == 1) return bfs_gema (act);
      for (int d = 0; d < 8; ++d) {
        Pos next = act + Dir(d);
        if (pos_ok(next) and visitados[next.i][next.j] == DirSize) {
          visitados[next.i][next.j] = Dir(d);
          Cell c = cell(next);

          if (c.type != Rock and not hellhound(next)) {
            if (c.owner != me() and c.id == -1 and not furys(next)) {
              return vuelta (p, next, visitados);
            }
            if (c.id == -1  and not furys(next)) Q.push(next);
          }
          
        }
      }
    }
    return None;
  }

  void move_furyans () {
    vector<int> id_furyans = furyans(me());

    for (int i = 0; i < int(id_furyans.size()); ++i) {
        Pos pos_act = unit(id_furyans[i]).pos;
        int id = id_furyans[i];
        Dir d = bfs_furyans(pos_act);
        command(id, d);
    }

  }

  void move_pioneers () {
    vector<int> id_pioneers = pioneers(me());
    for (int i = 0; i < int(id_pioneers.size()); ++i) {
      Pos pos_act = unit(id_pioneers[i]).pos;
      Dir d = bfs_pioneers(pos_act, i, id_pioneers.size());
      command(id_pioneers[i], d);
    }
  }

  /**
   * Play method, invoked once per each round.
   */
  virtual void play () {
    move_furyans();
    move_pioneers();
  }

};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
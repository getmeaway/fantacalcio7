<?php
class RealTeam {
  var $id;
  var $name;

  function __construct($id, $name) {
    $this->id = $id;
    $this->name = $name;
  }

  static function get($id) {
    $team = null;
    $query = db_select("fanta_real_teams", "r");
    $query->condition("rt_id", $id);
    $query->fields("r");
    $result = $query->execute();
    foreach ($result as $row) {
      $team = new RealTeam($row->rt_id, $row->name);
    }
    
    return $team;
  }

  static function exists($id) {
    return self::get($id) != null;
  }

  static function all() {
    $teams = array();
    $query = db_select("fanta_real_teams", "r");
    $query->fields("r");
    $result = $query->execute();
    foreach ($result as $row) {
      $teams[$row->rt_id] = new RealTeam($row->rt_id, $row->name);
    }
    
    return $teams;
  }
  
  static function allNames() {
    $teams = array();
    $query = db_select("fanta_real_teams", "r");
    $query->fields("r");
    $result = $query->execute();
    foreach ($result as $row) {
      $teams[$row->rt_id] = strtolower($row->name);
    }
  
    return $teams;
  }

  static function allActive() {
    $teams = array();
    $query = db_select("fanta_teams", "t");
    $query->condition("active", 1);
    $query->orderBy("name");
    $query->fields("t");
    $result = $query->execute();
    foreach ($result as $row) {
      array_push($teams, new Team($row->t_id, $row->name, $row->uid));
    }
    
    return $teams;
  }

  static function allByGroup($g_id) {
    $teams = array();
    $query = db_select("fanta_teams", "t");
    $query->join("fanta_teams_groups", "g", "g.t_id =  t.t_id");
    $query->fields("t");
    $query->condition("g_id", $g_id);
    $result = $query->execute();
    foreach ($result as $row) {
      array_push($teams, new Team($row->t_id, $row->name, $row->uid));
    }
    
    return $teams;
  }

  static function allByUser($u_id) {
    $teams = array();
    $query = db_select("fanta_teams", "t");
    // $query->join("fanta_teams_groups", "g", "g.t_id = t.t_id");
    $query->fields("t");
    $query->condition("t.uid", $u_id);
    $result = $query->execute();
    foreach ($result as $row) {
      array_push($teams, new Team($row->t_id, $row->name, $row->uid));
    }
    
    return $teams;
  }

  function getSquad() {
    $squad = array();
    
    $round = Round::getLast();
    
    $selled_players = array();
    $query = db_select("fanta_squads", "s");
    $query->condition("s.t_id", $this->id);
    $query->condition("s.status", -1);
    $query->fields("s", array("pl_id", "timestamp"));
    $result = $query->execute();
    while ($row = $result->fetchObject()) {
      if (!isset($selled_players[$row->pl_id]) || $selled_players[$row->pl_id] < $row->timestamp)
        $selled_players[$row->pl_id] = $row->timestamp;
    }
    
    $query = db_select("fanta_squads", "s");
    $query->join("fanta_players", "p", "s.pl_id = p.pl_id");
    $query->join("fanta_players_rounds", "r", "s.pl_id = r.pl_id");
    $query->join("fanta_real_teams", "rt", "rt.rt_id = r.rt_id");
    $query->condition("s.t_id", $this->id);
    $query->condition("r.round", $round);
    $query->fields("s");
    $query->fields("p", array("name", "role"));
    $query->fields("r", array("quotation"));
    $query->addField("rt", "name", "team_name");
    $query->orderBy("role", "ASC");
    $query->orderBy("name", "ASC");
    $result = $query->execute();
    while ($row = $result->fetchObject()) {
      if (!array_key_exists($row->pl_id, $selled_players) || $selled_players[$row->pl_id] < $row->timestamp)
        $squad[$row->pl_id] = $row;
    }
    
    return $squad;
  }

  function numWin() {
    $query = db_select("fanta_real_teams_matches", "m");
    $query->condition("winner_id", $this->id);
    $query->condition("played", 1);
    $query->addExpression("COUNT(m_id)", "n");
    
    $result = $query->execute();
    return $result->fetchObject()->n;
  }

  function numLost() {
    $query = db_select("fanta_real_teams_matches", "m");
    
    $or = db_or()->condition('rt1_id', $this->id)->condition('rt2_id', $this->id);
    
    $query->condition($or);
    $query->condition("winner_id", $this->id, "<>");
    $query->condition("winner_id", "-1", "<>");
    $query->condition("played", 1);
    $query->addExpression("COUNT(m_id)", "n");
    
    $result = $query->execute();
    
    return $result->fetchObject()->n;
  }

  function numDraw() {
    $query = db_select("fanta_real_teams_matches", "m");
    
    $or = db_or()->condition('rt1_id', $this->id)->condition('rt2_id', $this->id);
    
    $query->condition($or);
    $query->condition("winner_id", "-1");
    $query->condition("played", 1);
    $query->addExpression("COUNT(m_id)", "n");
    
    $result = $query->execute();
    
    return $result->fetchObject()->n;
  }

  function goalsFor() {
    $query = db_select("fanta_real_teams_matches", "m");
    $query->condition("rt1_id", $this->id);
    $query->condition("played", 1);
    $query->addExpression("SUM(goals_1)", "n");
    
    $result = $query->execute();
    $goals_home = $result->fetchObject()->n;
    
    $query = db_select("fanta_real_teams_matches", "m");
    $query->condition("rt2_id", $this->id);
    $query->condition("played", 1);
    $query->addExpression("SUM(goals_2)", "n");
    
    $result = $query->execute();
    $goals_away = $result->fetchObject()->n;
    
    return $goals_home + $goals_away;
  }

  function goalsAgainst() {
    $query = db_select("fanta_real_teams_matches", "m");
    $query->condition("rt1_id", $this->id);
    $query->condition("played", 1);
    $query->addExpression("SUM(goals_2)", "n");
    
    $result = $query->execute();
    $goals_home = $result->fetchObject()->n;
    
    $query = db_select("fanta_real_teams_matches", "m");
    $query->condition("rt2_id", $this->id);
    $query->condition("played", 1);
    $query->addExpression("SUM(goals_1)", "n");
    
    $result = $query->execute();
    $goals_away = $result->fetchObject()->n;
    
    return $goals_home + $goals_away;
  }

  static function getMatches() {
    $matches = array();
    
    $query = db_select("fanta_real_teams_matches", "m");
    $query->join("fanta_real_teams", "rt1", "rt1.rt_id = m.rt1_id");
    $query->join("fanta_real_teams", "rt2", "rt2.rt_id = m.rt2_id");
    $query->join("fanta_rounds", "r", "r.round = m.round");
    $query->fields("m");
    $query->addField("rt1", "name", "home_team");
    $query->addField("rt2", "name", "away_team");
    $query->addField("r", "date", "date");
    
    $result = $query->execute();
    
    while ($row = $result->fetchObject()) {
      $match = new Match();
      $match->id = $row->m_id;
      $match->rt1_id = $row->rt1_id;
      $match->rt2_id = $row->rt2_id;
      $match->home_team = $row->home_team;
      $match->away_team = $row->away_team;
      $match->date = $row->date;
      $match->round = $row->round;
      $match->goals_1 = $row->goals_1;
      $match->goals_2 = $row->goals_2;
      $match->played = $row->played;
      // $m_id = $row->m_id;
      // $matches[$m_id] = $row;
      // $c_id = $groups[$row->g_id];
      // $matches[$m_id]->date = $dates[$c_id][$row->round];
      
      $matches[$row->round][$match->id] = $match;
    }
    
    ksort($matches);
    
    return $matches;
  }
}

<?php
class Match {
  var $id;
  var $round;
  var $competition;
  var $group;
  var $home_team;
  var $away_team;
  var $home_points;
  var $away_points;
  var $home_modifier_0;
  var $away_modifier_0;
  var $home_modifier_1;
  var $away_modifier_1;
  var $home_modifier_2;
  var $away_modifier_2;
  var $home_modifier_3;
  var $away_modifier_3;
  var $home_bonus;
  var $away_bonus;
  var $home_total;
  var $away_total;
  var $home_goals;
  var $away_goals;
  var $winner;
  var $played;

  static function get($id) {
  }

  static function getByGroup($g_id) {
    $matches = array();
    
    $query = db_select("fanta_matches", "m");
    $query->condition("m.g_id", $g_id);
    $query->join("fanta_teams", "t1", "t1.t_id = m.t1_id");
    $query->join("fanta_teams", "t2", "t2.t_id = m.t2_id");
    $query->join("fanta_groups", "g", "g.g_id = m.g_id");
    $query->join("fanta_rounds_competitions", "rc", "rc.competition_round = m.round AND rc.c_id = g.c_id");
    $query->join("fanta_rounds", "r", "r.round = rc.round");
    $query->fields("m");
    $query->addField("t1", "name", "home_team");
    $query->addField("t2", "name", "away_team");
    $query->addField("r", "date", "date");
    $query->addField("rc", "round_label", "round_label");
    
    $result = $query->execute();
    
    while ($row = $result->fetchObject()) {
      $match = new Match();
      $match->id = $row->m_id;
      $match->home_team = $row->home_team;
      $match->away_team = $row->away_team;
      $match->date = $row->date;
      $match->round = $row->round;
      $match->round_label = $row->round_label;
      $match->played = $row->played;
      $match->goals_1 = $row->goals_1;
      $match->goals_2 = $row->goals_2;
      $match->tot_1 = $row->tot_1;
      $match->tot_2 = $row->tot_2;
      // $m_id = $row->m_id;
      // $matches[$m_id] = $row;
      // $c_id = $groups[$row->g_id];
      // $matches[$m_id]->date = $dates[$c_id][$row->round];
      
      $matches[$row->round][$match->id] = $match;
    }
    
    return $matches;
  }
  
  static function getMatchesByRound($round) {
    
    $query = db_select("fanta_matches", "m");
    $query->join("fanta_teams", "t1", "t1.t_id = m.t1_id");
    $query->join("fanta_teams", "t2", "t2.t_id = m.t2_id");
    $query->join("fanta_groups", "g", "g.g_id = m.g_id");
    $query->join("fanta_rounds_competitions", "rc", "rc.competition_round = m.round AND rc.c_id = g.c_id");
    $query->join("fanta_rounds", "r", "r.round = rc.round");
    $query->fields("m");
    $query->addField("t1", "name", "home_team");
    $query->addField("t2", "name", "away_team");
    $query->addField("r", "date", "date");
    $query->addField("rc", "round_label", "round_label");
    $query->addField("g", "c_id", "c_id");
    
    $query->condition("r.round", $round);
    
    $result = $query->execute();
    
    while ($row = $result->fetchObject()) {
      $match = new Match();
      $match->id = $row->m_id;
      $match->home_team = $row->home_team;
      $match->away_team = $row->away_team;
      $match->date = $row->date;
      $match->round = $row->round;
      $match->round_label = $row->round_label;
      $match->played = $row->played;
      $match->goals_1 = $row->goals_1;
      $match->goals_2 = $row->goals_2;
      $match->tot_1 = $row->tot_1;
      $match->tot_2 = $row->tot_2;
      $match->c_id = $row->c_id;
      // $m_id = $row->m_id;
      // $matches[$m_id] = $row;
      // $c_id = $groups[$row->g_id];
      // $matches[$m_id]->date = $dates[$c_id][$row->round];
    
      $matches[$match->id] = $match;
    }
    
    return $matches;
  }
}
<?php

/**
 * @file fantamercato.autocomplete.inc
 * autocomplete functions for Fantamercato.
 *
 */

function fantamercato_autocomplete_players_group($g_id, $name) {
  $players = array();
  $roles = array("P", "D", "C", "A");
  $round = Round::getNext();

  $teams = Team::allByGroup($g_id);

//   $sql = 'SELECT p.pl_id, p.name, t.long_name AS team, p.role
//           FROM {fanta_players} p, {fanta_players_teams} r, {fanta_real_teams} t
//           WHERE p.pl_id = r.pl_id 
//           AND t.rt_id = r.rt_id
//           AND r.round = "%d"
//           AND LOWER(p.name) LIKE LOWER("%s%%")
//           AND p.pl_id NOT IN (
//             SELECT pl_id FROM {fanta_squads} WHERE t_id IN (' . $teams_list . ')
//             AND in_team = 1) 		
//           ORDER BY p.name';
  //$result = db_query_range($sql, $round, addslashes($name), 0, 10);
  
  $subquery = db_select("fanta_squads", "s")
  ->condition("t_id", array_keys($teams), "IN")
  ->condition("status", 1)
  ->fields("s", array("pl_id")); 
  
  $query = db_select("fanta_players", "p");
  $query->join("fanta_players_rounds", "r", "r.pl_id = p.pl_id");
  $query->join("fanta_real_teams", "rt", "rt.rt_id = r.rt_id");
  $query->fields("p");
  $query->addField("rt", "name", "team");
  $query->condition("r.round", $round);
  $query->condition("p.name", db_like(addslashes($name)) . '%', 'LIKE');
  $query->condition("p.pl_id", $subquery, "NOT IN");
  $query->orderBy("p.name");
  $query->range(0, 10);
  
  $result = $query->execute(); //print $query;
  
  foreach ($result as $row) {
    $id = $row->pl_id . ", " . htmlspecialchars(stripslashes($row->name)) . " (" . ucwords($row->team) . ") - " . $roles[$row->role];
    $players[$id] = $id;
  }
  print drupal_json_output($players);
  exit();
}

function fantamercato_autocomplete_players_team($g_id, $role, $name) {
  $players = array();
  $roles = array("P", "D", "C", "A");
  $round = Round::getNext();

  $teams = Team::allByGroup($g_id);

//   $sql = 'SELECT p.pl_id, p.name, r.long_name AS team
//       FROM {fanta_players} p, {fanta_players_teams} t, {fanta_real_teams} r
//       WHERE t.pl_id = p.pl_id
//       AND r.rt_id = t.rt_id
//       AND t.round = "%d"
//       AND LOWER(p.name) LIKE LOWER("%s%%")
//       AND p.role = "%d"
//       AND p.pl_id NOT IN 
// 	(SELECT pl_id FROM {fanta_squads} 
// 	 WHERE t_id IN (' . $teams_list . ')
// 	 AND in_team = 1)
//       ORDER BY p.name';
  
  $subquery = db_select("fanta_squads", "s")
  ->condition("t_id", array_keys($teams), "IN")
  ->condition("status", 1)
  ->fields("s", array("pl_id"));
  
 $query = db_select("fanta_players", "p");
  $query->join("fanta_players_rounds", "r", "r.pl_id = p.pl_id");
  $query->join("fanta_real_teams", "rt", "rt.rt_id = r.rt_id");
  $query->fields("p");
  $query->addField("rt", "name", "team");
  $query->condition("r.round", $round);
  $query->condition("p.role", $role);
  $query->condition("p.name", db_like(addslashes($name)) . '%', 'LIKE');
  $query->condition("p.pl_id", $subquery, "NOT IN");
  $query->orderBy("p.name");
  $query->range(0, 10);
  
  $result = $query->execute(); //print $query;
  
  foreach ($result as $row) {
    $id = $row->pl_id . ", " . htmlspecialchars(stripslashes($row->name)) . " (" . ucwords($row->team) . ") - " . $roles[$row->role];
    $players[$id] = $id;
  }
  print drupal_json_output($players);
  exit();
}

function fantamercato_autocomplete_players_swaps($g_id, $name) {
  $players = array();
  $roles = array("P", "D", "C", "A");
  $round = Round::getNext();

  $teams = Team::allByGroup($g_id);

//   $sql = 'SELECT p.pl_id, p.name, p.role, rt.long_name as team, r.t_id
//       FROM {fanta_players} p, {fanta_players_teams} t, {fanta_real_teams} rt, {fanta_squads} r
//       WHERE t.pl_id = p.pl_id
//       AND r.pl_id = p.pl_id
//       AND rt.rt_id = t.rt_id
//       AND t.round = "%d"
//       AND LOWER(p.name) LIKE LOWER("%s%%")
//       AND r.t_id IN (' . $teams_list . ')
//       AND r.in_team = 1 
//       ORDER BY p.name';
  
  
  $query = db_select("fanta_players", "p");
  $query->join("fanta_players_rounds", "r", "r.pl_id = p.pl_id");
  $query->join("fanta_real_teams", "rt", "rt.rt_id = r.rt_id");
  $query->join("fanta_squads", "s", "s.pl_id = p.pl_id");
  $query->join("fanta_teams", "t", "s.t_id = t.t_id");
  $query->fields("p");
  $query->fields("s", array("t_id"));
  $query->addField("rt", "name", "real_team");
  $query->addField("t", "name", "team");
  $query->condition("r.round", $round);
  $query->condition("p.name", db_like(addslashes($name)) . '%', 'LIKE');
  $query->condition("s.status", 1);
  $query->condition("s.t_id", array_keys($teams), "IN");
  $query->orderBy("p.name");
  $query->range(0, 10);
  
  $result = $query->execute(); //print $query;
  
  foreach ($result as $row) {
    $id = $row->pl_id . "," . $row->t_id . ", " . htmlspecialchars(stripslashes($row->name)) . " (" . ucwords($row->real_team) . ") - " . $roles[$row->role] . " - " . $row->team;
    $players[$id] = $id;
  }
  print drupal_json_output($players);
  exit();
}

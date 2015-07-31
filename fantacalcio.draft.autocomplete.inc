<?php

/**
 * @file fantamercato.autocomplete.inc
 * autocomplete functions for Fantamercato.
 *
 */

function fantamercato_autocomplete_players_group($g_id, $name) {
  $players = array();
  $roles = array("P", "D", "C", "A");
  $round = db_result(db_query("SELECT MAX(round) FROM {fanta_players_teams}"));

  $teams_list = get_group_teams_list($g_id);

  $sql = 'SELECT p.pl_id, p.name, t.long_name AS team, p.role
          FROM {fanta_players} p, {fanta_players_teams} r, {fanta_real_teams} t
          WHERE p.pl_id = r.pl_id 
          AND t.rt_id = r.rt_id
          AND r.round = "%d"
          AND LOWER(p.name) LIKE LOWER("%s%%")
          AND p.pl_id NOT IN (
            SELECT pl_id FROM {fanta_squads} WHERE t_id IN (' . $teams_list . ')
            AND in_team = 1) 		
          ORDER BY p.name';
  $result = db_query_range($sql, $round, addslashes($name), 0, 10);
  while ($row = db_fetch_object($result)) {
    $id = $row->pl_id . ", " . htmlspecialchars(str_replace("'", "&rsquo;", stripslashes($row->name))) . " (" . ucwords($row->team) . ") - " . $roles[$row->role];
    $players[$id] = $id;
  }
  print drupal_to_js($players);
  exit();
}

function fantamercato_autocomplete_players_team($g_id, $role, $name) {
  $players = array();
  $round = db_result(db_query("SELECT MAX(round) FROM {fanta_players_teams}"));

  $teams_list = get_group_teams_list($g_id);

  $sql = 'SELECT p.pl_id, p.name, r.long_name AS team
      FROM {fanta_players} p, {fanta_players_teams} t, {fanta_real_teams} r
      WHERE t.pl_id = p.pl_id
      AND r.rt_id = t.rt_id
      AND t.round = "%d"
      AND LOWER(p.name) LIKE LOWER("%s%%")
      AND p.role = "%d"
      AND p.pl_id NOT IN 
	(SELECT pl_id FROM {fanta_squads} 
	 WHERE t_id IN (' . $teams_list . ')
	 AND in_team = 1)
      ORDER BY p.name';
  $result = db_query_range($sql, $round, addslashes($name), $role, 0, 10);
  while ($row = db_fetch_object($result)) {
    $id = $row->pl_id . ", " . filter_xss(str_replace("'", "&rsquo;", stripslashes($row->name))) . " (" . ucwords($row->team) . ")";
    $players[$id] = $id;
  }
  print drupal_to_js($players);
  exit();
}

function fantamercato_autocomplete_players_swaps($g_id, $name) {
  $players = array();
  $roles = array("P", "D", "C", "A");
  $round = db_result(db_query("SELECT MAX(round) FROM {fanta_players_teams}"));

  $teams_list = get_group_teams_list($g_id);
  $teams = get_teams();

  $sql = 'SELECT p.pl_id, p.name, p.role, rt.long_name as team, r.t_id
      FROM {fanta_players} p, {fanta_players_teams} t, {fanta_real_teams} rt, {fanta_squads} r
      WHERE t.pl_id = p.pl_id
      AND r.pl_id = p.pl_id
      AND rt.rt_id = t.rt_id
      AND t.round = "%d"
      AND LOWER(p.name) LIKE LOWER("%s%%")
      AND r.t_id IN (' . $teams_list . ')
      AND r.in_team = 1 
      ORDER BY p.name';
  $result = db_query_range($sql, $round, addslashes($name), 0, 10);
  while ($row = db_fetch_object($result)) {
    $id = $row->pl_id . ", " . $row->t_id . ", " . (str_replace("'", "&rsquo;", stripslashes($row->name))) . " (" . $teams[$row->t_id]->name . " : " . ucwords($row->team) . ") - " . $roles[$row->role];
    $players[$id] = $id;
  }
  print drupal_to_js($players);
  exit();
}

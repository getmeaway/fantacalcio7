<?php

/**
* @file fantacalcio.standings.inc
* 
*/
function fantacalcio_main_league_standings() {
  drupal_set_title(variable_get("fantacalcio_main_league", "") . " - " . t("Classifica"));
  
  drupal_add_js(drupal_get_path("module", "fantacalcio") . "/js/jquery.tablesorter.js");
  drupal_add_js(drupal_get_path("module", "fantacalcio") . "/js/jquery.metadata.js");
  drupal_add_js("jQuery(window).ready(function() {jQuery(\".standings	\").tablesorter({cssAsc: \"sort-asc\", cssDesc: \"sort-desc\", cssHeader: \"sort-header\"});})", "inline");
  
  $output = array();
  
  $teams = RealTeam::all();
  $points = array();
  $goals_for = array();
  $points_for = array();
  
  if ($teams) {
    
    foreach ($teams as $t_id => $team) {
      
      $teams[$t_id]->n_win = 0 + $team->numWin();
      $teams[$t_id]->n_lost = 0 + $team->numLost();
      $teams[$t_id]->n_draw = 0 + $team->numDraw();
      $teams[$t_id]->goals_in = 0 + $team->goalsFor();
      $teams[$t_id]->goals_against = 0 + $team->goalsAgainst();
      $teams[$t_id]->pts = $teams[$t_id]->n_win * 3 + $teams[$t_id]->n_draw * 1;
      $points[$t_id] = $teams[$t_id]->n_win * 3 + $teams[$t_id]->n_draw * 1;
      $goals_for[$t_id] = $teams[$t_id]->goals_in;
    }
    
    array_multisort($points, SORT_DESC, $goals_for, SORT_DESC, $teams);
    
    $header = array(
      "", 
      t("Squadra"), 
      t("Pts"), 
      t("Vinte"), 
      t("Perse"), 
      t("Pari"), 
      t("Gol f."), 
      t("Gol s."), 
      t("Diff Gol"));
    
    $i = 0;
    $rows = array();
    
    foreach ($teams as $t_id => $team) {
      
      $i++;
      
      $rows[$t_id][] = array("data" => $i, "data-title" => t("Posizione"));
      $rows[$t_id][] = array(
        "data" => l($team->name, "squadre/" . $team->id), 
        "data-title" => t("Squadra"));
      $rows[$t_id][] = array("data" => $team->pts, "data-title" => t("Punti"));
      $rows[$t_id][] = array("data" => $team->n_win, "data-title" => t("Vinte"));
      $rows[$t_id][] = array(
        "data" => $team->n_lost, 
        "data-title" => t("Perse"));
      $rows[$t_id][] = array("data" => $team->n_draw, "data-title" => t("Pari"));
      $rows[$t_id][] = array(
        "data" => $team->goals_in, 
        "data-title" => t("gol f."));
      $rows[$t_id][] = array(
        "data" => $team->goals_against, 
        "data-title" => t("Gol s."));
      $rows[$t_id][] = array(
        "data" => ($team->goals_in - $team->goals_against), 
        "data-title" => t("Diff. Gol"));
    }
  }
  
  return theme("table", array(
    "header" => $header, 
    "rows" => $rows, 
    "attributes" => array(
      "class" => "collapse-table table table-responsive table-hover standings", 
      "id" => "main_league_standings")));
}

function fantacalcio_main_league_calendar() {
  drupal_set_title(variable_get("fantacalcio_main_league", "") . " - " . t("Calendario"));
  
  $matches = RealTeam::getMatches();
  
  $rounds_list = array();
  foreach ($matches as $round => $round_matches) {
    $rounds_list[$round] = $round;
  }
  ksort($rounds_list);
  
  return theme("calendar", array(
    "matches" => $matches,
  	"is_main_competition" => true,
    "rounds_list" => $rounds_list)); // , "last_round" => $last_round, "next_round" => $next_round)); //"ABC";//theme_table(array("header" => $header, "rows" => $rows, "attributes" => array("class" => "table table-responsive tablesorter tablesorter-blue classifica", "id" => "classifica_" . $g_id), "caption" => "", "sticky" => "", "empty" => "", "colgroups" => array()));
}
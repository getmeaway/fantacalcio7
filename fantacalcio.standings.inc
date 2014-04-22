<?php
/**
* @file fantacalcio.standings.inc
* 
*/

function fantacalcio_classifica($c_name = '') {

  if (empty($c_name) || !competition_has_arg($c_name, "standings")) 
    $out = choose_competition(array("standings" => 1));

  else {
    drupal_set_title(check_plain("Classifica - " . ucwords($c_name)));
    $out = make_tabs('get_standing_table', $c_name, "standings");
  }

  return $out;

}

function get_standing_table($g_id) {

  #jquery tablesorter
  if (!is_mobile()) {
    drupal_add_css('sites/all/plugins/tablesorter/css/theme.blue.css', 'themes', 'all', TRUE);
    drupal_add_js('sites/all/plugins/tablesorter/js/jquery.tablesorter.js', 'footer');
  }

  global $user;

  $teams = get_teams_group($g_id);

  $c_id = get_competition_group($g_id);

  if ($teams) {

    foreach ($teams as $t_id => $team) {

    #$teams[$t_id]['n_played'] = 0 + get_num_played($t_id, $g_id);
    $teams[$t_id]->n_win = 0 + get_num_win($t_id, $g_id);
    $teams[$t_id]->n_lost = 0 + get_num_lost($t_id, $g_id);
    $teams[$t_id]->n_draw = 0 + get_num_draw($t_id, $g_id);
    $teams[$t_id]->goals_in = 0 + get_goals_in($t_id, $g_id);
    $teams[$t_id]->goals_against = 0 + get_goals_out($t_id, $g_id);
    $teams[$t_id]->points_in = 0 + get_points_in($t_id, $g_id);
    $teams[$t_id]->points_against = 0 + get_points_out($t_id, $g_id);
    $teams[$t_id]->pts = $teams[$t_id]->n_win * 3 + $teams[$t_id]->n_draw * 1;
    $pts[$t_id] = $teams[$t_id]->n_win * 3 + $teams[$t_id]->n_draw * 1;
    $gol_f[$t_id] = $teams[$t_id]->goals_in;
    $pts_f[$t_id] = $teams[$t_id]->points_in;
  }

  array_multisort($pts, SORT_DESC, $gol_f, SORT_DESC, $pts_f, SORT_DESC, $teams);

  if (!is_mobile()) {
    $js = "$(function() {
    $('#classifica_" . $g_id . "').tablesorter({sortList: [[2,1],[6,1],[9,1]], widgets: ['zebra'], headers: { 0:{sorter: false}, 1:{sorter: false}}});
  });  ";
    drupal_add_js($js, 'inline', 'footer');
  }

//  $out .= "\n<table id='classifica_" . $g_id . "' name='classifica_" . $g_id . "' class='table table-responsive tablesorter tablesorter-blue classifica' data-role=table data-mode=columntoggle data-column-btn-text='Espandi...' data-column-btn-theme=d>" .

  $header = array("", "Squadra", "Pts", "Vinte", "Perse", "Pari", "Gol f.", "Gol s.", "Diff Gol", "Pti f.", "Pti s.", "Diff Pti");

#  $out .= "\n<tbody>";

  $i = 0;

  $rows = array();

  foreach ($teams as $t_id => $team) {

    $i++;

    $team_class = ($team->t_id == $user->t_id) ? " class=bold" : "";

    $rows[$t_id][] = $i;
    $rows[$t_id][] = array("data" => l($team->name, "squadre/" . $team->t_id), "attributes" => array("class" => $team_class));
    $rows[$t_id][] = $team->pts;
    $rows[$t_id][] = $team->n_win;
    $rows[$t_id][] = $team->n_lost;
    $rows[$t_id][] = $team->n_draw;
    $rows[$t_id][] = $team->goals_in;
    $rows[$t_id][] = $team->goals_against;
    $rows[$t_id][] = ($team->goals_in - $team->goals_against);
    $rows[$t_id][] = $team->points_in;
    $rows[$t_id][] = $team->points_against;
    $rows[$t_id][] = ($team->points_in - $team->points_against);

  }

  }
  return theme_table(array("header" => $header, "rows" => $rows, "attributes" => array("class" => "table table-responsive tablesorter tablesorter-blue classifica", "id" => "classifica_" . $g_id), "caption" => "", "sticky" => "", "empty" => "", "colgroups" => array()));
}

function get_num_played($t_id, $g_id) {
  $sql = "SELECT count(*) as n FROM {fanta_matches} " .
      "WHERE (t1_id = :t_id OR t2_id = :t_id) " .
      "AND g_id = :g_id " .
      "AND played = 1";
  $result = db_query($sql, array(":t_id" => $t_id, ":g_id" => $g_id));
  return $result->fetchObject()->n;
}

function get_num_win($t_id, $g_id) {
  $sql = "SELECT count(*) as n FROM {fanta_matches} " .
      "WHERE winner_id = :t_id " .
      "AND g_id = :g_id " .
      "AND played = 1";
  $result = db_query($sql, array(":t_id" => $t_id, ":g_id" => $g_id));
  return $result->fetchObject()->n;
}

function get_num_lost($t_id, $g_id) {
  $sql = "SELECT count(*) as n FROM {fanta_matches} " .
      "WHERE (t1_id = :t_id OR t2_id = :t_id) " .
      "AND winner_id != :t_id " .
      "AND winner_id != '-1' " .
      "AND g_id = :g_id " .
      "AND played = 1";
  $result = db_query($sql, array(":t_id" => $t_id, ":g_id" => $g_id));
  return $result->fetchObject()->n;
}

function get_num_draw($t_id, $g_id) {
  $sql = "SELECT count(*) as n FROM {fanta_matches} " .
      "WHERE (t1_id = :t_id OR t2_id = :t_id)" .
      "AND winner_id = '-1' " .
      "AND g_id = :g_id " .
      "AND played = 1";
  $result = db_query($sql, array(":t_id" => $t_id, ":g_id" => $g_id));
  return $result->fetchObject()->n;
}

function get_goals_in($t_id, $g_id) {
  $sql = "SELECT sum(goals_1) as n FROM {fanta_matches} " .
      "WHERE t1_id = :t_id " .
      "AND played = 1 " .
      "AND g_id = :g_id ";
  $result = db_query($sql, array(":t_id" => $t_id, ":g_id" => $g_id));
  $goals_1 = $result->fetchObject()->n;

  $sql = "SELECT sum(goals_2) as n FROM {fanta_matches} " .
      "WHERE t2_id = :t_id " .
      "AND played = 1 " .
      "AND g_id = :g_id" ;
  $result = db_query($sql, array(":t_id" => $t_id, ":g_id" => $g_id));
  $goals_2 = $result->fetchObject()->n;
  return $goals_1 + $goals_2;
}

function get_goals_out($t_id, $g_id) {
  $sql = "SELECT sum(goals_1) as n FROM {fanta_matches} " .
      "WHERE t2_id = :t_id " .
      "AND played = 1 " .
      "AND g_id = :g_id ";
  $result = db_query($sql, array(":t_id" => $t_id, ":g_id" => $g_id));
  $goals_1 = $result->fetchObject()->n;

  $sql = "SELECT sum(goals_2) as n FROM {fanta_matches} " .
      "WHERE t1_id = :t_id " .
      "AND played = 1 " .
      "AND g_id = :g_id" ;
  $result = db_query($sql, array(":t_id" => $t_id, ":g_id" => $g_id));
  $goals_2 = $result->fetchObject()->n;
  return $goals_1 + $goals_2;
}

function get_points_in($t_id, $g_id) {
  $sql = "SELECT sum(tot_1) as n FROM {fanta_matches} " .
      "WHERE t1_id = :t_id " .
      "AND played = 1 " .
      "AND g_id = :g_id ";
  $result = db_query($sql, array(":t_id" => $t_id, ":g_id" => $g_id));
  $pt_1 = $result->fetchObject()->n;

  $sql = "SELECT sum(tot_2) as n FROM {fanta_matches} " .
      "WHERE t2_id = :t_id " .
      "AND played = 1 " .
      "AND g_id = :g_id" ;
  $result = db_query($sql, array(":t_id" => $t_id, ":g_id" => $g_id));
  $pt_2 = $result->fetchObject()->n;
  return $pt_1 + $pt_2;

}

function get_points_out($t_id, $g_id) {

  $sql = "SELECT sum(tot_1) as n FROM {fanta_matches} " .
      "WHERE t2_id = :t_id " .
      "AND played = 1 " .
      "AND g_id = :g_id ";
  $result = db_query($sql, array(":t_id" => $t_id, ":g_id" => $g_id));
  $pt_1 = $result->fetchObject()->n;

  $sql = "SELECT sum(tot_2) as n FROM {fanta_matches} " .
      "WHERE t1_id = :t_id " .
      "AND played = 1 " .
      "AND g_id = :g_id" ;
  $result = db_query($sql, array(":t_id" => $t_id, ":g_id" => $g_id));
  $pt_2 = $result->fetchObject()->n;
  return $pt_1 + $pt_2;

}
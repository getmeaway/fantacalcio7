<?php

/**
 * @file fantacalcio.stats.teams.inc
 *
 */

function team_stats($t_id) {

  #pt max, pt min, pt medio, gol medio, pt, vinte, perse , pareggiate, striscia pos, striscia neg,
  $max_pt = 0; 
  $min_pt = 1000; 
  $tot_pt = 0; 
  $tot_gol = 0; 
  $rounds = 0;
  $sql = "SELECT * FROM {fanta_matches} " .
      "WHERE (t1_id = :t_id OR t2_id = :t_id) " .
      "AND played = 1 " .
      "AND g_id IN (SELECT g_id FROM {fanta_groups} WHERE c_id = :c_id)";
  $result = db_query($sql, array(":t_id" => $t_id, ":c_id" => variable_get("fantacalcio_main_competition", 1)));
  foreach ($result as $row) {
    if ($t_id == $row->t1_id) {
      $pt = $row->tot_1;
      $gol = $row->goals_1;
    }
    if ($t_id == $row->t2_id) {
      $pt = $row->tot_2;
      $gol = $row->goals_2;
    }

    $tot_pt += $pt;
    $tot_gol += $gol;

    #punteggio massimo
    if ($max_pt < $pt) {
      $max_pt = $pt;
      $round_max = $row->round;
    }

    #punteggio minimo
    if ($min_pt > $pt) {
      $min_pt = $pt;
      $round_min = $row->round;
    }
    $rounds++;
  }

  $position = get_team_position($t_id);

  $header = array();
  $rows[] = array("Posizione", $position["position"] . "&deg; (" . $position["point"] . " punti)");
  $rows[] = array("Distacco dal " . $position["team_gap"] . "&deg;", $position["gap"]);
  $rows[] = array("Punteggio Massimo", $max_pt . " (" . $round_max . "&ordf; giornata)");
  $rows[] = array("Punteggio Minimo", $min_pt . " (" . $round_min . "&ordf; giornata)");
  
  if ($rounds) {
    $rows[] = array("Media Punti", round($tot_pt / $rounds, 2));
    $rows[] = array("Media Gol", round($tot_gol / $rounds, 2));
  }

  $round_stop = get_last_competition_round(variable_get("fantacalcio_main_competition", 1));

  $charts_links = array(l("Classifica", "statistiche/grafici/chart/" . $t_id . "/standing/1/" . $round_stop, array("attributes" => array("class" => "thickbox", "rel" => "lightframe[" . $t_id . "|width:770;height:450]", "title" => "Classifica"))), 
                        l("Goal fatti", "statistiche/grafici/chart/" . $t_id . "/goals/1/" . $round_stop, array("attributes" => array("class" => "thickbox", "rel" => "lightframe[" . $t_id . "|width:770;height:450]", "title" => "Goal fatti"))),
                        l("Punti fatti", "statistiche/grafici/chart/" . $t_id . "/points/1/" . $round_stop, array("attributes" => array("class" => "thickbox", "rel" => "lightframe[" . $t_id . "|width:770;height:450]", "title" => "Punti fatti")))
                       );

  return theme_table(array("header" => $header, "rows" => $rows, "attributes" => array(), "colgroups" => array(), "sticky" => "", "caption" => "", "empty" => "")) 
  . "" .  theme_item_list(array("items" => $charts_links, "title" => NULL, "type" => "ul", "attributes" => array("class" => "float"))) . "<div class=clear></div>";
}

function choose_t_id_form($form_state, $teams, $t_id) {

  $form['team'] = array("#type" => "fieldset");

  $form['team']['t_id'] = array(
            "#title" => "Squadra",
            "#type" => "select",
            "#options" => $teams,
            "#default_value" => $t_id
            );

  $form['team']['submit'] = array(
            "#type" => "submit",
            "#value" => t("Ok"),
            );

  return $form;

}

function choose_t_id_form_submit($form, $form_state) {

  $t_id = $form_state['values']['t_id'];

  drupal_goto("statistiche/squadra/" . $t_id);
}

function get_team_position($t_id) {

  $sql = "SELECT g.g_id 
      FROM {fanta_teams_groups} t, {fanta_groups} g
      WHERE g.g_id = t.g_id
      AND t.t_id = :t_id
      AND g.c_id = :c_id";
  $result = db_query($sql, array(":t_id" => $t_id, ":c_id" => variable_get("fantacalcio_main_competition", 1)));
  $g_id = $result->fetchField();

  $sql= "SELECT * FROM {fanta_matches} WHERE g_id = :g_id AND played = 1";
  $result = db_query($sql, array(":g_id" => $g_id));
  foreach ($result as $row) {
    $matches[$row->m_id] = $row;
  }

  $points = array();

  if ($matches) {
    foreach ($matches as $m_id => $match) {
      switch ($match->winner_id) {
        case $match->t1_id: 
          if (isset($points[$match->t1_id]))
            $points[$match->t1_id] += 3; 
          else
            $points[$match->t1_id] = 3; 
          break;
        case $match->t2_id: 
          if (isset($points[$match->t2_id]))
            $points[$match->t2_id] += 3;
          else 
            $points[$match->t2_id] = 3;
          break;
        case -1: 
          if (isset($points[$match->t1_id]))
            $points[$match->t1_id] += 1; 
          else
            $points[$match->t1_id] = 1;
          if (isset($points[$match->t2_id]))
            $points[$match->t2_id] += 1;
          else 
            $points[$match->t2_id] = 1;
          break;
      }
    }

    $max_point = 0; $t_id_max_point = 0;
    foreach ($points as $team_id => $point) {
      if ($t_id == $team_id) $position['point'] = $point;

      if ($max_point < $point) {
        $max_point = $point;
        $t_id_max_point = $team_id;
      }
    }

    rsort($points);
    $points_teams = array_flip($points);

    if ($position["point"] == $max_point) {
      $position["gap"] = $position["point"] - $points[1];
      $position["team_gap"] = 2;
    }
    else {
      $position["gap"] = $points[0] - $position["point"];
      $position["team_gap"] = 1;
    }

    $position["position"] = $points_teams[$position["point"]] +1;

    return $position;
  }
}

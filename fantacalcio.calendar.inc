<?php

/**
* @file fantacalcio.calendar.inc
* 
*/
function fantacalcio_calendar($name = '', $round = null) {
  $competition = Competition::getByName($name);
  
  if ($competition == null || $competition->has_matches != 1) {
    
    $items = Competition::choose(array(
      "has_matches" => 1, 
      "type" => COMPETITION_TYPE_SD), "calendario");
    return theme_item_list(array(
      "items" => $items, 
      "attributes" => array("class" => array("list-group")), 
      "type" => "ul", 
      "title" => ""));
  }
  else {
    drupal_set_title(check_plain(t("Calendario") . " - " . ucwords($name)));
    
    if ($competition->is_default && $competition->type != COMPETITION_TYPE_GP)
      return get_matches_table($competition);
    else
      return get_matches_table($competition);
  }
}

function get_matches_table($competition) {
  global $user;
  
  $output = array();
  $index = 0;
 
  $_groups = $competition->groups;
 
  uasort ($_groups , function($a, $b){if ($a->matches_order >= $b->matches_order) return 1; else return -1;} );

  $already_active = false;
 
  foreach ($_groups as $g_id => $group) {
    
    if ($group->matches_order > 0) {
      
      $index++;
      
      $matches = Match::getByGroup($g_id);
      ksort($matches);
      
      $rounds = array();
      foreach ($matches as $round => $round_matches) {
        if (count(array_values($round_matches)) > 0) {
          $match = array_values($round_matches);
          $rounds[$round] = (object) array("date" => $match[0]->date);
        }
      }

      $_last_round = Round::getLastForCompetition($competition->id);
      if ($_last_round != null)
        $last_round = $_last_round->competitions[$competition->id]->competition_round;
      else 
        $last_round = null;
      
      $_next_round = Round::getNextForCompetition($competition->id);
      if ($_next_round != null)
        $next_round = $_next_round->competitions[$competition->id]->competition_round;
      else 
        $next_round = null;
      
      if ($last_round != null && $last_round > 0 && isset($matches[$last_round])) {
        $last_round_data = new stdClass();
        $last_round_data->matches = $matches[$last_round];
        $last_round_data->round = $last_round;
      }
      else {
        $last_round_data = null;
      }
      
      if ($next_round != null && $next_round > 0 && isset($matches[$next_round])) {
        $next_round_data = new stdClass();
        $next_round_data->matches = $matches[$next_round];
        $next_round_data->round = $next_round;
      }
      else {
        $next_round_data = null;
      }
//       theme("table", array(
//         "header" => array(), 
//         "rows" => $matches[1], 
//         "attributes" => array(), 
//         "sticky" => false, 
//         "empty" => ""));
      
//       $next_round = theme("table", array(
//         "header" => array(), 
//         "rows" => $matches[2], 
//         "attributes" => array(), 
//         "sticky" => false, 
//         "empty" => ""));
      
//       print_r($user);
      
      $group->output = theme("calendar", array(
        "user_teams" => isset($user->teams) ? array_keys($user->teams) : array(),
        "matches" => $matches, 
        "rounds" => $rounds, 
        "rounds_list" => null, 
        "is_main_competition" => false, 
        "last_round" => $last_round_data, 
        "next_round" => $next_round_data));
      
      if (isset($user->groups)) { 
        if (in_array($g_id, $user->groups[$competition->id]) && !$already_active) {
          $group->active = true;
	  $already_active = true;
        } else 
          $group->active = false;
      }
      else if ($index == 1)
        $group->active = true;
      else 
        $group->active = false;
      
      $groups[$g_id] = $group;
    }
  }
  
  $output["groups"] = $groups;
  
  return theme("group_tabs", $output);
}

function _get_matches_table($g_id) {
  $teams = Team::all();
  $rounds = get_rounds_by_g_id($g_id);
  
  $out = "<div class='-container-fluid'>";
  $out .= "<div class='row'>";
  
  // ultima e prossima giornata
  if ($last_round = get_last_competition_round(get_cid_by_gid($g_id))) {
    $matches = get_round_matches($last_round, $g_id);
    $out_last = "\n<div class=\"col-xs-12 col-md-6\">" . "\n<h4>Ultima giornata</h4>";
    $out_last .= show_matches_for_round($matches, $teams, $rounds, $last_round, $g_id);
    $out_last .= "\n</div>";
  }
  
  if ($next_round = get_next_competition_round(get_cid_by_gid($g_id))) {
    $matches = get_round_matches($next_round, $g_id);
    $out_next = "\n<div class=\"col-xs-12 col-md-6\">" . "\n<h4>" . (get_round_date($next_round, get_cid_by_gid($g_id)) > time() ? "Prossima giornata" : "Giornata in corso") . "</h4>";
    $out_next .= show_matches_for_round($matches, $teams, $rounds, $next_round, $g_id);
    $out_next .= "\n</div>";
  }
  
  if ((time() - get_round_date($last_round, get_cid_by_gid($g_id))) > (get_round_date($next_round, get_cid_by_gid($g_id)) - time()))
    $out .= $out_next . $out_last;
  else
    $out .= $out_last . $out_next;
  
  $out .= "\n</div>";
  $out .= "\n</div>";
  
  $num_rounds = get_number_rounds($g_id);
  
  // elenco giornate
  if ($num_rounds > 1)
    $out .= "<div class='navbar navbar-default navbar-static-top'>" . get_round_list($g_id) . "</div>";
    
    // calendario completo
  $out .= "<div class='no-padding'>";
  $out .= show_matches(get_matches($g_id), $g_id);
  $out .= "</div>";
  
  $out .= "</div>";
  
  return $out;
}

function get_matches_round($g_id, $args = array()) {
  $out = "\n<div style='clear:both;height:20px;'></div>";
  
  // calendario completo
  $matches = get_round_matches($args['round'], $g_id);
  $out .= show_matches($matches, $g_id);
  
  return $out;
}

function get_round_matches($round, $g_id = '', $c_id = '') {
  $dates = get_matches_date();
  $groups = get_competitions_groups();
  
  if ($g_id) {
    $sql = "SELECT * FROM {fanta_matches} WHERE g_id = :g_id AND round = :round";
    $result = db_query($sql, array(":g_id" => $g_id, ":round" => $round));
  }
  if ($c_id) {
    $sql = "SELECT * FROM {fanta_matches} WHERE g_id IN (SELECT g_id FROM {fanta_groups} WHERE c_id = :c_id) AND round = :round";
    $result = db_query($sql, array(":c_id" => $c_id, ":round" => $round));
  }
  
  $matches = array();
  foreach ($result as $row) {
    $m_id = $row->m_id;
    $matches[$m_id] = $row;
    $matches[$m_id]->date = $dates[$groups[$row->g_id]][$row->round];
  }
  return $matches;
}

function show_matches($matches, $g_id = '', $is_header = FALSE) {
  global $user;
  
  $out = "";
  
  // $teams = get_teams();
  // $rounds = get_rounds_by_g_id($g_id);
  
  foreach ($matches as $m_id => $match) {
    $round = $match->round;
    $m_id = $match->m_id;
    $round_matches[$round][$m_id] = $match;
  }
  
  if ($round_matches) {
    
    ksort($round_matches);
    
    $i = 0;
    
    // table body
    foreach ($round_matches as $r_id => $matches_for_round) {
      $i++;
      
      if ($i % 2 == 1)
        $out .= "<div class=\"row\">";
      
      $out .= "<div class='round col-xs-12 col-md-6 col-lg-6' id='" . $g_id . "_" . $r_id . "'>" . show_matches_for_round($matches_for_round, $teams, $rounds, $r_id, $g_id) . "</div>";
      
      if ($i % 2 == 0)
        $out .= "</div>";
    }
  }
  
  return $out;
}

function show_matches_for_round($matches, $teams, $rounds, $round, $g_id) {
  global $user;
  
  $header = array();
  $rows = array();
  
  $attributes = array(
    "id" => "round_", 
    "class" => "table table-responsive table-striped");
  
  foreach ($matches as $m_id => $match)
    $date = $match->date;
    
    // $date = get_round_date($match->round, get_cid_by_gid($g_id));
    
  // table header
  $round_name = (empty($rounds[$match->round]->round_label)) ? $round . "&deg; Giornata" : $rounds[$match->round]->round_label;
  
  array_push($header, array(
    "data" => $round_name . " " . date("d-m-Y H:i", $date) . " ", 
    "colspan" => 4));
  
  foreach ($matches as $m_id => $match) {
    
    if (!empty($match->match_label))
      $rows[$m_id][] = array(
        "data" => "(" . $match->match_label . ")", 
        "class" => "team");
    
    $t1_name = ($match->t1_id != 0) ? l($teams[$match->t1_id]->name, "squadre/" . $match->t1_id) : $match->t1_label;
    $t2_name = ($match->t2_id != 0) ? l($teams[$match->t2_id]->name, "squadre/" . $match->t2_id) : $match->t2_label;
    
    $t1_name = ($match->t1_id == $user->t_id) ? "<span class=bold>" . $t1_name . "</span>" : $t1_name;
    $t2_name = ($match->t2_id == $user->t_id) ? "<span class=bold>" . $t2_name . "</span>" : $t2_name;
    
    $rows[$m_id][] = array(
      "data" => $t1_name . " - " . $t2_name, 
      "class" => "team");
    
    $link_attributes = array(
      "class" => "_thickbox", 
      "rel" => "lightframe['" . $match->round . "_" . $match->g_id . "'|width:970px;]", 
      "title" => "Dettagli partita", 
      "data-toggle" => "modal");
    
    if ($match->played == 1) {
      $overtime = "";
      $penalties = "";
      if (is_match_draw($match) && $round->next) {
        $overtime = " (" . $match->goals_ot_1 . "-" . $match->goals_ot_2 . ")ot";
        if ($match->goals_ot_1 == $match->goals_ot_2)
          $penalties = " (" . $match->penalties_1 . "-" . $match->penalties_2 . ")rig";
      }
      $rows[$m_id][] = array(
        "data" => $match->goals_1 . "&nbsp;-&nbsp;" . $match->goals_2 . $overtime . $penalties, 
        "class" => "gol");
      $rows[$m_id][] = array(
        "data" => $match->tot_1 . "&nbsp;-&nbsp;" . $match->tot_2, 
        "class" => "pt hidden-xs");
    }
    else
      $rows[$m_id][] = array("data" => "&nbsp;", "colspan" => 2);
    
    // link statistiche
    // if ($match->round == get_next_competition_round(get_cid_by_gid($match->g_id)) || $match->played == 1) {
    // $rows[$m_id][] = l("<i class=\"fa fa-2x fa-bar-chart-o\"></i>", "scheda/partita/" . $m_id, array("html" => "true", "attributes" => $link_attributes));
    // }
    // else $rows[$m_id][] = array("data" => "&nbsp;", "colspan" => 1);
  }
  
  return theme_table(array(
    "header" => $header, 
    "rows" => $rows, 
    "attributes" => $attributes, 
    "caption" => "", 
    "sticky" => "", 
    "empty" => "", 
    "colgroups" => array()));
}

function get_round_list($g_id) {
  $result = db_query("SELECT DISTINCT round FROM {fanta_matches} WHERE g_id = :g_id", array(
    ":g_id" => $g_id));
  foreach ($result as $row) {
    $rounds_gid[] = $row->round;
  }
  $c_id = get_cid_by_gid($g_id);
  $sql = "SELECT * FROM {fanta_rounds} r, {fanta_rounds_competitions} c WHERE c.round = r.round AND c.c_id = :c_id ";
  $result = db_query($sql, array(":c_id" => $c_id));
  foreach ($result as $row) {
    $round = $row->competition_round;
    if (in_array($round, $rounds_gid))
      $rounds[$round] = empty($row->round_label) ? $row->competition_round : $row->round_label;
  }
  
  ksort($rounds);
  
  $out = "";
  $out .= "<div class='round_list' id='round-list'>";
  $out .= "\n<ul>";
  foreach ($rounds as $round => $round_label) {
    $out .= "\n<li class='round_list'>" . "<a href='#" . $g_id . "_" . $round . "'>" . $round_label . "</a>" . "</li>";
  }
  $out .= "\n</ul>";
  $out .= "\n</div>";
  return $out;
}

function _fantacalcio_calendario($c_name = '') {
  if (empty($c_name) || !competition_has_arg($c_name, "matches"))
    $out = choose_competition(array("matches" => 1));
  
  else {
    drupal_set_title(check_plain("Calendario - " . ucwords($c_name)));
    $out = make_tabs('get_matches_table', $c_name, "matches");
  }
  return $out;
}

function fantacalcio_calendario_round($c_name = '', $round) {
  if (empty($c_name) || !competition_has_arg($c_name, "matches"))
    $out = choose_competition(array("matches" => 1));
  
  else {
    if (round_exists($round)) {
      $round_data = get_competition_round($round, get_competition_id($c_name));
      drupal_set_title(filter_xss("Calendario - " . ucwords($c_name) . " - " . ($round_data->round_label != "" ? $round_data->round_label : $round . "&ordf; giornata")));
      $out = make_tabs('get_matches_round', $c_name, "matches", array(
        'round' => $round));
    }
  }
  return $out;
}

function show_match_details($m_id) {
  $match = Match::get($m_id);

  if ($match != null) { 
  $round = Round::getByCompetitionRound($match->round, $match->c_id);
  
//   $empty_rows = get_details_empty_rows($match);
  
  $out = "<div style='margin:auto; margin-bottom: 10px; text-align: center;'>";
  
  $out .= "<h3>" . (!empty($round->round_label) ? $round->round_label : ($match->round . "&ordf; Giornata")) . "</h3><br>";
  $out .= "<h4>" . Team::get($match->t1_id)->name . " - " . Team::get($match->t2_id)->name . "</h4><hr>";
  $out .= "</div>";
  
  $out .= "<div class='team-details col-xs-12 col-sm-6'>" . show_team_details($match->t1_id, $match->round, $match->c_id, $match->id, $match->played) . "</div>";
  
  $out .= "<div class='team-details col-xs-12 col-sm-6'>" . show_team_details($match->t2_id, $match->round, $match->c_id, $match->id, $match->played) . "</div>";
  
  if ($round->next && $match->isDraw()) {
    $out .= "<div class='clear'></div>";
    $out .= "<center>";
    $out .= "<h4>Tempi Supplementari</h4>";
    
    // overtime team 1
    $out .= "<div class='team-details first-team'>" . show_match_overtime($match, $match->t1_id) . "</div>";
    
    // overtime team 2
    $out .= "<div class='team-details'>" . show_match_overtime($match, $match->t2_id) . "</div>";
    
//     if ($match->goals_ot_1 == $match->goals_ot_2 && false) {
//       $last_penalty_index = 5;//get_penalties_last_index(get_penalties($match));
//       $out .= "<h4>Rigori</h4>";
      
//       // penalties team 1
//       $out .= "<div class='team-details first-team'>" . show_match_penalties($match, $match->t1_id, $last_penalty_index) . "</div>";
      
//       // penalties team 2
//       $out .= "<div class='team-details'>" . show_match_penalties($match, $match->t2_id, $last_penalty_index) . "</div>";
//     }
    $out .= "</center>";
  }
  
  return $out;
  }
  else 
    return t("Nessuna partita con questo id");
}

function show_match_overtime($match, $t_id) {
//   $sql = "SELECT round FROM {fanta_rounds_competitions} " . "WHERE competition_round = '%d' " . "AND c_id = '%d'";
//   $result = db_query($sql, $match->round, get_cid_by_gid($match->g_id));
//   $round = db_result($result);
  $round = Round::get($match->round, $match->c_id);
  $votes = $round->getVotes(variable_get("fantacalcio_votes_provider", 1));
//   $players = get_players($round);
  
  // riserve non entrate
  $sql = "SELECT * FROM {fanta_lineups} f, {fanta_players} p
      WHERE f.pl_id = p.pl_id
      AND f.t_id = '%d'
      AND f.round = '%d'
      AND f.c_id = '%d'
      AND f.has_played = 2 
      ORDER BY p.role";
//   $result = db_query($sql, $t_id, $match->round, get_cid_by_gid($match->g_id));
//   while ($row = db_fetch_array($result)) {
//     $reserves[$row['pl_id']] = $row;
//   }
  
  $query = db_select("fanta_lineups", "l");
  $query->join("fanta_players", "p", "p.pl_id = l.pl_id");
  $query->join("fanta_players_rounds", "pr", "p.pl_id = pr.pl_id");
  $query->join("fanta_real_teams", "rt", "rt.rt_id = pr.rt_id");
  $query->condition("l.t_id", $t_id);
  $query->condition("l.round", $match->round);
  $query->condition("l.c_id", $match->c_id);
  $query->condition("l.has_played", 2);
  $query->fields("l");
  $query->fields("p");
  $query->addField("rt", "name", "team");
  $query->orderBy("role");
  
  $result = $query->execute();
  foreach($result as $row) {
    $reserves[$row->pl_id] = $row;
  }
  
//   $teams = get_teams();
  
  $out = "<div style='margin:auto; margin-top: 10px; text-align: center;'>";
  
  $points = 0;
  
  $header_data = array(
    "", 
    "", 
    t("Nome"), 
    t("Squadra"), 
    "", 
    t("Tot"), 
    t("Voto"), 
    t("Gol Fatti"), 
    t("Gol Subiti"), 
    t("Assist"), 
    t("Rigori Sbagliati"), 
    t("Rigori Parati"), 
    t("Autogol"));
  // $header = array("", "", "Nome", "Squadra", "", "Tot", "Voto", "GF", "GS", "Assist", "Rig Sba", "Rig Par", "Auto gol");
  $header[] = array("data" => "", "class" => array("team-details-header"));
  $header[] = array("data" => t("Nome"), "class" => array("team-details-header"));
  $header[] = array("data" => "", "class" => array("team-details-header"));
  $header[] = array("data" => "", "class" => array("team-details-header"));
  $header[] = array("data" => t("Tot"), "class" => array("team-details-header"));
  $header[] = array("data" => t("Voto"), "class" => array("team-details-header"));
  $header[] = array("data" => t("Goal Fatti"), "class" => array("team-details-header"));
  $header[] = array("data" => t("Goal Subiti"), "class" => array("team-details-header"));
  $header[] = array("data" => t("Assist"), "class" => array("team-details-header"));
  $header[] = array(
    "data" => t("Rigori Sbagliati"), 
    "class" => array("team-details-header"));
  $header[] = array("data" => t("Rigori Parati"), "class" => array("team-details-header"));
  $header[] = array("data" => t("Auto goal"), "class" => array("team-details-header"));
  
  $rows = array();
  
  foreach ($reserves as $reserve) {
    $pl_id = $reserve->pl_id;
    
    $data = array();
    
    $data[] = array(
      "data" => "<span class='team-details-'>" . fantacalcio_show_role($reserve->role) . "</span>", 
      "class" => "role");
    // $data[] = "<span class='team-details-role role'>" . convert_role($players[$reserve['pl_id']]->role) . "</span>";
    $data[] = "<span class='team-details-name'>" . $reserve->name . "</span>";
    $data[] = "<span class='team-details-role'>" . substr(ucwords($reserve->team), 0, 3) . "</span>";
    $data[] = fantacalcio_show_cards($votes[$pl_id]->yellow_cards, $votes[$pl_id]->red_cards);
    $data[] = array(
      "data" => $votes[$pl_id]->total, 
      "class" => get_details_class($votes[$pl_id]->total) . " team-details-tot");
    $data[] = array(
      "data" => $votes[$pl_id]->vote, 
      "class" => get_details_class($votes[$pl_id]->vote));
    $data[] = array(
      "data" => ($votes[$pl_id]->goals_for + $votes[$pl_id]->penalty_goals) . (($votes[$pl_id]->penalty_goals > 0) ? "(" . $votes[$pl_id]->rig_fatto . ")" : ""), 
      "class" => get_details_class(($votes[$pl_id]->goals_for + $votes[$pl_id]->penalty_goals), "bonus"));
    $data[] = array(
      "data" => $votes[$pl_id]->goals_against, 
      "class" => get_details_class($votes[$pl_id]->goals_against, "malus"));
    $data[] = array(
      "data" => $votes[$pl_id]->assists, 
      "class" => get_details_class($votes[$pl_id]->assists, "bonus"));
    $data[] = array(
      "data" => $votes[$pl_id]->missed_penalties, 
      "class" => get_details_class($votes[$pl_id]->missed_penalties, "malus"));
    $data[] = array(
      "data" => $votes[$pl_id]->saved_penalties, 
      "class" => get_details_class($votes[$pl_id]->saved_penalties, "bonus"));
    $data[] = array(
      "data" => $votes[$pl_id]->own_goals, 
      "class" => get_details_class($votes[$pl_id]->own_goals, "malus"));
    
    $points += $votes[$pl_id]->total;
    
    $rows[$pl_id] = array(
      "data" => $data, 
      "class" => array("role_" . $reserve->role));
  }
  
  for ($i = count($reserves); $i < 3; $i++) {
    $rows[][] = array(
      "data" => "&nbsp;", 
      "colspan" => 13, 
      "class" => array("role_" . $reserve->role));
  }
  
  $rows['space'][] = array("data" => "", "colspan" => 13);
  
  $out .= "<div class='team-details-lineup'>" . theme("table", array("header" => $header, "rows" => $rows)) . "</div>";
  
  $goals_ot = floor(($points - 16) / 4);
  $goals_ot = ($goals_ot >= 0) ? $goals_ot : 0;
  
  $rows = array();
  $rows[] = array(t("Punti"), $points);
  
  $rows[] = array(
    "data" => array(t("Totale"), $points), 
    "style" => "font-weight:bold");
  $rows[] = array(
    "data" => array(t("Gol"), $goals_ot), 
    "style" => "font-weight:bold");
  
  $out .= "<div style='margin:auto; margin-top: 10px;'>" 
      . theme("table", array("rows" => $rows, "attributes" => array(
    "width" => "100%", 
    "class" => array("table", "table-responsive"),
    "style" => "", 
    "align" => "char", 
    "char" => "."))) . "</div>";
  
  $out .= "</div>";
  
  return $out;
}

function show_match_penalties($match, $t_id, $last_penalty_index) {
  $sql = "SELECT round FROM {fanta_rounds_competitions} " . "WHERE competition_round = '%d' " . "AND c_id = '%d'";
  $result = db_query($sql, $match->round, get_cid_by_gid($match->g_id));
  $round = db_result($result);
  
  $votes = get_votes($round);
  $players = get_players($round);
  
  // rigoristi ordinati (solo quelli necessari)
  $sql = "SELECT * FROM {fanta_lineups} f
      WHERE f.t_id = '%d'
      AND f.round = '%d'
      AND f.c_id = '%d'
      AND f.has_played = 1 
      ORDER BY penalty_position";
  $result = db_query($sql, $t_id, $match->round, get_cid_by_gid($match->g_id));
  while ($row = db_fetch_array($result)) {
    $penalties[$row['pl_id']] = $row;
  }
  
  $teams = get_teams();
  
  $out = "<div style='margin:auto; margin-top: 10px; text-align: center;'>";
  
  $points = 0;
  
  $header[] = array("data" => "", "class" => "team-details-header");
  $header[] = array("data" => "", "class" => "team-details-header");
  $header[] = array("data" => "Voto", "class" => "team-details-header");
  $header[] = array("data" => "", "class" => "team-details-header");
  $header[] = array("data" => "", "class" => "team-details-header");
  
  $rows = array();
  
  $i = 0;
  foreach ($penalties as $penalty) {
    if ($i < $last_penalty_index) {
      $pl_id = $penalty['pl_id'];
      
      $data = array();
      $data[] = array(
        "data" => "<span class='team-details-'>" . convert_role($players[$penalty['pl_id']]->role) . "</span>", 
        "class" => "role");
      // $data[] = "<span class='team-details-role role'>" . convert_role($players[$penalty['pl_id']]->role) . "</span>";
      $data[] = "<span class='team-details-name'>" . compress_string($players[$penalty['pl_id']]->name) . "</span>";
      $data[] = "<span class='team-details-role'>" . ucwords(convert_teams($players[$penalty['pl_id']]->team)) . "</span>";
      $data[] = array(
        "data" => $votes[$pl_id]->vote, 
        "class" => get_details_class($votes[$pl_id]->vote));
      $data[] = array(
        "data" => get_penalty_result($votes[$pl_id]->vote), 
        "class" => get_details_class(($votes[$pl_id]->gol_fatto + $votes[$pl_id]->rig_fatto), "bonus"));
      
      $points += ($votes[$pl_id]->vote >= 6) ? 1 : 0;
      
      $rows[$pl_id] = array(
        "data" => $data, 
        "class" => "role_" . $players[$pl_id]->role);
      
      $i++;
    }
  }
  
  $rows['space'][] = array("data" => "", "colspan" => 5);
  $rows[] = array(
    "data" => array(array("data" => "Totale", "colspan" => 4), $points), 
    "style" => "font-weight:bold");
  
  $out .= "<div class='team-details-lineup'>" . theme_table($header, $rows, array(
    "style" => "")) . "</div>";
  
  $out .= "</div>";
  
  return $out;
}

function show_team_details($t_id, $competition_round, $c_id, $m_id, $played) {
  if ($played == 0)
    return show_team_details_not_played($t_id, $competition_round, $c_id, $m_id);
  elseif ($played == 1)
    return show_team_details_played($t_id, $competition_round, $c_id, $m_id);
}

function show_team_details_played($t_id, $competition_round, $c_id, $m_id) {
  $team = Team::get($t_id);
  
  $out = "<div style='margin:auto; margin-top: 10px; text-align: center;'>";
  $out .= "<h3>" . $team->name . "</h3>";
  $out .= "<p><img src='" . base_path() . drupal_get_path("module", "fantacalcio") . "/images/shirts/" . $team->shirt . "'></p>";
  
  $round = Round::getByCompetitionRound($competition_round, $c_id);
  
  $votes = $round->getVotes(variable_get("fantacalcio_votes_provider", 1));
  // $players = get_players($round->round);
  
  $regulars = get_details_regulars($t_id, $c_id, $competition_round, $round->round);
  $reserves = get_details_reserves($t_id, $c_id, $competition_round, $round->round);
  
  $module = array(0, 0, 0, 0);
  foreach ($regulars as $regular) {
    $module[$regular->role]++;
  }
  unset($module[0]);
  
  $out .= "<p>" . implode("-", $module) . "</p>";
  $out .= "</div>";
  
  $points = 0;
  
  $header_data = array(
    "", 
    "", 
    t("Nome"), 
    t("Squadra"), 
    "", 
    t("Tot"), 
    t("Voto"), 
    t("Gol Fatti"), 
    t("Gol Subiti"), 
    t("Assist"), 
    t("Rigori Sbagliati"), 
    t("Rigori Parati"), 
    t("Autogol"));
  $header[] = array("data" => "", "class" => "team-details-header");
  $header[] = array("data" => "", "class" => "team-details-header");
  $header[] = array("data" => "Nome", "class" => "team-details-header");
  $header[] = array("data" => "", "class" => "team-details-header");
  $header[] = array("data" => "", "class" => "team-details-header");
  $header[] = array("data" => "Tot", "class" => "team-details-header");
  $header[] = array("data" => "Voto", "class" => "team-details-header");
  
  if (variable_get("fantacalcio_points_goals_for", 0) != 0)
    $header[] = array("data" => "Goal Fatti", "class" => "team-details-header");
  
  if (variable_get("fantacalcio_points_goals_against", 0) != 0)
    $header[] = array("data" => "Goal Subiti", "class" => "team-details-header");
  
  if (variable_get("fantacalcio_points_assists", 0) != 0)
    $header[] = array("data" => "Assist", "class" => "team-details-header");
  
  if (variable_get("fantacalcio_points_missed_penalties", 0) != 0)
    $header[] = array(
    "data" => "Rigori Sbagliati", 
    "class" => "team-details-header");
  
  if (variable_get("fantacalcio_points_saved_penalties", 0) != 0)
    $header[] = array("data" => "Rigori Parati", "class" => "team-details-header");
  
  if (variable_get("fantacalcio_points_own_goals", 0) != 0)
    $header[] = array("data" => "Auto goal", "class" => "team-details-header");
  
  if (variable_get("fantacalcio_points_draw_goal", 0) != 0)
    $header[] = array("data" => "Gol pareggio", "class" => "team-details-header");
  
  if (variable_get("fantacalcio_points_win_goal", 0) != 0)
    $header[] = array("data" => "Gol vittoria", "class" => "team-details-header");

  $rows = array();
  
  foreach ($regulars as $regular) {
    $pl_id = $regular->pl_id;
    
    $row_classes = array();
        
    $data = array();
    
    $data[] = ($regular->has_played == 1) ? "" : "<i class='fa fa-lg fa-long-arrow-left text-danger'></i>";
    
    $data[] = array(
      "data" => "<span class='team-details-'>" . fantacalcio_show_role($regular->role) . "</span>", 
      "class" => array("role"));
    $data[] = "<span class='team-details-name'>" . $regular->name . "</span>";
    $data[] = "<span class='team-details-role'>" . substr(ucwords($regular->team), 0, 3) . "</span>";
    
    if ($regular->has_played == 1) {
      $data[] = fantacalcio_show_cards($votes[$pl_id]->yellow_cards, $votes[$pl_id]->red_cards);
      $data[] = array(
        "data" => $votes[$pl_id]->total, 
        "class" => array(get_details_class($votes[$pl_id]->total) . " team-details-tot"));
      $data[] = array("data" => $votes[$pl_id]->vote);

      if (variable_get("fantacalcio_points_goals_for", 0) != 0)
      $data[] = array(
        "data" => ($votes[$pl_id]->goals_for + $votes[$pl_id]->penalty_goals > 0 ? ($votes[$pl_id]->goals_for + $votes[$pl_id]->penalty_goals) . (($votes[$pl_id]->penalty_goals > 0) ? "(" . $votes[$pl_id]->penalty_goals . ")" : "") : "-"), 
        "class" => array(get_details_class(($votes[$pl_id]->goals_for + $votes[$pl_id]->penalty_goals), "bonus")));
      
      if (variable_get("fantacalcio_points_goals_against", 0) != 0)
      $data[] = array(
        "data" => ($votes[$pl_id]->goals_against > 0 ? $votes[$pl_id]->goals_against : "-"), 
        "class" => array(get_details_class($votes[$pl_id]->goals_against, "malus")));
      
      if (variable_get("fantacalcio_points_assists", 0) != 0)
$data[] = array(
        "data" => ($votes[$pl_id]->assists > 0 ? $votes[$pl_id]->assists : "-"), 
        "class" => array(get_details_class($votes[$pl_id]->assists, "bonus")));

      if (variable_get("fantacalcio_points_missed_penalties", 0) != 0)
      $data[] = array(
        "data" => ($votes[$pl_id]->missed_penalties > 0 ? $votes[$pl_id]->missed_penalties : "-"), 
        "class" => array(get_details_class($votes[$pl_id]->missed_penalties, "malus")));

      if (variable_get("fantacalcio_points_saved_penalties", 0) != 0)
      $data[] = array(
        "data" => ($votes[$pl_id]->saved_penalties > 0 ? $votes[$pl_id]->saved_penalties : "-"), 
        "class" => array(get_details_class($votes[$pl_id]->saved_penalties, "bonus")));
      
      if (variable_get("fantacalcio_points_own_goals", 0) != 0)
        $data[] = array(
        "data" => ($votes[$pl_id]->own_goals > 0 ? $votes[$pl_id]->own_goals : "-"), 
        "class" => array(get_details_class($votes[$pl_id]->own_goals, "malus")));
     
if (variable_get("fantacalcio_points_draw_goal", 0) != 0)
        $data[] = array(
        "data" => ($votes[$pl_id]->draw_goal > 0 ? $votes[$pl_id]->draw_goal : "-"),
        "class" => array(get_details_class($votes[$pl_id]->draw_goal, "bonus")));
 
if (variable_get("fantacalcio_points_win_goal", 0) != 0)
        $data[] = array(
        "data" => ($votes[$pl_id]->win_goal > 0 ? $votes[$pl_id]->win_goal : "-"),
        "class" => array(get_details_class($votes[$pl_id]->win_goal, "bonus")));

      $points += $votes[$pl_id]->total;
    }
    else {
      array_push($row_classes, "not_played");
      $data[] = array("data" => "", "colspan" => (count($header) - 4));
    }
    
    $rows[$pl_id] = array(
      "data" => $data,       
      "class" => $row_classes);
  }
  
  $rows['space'][] = array("data" => "", "colspan" => 13);
  
  $played_reserves = 0;
  
  foreach ($reserves as $reserve) {

    $pl_id = $reserve->pl_id;
    
    $data = array();
    
    $row_classes = array();
    if ($reserve->has_played == 1)
      $data[] = "<i class='fa fa-lg fa-long-arrow-right text-success'></i>";
    else {
      array_push($row_classes, "not_played");
      $data[] = "";
    }
    
    // $data[] = "<span class='team-details-role role'>" . convert_role($players[$reserve['pl_id']]->role) . "</span>";
    $data[] = array(
      "data" => "<span class='team-details-'>" . fantacalcio_show_role($reserve->role) . "</span>", 
      "class" => array("role"));
    $data[] = "<span class='team-details-name'>" . $reserve->name . "</span>";
    $data[] = "<span class='team-details-role'>" . substr(ucwords($reserve->team), 0, 3) . "</span>";
    
    if (isset($votes[$pl_id]) && $votes[$pl_id]->has_vote == 1) {
      $data[] = fantacalcio_show_cards($votes[$pl_id]->yellow_cards, $votes[$pl_id]->red_cards);
      $data[] = array(
        "data" => $votes[$pl_id]->total, 
        "class" => array(get_details_class($votes[$pl_id]->total) . " team-details-tot"));
      $data[] = array("data" => $votes[$pl_id]->vote);
      $data[] = array(
        "data" => ($votes[$pl_id]->goals_for + $votes[$pl_id]->penalty_goals > 0 ? ($votes[$pl_id]->goals_for + $votes[$pl_id]->penalty_goals) . (($votes[$pl_id]->penalty_goals > 0) ? "(" . $votes[$pl_id]->penalty_goals . ")" : "") : "-"), 
        "class" => array(get_details_class(($votes[$pl_id]->goals_for + $votes[$pl_id]->penalty_goals), "bonus")));
      
      $data[] = array(
        "data" => ($votes[$pl_id]->goals_against > 0 ? $votes[$pl_id]->goals_against : "-"), 
        "class" => array(get_details_class($votes[$pl_id]->goals_against, "malus")));
      $data[] = array(
        "data" => ($votes[$pl_id]->assists > 0 ? $votes[$pl_id]->assists : "-"), 
        "class" => array(get_details_class($votes[$pl_id]->assists, "bonus")));
      $data[] = array(
        "data" => ($votes[$pl_id]->missed_penalties > 0 ? $votes[$pl_id]->missed_penalties : "-"), 
        "class" => array(get_details_class($votes[$pl_id]->missed_penalties, "malus")));
      $data[] = array(
        "data" => ($votes[$pl_id]->saved_penalties > 0 ? $votes[$pl_id]->saved_penalties : "-"), 
        "class" => array(get_details_class($votes[$pl_id]->saved_penalties, "bonus")));
      $data[] = array(
        "data" => ($votes[$pl_id]->own_goals > 0 ? $votes[$pl_id]->own_goals : "-"), 
        "class" => array(get_details_class($votes[$pl_id]->own_goals, "malus")));
    }
    else {
      $data[] = array("data" => "", "colspan" => 9);
    }
      
    if ($reserve->has_played == 1) {
      $points += $votes[$pl_id]->total;
      $played_reserves++;
    }
    
    $rows[$pl_id] = array(
      "data" => $data, 
      "class" => $row_classes);
  }
  
  $out .= "<div class='team-details-lineup overflow-x'>" . theme("table", array(
    "header" => $header, 
    "rows" => $rows, 
    "attributes" => array("style" => "font-size: 90%"))) . "</div>";
  
  $modifiers = get_details_modifiers($t_id, $m_id);
  
  $bonus = get_bonus($t_id, $m_id);
    
  $total = $points + array_sum($modifiers) + $bonus;
  $goals = floor(($total - 60) / 6);
  $goals = ($goals >= 0) ? $goals : 0;
  
  $rows = array();
  $rows[] = array("Punti", $points);
  if (variable_get("fantacalcio_modifier_role_0", 0))
    $rows[] = array("Mod Portiere", $modifiers[0]);
  if (variable_get("fantacalcio_modifier_role_1", 0))
    $rows[] = array("Mod Difesa", $modifiers[1]);
  if (variable_get("fantacalcio_modifier_role_2", 0))
    $rows[] = array("Mod Centrocampo", $modifiers[2]);
  if (variable_get("fantacalcio_modifier_role_3", 0))
    $rows[] = array("Mod Attacco", $modifiers[3]);
  
  if ($bonus)
    $rows[] = array("Bonus", $bonus);
  
  $rows[] = array(
    "data" => array("Totale", $total), 
    "style" => "font-weight:bold");
  $rows[] = array("data" => array("Gol", $goals), "style" => "font-weight:bold");
  
  $out .= "<div style='margin:auto; margin-top: 10px;'>" . theme("table", array(
    "header" => array(), 
    "rows" => $rows, 
    "attributes" => array(
      "width" => "100%", 
      "style" => "",
      "class" => array("table", "table-responsive"), 
      "align" => "char", 
      "char" => "."))) . "</div>";
  
  return $out;
}

function show_team_details_not_played($t_id, $competition_round, $c_id, $m_id) {
  
  // squadra trovata
  if (Team::exists($t_id)) {
    
    $team = Team::get($t_id);
    $competition = Competition::get($c_id);
    $round = Round::get($competition_round, $c_id);
    
    $out = "<div style='margin:auto; margin-top: 10px; text-align: center;'>";
    $out .= "<h3>" . $team->name . "</h3>";
    $out .= "<p><img src='" . base_path() . drupal_get_path("module", "fantacalcio") . "/images/shirts/" . $team->shirt . "'></p>";
    
    $round_label = isset($round->label) ? $round->label : $round->competitions[$c_id]->competition_round. "a giornata";
    
//     drupal_set_title(filter_xss($competition->name . " - " . $round_label));
    
    $lineup = Lineup::get($competition->id, $team->id, $round->competitions[$c_id]->competition_round);
     
    if ($lineup) {
      
      ksort($lineup->positions);
      
      // ordino i titolari(ruolo, nome)
      $roles = array();
      $names = array();
      $module = array(0, 0, 0, 0);
      
      foreach ($lineup->positions[1] as $regular) {
        $module[$regular->role]++;
        $roles[] = $regular->role;
        $names[] = $regular->name;
      }
      
      array_multisort($roles, SORT_ASC, $names, SORT_ASC, $lineup->positions[1]);
      
      // ordino le riserve(ruolo, posizione, nome)
      // ordino i titolari(ruolo, nome)
      $roles = array();
      $names = array();
      
      $reserves = array();
      foreach ($lineup->positions as $position => $players_positions) {
        if ($position > 1)
          $reserves = array_merge($reserves, $players_positions);
      }
      
      foreach ($reserves as $reserve_player) {
        $roles[] = $reserve_player->role;
        $names[] = $reserve_player->name;
      }
      
      array_multisort($roles, SORT_ASC, $names, SORT_ASC, $reserves);
      
      // modulo
      if ($module) {
        unset($module[0]);
   
        $out .= "<p>" . implode("-", $module) . "</p>";
      }
      
      
      $out .= "</div>";
      
      // mostra l'ordine dei rigoristi se è previsto il passaggio del turno
      $show_penalties = $round->next;
      
      // titolari
      $header_startings = array(array("data" => t("Titolari"), "colspan" => 5));
      $rows_startings = team_details_show_position($lineup->positions[1], $show_penalties);
      
      // riserve
      $header_reserves = array(array("data" => t("Riserve"), "colspan" => 5));
      $rows_reserves = team_details_show_position($reserves);
      
      $out .= theme_table(array(
        "header" => $header_startings, 
        "rows" => $rows_startings, 
        "attributes" => array("class" => "table table-responsive"), 
        "colgroups" => array(), 
        "sticky" => "", 
        "caption" => "", 
        "empty" => ""));
      $out .= theme_table(array(
        "header" => $header_reserves, 
        "rows" => $rows_reserves, 
        "attributes" => array("class" => "table table-responsive"), 
        "colgroups" => array(), 
        "sticky" => "", 
        "caption" => "", 
        "empty" => ""));
      
    }
    else
      $out .= "<p>" . t("Formazione non trovata") . "</p>";
  }
  
  else
    $out = "<h3>" . t("Squadra inesistente") . "</h3><br>";
  
  return $out;

}

function team_details_show_position($position, $show_penalties = FALSE) {
  $rows = array();

  if ($position) {
    $positions = array();
    foreach ($position as $pl_id => $player) {
      $roles[] = $player->role;
      $names[] = $player->name;
      $positions[] = $player->position;
    }

    array_multisort($roles, $positions, $names, $position);

    foreach ($position as $p_id => $player) {
      $rows[] = array(
          "data" => array(
              array(
                  "data" => fantacalcio_show_role($player->role),
                  "class" => "role"),
              array("data" => $player->name, "class" => "name"),
              ucwords(strtolower($player->team)),

              // "<a href=\"scheda/giocatore/1\" data-toggle='modal' data-target='#player-stat-modal' class=\"player-stats\" id=\"player-stat-" . $pl_id . "\"><i class=\"fa fa-bar-chart\"></i></a>",
              l("<i class=\"fa fa-bar-chart\"></i>", "scheda/giocatore/" . $player->id, array(
                  "html" => true,
                  "attributes" => array(
                      "data-toggle" => "modal",
                      "data-target" => "#player-stats-modal"))),

              // '<a tabindex="0" class="player-stats" data-toggle="popover" data-placement="top" data-template="<div class=\'popover\' role=\'tooltip\'><div class=\'arrow\'></div><p class=\'text-danger popover-title\'></h1><div class=\'popover-content\'></div></div>" data-trigger="focus" title="' . $player->name . '" data-content="And hers some amazing content. Its very engaging. Right?"><i class="fa fa-bar-chart"></i></a>',
              $show_penalties ? "<span title='Ordine rigoristi'>(" . ($player->penalty_position + 12) . ")</span>" : ""),
          "class" => array("role-" . $player->role));
    }
  }

  return $rows;
}

function get_details_regulars($t_id, $c_id, $round, $vote_round) {
  $lineup = array();
  $names = array();
  $positions = array();
  $roles = array();
  
  $sql = "SELECT * FROM {fanta_lineups} " . "WHERE t_id = '%d' " . "AND c_id = '%d' " . "AND round = '%d' " . "AND position = 1";
  // $result = db_query($sql, $t_id, $c_id, $round);
  
  $query = db_select("fanta_lineups", "l");
  $query->join("fanta_players", "p", "p.pl_id = l.pl_id");
  $query->join("fanta_players_rounds", "pr", "p.pl_id = pr.pl_id");
  $query->join("fanta_real_teams", "rt", "rt.rt_id = pr.rt_id");
  $query->fields("l");
  $query->fields("p");
  $query->addField("rt", "name", "team");
  $query->condition("t_id", $t_id);
  $query->condition("c_id", $c_id);
  $query->condition("l.round", $round);
  $query->condition("pr.round", $vote_round);
  $query->condition("position", 1);
  
  $result = $query->execute();
  
  foreach ($result as $row) {
    $lineup[] = $row;
    $names[] = $row->name;
    $roles[] = $row->role;
    $positions[] = $row->position;
  }
  array_multisort($positions, SORT_ASC, $roles, SORT_ASC, $names, SORT_ASC, $lineup);
  
  return $lineup;
}

function get_details_reserves($t_id, $c_id, $round, $vote_round) {
  $lineup = array();
  $names = array();
  $positions = array();
  $roles = array();
  // $sql = "SELECT * FROM {fanta_lineups} " . "WHERE t_id = '%d' " . "AND c_id = '%d' " . "AND round = '%d' " . "AND position > 1";
  // $result = db_query($sql, $t_id, $c_id, $round);
  // while ($row = db_fetch_array($result)) {
  // $lineup[] = $row;
  // $names[] = $players[$row->pl_id]->name;
  // $roles[] = $players[$row->pl_id]->role;
  // $positions[] = $row->position;
  // }
  
  // array_multisort($roles, SORT_ASC, $positions, SORT_ASC, $names, SORT_ASC, $lineup);
  
  // return $lineup;
  
  $query = db_select("fanta_lineups", "l");
  $query->join("fanta_players", "p", "p.pl_id = l.pl_id");
  $query->join("fanta_players_rounds", "pr", "l.pl_id = pr.pl_id");
  $query->join("fanta_real_teams", "rt", "rt.rt_id = pr.rt_id");
  $query->fields("l");
  $query->fields("p");
  $query->addField("rt", "name", "team");
  $query->condition("t_id", $t_id);
  $query->condition("c_id", $c_id);
  $query->condition("l.round", $round);
  $query->condition("pr.round", $vote_round);
  $query->condition("position", 1, ">");
  
  $result = $query->execute();
  
  foreach ($result as $row) {
    $lineup[] = $row;
    $names[] = $row->name;
    $roles[] = $row->role;
    $positions[] = $row->position;
  }
  array_multisort($roles, SORT_ASC, $positions, SORT_ASC, $names, SORT_ASC, $lineup);
  
  return $lineup;
}

function get_details_empty_rows($match) {
  $query = db_select("fanta_lineups", "l");
  $query->condition("c_id", $match->c_id);
  $query->condition("round", $match->round);
  $query->condition("position", 1, ">");
  $query->condition("has_played", 1);
  $query->fields("l");
  
  $query1 = $query;
  $query1->condition("t_id", $match->t1_id);
  
  $result1 = $query1->execute();
  $t1_reserves = $result1->rowCount();
  
  $query2 = $query;
  $query2->condition("t_id", $match->t2_id);
  $t2_reserves = $query2->execute()->rowCount();
  
  return max($t1_reserves, $t2_reserves);
}

function get_penalty_result($vote) {
  if ($vote >= 6)
    return "<img src='" . base_path() . drupal_get_path("module", "fantacalcio") . "/images/penalty_goal.png'>";
  else
    return "<img src='" . base_path() . drupal_get_path("module", "fantacalcio") . "/images/penalty_missed.png'>";
}

function get_details_modifiers($t_id, $m_id) {
  $match = Match::get($m_id);
  $modifier = array();
  
  if ($t_id == $match->t1_id) {
    $modifier[0] = $match->mod_1_role_0;
    $modifier[1] = $match->mod_1_role_1;
    $modifier[2] = $match->mod_1_role_2;
    $modifier[3] = $match->mod_1_role_3;
  }
  if ($t_id == $match->t2_id) {
    $modifier[0] = $match->mod_2_role_0;
    $modifier[1] = $match->mod_2_role_1;
    $modifier[2] = $match->mod_2_role_2;
    $modifier[3] = $match->mod_2_role_3;
  }
  
  return $modifier;
}

function get_bonus($t_id, $m_id) {
  $match = Match::get($m_id);
  $bonus = array();
  
  if ($t_id == $match->t1_id) {
    $bonus = $match->bonus_t1;
  }
  if ($t_id == $match->t2_id) {
    $bonus = $match->bonus_t2;
  }
  
  return $bonus;
}

function get_details_class($value, $type = "") {
  $class = "";
  if ($value > 0)
    $class = "bold " . $type;
  
  return $class;
}

<?php

/**
 * @file fantastats.topflop.inc
 *
 */
 
function statistiche_topflop_all() {
  $round = Round::getLast();
  $competition = Competition::getDefault();
  
  $choose_round_form = drupal_get_form("topflop_choose_round_all_form", $competition->id, $round);
  
  return drupal_render($choose_round_form) . topflop_all_round($round);
}

function statistiche_topflop_all_round($round) {
  
  $competition = Competition::getDefault();
  $choose_round_form = drupal_get_form("topflop_choose_round_all_form", $competition->id, $round);
  
  return drupal_render($choose_round_form) . topflop_all_round($round);
}

function statistiche_topflop($round = null, $t_id = null) {
  
  global $user;
  
  $competition = Competition::getDefault();
  
  $choose_rounds = array();
    
  $round = isset($round) && Round::exists($round) ? $round : Round::getLast();
  
  $all_rounds = Round::allLineup($competition->id);
    foreach ($all_rounds as $lineup_round) {
      $choose_rounds[$lineup_round->label] = base_path() . "formazioni/view/" . strtolower($competition->sanitized_name) . "/" . $lineup_round->competition_round;
    }

	$output["round_label"] = $all_rounds[$round]->label;
  $output["choose_rounds"] = $choose_rounds;
  
  $competition = Competition::getDefault();
    
  $teams_list = array();
  
  if ($competition) {
  
    foreach ($competition->groups as $g_id => $group) {
  
      $teams = Team::allByGroup($g_id);
  
      $expanded = false;
  
      $teams_group_list = array();
      foreach ($teams as $team_id => $team) {
  
        $classes = array("list-group-item");
        if (isset($t_id) && is_numeric($t_id)) {
          if ($team_id == $t_id) {
            array_push($classes, "active");
            $expanded = true;
          }
        }
        else
          if ($team->user == $user->uid) {
            array_push($classes, "mine");
            $expanded = true;
          }
  
        array_push($teams_group_list, array(
        "data" => l($team->name, "statistiche/topflop/" . $round . "/" . $team->id),
        "class" => $classes,
        "data-name" => $team->name));
      }
  
      $teams_list[$g_id] = array(
          "group_name" => $group->name,
          "teams" => theme_item_list(array(
              "items" => $teams_group_list,
              "attributes" => array("class" => array("list-group")),
              "type" => "ul",
              "title" => "")),
	"count" => count($teams_group_list),
          "expanded" => $expanded);
    }
  
    // print_r($teams_list);
  
    // $output = array(
    // "teams_list" => array(
    // "#items" => $teams_list,
    // "#theme" => "item_list",
  // ));
  
  $output["teams_list"] = $teams_list;
  
  if (isset($t_id) && Team::get($t_id) != null) {
    $output["main_output"] = topflop_round_team($t_id, $round);
  }
  
  return theme("columns", $output);
  }
  else {
    return t("Nessuna competizione");
  }
  
  
  //return make_columns("topflop_round_team", "statistiche/topflop/" . $round, array("round" => $round, "t_id" => $t_id), "", "topflop_choose_round_form");
}

function statistiche_topflop_round($round) {
  return make_columns("topflop_round_team", "statistiche/topflop/" . $round, array("round" => $round, "t_id" => $t_id), "", "topflop_choose_round_form");
}

function statistiche_topflop_round_team($round, $t_id) {
  return make_columns("topflop_round_team", "statistiche/topflop/" . $round, array("round" => $round, "t_id" => $t_id), "", "topflop_choose_round_form");
}

function topflop_round_team($t_id, $round) {
  
  $competition = Competition::getDefault();
  
  $team = Team::get($t_id);
  $top = top_round_team_lineup($round, $t_id, $competition->id);
  $flop = flop_round_team_lineup($round, $t_id, $competition->id);
  $tot_match = get_tot_match($round, $t_id);
  
  $header = array("Ruolo", "Nome", "", "Totale", "Voto", "GF", "GS", "Assist", "Rig. Parato", "Rig. Sbagliato", "Autogol");
  
  $out = "<h3>" . $round . "&ordf; giornata</h3><br>";
  $out .= "<h3 class='team_name'>" . $team->name . "</h3><br>";
  $out .= "<div class='shirt'><img src='" . base_path() . drupal_get_path("module", "fantacalcio") . "/images/shirts/" . $team->shirt . "'></div>";
  
  //TOP
  $out .= "<p><strong>TOP</strong></p>";
  $out .= "<p><strong>" . substr($top['module'], 2) . "</strong></p>";
  $out .= "<div class='overflow-x'>";
  $out .= theme("table", array("header" => $header, "rows" => $top['line_up'], "attributes" => array("class" => array("table", "table-responsive"))));
  $out .= "</div>";
  
  //TOP - differenza col totale
  $rows_summary = array();
  $rows_summary[] = array("Totale Partita", $tot_match);
  $rows_summary[] = array("Totale Top", $top['total']);
  $rows_summary[] = array("Differenza", ($tot_match - $top['total']));
  $out .= theme("table", array("header" => array(), "rows" => $rows_summary));

  //FLOP
  $out .= "<p><strong>FLOP</strong></p>";
  $out .= "<p><strong>" . substr($flop['module'], 2) . "</strong></p>";
  $out .= "<div class='overflow-x'>";
  $out .= theme("table", array("header" => $header, "rows" => $flop['line_up']));
  $out .= "</div>";

  //FLOP - differenza col totale
  $rows_summary = array();
  $rows_summary[] = array("Totale Partita", $tot_match);
  $rows_summary[] = array("Totale Flop", $flop['total']);
  $rows_summary[] = array("Differenza", ($tot_match - $flop['total']));
  $out .= theme("table", array("header" => array(), "rows" => $rows_summary));
  
  return $out;
}

function top_round_team_lineup($round, $t_id, $c_id) {
  
  $modules = explode("\r\n", variable_get("fantacalcio_regulars_modules", ""));
  $max = 0; $max_module = "";
  $line_up = array();
  
  if (count($modules) > 0) {
  
    foreach ($modules as $module) {
      $number_roles = explode("-", $module);
      $tot_module = 0;
      
      foreach ($number_roles as $role => $number_role) {
      
      	$query = db_select("fanta_votes", "v");
      	$query->join("fanta_players", "p", "p.pl_id = v.pl_id");
      	$query->join("fanta_squads", "s", "s.pl_id = p.pl_id");
      	$query->condition("v.round", $round);
      	$query->condition("v.has_vote", 1);
      	$query->condition("p.role", $role);
      	$query->condition("v.provider", variable_get("fantacalcio_votes_provider", 1));
      	$query->condition("s.t_id", $t_id);
      	$query->fields("v");
      	$query->fields("p");
      	$query->orderBy("v.total", "DESC");
      	$query->range(0, $number_role);
      	    
        $sql = "SELECT * FROM {fanta_votes} v, {fanta_players} p
            WHERE v.round = '%d' 
            AND p.pl_id = v.pl_id
            AND p.role = '%d' 
            AND v.provider = '%d'
            AND p.pl_id IN (SELECT pl_id FROM {fanta_squads} WHERE t_id = '%d' AND in_team = 1)          
            ORDER BY v.total DESC";
  //      $result = db_query_range($sql, array($round, $role, variable_get("fantacalcio_votes_provider", 1), $t_id), 0, $number_role);
  
  		$result = $query->execute();
        
        foreach ($result as $row) {
        
          $position = Lineup::getPlayerPosition($row->pl_id, $c_id, $t_id, $round);
          $position_class = ($position == 0) ? "out_0" : (($position > 1) ? "out_2" : "");
          $row_data = array( fantacalcio_show_role($row->role), $row->name, fantacalcio_show_cards($row->yellow_cards, $row->red_cards),
                    "<strong>" . $row->total . "</strong>", "<strong>" . $row->vote . "</strong>", 
                    ($row->goals_for + $row->penalty_goals), $row->goals_against, $row->assists,  
                    $row->saved_penalties, $row->missed_penalties, $row->own_goals);
          $line_up[$module][] = array("data" => $row_data, "class" => array("role_" . $row->role . " " . $position_class));
          $tot_module += $row->total;
        }
      }
      
      if ($tot_module > $max) {
        $max = $tot_module;
        $max_module = $module;
      }
      
    }
    
    return array("module" => $max_module, "line_up" => $line_up[$max_module], "total" => $max);
  
  }
  
}

function top_round_all_lineup($round, $c_id) {
  
  $modules = explode("\r\n", variable_get("fantacalcio_regulars_modules", ""));
  $max = 0; $max_module = "";
  
  if (count($modules) > 0) {
    
    $line_up = array();
    
    foreach ($modules as $module) {
      $number_roles = explode("-", $module);
      $tot_module = 0;
      
      foreach ($number_roles as $role => $number_role) {
        $query = db_select("fanta_votes", "v");
      	$query->join("fanta_players", "p", "p.pl_id = v.pl_id");
      	$query->condition("v.round", $round);
      	$query->condition("v.has_vote", 1);
      	$query->condition("p.role", $role);
      	$query->condition("v.provider", variable_get("fantacalcio_votes_provider", 1));
      	$query->fields("v");
      	$query->fields("p");
      	$query->orderBy("v.total", "DESC");
      	$query->range(0, $number_role);
      	    
        $sql = "SELECT * FROM {fanta_votes} v, {fanta_players} p
            WHERE v.round = '%d' 
            AND p.pl_id = v.pl_id
            AND p.role = '%d' 
            AND v.provider = '%d'
            AND p.pl_id IN (SELECT pl_id FROM {fanta_squads} WHERE t_id = '%d' AND in_team = 1)          
            ORDER BY v.total DESC";
  //      $result = db_query_range($sql, array($round, $role, variable_get("fantacalcio_votes_provider", 1), $t_id), 0, $number_role);
  
  		$result = $query->execute();
        
        foreach ($result as $row) {
        
          $row_data = array( fantacalcio_show_role($row->role), $row->name, fantacalcio_show_cards($row->yellow_cards, $row->red_cards),
                    "<strong>" . $row->total . "</strong>", "<strong>" . $row->vote . "</strong>", 
                    ($row->goals_for + $row->penalty_goals), $row->goals_against, $row->assists,  
                    $row->saved_penalties, $row->missed_penalties, $row->own_goals);
          $line_up[$module][] = array("data" => $row_data, "class" => array("role_" . $row->role . " " ));
          $tot_module += $row->total;
        }
      }
      
      if ($tot_module > $max) {
        $max = $tot_module;
        $max_module = $module;
      }
      
    }
    
    return array("module" => $max_module, "line_up" => $line_up[$max_module], "total" => $max);
  }
}

function flop_round_team_lineup($round, $t_id, $c_id) {

  $modules = explode("\r\n", variable_get("fantacalcio_regulars_modules", ""));
  $min = 1000; $min_module = "";
  
  if (count($modules) > 0) {

    $line_up = array();
    
    foreach ($modules as $module) {
      $number_roles = explode("-", $module);
      $tot_module = 0; $module_ok = TRUE;
  
      foreach ($number_roles as $role => $number_role) {
        
        $query = db_select("fanta_votes", "v");
      	$query->join("fanta_players", "p", "p.pl_id = v.pl_id");
      	$query->join("fanta_squads", "s", "s.pl_id = p.pl_id");
      	$query->condition("v.round", $round);
      	$query->condition("v.has_vote", 1);
      	$query->condition("p.role", $role);
      	$query->condition("v.provider", variable_get("fantacalcio_votes_provider", 1));
      	$query->condition("s.t_id", $t_id);
      	$query->fields("v");
      	$query->fields("p");
      	$query->orderBy("v.total", "ASC");
      	$query->range(0, $number_role);
      	    
        $sql = "SELECT * FROM {fanta_votes} v, {fanta_players} p
            WHERE v.round = '%d' 
            AND p.pl_id = v.pl_id
            AND p.role = '%d' 
            AND v.provider = '%d'
            AND p.pl_id IN (SELECT pl_id FROM {fanta_squads} WHERE t_id = '%d' AND in_team = 1)          
            ORDER BY v.total DESC";
  //      $result = db_query_range($sql, array($round, $role, variable_get("fantacalcio_votes_provider", 1), $t_id), 0, $number_role);
  
  		$result = $query->execute();
        
        if ($result->rowCount() == $number_role) {
          foreach ($result as $row) {
        
          $position = Lineup::getPlayerPosition($row->pl_id, $c_id, $t_id, $round);
          $position_class = ($position == 0) ? "out_0" : (($position > 1) ? "out_2" : "");
          $row_data = array( fantacalcio_show_role($row->role), $row->name, fantacalcio_show_cards($row->yellow_cards, $row->red_cards),
                    "<strong>" . $row->total . "</strong>", "<strong>" . $row->vote . "</strong>", 
                    ($row->goals_for + $row->penalty_goals), $row->goals_against, $row->assists,  
                    $row->saved_penalties, $row->missed_penalties, $row->own_goals);
          $line_up[$module][] = array("data" => $row_data, "class" => array("role_" . $row->role . " " . $position_class));
          $tot_module += $row->total;
        	}	
        }
        else {
          $module_ok = FALSE;
        }
      }
      
      if ($module_ok) {
        if ($tot_module < $min) {
          $min = $tot_module;
          $min_module = $module;
        }
      }
      else unset($line_up[$module]);
      
    }
  
    return array("module" => $min_module, "line_up" => $line_up[$min_module], "total" => $min);
  }
}

function flop_round_all_lineup($round, $c_id) {
  $modules = explode("\r\n", variable_get("fantacalcio_regulars_modules", ""));
  $min = 1000; $min_module = "";
  
  if (count($modules) > 0) {
    
    $line_up = array();
    
    foreach ($modules as $module) {
      $number_roles = explode("-", $module);
      $tot_module = 0; $module_ok = TRUE;
      
      foreach ($number_roles as $role => $number_role) {
        
        $query = db_select("fanta_votes", "v");
      	$query->join("fanta_players", "p", "p.pl_id = v.pl_id");
      	$query->condition("v.round", $round);
      	$query->condition("v.has_vote", 1);
      	$query->condition("p.role", $role);
      	$query->condition("v.provider", variable_get("fantacalcio_votes_provider", 1));
      	$query->fields("v");
      	$query->fields("p");
      	$query->orderBy("v.total", "ASC");
      	$query->range(0, $number_role);
      	    
        $sql = "SELECT * FROM {fanta_votes} v, {fanta_players} p
            WHERE v.round = '%d' 
            AND p.pl_id = v.pl_id
            AND p.role = '%d' 
            AND v.provider = '%d'
            AND p.pl_id IN (SELECT pl_id FROM {fanta_squads} WHERE t_id = '%d' AND in_team = 1)          
            ORDER BY v.total DESC";
  //      $result = db_query_range($sql, array($round, $role, variable_get("fantacalcio_votes_provider", 1), $t_id), 0, $number_role);
  
  		$result = $query->execute();
        
        if ($result->rowCount() == $number_role) {
          foreach ($result as $row) {
        
          $row_data = array( fantacalcio_show_role($row->role), $row->name, fantacalcio_show_cards($row->yellow_cards, $row->red_cards),
                    "<strong>" . $row->total . "</strong>", "<strong>" . $row->vote . "</strong>", 
                    ($row->goals_for + $row->penalty_goals), $row->goals_against, $row->assists,  
                    $row->saved_penalties, $row->missed_penalties, $row->own_goals);
          $line_up[$module][] = array("data" => $row_data, "class" => array("role_" . $row->role ));
          $tot_module += $row->total;
        }
        }
        else {
          $module_ok = FALSE;
        }
      }
      
      if ($module_ok) {
        if ($tot_module < $min) {
          $min = $tot_module;
          $min_module = $module;
        }
      }
      else unset($line_up[$module]);
      
    }
    
    return array("module" => $min_module, "line_up" => $line_up[$min_module], "total" => $min);
  }
}

function topflop_all_round($round) {
  
  $competition = Competition::getDefault();
  
  $top = top_round_all_lineup($round, $competition->id);
  $flop = flop_round_all_lineup($round, $competition->id);
  
  $header = array("Ruolo", "Nome", "", "Totale", "Voto", "GF", "GS", "Assist", "Rig. Parato", "Rig. Sbagliato", "Autogol");
  
  $out = "<h3>" . $round . "&ordf; giornata</h3><br>";
  
  //TOP
  $out .= "<p><strong>TOP</strong></p>";
  $out .= "<p><strong>" . substr($top['module'], 2) . "</strong></p>";
  $out .= "<div class='overflow-x'>";
  $out .= theme("table", array("header" => $header, "rows" => $top['line_up']));
  $out .= "</div>";
  
  //TOP - differenza col totale
  $rows_summary = array();
  $rows_summary[] = array("Totale Top", array("data" => $top['total'], "class" => "bold"));
  $out .= theme("table", array("header" => array(), "rows" => $rows_summary));

  //FLOP
  $out .= "<p><strong>FLOP</strong></p>";
  $out .= "<p><strong>" . substr($flop['module'], 2) . "</strong></p>";
  $out .= "<div class='overflow-x'>";
  $out .= theme("table", array("header" => $header, "rows" => $flop['line_up']));
  $out .= "</div>";

  //FLOP - differenza col totale
  $rows_summary = array();
  $rows_summary[] = array("Totale Flop", array("data" => $flop['total'], "class" => "bold"));
  $out .= theme("table", array("header" => array(), "rows" => $rows_summary));
  
  return $out;
}

function get_tot_match($round, $t_id) {
//   $sql = "SELECT * FROM {fanta_matches} 
//       WHERE g_id IN (SELECT g_id FROM {fanta_groups} WHERE c_id = '%d')
//       AND round = '%d' 
//       AND (t1_id = '%d' OR t2_id = '%d')";
//   $result = db_query($sql, variable_get("fantacalcio_main_competition", 1), get_competition_round_by_round($round, variable_get("fantacalcio_main_competition", 1)), $t_id, $t_id);
  
//   $result = $query->execute();
  
  $match = Match::getByTeamAndRound($t_id, Competition::getDefault()->id, $round);
  
  if ($t_id == $match->t1_id) 
    return ($match->tot_1 - $match->mod_1_role_0 - $match->mod_1_role_3 - $match->mod_1_role_3 - $match->mod_1_role_3);
  if ($t_id == $match->t2_id) 
    return ($match->tot_2 - $match->mod_2_role_0- $match->mod_2_role_1 - $match->mod_2_role_2 - $match->mod_2_role_3);
}

function topflop_choose_round_form($form, &$form_state, $c_id, $round) {

  $max_round = get_last_votes();
  
  for ($i = 1; $i <= $max_round; $i++)
    $rounds[$i] = $i . filter_xss("a Giornata");
  
  $form['round'] = array(
    '#type' => 'select',
    '#options' => $rounds,
    '#default_value' => $round,
    '#attributes' => array("class" => "form-control", "data-mini" => "false", "data-inline" => "true", "onchange" => "this.form.submit()")
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#attributes' => array("class" => "hide")
  );
  
  return $form;
}

function topflop_choose_round_form_submit($form, &$form_state) {
  $round = $form_state['values']['round'];

  drupal_goto("statistiche/topflop/" . $round);
}

function topflop_choose_round_all_form($form, &$form_state, $c_id, $round) {

  $max_round = Round::getLast();
  
  for ($i = 1; $i <= $max_round; $i++)
    $rounds[$i] = $i . filter_xss("a Giornata");
  
  $form['round'] = array(
    '#type' => 'select',
    '#options' => $rounds,
    '#default_value' => $round,
    '#attributes' => array("class" => array("form-control"), "data-mini" => "false", "data-inline" => "true", "onchange" => "this.form.submit()")
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#attributes' => array("class" => array("hide"))
  );
  
  return $form;
}

function topflop_choose_round_all_form_submit($form, &$form_state) {
  $round = $form_state['values']['round'];

  drupal_goto("statistiche/topflop-all/" . $round);
}

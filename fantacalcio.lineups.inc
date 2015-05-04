<?php 
/**
*
* @file fantacalcio.lineup.inc
*
*/

/********************/
/* menu        */
/********************/
function _fantacalcio_lineup() {
  if (user_access("access fantacalcio")) $items[] = l("Visualizza", "formazioni/view");
  if (user_access("inserisci lineup")) $items[] = l("Inserisci", "formazioni/insert");
  if (user_access("inserisci lineup")) $items[] = l("Multe", "formazioni/multe");
  if (user_access("amministra formazioni")) $items[] = l("Amministra", "formazioni/admin");
  return theme_item_list(array("items" => $items, "title" => "", "type" => "ul", "attributes" => array()));
}

/************************/
/* visualizzazione    */
/************************/
function _fantacalcio_lineup_view($competition_name, $round_number =  null, $t_id = null) {

	global $user;
	  
  $selected_competition = Competition::getByName($competition_name);
  
  $competitions = Competition::all(array("has_lineups" => 1));
  
  foreach($competitions as $c_id => $competition) {
  	
  	$round_number = (!isset($round_number) || $round_number == null || !Round::existsInCompetition($round_number, $c_id)) ? Round::getLastLineup($c_id) : $round_number;

//   	$main_round_number = Round::getRoundByCompetitionRound($round_number, $competition->id);
  	
//   	$round = Round::getCompetitionRound($main_round_number, $c_id);
  	
  	$round = Round::getByCompetitionRound($round_number, $competition->id);print_r($round);echo "<br>";
  	
  	if ($round == null)
  		$round = Round::getLastLineups($competition->id);
  	
  	$teams_list = array();

  	foreach($competition->groups as $g_id => $group) {
  	
  		$teams = Team::allByGroupLineups($c_id, $g_id, $round->competition_round);
  	
  		$expanded = false;
  			
  		$teams_group_list = array();
  		foreach ($teams as $team_id => $team) {
  				
  			$classes = array("list-group-item");
  			if (isset($array_args["t_id"]) && $team_id == $t_id )
  				array_push($classes, "active");
  			if ($team->user == $user->uid) {
  				array_push($classes, "mine");
  				$expanded = true;
  			}
  				
  			array_push($teams_group_list, array("data" => l($team->name, "formazioni/view/" .$competition->name . "/" . $round->competition_round . "/" . $team->id), "class" => $classes, "data-name" => $team->name));
  		}
  	
  		$teams_list[$g_id] = array("group_name" => $group->name,
  				"teams" => theme_item_list(array("items" => $teams_group_list, "attributes" => array("class" => array("list-group")), "type" => "ul", "title" => "")),
  				"expanded" => $expanded);
  	}
  	
  	$columns_output["teams_list"] = $teams_list;
  	
  	if ($selected_competition == $competition && Team::exists($t_id))
  		$columns_output["main_output"] = show_lineup(Team::get($t_id), $selected_competition, $round);
  	
  	$choose_rounds = array();
  	$max_round = 19;//TODO
  	for($i = 1; $i <= $max_round; $i++) {
  		$choose_rounds[$i] = base_path() . "formazioni/view/" . strtolower($competition->name) . "/" . $i;
  	}
  	
  	$columns_output["choose_rounds"] = $choose_rounds;
  	
  	$competition->output = theme("columns", $columns_output);  	 
  	  	
  	$competition->active = (isset($competition_name) && Competition::getByName($competition_name) != null && strtolower($competition->name) == strtolower($competition_name));
  }
  
  $output = array();
  $output["competitions"] = $competitions;
  $output["active"] = 1;//TODO
  
  return theme("lineup-view", $output);
}

function _fantacalcio_formazioni_box($c_name, $round, $t_id) {
  $args = array("c_name" => $c_name, "round" => $round, "t_id" => $t_id);
  $args['c_id'] = get_competition_id($args['c_name']);
  return show_lineup($args);
}

// function get_lineup($args = array()) {

//   $c_name = $args["c_name"];
//   $t_id = $args["t_id"];
//   $round = $args["round"];

//   $c_id = get_competition_id(strtolower($c_name));
//   $args["c_id"] = $c_id;
//   $round = (!empty ($round)) ? $round : get_last_round_lineup($c_id);

//   $out = make_columns('show_lineup', 'formazioni/view/' . $c_name . '/' . $round, $args, 'filter_teams_formazioni', 'choose_formazioni_round_form');

//   return $out;

// }

function show_lineup($team, $competition, $round) {

  $round = (isset ($round)) ? $round : Round::getLastLineups($competition_id);

  #squadra trovata
  if (Team::exists($team->id)) {

  	drupal_set_title(filter_xss($competition->name . " - " . $round->label));

    $out = "<h3 class='team_name'>" . $team->name . "</h3><br>";

    $out .= "<div class='shirt'><img src='" . base_path() . drupal_get_path("module", "fantacalcio") . "/images/shirts/" . $team->shirt . "'></div>";
        
    $lineup = Lineup::get($competition->id, $team->id, $round->competition_round);

    if ($lineup) {

      ksort($lineup->positions);

      #ordino i titolari(ruolo, nome)
      $roles = array();
      $names = array();
      $module = array(0, 0, 0, 0);

      foreach ($lineup->positions[1] as $starting_player) {
        $module[$starting_player->role]++;
        $roles[] = $starting_player->role;
        $names[] = $starting_player->name;
      }

      array_multisort($roles, SORT_ASC, $names, SORT_ASC, $lineup->positions[1]);

      #ordino le riserve(ruolo, posizione, nome)
      #ordino i titolari(ruolo, nome)
      $roles = array();
      $names = array();
      
      $reserves = array();
      foreach ($lineup->positions as $position => $players_positions) {
      	if($position > 1)
     		$reserves = array_merge($reserves, $players_positions);
      }

      foreach ($reserves as $reserve_player) {
        $roles[] = $reserve_player->role;
        $names[] = $reserve_player->name;
      }

      array_multisort($roles, SORT_ASC, $names, SORT_ASC, $reserves);

      //modulo
      if ($module) {
        ksort($module);

        unset($module[0]);
        $out .= "<p><strong>" . implode(" - ", $module) . "</strong></p>";
      }
      
      //mostra l'ordine dei rigoristi se è previsto il passaggio del turno
      $show_penalties = $round->next;

      #titolari
      $header_startings = array( array("data" => t("Titolari"), "colspan" => 5) );
      $rows_startings = show_position($lineup->positions[1], $show_penalties);

      #riserve
      $header_reserves = array(array("data" => t("Riserve"), "colspan" => 5));
      $rows_reserves = show_position($reserves);

      $out .= theme_table(array("header" => $header_startings, "rows" => $rows_startings, "attributes" => array("class" => "table table-responsive"), "colgroups" => array(), "sticky" => "", "caption" => "", "empty" => "" ));
      $out .= theme_table(array("header" => $header_reserves, "rows" => $rows_reserves , "attributes" => array("class" => "table table-responsive"), "colgroups" => array(), "sticky" => "", "caption" => "", "empty" => "" ));

    } 
    else $out .= "<p>" . t("Formazione non trovata") . "</p>";

  } 
  
  else $out = "<h3>" . t("Squadra inesistente") . "</h3><br>";

  return $out;
}

function show_position($position, $show_penalties = FALSE) {

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
                      array("data" => "<span class=\"fa-stack\">
					<i class=\"fa fa-square fa-stack-2x role-" . $player->role . "\"></i>
					<i class=\"fa fa-stack-1x\" style=\"color: white;\"><span class=\"font-normal\">" . Player::convertRole($player->role) . "</span></i>
				</span>", "class" => "role"),
                      array("data" => $player->name, "class" => "name"),
                      ucwords(strtolower($player->team)),
                      "<a href=\"#\" data-toggle=\"modal\" data-target=\"#player-stats-modal\" class=\"player-stats\" id=\"player-stat-" . $pl_id . "\"><i class=\"fa fa-bar-chart\"></i></a>",
                      $show_penalties ? "<span title='Ordine rigoristi'>(" . ($player->penalty_position + 12) . ")</span>" : ""
                      ),
              "class" => array("role-" . $player->role)
              );

    }
  }

  return $rows;
}

/************************/
/* END visualizzazione  */
/************************/

/************************/
/* inserimento      */
/************************/

function fantacalcio_lineup_insert_start($team_id = "", $competition_name = "") {
  global $user;
  
  //teams
  $teams = array();
  if (!empty($team_id) && Team::exists($team_id)) {
  	$team = Team::get($team_id);
  	array_push($teams, $team);
  }
  else  
  	$teams = Team::allByUser($user->uid);
  
  //competizioni attive nella giornata
  $round = Round::getLast() + 1;
  
  //competitions
  if (!empty($competition_name) && $competition_name != null && Competition::getByName($competition_name) != null) {
  	$competition = Competition::getByName($competition_name);
  	$round_competitions = Competition::allForRound($round, $competition->id);
  }
  else
  	$round_competitions = Competition::allForRound($round);
  
  $lineups_to_insert = array();
  foreach($round_competitions as $c_id => $competition) {
  	foreach($teams as $t_id => $team) {
  		if ($team->inCompetition($c_id) && (!$competition->has_matches || ($competition->has_matches && $team->hasMatch($c_id, $competition->competition_round)) )) {
  			$lineups_to_insert[] = (object) array("team" => $team, "competition" => $competition);
  		}
  	}
  }
  
  //zero squadre --> Avviso
  if (count($lineups_to_insert) == 0)
  	return t("Non hai ancora nessuna squadra");
  
//   1 squadra --> redirect
  if (count($lineups_to_insert) == 1)
  	return fantacalcio_lineup_insert($competition, $team, $competition->competition_round, $is_admin = FALSE);
  
  //+1 squadre --> scelta
  if (count($lineups_to_insert) >= 1) {
  	$own_teams_rows = array();
  
  	foreach($lineups_to_insert as $lineup_to_insert) {
  		$classes = array();
  		$row = array();
  		if ($lineup_to_insert->team->isConfirmed())
  			array_push($classes, "bold");
  			array_push($row, l($lineup_to_insert->team->name, "formazioni/insert/" . $lineup_to_insert->team->id . "/" . $lineup_to_insert->competition->sanitized_name) );
  			if (Lineup::exists($lineup_to_insert->team->id, $lineup_to_insert->competition->id, $lineup_to_insert->competition->competition_round))
  				$class = "fa fa-check-circle";
  			else 
  				$class = "fa fa-minus-circle";
  			array_push($row, "<i class='" . $class . "'></i>");
  			array_push($row, $lineup_to_insert->competition->name . " - " . $lineup_to_insert->competition->round_label . "" );
  		array_push($own_teams_rows, array("data" => $row, "class" => $classes, "data-name" => $lineup_to_insert->team->name));
  	}
  
  	return array(  				
  		"#rows" => $own_teams_rows,
  		"#attributes" => array("class" => array("table", "table-responsive")),
  		"#theme" => "table",
  	);
  }
  
}

function fantacalcio_lineup_insert_start_admin($t_id, $competition_name = '') {
  return fantacalcio_lineup_insert_choose_competition($t_id, $competition_name, TRUE);
}
/*
function fantacalcio_lineup_insert_choose_competition($t_id, $competition_name = "", $is_admin = FALSE) {

  global $user;

  $team = Team::get($t_id);

  if ($team->user != $user->uid) 
  	return drupal_goto(drupal_access_denied());
  
  $competition = Competition::getByName($competition_name);
  
  if (empty ($competition_name) || $competition == null) {
  
    #controllo quante formazioni vanno inserite per la giornata
    $round = Round::getLast() + 1;
    
    //competizioni attive nella giornata
    $competitions = Competition::allForRound($round);
    
    $inserted_lineups = array();
	foreach ($competitions as $c_id => $competition) {
// 		foreach($teams)
	}
    
    
    $sql = "SELECT * FROM {fanta_rounds_competitions} WHERE round = :round";
    $result = db_query($sql, array(":round" => $vote_round));
    foreach ($result as $row) {
      $sqla = "SELECT * FROM {fanta_matches} " .
          "WHERE g_id IN (SELECT g_id FROM {fanta_groups} WHERE c_id = :c_id) " .
          "AND round = :round " .
          "AND (t1_id = :t_id OR t2_id = :t_id)";
      $resulta = db_query($sqla, array(":c_id" => $row->c_id, ":round" => $row->competition_round, ":t_id" => $t_id));
      foreach ($resulta as $rowa) {
        $lineups[$rowa->g_id] = $rowa->round;
      }
    }

    if (count($lineups) > 1) {
      $out = "<p>Inserire una lineup per ogni partita</p>";
      foreach ($lineups as $g_id => $round) {
        $c_id = get_cid_by_gid($g_id);
        $competition = $competitions[$c_id];

        $insert = check_lineup_insert($t_id, $c_id, $round);
        $opponent = get_opponent($t_id, $c_id, $round);
        $match = get_match($round, $t_id, $c_id);
        $opponent_link = l($teams[$opponent]->name, "formazioni/view/" . $competition->name . "/" . $round . "/" . $opponent, array("attributes" => array("title" => "lineup " . $teams[$opponent]->name)) );

        $round_label = get_round_label($match);

        $rows[$c_id][] = "<a href='" . base_path() . arg(0) . "/" . arg(1) . "/" . $competition->name . "'>" . $competition->name . " - " . $round_label . "</a>";
        $rows[$c_id][] = "<span class='insert'>" . $insert . "</span>";
        $rows[$c_id][] = "<span class='opponent'>" . $teams[$t_id]->name . " - " . $opponent_link . "</span>";
      }

      $out .= "<div style='width: 70%;'>" . theme_table(array(), $rows, array()) . "</div>";

      return $out;
    }
    elseif (count($lineups) == 1) {
      foreach ($lineups as $c_id => $round) $competition_name = $competitions[$c_id]->name;

      //return insert_lineup($competition_name);
      return fantacalcio_lineup_insert(Competition::get(variable_get("fantacalcio_main_competition", 1)), $team, $is_admin);
    }
    else return fantacalcio_lineup_insert(Competition::get(variable_get("fantacalcio_main_competition", 1)), $team, $is_admin);
  }

  else return fantacalcio_lineup_insert(Competition::getByName($competition_name), $team, $is_admin);

}
*/
//fantacalcio_lineup_insert_start
function fantacalcio_lineup_insert($competition, $team, $competition_round, $is_admin = FALSE) {
	
	global $user;
	
	$output = array();
  
	$round = Round::getByCompetitionRound($competition_round, $competition->id);
	
	drupal_set_title(t("Inserisci formazione") . " / " . $team->name . " - " . $competition->name);
	
  	#lineup inserita in orario
  	if ($round->date != time() || $is_admin) {

    	$output["competition_name"] = ucwords($competition->name);
    	$output["round"] = $round->label;

// 	    $args = array(
// 	      't_id' => $team->id,
// 	      'c_id' => $competition->id,
// 	      'round' => $round,
// 	    );
    	//print_r($team);die();
    	$lineup_form = (drupal_get_form('fantacalcio_lineup_form', $competition, $team, $round));
    	$output["lineup_form"] = $lineup_form;
    	
    	//$args_confirm = get_args_confirm($args);

    	//$out .= confirm_lineup($args_confirm);
      
		$lineup = Lineup::get($competition->id, $team->id, $round->competition_round);
    	if ($lineup != null)
    		$output["lineup_check"] = show_line_up_check($lineup->check());
    		  
    	//$out .= "<div id='_lineup_form'>" . drupal_render($lineup_form) . "</div>";
    
		  
	    //link lineup avversario
	    $out = "\n<div class='ui-block-b'>"
	          . l("lineup avversario", "formazioni/box/" . $competition->name . "/" . $round->competition_round . "/" . get_opponent($team->id, $competition->id, $round->competition_round), array("html" => TRUE, "attributes" => array("rel" => "lightframe[box|width:500px;]")))
	          . "</div>";
	    
    	$output = array_merge($output, make_lineup($team, $competition, $round));
  }
  else {
	fantacalcio_lineup_insert_players('old', $team->id, $competition->id, $round->competition_round);
    drupal_goto("formazioni/view/" . $competition->name . "/" . $round->competition_round . "/" . $team->id);
  }
  
  return theme("lineup-insert", $output);

}

function fantacalcio_lineup_form($form, &$form_state, $competition, $team, $round) {

  global $user;

  $lineup = Lineup::get($competition->id, $team->id, $round->competition_round);
  
  $checks = array("regulars_number" => false, "regulars_module" => false, "reserves_number" => false, "reserves_module" => false);
  if($lineup)
  	$checks = $lineup->check();

  //form redirect (rigoristi se previsto il passaggio del turno)
  //if (is_passaggio_turno($args['c_id'], $args['round']) )
    $form['#redirect'] = 'formazioni/insert/' . $competition->name . '/penalties' ;
  //else $form['#redirect'] = 'formazioni/view/' . get_competition_name($args['c_id']) . '/' . $args['round'] . '/' . $args['t_id'] ;

  if (isset($args['status']) && $args['status'] == 1)
    $disabled = FALSE;
  else
    $disabled = TRUE;

  $form['t_id'] = array(
    '#type' => 'hidden',
    '#value' => $team->id
  );

  $form['c_id'] = array(
    '#type' => 'hidden',
    '#value' => $competition->id
  );

  $form['round'] = array(
    '#type' => 'hidden',
    '#value' => $round->round
  );

  $form['lineup'] = array(
    '#type' => 'hidden',
    '#value' => json_encode(Lineup::getForForm($competition->id, $team->id, $round->competition_round)),
  	'#id' => "tmp-lineup",
  );
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Invia',
    '#disabled' => ($checks['regulars_number'] == 1 && $checks['regulars_module'] == 1 && $checks['reserves_number'] == 1 && $checks['reserves_module'] == 1),
    '#validate' => array('fantacalcio_lineup_form_validate'),
    '#id' => "line_up_submit",
	'#prefix' => "<div class='ui-grid-a'><div class='ui-block-a'>",
	'#suffix' => "</div>",
  );

  $form['clear'] = array(
    '#type' => 'submit',
    '#value' => 'Cancella',
    '#validate' => array(
      'fantacalcio_lineup_form_clear'
    ),
    '#attributes' => array(
      'onclick' => "return confirm_reset_lineup()"
    ),
	'#prefix' => "<div class='ui-block-b'>",
	'#suffix' => "</div></div>",
  );

  return $form;
}

function fantacalcio_lineup_form_submit($form, &$form_state) {
  global $user;

  $t_id = $form_state['values']['t_id'];
  $c_id = $form_state['values']['c_id'];
  $round = $form_state['values']['round'];

//   $ts_match = get_timestamp_match($c_id, $round);

  #lineup inviata in orario
  if (Round::get($round, $c_id)->date > time() || true) {

    $lineup = json_decode($form_state['input']['lineup']);

    //cancello la vecchia lineup
//     $sql = "DELETE FROM {fanta_lineups} WHERE t_id = '%d' AND c_id = '%d' AND round = '%d'";
//     db_query($sql, $t_id, $c_id, $round);
    
    $query = db_delete("fanta_lineups");
    $query->condition("t_id", $t_id);
    $query->condition("c_id", $c_id);
    $query->condition("round", $round);
    $query->execute();

    foreach($lineup as $pl_id => $player) {
      //insert
      if ($player->position > 0) {
        $sql = "INSERT INTO {fanta_lineups} (t_id, pl_id, c_id, round, position, timestamp, uid) VALUES ('%d', '%d', '%d', '%d', '%d', '%d', '%d')";
//         db_query($sql, $t_id, $pl_id, $c_id, $round, $player->position, time(), $user->uid);
        
        $query = db_insert("fanta_lineups");
        $query->fields(array("t_id" => $t_id, "pl_id" => $pl_id, "c_id" => $c_id, "round" => $round, "position" => $player->position, "timestamp" => time(), "uid" => $user->uid));
        $query->execute();
      }
    }

    fantacalcio_lineup_insert_players('new', $t_id, $c_id, $round);
    
    drupal_set_message(t("Formazione inserita correttamente."));

    drupal_goto("formazioni/view/" . Competition::get($c_id)->name . "/" . $round . "/" . $t_id);
  }
  #lineup inviata in ritardo -> giornata precedente
  else {
    fantacalcio_lineup_insert_players('old', $t_id, $c_id, $round);
    drupal_set_message(t("Tempo scaduto. Userai la lineup della giornata precedente"));
  }

  return;
}

function fantacalcio_lineup_form_clear($form, &$form_state) {

  #cancella la lineup inserita
  
  $query = db_delete("fanta_lineups");
  $query->condition("t_id", $form_state['values']['t_id']);
  $query->condition("c_id", $form_state['values']['c_id']);
  $query->condition("round", $form_state['values']['round']);
  $query->execute();
  
  $query = db_delete("fanta_lineups_inserts");
  $query->condition("t_id", $form_state['values']['t_id']);
  $query->condition("c_id", $form_state['values']['c_id']);
  $query->condition("round", $form_state['values']['round']);
  $query->execute();

  drupal_set_message(t("Formazione cancellata"));
  
  watchdog('fantacalcio', '@team: formazione cancellata', array('@team' => Team::get($form_state['values']['t_id'])->name), WATCHDOG_NOTICE);

  return $form_state['rebuild'] = TRUE;
}

function fantacalcio_lineup_form_validate($form, &$form_state) {

  $t_id = $form_state['input']['t_id'];
  $c_id = $form_state['input']['c_id'];
  $round = $form_state['input']['round'];

  $tmp_lineup = json_decode($form_state['input']['lineup']);
  print_r($tmp_lineup);
  
  $positions = array();
  foreach($tmp_lineup as $pl_id => $lineup_player) {
  	$player = new Player($pl_id, "", $lineup_player->role);
  	$positions[$lineup_player->position][$pl_id] = $player;
  }
  
  $lineup = new Lineup();
	$lineup->round = $form_state['values']['round'];
	$lineup->team = Team::get($form_state['values']['t_id']);
	$lineup->competition = Competition::get($form_state['values']['c_id']);
	$lineup->positions = $positions;	
  

  //check giocatori in rosa
  $check_players_in_team = true; 
  foreach($lineup->positions as $position => $positions) {
  	if($position != 0) {
	  	foreach($positions as $pl_id => $player) {
	// 	    $sql = "SELECT * FROM {fanta_squads} WHERE pl_id = '%d' ANd t_id = '%d' AND in_team = 1";
		    
		    $query = db_select("fanta_squads", "s");
		    $query->condition("pl_id", $pl_id);
		    $query->condition("t_id", $t_id);
		    $query->condition("status", 1);
		    $query->fields("s");
		    
		    $result = $query->execute();
		    if ($result->rowCount() == 0) {
		    	echo "ROSA ".$pl_id;
		        form_set_error("", t("Errore nella formazione"));
		    }
	  	}
  	}
  }

  if ($check_players_in_team) {
    //check ruolo giocatori
    $check_players_role = true;
    foreach($lineup->positions as $position => $positions) {
	  	foreach($positions as $pl_id => $player) {
	    	if (Player::get($pl_id)->role != $player->role) {	    		
	          form_set_error("", t("Errore nella formazione"));
	    	}
	    }
    }

    if ($check_players_role) {
      //check modulo
      $checks = $lineup->check();//print_r($line_up); print_r($checks);die();

      if (!$checks['regulars_number'] && $checks['regulars_number'] != 1)
        form_set_error("", t("Errore nella formazione: Numero titolari errato - Numero titolari consentito: 11"));
      if (!$checks['regulars_module'] && $checks['regulars_module'] != 1)
        form_set_error("", t("Errore nella formazione: Modulo titolari errato - Moduli titolari consentiti: " . variable_get("fantacalcio_moduli_titolari", "")));    
      if (!$checks['reserves_number'] && $checks['reserves_number'] != 1)
        form_set_error("", t("Errore nella formazione: Numero riserve errato - Numero riserve consentite: 7"));
      if (!$checks['reserves_module'] && $checks['reserves_module'] != 1)
        form_set_error("", t("Errore nella formazione: Modulo riserve errato - Moduli riserve consentiti: " . variable_get("fantacalcio_moduli_riserve", "")));    
    }
    else {
      //error
    	echo "MDOULO";
      form_set_error("", t("Errore nella formazione"));
    }
  }
  else {
    //error
  	echo "ELSE";
    form_set_error("", t("Errore nella formazione"));
  }
}

function make_lineup($team, $competition, $round, $admin = FALSE) {
  
  global $user;

  $lineup = array();
  $regulars = array();
  $reserves = array();
  
  $lineup = Lineup::get($competition->id, $team->id, $round->competition_round);
  
  $squad = $team->getSquad();

  if ($squad) {
  foreach ($squad as $player) {
    $roles[] = $player->role;
    $names[] = $player->name;
  }
  
  array_multisort($roles, SORT_ASC, $names, SORT_ASC, $squad);

  //rosa
  $out_squad = "";
  
  $lineup_player_ids = array();
  if (isset($lineup->positions)) {
	  foreach($lineup->positions as $positions) {
	  	foreach($positions as $player)
	  		$lineup_player_ids[$player->id] = $player->id;
	  }
  }
  
  foreach ($squad as $player) {
    if (!array_key_exists($player->pl_id, $lineup_player_ids)) {

      $classes = array("role_" . $player->role);

//       $out_squad .= "\n<div  class='groupItem' id='$pl_id' name='" . $player->role . "_" . $competition->id . "_" . $team->id . "'>" .
//       "\n<div  class='itemHeader'>\n" ;

//       $out_squad .= "\n<table width='100%'>" .
//       "\n<tr class=''>" .
//       "\n<td class='role' width='10%'>" . Player::convertRole($player->role) . "</td>" .
//       "\n<td width='50%'>" . $player->name . "</td>" .
//       "\n<td width='35%'>" . ucwords(strtolower($player->team)) . "</td>" .
//       "\n<td><a rel='lightframe[" . $team->id . "|width:770px;]' href='" . base_path() . "scheda/giocatore/" . $pl_id . "' title='Dettagli giocatore'>" .
//       "<img src='" . base_path() . drupal_get_path("module", "fantacalcio") . "/images/stats.png'>" .
//       "</a></td>".
//       "\n</tr>" .
//       "\n</table>";
      
      $row_data = array();
      
      $row_data[] = array("data" => "<span class='fa-stack'>
						<i class='fa fa-square fa-stack-2x role-" . $player->role . "'></i>
						<i class='fa fa-stack-1x' style='color: white;'><span class='font-normal'>" . Player::convertRole($player->role) . "</span></i>
					</span>", "class" => $classes);
      $row_data[] = array("data" => $player->name);
      $row_data[] = array("data" => ucwords(strtolower($player->team)));
      $row_data[] = array("data" => "<a rel='lightframe[" . $team->id . "|width:770px;]' href='" . base_path() . "scheda/giocatore/" . $player->pl_id . "' title='Dettagli giocatore'>" .
      "<img src='" . base_path() . drupal_get_path("module", "fantacalcio") . "/images/stats.png'>" .
      "</a>");
      
      $squad_rows[] = array("data" => $row_data, "class" => $classes, "data-id" => $player->pl_id, "data-name" => $player->name, "data-role" => $player->role);

//         $out_squad .= "\n</div>"
//           # . "\n<div class='itemContent'>\n"
//           #   . "\n</div>"
//            . "\n</div>";

//       $i++;
    }
  }
  
  $out_squad = theme_table(array(
  			"header" => array(), 
  			"rows" => $squad_rows, 
  			"attributes" => array("id" => "lineup-squad", "class" => array("table", "table-responsive", "lineup-group"), "data-position" => 0), 
  			"sticky" => TRUE, 
  			"caption" => "" , 
  			"colgroups" => array(),
  			"empty" => ""
  ));

  #titolari
  $regulars_rows = array();
  
  if (isset($lineup->positions)) {
	  $roles = array(); $names = array();
	  foreach ($lineup->positions[1] as $regular) {
	    $roles[] = $regular->role;
	    $names[] = $regular->name;
	  }
	
	  array_multisort($roles, SORT_ASC, $names, SORT_ASC, $lineup->positions[1]);
  
  
  
	  
	  foreach ($lineup->positions[1] as $regular) {
	    $class = "role_" . $regular->role;
	
	//     $out_regulars .= "\n<div  class='groupItem' id='$pl_id' name='" . $regular->role . "_" . $competition->id . "_" . $team->id . "'>" .
	//       "\n<div  class='itemHeader'>\n";
	    
	//     $out_regulars .= "\n<table width='100%'>" .
	//     "\n<tr class='$class'>" .
	//     "\n<td class='role' width='10%'>" . Player::convertRole($regular->role) . "</td>" .
	//     "\n<td width='50%'>" . $regular->name . "</td>" .
	//     "\n<td width='35%'>" . ucwords(strtolower($regular->team)) . "</td>" .
	//     "\n<td width='5%' style='padding-left:5px;'>".
	//     "\n<td><a rel='lightframe[" . $team->id . "|width:770px;]' href='" . base_path() . "scheda/giocatore/" . $regular->id . "' title='Dettagli giocatore'>" .
	//     "<img src='" . base_path() . drupal_get_path("module", "fantacalcio") . "/images/stats.png'>" .
	//     "</a></td>".
	//     "\n</td>" .
	//     "\n</tr>" .
	//     "\n</table>";
	    
	//     $out_regulars .= "\n</div>"
	//       # . "\n<div class='itemContent'>\n"
	//       #   . "\n</div>"
	//        . "\n</div>";
	    $row_data = array();
	    
	    $row_data[] = array("data" => "<span class='fa-stack'>
							<i class='fa fa-square fa-stack-2x role-" . $regular->role . "'></i>
							<i class='fa fa-stack-1x' style='color: white;'><span class='font-normal'>" . Player::convertRole($regular->role) . "</span></i>
						</span>", "class" => $classes);
	    $row_data[] = array("data" => $regular->name);
	    $row_data[] = array("data" => ucwords(strtolower($regular->team)));
	    $row_data[] = array("data" => "<a rel='lightframe[" . $team->id . "|width:770px;]' href='" . base_path() . "scheda/giocatore/" . $regular->id . "' title='Dettagli giocatore'>" .
	    		"<img src='" . base_path() . drupal_get_path("module", "fantacalcio") . "/images/stats.png'>" .   		
	    		"</a>");          
	       
	    $regulars_rows[] = array("data" => $row_data, "class" => $classes, "data-id" => $regular->id, "data-name" => $regular->name, "data-role" => $regular->role);
	
	  }
  }

  	$regulars_rows[] = array("data" => array(""), "class" => array("empty-row"));

  
  $out_regulars = theme_table(array(
  		"header" => array(),
  		"rows" => $regulars_rows,
  		"attributes" => array("id" => "lineup-regulars", "class" => array("table", "table-responsive", "lineup-group"), "data-position" => 1),
  		"sticky" => TRUE,
  		"caption" => "" ,
  		"colgroups" => array(),
  		"empty" => ""
  ));

  #riserve
  $reserves_rows = array();
  
  if (isset($lineup->positions)) {
  
	  $reserves = array();
	  foreach($lineup->positions as $position => $reserve_positions) {
	  	if ($position != 1)
	  		$reserves[$position] = $reserve_positions;
	  }
	
	  $roles = array(); $names = array(); $positions = array(); $tmp_reserves = array();
	  foreach ($reserves as $position => $position_reserves) {
	  	foreach ($position_reserves as $reserve) {
	    	$roles[] = $reserve->role;
	    	$names[] = $reserve->name;
	    	$positions[] = $position;
	    	$tmp_reserves[] = $reserve;
	  	}
	  }
	  
	  $reserves = $tmp_reserves;
	
	  array_multisort($roles, SORT_ASC, $positions, SORT_ASC, $names, SORT_ASC, $reserves);
	  
	  foreach ($reserves as $reserve) {
	    $class = array("role_" . $reserve->role);
	    
	    $row_data = array();
	    
	    $row_data[] = array("data" => "<span class='fa-stack'>
							<i class='fa fa-square fa-stack-2x role-" . $reserve->role . "'></i>
							<i class='fa fa-stack-1x' style='color: white;'><span class='font-normal'>" . Player::convertRole($reserve->role) . "</span></i>
						</span>", "class" => $classes);
	    $row_data[] = array("data" => $reserve->name);
	    $row_data[] = array("data" => ucwords(strtolower($reserve->team)));
	    $row_data[] = array("data" => "<a rel='lightframe[" . $team->id . "|width:770px;]' href='" . base_path() . "scheda/giocatore/" . $reserve->id . "' title='Dettagli giocatore'>" .
	    		"<img src='" . base_path() . drupal_get_path("module", "fantacalcio") . "/images/stats.png'>" .
	    		"</a>");
	
	    $reserves_rows[] = array("data" => $row_data, "class" => $classes, "data-id" => $reserve->id, "data-name" => $reserve->name, "data-role" => $reserve->role);
	  }
  }
  
	$reserves_rows[] = array("data" => array(""), "class" => array("empty-row"));
  
  
  $out_reserves = theme_table(array(
  		"header" => array(),
  		"rows" => $reserves_rows,
  		"attributes" => array("id" => "lineup-reserves", "class" => array("table", "table-responsive", "lineup-group"), "data-position" => 2),
  		"sticky" => TRUE,
  		"caption" => "" ,
  		"colgroups" => array(),
  		"empty" => ""
  ));

  }
  
  return array("squad" => $out_squad, "regulars" => $out_regulars, "reserves" => $out_reserves);
}

function make_lineup_mobile($t_id, $c_id, $round, $admin = FALSE) {

  global $user;

  drupal_add_js(drupal_get_path("module", "fantacalcio") . "/js/lineup_insert.js");

  $players = get_players(get_round_by_competition_round($round, $c_id));

  $lineup = array();
  $titolari = array();
  $riserve_1 = array();
  $riserve_2 = array();
  $sql = "SELECT * FROM {fanta_lineups} 
        WHERE round = '%d' 
        AND c_id = '%d' 
        AND t_id = '%d' ";
  $result = db_query($sql, $round, $c_id, $t_id);
  while ($row = db_fetch_array($result)) {
    $lineup[$row['pl_id']] = $row;
    if ($row['position'] == 1)
      array_push($titolari, $row);
    elseif ($row['position'] == 2) array_push($riserve_1, $row);
    elseif ($row['position'] >= 3) array_push($riserve_2, $row);
  }

  $sql = "SELECT * FROM {fanta_squads} WHERE t_id = '%d' AND in_team = 1";
  $result = db_query($sql, $t_id);
  while ($row = db_fetch_array($result)) {
    $rose[] = $row;
  }

  if ($rose) {
    foreach ($rose as $rosa) {
      $roles[] = $players[$rosa['pl_id']]->role;
      $names[] = $players[$rosa['pl_id']]->name;
    }

    array_multisort($roles, SORT_ASC, $names, SORT_ASC, $rose);
  
    #rosa
    foreach ($rose as $player) {
      $pl_id = $player['pl_id'];
      $rosa[$pl_id] = $player;

      $class = "role_" . $players[$pl_id]->role;
            
      $rows[$pl_id] = array("class" => $class = "role_" . $players[$pl_id]->role,
                "data" => array(
                      array("data" => convert_role($players[$pl_id]->role), "class" => "role", "width" => "7%"),
                      array("data" => $players[$pl_id]->name, "width" => "40%"),
                      array("data" => ucwords(strtolower($players[$pl_id]->team)), "width" => "25%"),
                      array("data" => position_button($pl_id, $t_id, $c_id, $players[$pl_id]->role, $lineup[$pl_id]['position']), "width" => "25%", "id" => "button_" . $pl_id, "class" => "button"),
                      array("data" => l("<img src='" . base_path() . drupal_get_path("module", "fantacalcio") . "/images/stats.png'>", 
                              "scheda/giocatore/" . $pl_id,
                              array("html" => TRUE, "title" => "Dettagli giocatore", "attributes" => array("class" => "thickbox", "rel" => "lightframe", "data-role" => "button", "data-mini" => "true"))), "width" => "7%")
                      ));
    }

  }
  
  return "<div id='lineup-mobile'>" . theme_table(array(), $rows, array("class" => "table table-responsive")) . "</div>";
}

function fantacalcio_lineup_insert_players($type = 'new', $t_id, $c_id, $round, $other_c_id = '', $other_round = '', $status = 1) {

  global $user;

  $status = ($status + 1 ) % 2;

  if ($type == 'new') {
    #registrazione lineup
//     $sql = "INSERT INTO {fanta_lineups_inserts} (t_id, c_id, round, status, timestamp) VALUES ('%d', '%d', '%d', 1, '%d')";
//     $result = db_query($sql, $t_id, $c_id, $round, time());
//     $teams = get_teams();
    
    $query = db_insert("fanta_lineups_inserts");
    $query->fields(array("t_id" => $t_id, "c_id" => $c_id, "round" => $round, "timestamp" => time(), "status" => 1 ));
    $query->execute();
    
    watchdog('fantacalcio', '@team: lineup inserita correttamente', array('@team' => Team::get($t_id)->name), WATCHDOG_NOTICE);
  }
  elseif ($type == 'old') {

    if (Lineup::exists($t_id, $c_id, $round)) {
      drupal_set_message("Hai già inserito la lineup");
    }
    else{
      #registrazione lineup
      $sql = "INSERT INTO {fanta_lineups_inserts} (t_id, c_id, round, status, timestamp) VALUES ('%d', '%d', '%d', 0, '%d')";
      $result = db_query($sql, $t_id, $c_id, $round, time());

      #inserimento lineup giornata precedente
      $sql = "SELECT * FROM {fanta_lineups} WHERE c_id = '%d' AND t_id='%d' AND round = '%d'";
      $result = db_query($sql, $c_id, $t_id, $round -1);
      while ($row = db_fetch_array($result)) {
        $pl_id = $row['pl_id'];
        $position = $row['position'];
        $sqla = "INSERT INTO {fanta_lineups} (pl_id, c_id, t_id, round, position, uid, timestamp) VALUES " .
        "(" . $pl_id . ", " . $c_id . ", " . $t_id . ", " . $round . ", " . $position . "," . $user->uid . "," . time() . ")";
        $resulta = db_query($sqla);
      }
      drupal_set_message("Userai la lineup della giornata precedente");
      $teams = get_teams();
      watchdog('fantacalcio', '@team: lineup inserita in ritardo', array('@team' => $teams[$t_id]->name), WATCHDOG_NOTICE);
    }
  }
  elseif ($type == 'confirm') {

    #registrazione lineup
    $sql = "INSERT INTO {fanta_lineups_inserts} (t_id, c_id, round, status, timestamp) VALUES ('%d', '%d', '%d', '%d', '%d')";
    $result = db_query($sql, $t_id, $c_id, $round, $status, time());

    #inserimento lineup giornata precedente
    $sql = "DELETE FROM {fanta_lineups} " .
        "WHERE c_id = '%d' " .
        "AND round = '%d' " .
        "AND t_id = '%d'";
    $result = db_query($sql, $c_id, $round, $t_id);

    $sql = "SELECT * FROM {fanta_lineups} WHERE c_id = '%d' AND t_id='%d' AND round = '%d'";
    $result = db_query($sql, $other_c_id, $t_id, $other_round);
    while ($row = db_fetch_array($result)) {
      $pl_id = $row['pl_id'];
      $position = $row['position'];
      $sqla = "INSERT INTO {fanta_lineups} (pl_id, c_id, t_id, round, position, uid, timestamp) VALUES " .
        "(" . $pl_id . ", " . $c_id . ", " . $t_id . ", " . $round . ", " . $position . "," . $user->uid . "," . time() . ")";
      $resulta = db_query($sqla);
    }
    drupal_set_message("lineup confermata");
    $teams = get_teams();
    watchdog('fantacalcio', '@team: lineup confermata', array('@team' => $teams[$t_id]->name), WATCHDOG_NOTICE);
  }
}

function confirm_lineup($args) {

  $t_id = $args["t_id"];
  $round = $args["round"];
  $c_id = $args["c_id"];

  if (!isset($args["other_c_id"]) or !isset($args["other_round"])) {

    $sql = "SELECT round FROM {fanta_rounds_competitions} WHERE c_id = :c_id AND competition_round = :round";
    $result = db_query($sql, array(":c_id" => $c_id, ":round" => $round));
    $vote_round = $result->fetchField();
    $sql = "SELECT * FROM {fanta_rounds_competitions} WHERE round = :round AND c_id <> :c_id";
    $result = db_query($sql, array(":c_id" => $c_id, ":round" => $vote_round));
    foreach ($result as $row) {
      $other_c_id = $row->c_id;
      $other_round = $row->competition_round;
    }
  }
  else {
    $other_c_id = $args["other_c_id"];
    $other_round = $args["other_round"];
  }
  
  $sql = "SELECT * FROM {fanta_lineups_inserts} " .
    "WHERE c_id = :c_id " .
    "AND round = :round " .
    "AND t_id = :t_id";
  $result = db_query($sql, array(":c_id" => $other_c_id, ":round" => $other_round, ":t_id" => $t_id));
  if ($result->rowCount() > 0) {
    $args = array(  "t_id" => $t_id,
            "other_c_id" => $other_c_id,
            "other_round" => $other_round,
            "other_competition_name" => get_competition_name($other_c_id),
            "c_id" => $c_id,
            "competition_name" => get_competition_name($c_id),
            "round" => $round);

    if (arg(0) == "formazioni" && arg(1) == "admin") $args["is_admin"] = 1;

    return "<div class='lineup-confirm'>"
       . "\n<div style='float:left;'>Vuoi utilizzare la stessa lineup che hai gi&agrave; inserito per la partita di "
      . $args['other_competition_name'] . " (" . $args['other_round'] . "&ordf; Giornata) ?</div>"
       . "\n<div style='float:left; margin-left: 20px;'>"
      . drupal_get_form("confirm_lineup_form", $args)
       . "</div><div class='clear'></div></div>";
  }
  else return "";

}

function get_args_confirm($args) {

  $t_id = $args['t_id'];
  $round = $args['round'];
  $c_id = $args['c_id'];
  
  $new_args = $args;

  $sql = "SELECT c.round 
      FROM {fanta_rounds} r, {fanta_rounds_competitions} c
      WHERE c.round = r.round
      AND c.c_id = :c_id
      AND c.competition_round = :round";
  $result = db_query($sql, array(":c_id" => $c_id, ":round" => $round));
  $vote_round = $result->fetchField();

  $sql = "SELECT *
      FROM {fanta_rounds} r, {fanta_rounds_competitions} c
      WHERE c.round = r.round
      AND c.c_id != :c_id
      AND c.round = :round
      ORDER BY c_id DESC";
  $result = db_query($sql, array(":c_id" => $c_id, ":round" => $vote_round));
  foreach ($result as $row) {
    $other_c_id = $row->c_id;
    $other_round = $row->competition_round;
  }

  if (isset($other_c_id) && isset($other_round)) {
  $sql = "SELECT * FROM {fanta_lineups_inserts} " .
      "WHERE c_id = :c_id " .
      "AND round = :round " .
      "AND t_id = :t_id";
  $result = db_query($sql, array(":c_id" => $other_c_id, ":round" => $other_round, ":t_id" => $t_id));

  if ($result->rowCount() > 0) {
    $new_args['other_c_id'] = $other_c_id;
    $new_args['other_round'] = $other_round;
  }
  else {
    $new_args['other_c_id'] = $c_id;
    $new_args['other_round'] = $round - 1;
  }
  }
  else {
    $new_args['other_c_id'] = $c_id;
    $new_args['other_round'] = $round - 1;
  }

  if (arg(0) == "lineup" && arg(1) == "admin")
    $new_args['is_admin'] = 1;
  
  return $new_args;
}

function get_opponent($t_id, $c_id, $round) {

//   $g_id = get_group_team($c_id, $t_id);

//   $sql = "SELECT * FROM {fanta_matches} " .
//       "WHERE g_id = :g_id " .
//       "AND round = :round " .
//       "AND (t1_id = :t_id OR t2_id = :t_id)";
//   $result = db_query($sql, array(":g_id" => $g_id, ":round" => $round, ":t_id" => $t_id ));
//   foreach ($result as $row) {
//     if ($t_id == $row->t1_id) return $row->t2_id;
//     elseif ($t_id == $row->t2_id) return $row->t1_id;
//   }
}


function confirm_lineup_form($form_state, $args) {

  //form redirect (rigoristi se previsto il passaggio del turno)
  if (is_passaggio_turno($args['c_id'], $args['round']) )
    $form['#redirect'] = 'formazioni/insert/' . get_competition_name($args['c_id']) . '/penalties' ;
  else $form['#redirect'] = 'formazioni/view/' . get_competition_name($args['c_id']) . '/' . $args['round'] . '/' . $args['t_id'] ;
  
  $form['other_c_id'] = array("#type" => "hidden", "#value" => $args['other_c_id']);
  $form['other_round'] = array("#type" => "hidden", "#value" => $args['other_round']);
  $form['c_id'] = array("#type" => "hidden", "#value" => $args['c_id']);
  $form['round'] = array("#type" => "hidden", "#value" => $args['round']);
  $form['t_id'] = array("#type" => "hidden", "#value" => $args['t_id']);

  if (isset($args['is_admin']) && $args['is_admin'] == 1) {
    $form['multa'] = array("#type" => "checkbox", "#title" => "Multa", "#attributes" => array("data-theme" => "e"));
  }
  else $form['multa'] = array("#type" => "hidden", "#value" => 0);

  $form['confirm']['submit'] = array(
              "#type" => "submit",
              "#value" => "Ok",
              "#title" => "conferma la lineup inserita",
              '#attributes' => array('onclick' => "return confirm_conferma_lineup()"),
              );

  return $form;

}

function confirm_lineup_form_submit($form, $form_state) {
  fantacalcio_lineup_insert_players('confirm', $form_state['values']['t_id'], $form_state['values']['c_id'], $form_state['values']['round'], $form_state['values']['other_c_id'], $form_state['values']['other_round'], $form_state['values']['multa']);
}

/****************************/
/* END inserimento      */
/****************************/

/****************************/
/* rigoristi        */
/****************************/

function fantacalcio_insert_penalties($competition_name) {
  global $user;

  $t_id = get_team_id_by_user($user->uid);
  $c_id = get_competition_id($competition_name);
  $round = get_next_round_lineup($c_id);
  
  return drupal_get_form('insert_penalties_form', $t_id, $c_id, $round);
}

function insert_penalties_form(&$form_state, $t_id, $c_id, $round) {
  $i = 1;
  //titolari da ordinare
  $sql = "SELECT * 
      FROM {fanta_lineups} f, {fanta_players} p, {fanta_players_teams} t, {fanta_real_teams} r
      WHERE f.pl_id = p.pl_id
      AND p.pl_id = t.pl_id
      AND t.rt_id = r.rt_id
      AND f.t_id = '%d' 
      AND f.c_id = '%d' 
      AND f.round = '%d'
      AND t.round = '%d'
      AND position = 1
      ORDER BY penalty_position";
  $result = db_query($sql, $t_id, $c_id, $round, get_round_by_competition_round($round, $c_id));
  while ($row = db_fetch_object($result)) {
    //create a partial table row containing the data from the table
    $data = array(
      convert_role($row->role),
      $row->name,
      ucwords($row->long_name),
      //$row->penalty_position
    );
   
    //add our static "row" data into a form value
    $form['rows'][$row->pl_id]['data']=array(
                   '#type' => 'value',
                   '#value' => $data
                   );
   
    //now create the weight form element.  
    //NOTE how we add the id into the element key
    $form['rows'][$row->pl_id]['weight-' . $row->pl_id]=array(
      '#type' => 'weight',
      '#default_value' => $row->penalty_position,
      '#delta' => 11,
      //add a specific class in here - we need this later
      '#attributes' => array('class' => 'weight'),
    );
    $i++;
  }
  
  $form['#redirect'] = 'formazioni/view/' . get_competition_name($c_id) . '/' . $round . '/' . $t_id ;
  
  $form['t_id'] = array(
    '#type' => 'hidden',
    '#value' => $t_id,
  );
  
  $form['c_id'] = array(
    '#type' => 'hidden',
    '#value' => $c_id,
  );
  
  $form['round'] = array(
    '#type' => 'hidden',
    '#value' => $round,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Conferma'),
  );
  
  $form['reset'] = array(
    '#type' => 'submit',
    '#value' => t('Annulla'),
    '#validate' => array(
      'insert_penalties_form_clear'
    )
  );

  return $form;
}

function insert_penalties_form_submit($form, &$form_state) {
  foreach ($form_state['values'] as $key => $data) {
    //we are only interested in weight elements
    if (substr($key, 0, 6) == 'weight') {
      //cunningly we have the DB id of the row in the element name
      //print_r ("<pre>".($data) . "</pre>");
      $pl_id = str_replace('weight-', '', $key);
      $sql = "UPDATE {fanta_lineups} 
          SET penalty_position = %d 
          WHERE pl_id = %d 
          AND t_id = %d 
          AND c_id = %d 
          AND round = %d";
      db_query($sql, $data, $pl_id, $form_state['values']['t_id'], $form_state['values']['c_id'], $form_state['values']['round']);    
    }
  }  
  
  drupal_set_message("Rigoristi ordinati");
}

function insert_penalties_form_clear($form, &$form_state) {
  return $form_state['rebuild'] = TRUE;
}

function theme_insert_penalties_form($form) {
  //loop through each "row" in the table array
  foreach ($form['rows'] as $id => $row) {
    
    //we are only interested in numeric keys
    if (intval($id)) { 
      $this_row = $row['data']['#value'];

      //Add the weight field to the row
      $this_row[] = drupal_render($form['rows'][$id]['weight-' . $id]);

      //Add the row to the array of rows
      $table_rows[] = array('data' => $this_row, 'class' => 'draggable');
    }
  }

  //Make sure the header count matches the column count
  $header = array(
    "Ruolo", "Nome", "Squadra", ""
  );

  $output = theme('table', $header, $table_rows, array('id' => 'penalty-position-table'));
  $output .= drupal_render($form);

  // Call add_tabledrag to add and setup the JS for us
  // The key thing here is the first param - the table ID
  // and the 4th param, the class of the form item which holds the weight
  drupal_add_tabledrag('penalty-position-table', 'order', 'sibling', 'weight');      

  return $output;
}
/********************************/
/* END  rigoristi        */
/********************************/

/********************************/
/* multe            */
/********************************/
function fantacalcio_show_multe() {

  $out = '';
  $multe = array();

  $teams = get_teams();

  $fines_sum = 0;
  foreach($teams as $t_id => $team) {

    $sql = "SELECT * FROM {fanta_payments} WHERE t_id = '%d'";
    $result = db_query($sql, $t_id);
    while($row = db_fetch_object($result)) {
      $quota = $row->initial_quota;
      $payed = $row->payed;
      $win_quota = $row->win_quota;
      $lost_quota = $row->lost_quota;
      $quota_fine = ($row->initial_quota_fine == 1 ? variable_get("fantacalcio_quota_fine", 5) : 0);
    }

    $lineups_fine = 0;
    $lineups_rounds = array();
    $sql = "SELECT * FROM {fanta_lineups_inserts} WHERE status = 0 AND c_id = '%d' AND t_id = '%d'";
    $result = db_query($sql, variable_get("fantacalcio_main_competition", 1), $t_id);
    while ($row = db_fetch_object($result)) {
      $lineups_fine++;
      array_push($lineups_rounds, $row->round);
    }
    $lineups_fine = $lineups_fine * ($lineups_fine + 1) / 2;
    $lineups_rounds = ($lineups_fine > 0 ? implode(",", $lineups_rounds) : "");

    $payoff = -$quota + $payed - $lost_quota - $quota_fine - $lineups_fine + $win_quota;
	
	$payoff_class = ($payoff > 0 ? "ok" : ($payoff < 0 ? "error" : ""));

    $rows[$t_id] = array(
	    array("data" => $team->name, "class" => "text-left"), 
	    array("data" => $quota . " &euro;", "class" => "text-right"), 
	    array("data" => $payed . " &euro;", "class" => "text-right"), 
	    array("data" => $win_quota . " &euro;", "class" => "text-right"), 
	    array("data" => $lost_quota . " &euro;", "class" => "text-right"), 
	    array("data" => $quota_fine . " &euro;", "class" => "text-right"), 
	    array("data" => $lineups_fine . " &euro;", "class" => "text-right"), 
	    array("data" => $lineups_rounds, "class" => "text-right"), 
	    array("data" => $payoff . " &euro;", "class" => "text-right bold " . $payoff_class)
	  );
    
	$fines_sum += $lineups_fine + $quota_fine;
  }

  $header = array(
        array("data" => "Squadra", "class" => "text-left"), 
	    array("data" => "Quota", "class" => "text-right"), 
	    array("data" => "Pagato", "class" => "text-right"), 
	    array("data" => "Vincita", "class" => "text-right"), 
	    array("data" => "Perdita", "class" => "text-right"), 
	    array("data" => "Multe quota", "class" => "text-right"), 
	    array("data" => "Multe", "class" => "text-right"), 
	    array("data" => "Giornate", "class" => "text-right"), 
	    array("data" => "Saldo", "class" => "text-right"));

  $out = theme_table(array("header" => $header, "rows" => $rows, "attributes" => array("class" => "text-right")));

  $out .= theme_table(array("header" => array(), "rows" => array( array(array("data" => "Totale multe", "class" => "bold"), "attributes" => array("data" => $fines_sum . " &euro;", "class" => "bold")))));
  
  return $out;
}

function fantacalcio_calcola_multe($num_multe) {
  #calcola la multa totale per ciascuna squadra
  return $num_multe * ($num_multe +1) / 2;
}

/********************************/
/* END multe          */
/********************************/

/********************/
/* utility          */
/********************/
function choose_formazioni_round_form($form, &$form_state, $c_id, $round) {
  $round_formazioni = get_round_formazioni($c_id);

  if ($round_formazioni) {

    $form['round'] = array(
      '#type' => 'select',
      '#options' => $round_formazioni,
      '#default_value' => $round,
      '#attributes' => array("class" => "form-control", "data-theme" => "c", "data-mini" => "false", "data-inline" => "true", "onchange" => "this.form.submit()")
    );

    $form['c_id'] = array(
      '#type' => 'hidden',
      '#default_value' => $c_id,
      '#value' => $c_id,
    );


    $form['submit'] = array(
      '#type' => 'submit',
      '#attributes' => array("class" => "hide")
    );
  
    return $form;

  }
}

function choose_formazioni_round_form_submit($form, &$form_state) {

  $c_id = $form_state['values']['c_id'];
  $round = $form_state['values']['round'];

  drupal_goto("formazioni/view/" . get_competition_name($c_id) . "/" . $round);
}

function filter_teams_formazioni($teams = array(), $args = array()) {

  $c_id = (isset($args['c_id']) && !empty($args['c_id'])) ? $args['c_id'] : variable_get("fantacalcio_main_competition", 1);

  $round = (isset($args['round']) && !empty($args['round'])) ? $args['round'] : get_last_round_lineup($c_id);

  $sql = "SELECT DISTINCT t_id FROM {fanta_lineups} 
      WHERE c_id = :c_id
      AND round = :round
	  AND position > 0";
  $result = db_query($sql, array(":c_id" => $c_id, ":round" => $round));
  $teams_ok = array();
  foreach ($result as $row) {
    $teams_ok[] = $row->t_id;
  }

  foreach ($teams as $t_id => $team) {
    if (!in_array($t_id, $teams_ok))
      unset($teams[$t_id]);
  }
  return $teams;
}

function get_round_formazioni($c_id) {
  //giornate della competizione
  $competition_rounds = get_competition_rounds($c_id);

  //filtro le giornate (solo quelle con formazioni)
  $sql = "SELECT DISTINCT f.round, r.round_label FROM {fanta_lineups} f, {fanta_rounds_competitions} r 
          WHERE r.competition_round = f.round 
          AND f.c_id = r.c_id
          AND f.c_id = :c_id";
  $result = db_query($sql, array(":c_id" => $c_id));
  foreach ($result as $row) {
    $rounds[$row->round] = filter_xss((!empty($row->round_label) ? $row->round_label : $row->round . "a Giornata"));
  }

  if ($rounds) {
    return $rounds;
  }
  else
    return array();
}

function position_button($pl_id, $t_id, $c_id, $role, $position = "") {
  $positions = array("Tribuna", "Titolare", "Riserva 1", "Riserva 2");
  $position = ($position == "") ? 0 : $position;
  $position = ($position > 3) ? 3 : $position;
  
  $display = array("none", "none", "none", "none");
  $display[$position] = "block";
  
  $out = "";
  $out .= "<button id='" . $pl_id . "_0' data-role=button data-mini='true' data-iconpos='notext' class='tribuna btn btn-danger' onclick='changePosition(" . $pl_id . ", " . $t_id . ", " . $c_id . ", " . $role . ", 1)'  style='display: " . $display[0] . "'>Tribuna</button>";
  $out .= "<button id='" . $pl_id . "_1' data-role=button data-mini='true' data-iconpos='notext' class='titolare btn btn-info' onclick='changePosition(" . $pl_id . ", " . $t_id . ", " . $c_id . ", " . $role . ", 2)'  style='display: " . $display[1] . "'>Titolare</button>";
  $out .= "<button id='" . $pl_id . "_2' data-role=button data-mini='true' data-iconpos='notext' class='riserva1 btn btn-alert' onclick='changePosition(" . $pl_id . ", " . $t_id . ", " . $c_id . ", " . $role . ", 3)' style='display: " . $display[2] . "'>Riserva 1</button>";
  $out .= "<button id='" . $pl_id . "_3' data-role=button data-mini='true' data-iconpos='notext' class='riserva2 btn btn-warning' onclick='changePosition(" . $pl_id . ", " . $t_id . ", " . $c_id . ", " . $role . ", 0)' style='display: " . $display[3] . "'>Riserva 2</button>";
  
  return $out;
}

// function fantacalcio_lineup_exists($t_id, $c_id, $round) {

//   $sql = "SELECT * FROM {fanta_lineups} " .
//       "WHERE t_id = '%d' " .
//       "AND c_id = '%d' " .
//       "AND round = '%d'";
//   $result = db_query($sql, $t_id, $c_id, $round);
//   if (db_affected_rows($result) > 0) return TRUE;
//   else return FALSE;

// }

// function fantacalcio_lineup_check($line_up){
//   //massima posizione
// 	$max_position = 0;

//   foreach($line_up as $pl_id => $player) {
// 		if ($player['position'] > $max_position)
// 			$max_position = $player['position'];
// 	}

// 	//inizializzo gli array
// 	$positions = array();
// 	for($j = 0; $j <= $max_position; $j++){
// 		$positions[$j] = array(0, 0, 0, 0);
// 	}

// 	//conto gli elementi (per posizione e per ruolo)
// 	foreach($line_up as $pl_id => $player) {
// 		$currPosition = $player['position'];
// 		$currRole = $player['role'];
// 		$positions[$currPosition][$currRole] = $positions[$currPosition][$currRole] + 1;
// 	}
		
// 	//moduli consentiti e numero giocatori
// 	$check_number_1 = TRUE; 
//     $check_module_1 = TRUE; 
//     $check_number_2_3 = TRUE; 
//     $check_module_2_3 = TRUE; 
// 	$modules_1 = array(array(1, 3, 4, 3), array(1, 3, 5, 2), array(1, 4, 3, 3), array(1, 4, 4, 2), array(1, 4, 5, 1), array(1, 5, 3, 2), array(1, 5, 4, 1), array(1, 6, 3, 1)); //TODO prenderli da variable_get
// 	$modules_2 = array(array(1, 1, 1, 1));
// 	$modules_3 = array(array(0, 1, 1, 1));
	
// 	//verifico titolari
// 	if (isset($positions[1]) && $positions[1] != null ) {
// 		//numero titolari
// 		$number_1 = $positions[1][0] + $positions[1][1] + $positions[1][2] + $positions[1][3];
// 		if($number_1 != 11)
//           $check_number_1 = FALSE;

// 		//modulo titolari
// 		$module_1 = array($positions[1][0], $positions[1][1], $positions[1][2], $positions[1][3]);
// 		if(!in_array($module_1, $modules_1))
//           $check_module_1 = FALSE;
// 	}
// 	else {
// 	  $check_number_1 = FALSE; 
//       $check_module_1 = FALSE; 
// 	}

// 	//verifico riserve
// 	if (isset($positions[2]) && $positions[2] != null && $positions[3] != null) {
// 		//numero riserve
// 		$number_2_3 = $positions[2][0] + $positions[2][1] + $positions[2][2] + $positions[3][3] + $positions[3][0] + $positions[3][1] + $positions[3][2] + $positions[3][3];
// 		if($number_2_3 != 7)
//           $check_number_2_3 = FALSE;

// 		//modulo riserve
// 		$module_2 = array($positions[2][0], $positions[2][1], $positions[2][2], $positions[2][3]);
// 		$module_3 = array($positions[3][0], $positions[3][1], $positions[3][2], $positions[3][3]);
// 		if(!in_array($module_2, $modules_2) || !in_array($module_3, $modules_3)) 
//           $check_module_2_3 = FALSE;
// 	}
// 	else {
// 	  $check_number_2_3 = FALSE; 
//       $check_module_2_3 = FALSE; 
// 	}
	
//   return array('number_1' => $check_number_1, 'module_1' => $check_module_1, 'number_2_3' => $check_number_2_3, 'module_2_3' => $check_module_2_3);
// }

function check_lineup_insert($t_id, $c_id, $round) {
  $sql = "SELECT i_id FROM {fanta_lineups_inserts} " .
      "WHERE t_id = '%d' " .
      "AND c_id = '%d' " .
      "AND round = '%d'";
  $result = db_query($sql, $t_id, $c_id, $round );
  if (db_result($result))
    return "<img src='" . base_path() . drupal_get_path('module', 'fantacalcio') . "/images/ok.png' title='lineup già inserita'>";
  else 
    return "<img src='" . base_path() . drupal_get_path('module', 'fantacalcio') . "/images/no.png' title='lineup non inserita'>";
}

function show_line_up_check($checks) {

//   if (false) {
//     $out = "<ul data-role='listview' >
// 			<li><img id='check_number_1' src='" .base_path() . drupal_get_path("module", "fantacalcio") . "/images/" . get_image_check($checks["number_1"]) . "' class='ui-li-icon'>Numero Titolari</li>
// 			<li><img id='check_module_1' src='" .base_path() . drupal_get_path("module", "fantacalcio") . "/images/" . get_image_check($checks["module_1"]) . "' class='ui-li-icon'>Modulo Titolari</li>
// 			<li><img id='check_number_2_3' src='" .base_path() . drupal_get_path("module", "fantacalcio") . "/images/" . get_image_check($checks["number_2_3"]) . "' class='ui-li-icon'>Numero Riserve</li>
// 			<li><img id='check_module_2_3' src='" .base_path() . drupal_get_path("module", "fantacalcio") . "/images/" . get_image_check($checks["module_2_3"]) . "' class='ui-li-icon'>Modulo Riserve</li>
// 			<li data-role='list-divider'>" . t("Legenda") . "</li>
// 			<li><img class='legenda legenda-titolare ui-li-icon' src=''>" . t("Titolare") . "</li>
// 			<li><img class='legenda legenda-riserva-1 ui-li-icon' src=''>" . t("Prima riserva") . "</li>
// 			<li><img class='legenda legenda-riserva-2 ui-li-icon' src=''>" . t("Seconda riserva") . "</li>
// 			<li><img class='legenda legenda-tribuna ui-li-icon' src=''>" . t("Tribuna") . "</li>
// 		</ul>";
		
//     return $out;
//   }
//   else {
    $rows = array();
    $rows[] = array("data" => array(t("Numero Titolari"), "<span id=\"regulars_number\"><i class='fa " . ($checks["regulars_number"] ? "fa-check-circle" : "fa-delete") . "'></i></span>", 
                                   t("Numero Riserve"),  "<span id=\"reserves_number\"><i class='fa " . ($checks["reserves_number"] ? "fa-check-circle" : "fa-delete") . "'></i></span>" ));
    $rows[] = array("data" => array(t("Modulo Titolari"), "<span id=\"regulars_module\"><i class='fa " . ($checks["regulars_module"] ? "fa-check-circle" : "fa-delete") . "'></i></span>", 
                                   t("Modulo Riserve"), "<span id=\"reserves_module\"><i class='fa " . ($checks["reserves_module"] ? "fa-check-circle" : "fa-delete") . "'></i></span>" ));					   
								   
    return theme_table(array("header" => array(), "rows" => $rows, "attributes" => array("class" => array("table", "table-responsive")), "colgroups" => array(), "sticky" => "", "empty" => "", "caption" => ""));
  
}

function _get_image_check($bool) {
  return "<img src='" . drupal_get_path("module", "fantacalcio") . "images/" . ($bool ? "yes.png" : "no.png") . "'>";
}
/********************/
/* END utility      */
/********************/

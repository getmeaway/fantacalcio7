<?php

/**
*
* @file fantacalcio.lineup.inc
*
*/

/**
 * *****************
 */
/* menu */
/**
 * *****************
 */
function _fantacalcio_lineup() {
  $items = array();
  if (user_access("access content fantacalcio"))
    $items[] = l("Visualizza", "formazioni/view");
  if (user_access("administer team"))
    $items[] = l("Inserisci", "formazioni/insert");
  if (user_access("administer team"))
    $items[] = l("Multe", "formazioni/multe");
//   if (user_access("administer fantacalcio"))
//     $items[] = l("Amministra", "formazioni/admin");
  return theme_item_list(array(
    "items" => $items, 
    "title" => "", 
    "type" => "ul", 
    "attributes" => array()));
}

/**
 * *********************
 */
/* visualizzazione */
/**
 * *********************
 */
function _fantacalcio_lineup_view($competition_name, $round_number = null, $t_id = null) {
  
  global $user;
  
  $selected_competition = Competition::getByName($competition_name);
  
  $competitions = Competition::all(array("has_lineups" => 1));
  
  $competition_is_selected = false;
  
  foreach ($competitions as $c_id => $competition) {
    /*
    if ($selected_competition->id == $competition->id) 
      $_round_number = (isset($round_number) && $round_number != null && Round::existsInCompetition($round_number, $competition->id)) ? $round_number : Round::getLastLineup($competition->id);
    else 
      $_round_number = Round::getLastLineup($competition->id);
    */
    if ($selected_competition != null && $selected_competition->id == $competition->id) {
      $_round_number = (isset($round_number) && $round_number != null && Round::existsInCompetition($round_number, $competition->id)) ? $round_number : Round::getLastLineup($competition->id);

    }
    else {
      $_round_number = Round::getLastLineup($competition->id);
    }

    $round = Round::getByCompetitionRound($_round_number, $competition->id);

    $teams_list = array();
    
    foreach ($competition->groups as $g_id => $group) {
    
      if ($competition->type == COMPETITION_TYPE_GP || ($competition->type == COMPETITION_TYPE_SD && $group->hasMatches($c_id, $round->competition_round))) {
      
      $teams = Team::allByGroupLineups($competition->id, $group->id, $round->competition_round);
      
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
        else if ($team->user == $user->uid) {
          array_push($classes, "mine");
          $expanded = true;
        }
        
        array_push($teams_group_list, array(
          "data" => l($team->name, "formazioni/view/" . $competition->name . "/" . $round->competition_round . "/" . $team->id), 
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
        "expanded" => $expanded);
      }
    }
    
    $columns_output["teams_list"] = $teams_list;
    
    if ($selected_competition == $competition && Team::exists($t_id))
      $columns_output["main_output"] = show_lineup(Team::get($t_id), $selected_competition, $round);
    else 
      $columns_output["main_output"] = "";
    
    $choose_rounds = array();
    $max_round = Round::getLastLineup($c_id);
    for ($i = 1; $i <= $max_round; $i++) {
      $choose_rounds[$i] = base_path() . "formazioni/view/" . strtolower($competition->sanitized_name) . "/" . $i;
    }
    
    $columns_output["round_number"] = isset($_round_number) ? $_round_number : 1;
    $columns_output["choose_rounds"] = $choose_rounds;
    
    $competition->output = theme("columns", $columns_output);
    
    $competition->active = (isset($competition_name) && strtolower($competition->sanitized_name) == strtolower($competition_name));
    if ($competition->active)
      $competition_is_selected = true;
    
  }
  
  $output = array();
  
  if (!$competition_is_selected) {
    $default_competition = Competition::getDefault();
    $competitions[$default_competition->id]->active = true;
  }
  
  $output["competitions"] = $competitions;
  
  return theme("lineup-view", $output);
}

function _fantacalcio_formazioni_box($c_name, $round, $t_id) {
  $args = array("c_name" => $c_name, "round" => $round, "t_id" => $t_id);
  $args['c_id'] = get_competition_id($args['c_name']);
  return show_lineup($args);
}

function show_lineup($team, $competition, $round) {
  $round = (isset($round)) ? $round : Round::getLastLineups($competition_id);
  
  // squadra trovata
  if (Team::exists($team->id)) {
    
    drupal_set_title(filter_xss($competition->name . " - " . $round->label));
    
    $out = "<h3 class='team_name'>" . $team->name . "</h3><br>";
    
    $out .= "<div class='shirt'><img src='" . base_path() . drupal_get_path("module", "fantacalcio") . "/images/shirts/" . $team->shirt . "'></div>";
    $lineup = Lineup::get($competition->id, $team->id, $round->competition_round);
    
    if ($lineup) {
      
      ksort($lineup->positions);
      
      // ordino i titolari(ruolo, nome)
      $roles = array();
      $names = array();
      $module = array(0, 0, 0, 0);
      
      foreach ($lineup->positions[1] as $starting_player) {
        $module[$starting_player->role]++;
        $roles[] = $starting_player->role;
        $names[] = $starting_player->name;
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
        ksort($module);
        
        unset($module[0]);
        $out .= "<p><strong>" . implode(" - ", $module) . "</strong></p>";
      }
      
      // mostra l'ordine dei rigoristi se è previsto il passaggio del turno
      $show_penalties = $round->next;
      
      // titolari
      $header_startings = array(array("data" => t("Titolari"), "colspan" => 5));
      $rows_startings = show_position($lineup->positions[1], $show_penalties);
      
      // riserve
      $header_reserves = array(array("data" => t("Riserve"), "colspan" => 5));
      $rows_reserves = show_position($reserves);
      
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
      
      // modal
      $out .= "<!-- Modal HTML -->
    <div id='player-stats-modal' class='modal fade'>
        <div class='modal-dialog'>
            <div class='modal-content'>
                <!-- Content will be loaded here from 'remote.php' file -->
            </div>
        </div>
    </div>";
    }
    else
      $out .= "<p>" . t("Formazione non trovata") . "</p>";
  }
  
  else
    $out = "<h3>" . t("Squadra inesistente") . "</h3><br>";
  
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
          $show_penalties ? "<span title='Ordine rigoristi'>(" . ($player->penalty_position) . ")</span>" : ""), 
        "class" => array("role-" . $player->role));
    }
  }
  
  return $rows;
}

/**
 * *********************
 */
/* END visualizzazione */
/**
 * *********************
 */

/**
 * *********************
 */
/* inserimento */
/**
 * *********************
 */
function fantacalcio_lineup_insert_start($team_id = "", $competition_name = "") {
  global $user;
  
  // teams
  $teams = array();
  if (!empty($team_id) && Team::exists($team_id)) {
    $team = Team::get($team_id);
    array_push($teams, $team);
  }
  else
    $teams = Team::allByUser($user->uid);
  
  if (count($teams) == 0)
    return t("Non hai ancora nessuna squadra");
    
    // competizioni attive nella giornata
  $round = Round::getLast() + 1;
  
  // competitions
  if (!empty($competition_name) && $competition_name != null && Competition::getByName($competition_name) != null) {
    $competition = Competition::getByName($competition_name);
    $round_competitions = Competition::allForRound($round, $competition->id);
  }
  else
    $round_competitions = Competition::allForRound($round);
  
  $lineups_to_insert = array();
  foreach ($round_competitions as $c_id => $competition) {
    foreach ($teams as $t_id => $team) {
      
      if ($team->inCompetition($c_id) && ($competition->type == COMPETITION_TYPE_GP || ($competition->type == COMPETITION_TYPE_SD && $team->hasMatch($c_id, $competition->competition_round)))) {
        $lineups_to_insert[] = (object) array(
          "team" => $team, 
          "competition" => $competition);
        $lineup_competition = $competition;
      }
    }
  }
  
  // zero squadre --> Avviso
  if (count($lineups_to_insert) == 0)
    return t("Non hai nessuna formazione da inserire");
    
    // 1 squadra --> redirect
  if (count($lineups_to_insert) == 1)
    return fantacalcio_lineup_insert($lineup_competition, $team, $lineup_competition->competition_round, $is_admin = FALSE);
    
    // +1 squadre --> scelta
  if (count($lineups_to_insert) >= 1) {
//     $own_teams_rows = array();
    
//     foreach ($lineups_to_insert as $lineup_to_insert) {
//       $classes = array();
//       $row = array();
//       if ($lineup_to_insert->team->isConfirmed())
//         array_push($classes, "bold");
//       array_push($row, l($lineup_to_insert->team->name, "formazioni/insert/" . $lineup_to_insert->team->id . "/" . $lineup_to_insert->competition->sanitized_name));
//       array_push($row, fantacalcio_check_value(Lineup::exists($lineup_to_insert->team->id, $lineup_to_insert->competition->id, $lineup_to_insert->competition->competition_round)));
//       array_push($row, $lineup_to_insert->competition->name . " - " . $lineup_to_insert->competition->round_label . "");
//       array_push($own_teams_rows, array(
//         "data" => $row, 
//         "class" => $classes, 
//         "data-name" => $lineup_to_insert->team->name));
//     }
    
    $teams = Team::allByUser($user->uid);
    
    //giornata
    $vote_round = Round::getNext();
    $round = Round::getByRound($vote_round);
    
    //orario giornata
    $date = $round->date;
    
    //per ogni competizione
    $out_competitions = "";
    foreach($round->competitions as $competition_id => $competition_round) {
    
      $competition = Competition::get($competition_id);
    
      //per ogni squadra dell'utente
      $rows = array();
      foreach($teams as $t_id => $team) {
    
        //partita utente?
        if ($team->hasMatch($competition_id, $competition_round->competition_round)) {
          //partita
          $match = Match::getByTeamAndRound($t_id, $competition_id, $competition_round->competition_round);
          //formazione inserita o no
          //         $rows[$t_id][] = l($match->home_team . " - " . $match->away_team, "scheda/partita/" . $match->id);
          $rows[$t_id][] = array("data" => $match->home_team . " - " . $match->away_team, "width" => array("50%"));
    
          $lineup = Lineup::get($competition_id, $t_id, $competition_round->competition_round);
    
          if ($lineup != null) {
            if (time() > $date)
              $rows[$t_id][] = "<i>" . t("Formazione inserita"). "</i>";
            else
              $rows[$t_id][] = "<i>" . l(t("Modifica formazione"), "formazioni/insert/" . $t_id . "/" . $competition->sanitized_name) . "</i>";
          }
          else {
            if (time() > $date)
              $rows[$t_id][] = "<i>" . t("Formazione non inserita"). "</i>";
            else
              $rows[$t_id][] = "<i>" . l(t("Inserisci formazione"), "formazioni/insert/" . $t_id . "/" . $competition->sanitized_name) . "</i>";
          }
          $rows[$t_id][] =  l("<i class='fa fa-bar-chart'></i>", "scheda/partita/" . $match->id, array("html" => true) ) ;
        }
    
      }
    
      if (count($rows) > 0) {
        $out_competitions .= "<strong>" . $competition->name . " - " . $competition_round->label . "</strong>";
        $out_competitions .= theme("table", array("rows" => $rows, "attributes" => array("class" => array("table", "table-responsive"))));
      }
    }
    
    $out = "<h4 class='pull-right clearfix'><strong>" . date("d-m-Y H:i", $round->date) . "</strong></h4>";
    $out .= "<div class='clearfix'></div>";
    $out .= $out_competitions;
    
    return $out;
    
//     return array(
//       "#rows" => $own_teams_rows, 
//       "#attributes" => array("class" => array("table", "table-responsive")), 
//       "#theme" => "table");
  }
}

function fantacalcio_lineup_insert_start_admin($t_id, $competition_name = '') {
  return fantacalcio_lineup_insert_choose_competition($t_id, $competition_name, TRUE);
}

// fantacalcio_lineup_insert_start
function fantacalcio_lineup_insert($competition, $team, $competition_round, $is_admin = FALSE) {
  
  global $user;
  
  if ($team->user != $user->uid)
    return drupal_goto(drupal_access_denied());
  
  // squadra non confermata --> redirect
  if (!$team->isSquadComplete()) {
    drupal_set_message(t("Attenzione. Per inserire la formazione devi prima completare la ") . l(t("rosa"), "mie/" . $team->id . "/movimenti"), "error");
    drupal_goto("formazioni");
  }
  
  global $user;
  
  $output = array();
  
  $round = Round::getByCompetitionRound($competition_round, $competition->id);
  
  drupal_set_title(t("Inserisci formazione") . " / " . $team->name . " - " . $competition->name);
  
  // lineup inserita in orario
  if ($round->date > time() || $is_admin) {
    
    $output["competition_name"] = ucwords($competition->name);
    $output["round"] = $round->label;
    
    // $args = array(
    // 't_id' => $team->id,
    // 'c_id' => $competition->id,
    // 'round' => $round,
    // );
    // print_r($team);die();
    $lineup_form = (drupal_get_form('fantacalcio_lineup_form', $competition, $team, $competition_round));
    $output["lineup_form"] = $lineup_form;
    
    $lineup_form_clean = (drupal_get_form('fantacalcio_lineup_form_clean', $competition, $team, $competition_round));
    $output["lineup_form_clean"] = $lineup_form_clean;
    
    $lineup_form_confirm = (drupal_get_form('fantacalcio_lineup_form_confirm', $competition, $team, $competition_round));
    $output["lineup_form_confirm"] = $lineup_form_confirm;
    // $args_confirm = get_args_confirm($args);
    
    // $out .= confirm_lineup($args_confirm);
    
    $lineup = Lineup::get($competition->id, $team->id, $competition_round);
    if ($lineup != null)
      $output["lineup_check"] = show_line_up_check($lineup->check(), $lineup->getCheckValues());
    else
      $output["lineup_check"] = show_line_up_check(array(
        'regulars_number' => false, 
        'regulars_module' => false, 
        'reserves_number' => false, 
        'reserves_module' => false), array(
        'regulars_number' => "", 
        'regulars_module' => "", 
        'reserves_number' => "", 
        'reserves_module' => ""));
      // $out .= "<div id='_lineup_form'>" . drupal_render($lineup_form) . "</div>";
      
    
    //ultima formazione
    $confirm = Lineup::getLastForTeamConfirm($competition->id, $team->id, $round->competition_round);
    
    $output["confirm"] = $confirm;
    
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
  
  $lineup = Lineup::get($competition->id, $team->id, $round);
  
  $checks = array(
    "regulars_number" => false, 
    "regulars_module" => false, 
    "reserves_number" => false, 
    "reserves_module" => false);
  if ($lineup)
    $checks = $lineup->check();
    
  if (isset($args['status']) && $args['status'] == 1)
    $disabled = FALSE;
  else
    $disabled = TRUE;
  
  $form['t_id'] = array('#type' => 'hidden', '#value' => $team->id);
  
  $form['c_id'] = array('#type' => 'hidden', '#value' => $competition->id);
  
  $form['round'] = array('#type' => 'hidden', '#value' => $round);
  
  $form['penalties'] = array('#type' => 'hidden', '#value' => Round::getCompetitionRound($round, $competition->id)->next);
  
  $form['lineup'] = array(
    '#type' => 'hidden', 
    '#value' => json_encode(Lineup::getForForm($competition->id, $team->id, $round)), 
    '#attributes' => array("id" => "tmp-lineup"));
  
  $form['submit'] = array(
    '#type' => 'submit', 
    '#value' => 'Invia', 
    
    // '#disabled' => ($checks['regulars_number'] == 1 && $checks['regulars_module'] == 1 && $checks['reserves_number'] == 1 && $checks['reserves_module'] == 1),
    // '#validate' => array('fantacalcio_lineup_form_validate'),
    '#attributes' => array(
      "id" => "line_up_submit", 
      "class" => array("btn", "btn-success")));
  
  // $form['clear'] = array(
  // '#type' => 'submit',
  // '#value' => 'Cancella',
  // '#validate' => array(
  // 'fantacalcio_lineup_form_clear'
  // ),
  // '#attributes' => array(
  // 'onclick' => "return confirm_reset_lineup()"
  // ),
  // '#prefix' => "<div class='ui-block-b'>",
  // '#suffix' => "</div></div>",
  // );
  
  return $form;
}

function fantacalcio_lineup_form_submit($form, &$form_state) {
  global $user;
  
  $t_id = $form_state['values']['t_id'];
  $c_id = $form_state['values']['c_id'];
  $round = $form_state['values']['round'];
  
  // lineup inviata in orario
  if (Round::get($round, $c_id)->date > time() || true) { // TODO
    
    $lineup = json_decode($form_state['input']['lineup']);
    
    // cancello la vecchia formazione
    $query = db_delete("fanta_lineups");
    $query->condition("t_id", $t_id);
    $query->condition("c_id", $c_id);
    $query->condition("round", $round);
    $query->execute();
    
    foreach ($lineup as $pl_id => $player) {
      // insert
      if ($player->position > 0) {
        
        $query = db_insert("fanta_lineups");
        $query->fields(array(
          "t_id" => $t_id, 
          "pl_id" => $pl_id, 
          "c_id" => $c_id, 
          "round" => $round, 
          "position" => $player->position, 
          "timestamp" => time(), 
          "uid" => $user->uid));
        $query->execute();
      }
    }

    fantacalcio_lineup_insert_players('new', $t_id, $c_id, $round);
    
    drupal_set_message(t("Formazione inserita correttamente."));
    
    watchdog("fantacalcio", t("%team: formazione inserita correttamente"), array(
      "%team" => Team::get($t_id)->name), WATCHDOG_INFO);

    if ($form_state['values']['penalties'])
      drupal_goto("formazioni/insert/" . $form_state['values']['t_id'] . "/" . Competition::get($c_id)->sanitized_name . "/penalties");
	else 
      drupal_goto("formazioni/view/" . Competition::get($c_id)->sanitized_name . "/" . $round . "/" . $t_id);
  }
  // lineup inviata in ritardo -> giornata precedente
  else {
    fantacalcio_lineup_insert_players('old', $t_id, $c_id, $round);
    drupal_set_message(t("Tempo scaduto. Userai la lineup della giornata precedente"));
  }
  
  return;
}

function fantacalcio_lineup_form_clean($form, &$form_state, $competition, $team, $round) {
  $form['t_id'] = array('#type' => 'hidden', '#value' => $team->id);
  
  $form['c_id'] = array('#type' => 'hidden', '#value' => $competition->id);
  
  $form['round'] = array('#type' => 'hidden', '#value' => $round);
  
  $form['submit'] = array(
    '#type' => 'submit', 
    '#value' => t('Cancella'), 
    '#attributes' => array(
      "id" => "line_up_clean", 
      "class" => array("btn", "btn-sm", "btn-warning")));
  
  return $form;
}

function fantacalcio_lineup_form_clean_submit($form, &$form_state) {
  
  // cancella la lineup inserita
  $competition = Competition::get($form_state['values']['c_id']);
  
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
  
  watchdog('fantacalcio', '@team: formazione cancellata', array(
    '@team' => Team::get($form_state['values']['t_id'])->name), WATCHDOG_NOTICE);
  
  drupal_goto("formazioni/insert/" . $form_state['values']['t_id'] . "/" . $competition->name);
}

function fantacalcio_lineup_form_confirm($form, &$form_state, $competition, $team, $round) {
  $form['t_id'] = array('#type' => 'hidden', '#value' => $team->id);
  
  $form['c_id'] = array('#type' => 'hidden', '#value' => $competition->id);
  
  $form['round'] = array('#type' => 'hidden', '#value' => $round);
  
  $form['penalties'] = array('#type' => 'hidden', '#value' => Round::getCompetitionRound($round, $competition->id)->next);
  
  $form['submit'] = array(
    '#type' => 'submit', 
    '#value' => t("Conferma"), 
    '#attributes' => array(
      "id" => "line_up_confirm", 
      "class" => array("btn", "btn-sm", "btn-info")));
  
  return $form;
}

function fantacalcio_lineup_form_confirm_submit($form, &$form_state) {
  
  global $user;
  
  // conferma la formazione inserita
  $competition = Competition::get($form_state['values']['c_id']);
  
  $query = db_delete("fanta_lineups");
  $query->condition("t_id", $form_state['values']['t_id']);
  $query->condition("c_id", $form_state['values']['c_id']);
  $query->condition("round", $form_state['values']['round']);
  $query->execute();
  
  //seleziono l'ultima formazione inserita
  $confirm_lineup = Lineup::getLastForTeam($form_state['values']['c_id'], $form_state['values']['t_id'], $form_state['values']['round']);
//   print_r($confirm_lineup);die();
  //inserisco la formazione
  foreach ($confirm_lineup->positions as $position => $positions_player) {
    foreach ($positions_player as $pl_id => $player) {
      // insert
      $query = db_insert("fanta_lineups");
      $query->fields(array(
          "t_id" => $form_state['values']['t_id'],
          "pl_id" => $pl_id,
          "c_id" => $form_state['values']['c_id'],
          "round" => $form_state['values']['round'],
          "position" => $position,
          "timestamp" => time(),
          "uid" => $user->uid));
      $query->execute();
    }
  }
  
  fantacalcio_lineup_insert_players('new', $form_state['values']['t_id'], $form_state['values']['c_id'], $form_state['values']['round']);
  
  //fantacalcio_lineup_insert_players('confirm', $form_state['values']['t_id'], $form_state['values']['c_id'], $form_state['values']['round'], $form_state['values']['other_c_id'], $form_state['values']['other_round'], $form_state['values']['multa']);
    
  drupal_set_message(t("Formazione confermata"));
  
  watchdog('fantacalcio', '@team: formazione confermata', array(
    '@team' => Team::get($form_state['values']['t_id'])->name), WATCHDOG_NOTICE);
  
  if ($form_state['values']['penalties'])
  	drupal_goto("formazioni/insert/" . $form_state['values']['t_id'] . "/" . $competition->sanitized_name . "/penalties");
  else
    drupal_goto("formazioni/view/" . $competition->name . "/" . $form_state['values']['round'] . "/" . $form_state['values']['t_id']);
}

function fantacalcio_lineup_form_validate($form, &$form_state) {
  $t_id = $form_state['values']['t_id'];
  $c_id = $form_state['values']['c_id'];
  $round = $form_state['values']['round'];
  
  $tmp_lineup = json_decode($form_state['values']['lineup']);
  // print_r($tmp_lineup);
  
  $positions = array();
  foreach ($tmp_lineup as $pl_id => $lineup_player) {
    $player = new Player($pl_id, "", $lineup_player->role);
    $positions[$lineup_player->position][$pl_id] = $player;
  }
  
  $lineup = new Lineup();
  $lineup->round = $round;
  // $lineup->team = Team::get($t_id);
  // $lineup->competition = Competition::get($c_id);
  $lineup->positions = $positions;
  
  // check giocatori in rosa
  $check_players_in_team = true;
  foreach ($lineup->positions as $position => $positions) {
    if ($position != 0) {
      // foreach($positions as $pl_id => $player) {
      // // $sql = "SELECT * FROM {fanta_squads} WHERE pl_id = '%d' ANd t_id = '%d' AND in_team = 1";
      
      // $query = db_select("fanta_squads", "s");
      // $query->condition("pl_id", $pl_id);
      // $query->condition("t_id", $t_id);
      // $query->condition("status", 1);
      // $query->fields("s");
      
      // $result = $query->execute();
      // if ($result->rowCount() == 0) {
      // echo "ROSA ".$pl_id;
      // form_set_error("", t("Errore nella formazione"));
      // }
      // }
    }
  }
  
  if ($check_players_in_team) {
    // check ruolo giocatori
    $check_players_role = true;
    
    if ($check_players_role) {
      // check modulo
      $checks = $lineup->check(); 
    }
    else {
      // error
      echo "MODULO";
      form_set_error("", t("Errore nella formazione"));
    }
  }
  else {
    // error
    echo "ELSE";
    form_set_error("", t("Errore nella formazione"));
  }
  
  return;
}

function make_lineup($team, $competition, $round, $admin = FALSE) {

global $user;

 
  drupal_add_js(drupal_get_path("module", "fantacalcio") . "/js/jquery.ui.js");
  drupal_add_js(drupal_get_path("module", "fantacalcio") . "/js/jquery.ui.touch-punch.min.js");
  drupal_add_js(drupal_get_path("module", "fantacalcio") . "/js/jquery.mobile.custom.min.js");
  
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
    
    // rosa
    $out_squad = "";
    
    $lineup_player_ids = array();
    if (isset($lineup->positions)) {
      foreach ($lineup->positions as $positions) {
        foreach ($positions as $player)
          $lineup_player_ids[$player->id] = $player->id;
      }
    }
    
    // mobile
    $squad_mobile = "";
    foreach ($squad as $_player) {
      
      $player = new Player($_player->pl_id, $_player->name, $_player->role);
      $player->pl_id = $_player->pl_id;
      $player->name = $_player->name;
      $player->role = $_player->role;
      $player->team = $_player->team;
      
      $player->position = "";
      $position_class = "";
      $position_class_icon = "";
      if (isset($lineup->positions) && isset($lineup->positions[1]) && array_key_exists($player->pl_id, $lineup->positions[1])) {
        $player->position = "T";
        $position_class = "regular";
        $position_class_icon = "text-success";
      }
      else 
        if (isset($lineup->positions) && isset($lineup->positions[2]) && array_key_exists($player->pl_id, $lineup_player_ids)) {
          $player->position = "R";
          $position_class = "reserve";
          $position_class_icon = "text-warning";
        }
      
      $position = " ";
      $hidden = " ";
      if ($player->position == "")
        $hidden = "not-visible";
      
      $position = "<span class='fa fa-stack " . $hidden . "'>" . "<i class='fa fa-stack-2x fa-circle-o position-class " . $position_class_icon . "'></i>" . "<i class='fa fa-stack-1x position " . $position_class_icon . "'>" . $player->position . "</i>" . "</span>";
      
      $squad_mobile .= "<div class='row'>" . "<div class='player-row col-xs-10 col-xs-offset-1 " . $position_class . "' data-id='" . $player->pl_id . "' data-role='" . $player->role . "' data-team='" . $team->id . "' data-competition='" . $competition->id . "'>" . "<table class='table player-table'><tr>" . "<td class='player-role'>" . fantacalcio_show_role($player->role) . "</td>" . "<td class='player-name'>" . $player->name . "</td>" . "<td class='player-team'>" . $player->team . "</td>" . 

      "<td class='player-status'>" . fantacalcio_show_status($player->name, $player->team, $player->getStatus($round->round)) . "</td>" . "<td class='player-position'>" . $position . "</td>" . "</tr>" . "</table>" . "<div class='position-message'></div>" . "</div>" . "</div>";
    }
    
    // web
    foreach ($squad as $_player) {
      if (!array_key_exists($_player->pl_id, $lineup_player_ids)) {
        
        $player = new Player($_player->pl_id, $_player->name, $_player->role);
        $player->pl_id = $_player->pl_id;
        $player->name = $_player->name;
        $player->role = $_player->role;
        $player->team = $_player->team;
        
        $classes = array("role_" . $player->role);
        
        $row_data = array();
        
        $row_data[] = array(
          "data" => "<span class='fa-stack'>
						<i class='fa fa-square fa-stack-2x squad-player-role-" . $player->role . "'></i>
						<i class='fa fa-stack-1x' style='color: white;'><span class='font-normal'>" . Player::convertRole($player->role) . "</span></i>
					</span>", 
          "class" => $classes);
        $row_data[] = array("data" => $player->name);
        $row_data[] = array("data" => ucwords(strtolower($player->team)));
        $row_data[] = array(
          "data" => fantacalcio_show_status($player->name, $player->team, $player->getStatus($round->round)), 
          "class" => array("player-status"));
        $row_data[] = array(
          "data" => "<a data-toggle='lightbox' rel='lightframe[" . $team->id . "|width:770px;]' href='" . base_path() . "scheda/giocatore/" . $player->pl_id . "' title='Dettagli giocatore'>" . "<i class=\"fa fa-bar-chart\"></i>" . "</a>");
        
        $squad_rows[] = array(
          "data" => $row_data, 
          "class" => $classes, 
          "data-id" => $player->pl_id, 
          "data-name" => $player->name, 
          "data-role" => $player->role);
        
        // $out_squad .= "\n</div>"
        // # . "\n<div class='itemContent'>\n"
        // # . "\n</div>"
        // . "\n</div>";
        
        // $i++;
      }
    }
    
    $out_squad = theme_table(array(
      "header" => array(), 
      "rows" => $squad_rows, 
      "attributes" => array(
        "id" => "lineup-squad", 
        "class" => array("table", "table-responsive", "lineup-group"), 
        "data-position" => 0), 
      "sticky" => TRUE, 
      "caption" => "", 
      "colgroups" => array(), 
      "empty" => ""));
    
    // titolari
    $regulars_rows = array();
    
    if (isset($lineup->positions) && isset($lineup->positions[1])) {
      $roles = array();
      $names = array();
      foreach ($lineup->positions[1] as $regular) {
        $roles[] = $regular->role;
        $names[] = $regular->name;
      }
      
      array_multisort($roles, SORT_ASC, $names, SORT_ASC, $lineup->positions[1]);
      
      foreach ($lineup->positions[1] as $_regular) {
        
        $regular = new Player($_regular->id, $_regular->name, $_regular->role);
        $regular->pl_id = $_regular->id;
        $regular->name = $_regular->name;
        $regular->role = $_regular->role;
        $regular->team = $_regular->team;
        
        $class = "role_" . $regular->role;
        
        $row_data = array();
        
        $row_data[] = array(
          "data" => "<span class='fa-stack'>
							<i class='fa fa-square fa-stack-2x squad-player-role-" . $regular->role . "'></i>
							<i class='fa fa-stack-1x' style='color: white;'><span class='font-normal'>" . Player::convertRole($regular->role) . "</span></i>
						</span>", 
          "class" => $classes);
        $row_data[] = array("data" => $regular->name);
        $row_data[] = array("data" => ucwords(strtolower($regular->team)));
        $row_data[] = array(
          "data" => fantacalcio_show_status($regular->name, $regular->team, $regular->getStatus($round->round)), 
          "class" => array("player-status"));
        $row_data[] = array(
          "data" => "<a rel='lightframe[" . $team->id . "|width:770px;]' href='" . base_path() . "scheda/giocatore/" . $regular->id . "' title='Dettagli giocatore'>" . "<i class=\"fa fa-bar-chart\"></i>" . "</a>");
        
        $regulars_rows[] = array(
          "data" => $row_data, 
          "class" => $classes, 
          "data-id" => $regular->id, 
          "data-name" => $regular->name, 
          "data-role" => $regular->role);
      }
    }
    
    $regulars_rows[] = array("data" => array(""), "class" => array("empty-row"));
    
    $out_regulars = theme_table(array(
      "header" => array(), 
      "rows" => $regulars_rows, 
      "attributes" => array(
        "id" => "lineup-regulars", 
        "class" => array("table", "table-responsive", "lineup-group"), 
        "data-position" => 1), 
      "sticky" => TRUE, 
      "caption" => "", 
      "colgroups" => array(), 
      "empty" => ""));
    
    // riserve
    $reserves_rows = array();
    
    if (isset($lineup->positions)) {
      
      $reserves = array();
      foreach ($lineup->positions as $position => $reserve_positions) {
        if ($position != 1)
          $reserves[$position] = $reserve_positions;
      }
      
      $roles = array();
      $names = array();
      $positions = array();
      $tmp_reserves = array();
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
      
      foreach ($reserves as $_reserve) {
        
        $reserve = new Player($_reserve->id, $_reserve->name, $_reserve->role);
        $reserve->pl_id = $_reserve->id;
        $reserve->name = $_reserve->name;
        $reserve->role = $_reserve->role;
        $reserve->team = $_reserve->team;
        
        $class = array("role_" . $reserve->role);
        
        $row_data = array();
        
        $row_data[] = array(
          "data" => "<span class='fa-stack'>
							<i class='fa fa-square fa-stack-2x squad-player-role-" . $reserve->role . "'></i>
							<i class='fa fa-stack-1x' style='color: white;'><span class='font-normal'>" . Player::convertRole($reserve->role) . "</span></i>
						</span>", 
          "class" => $classes);
        $row_data[] = array("data" => $reserve->name);
        $row_data[] = array("data" => ucwords(strtolower($reserve->team)));
        $row_data[] = array(
          "data" => fantacalcio_show_status($reserve->name, $reserve->team, $reserve->getStatus($round->round)), 
          "class" => array("player-status"));
        $row_data[] = array(
          "data" => "<a rel='lightframe[" . $team->id . "|width:770px;]' href='" . base_path() . "scheda/giocatore/" . $reserve->id . "' title='Dettagli giocatore'>" . "<i class=\"fa fa-bar-chart\"></i>" . "</a>");
        
        $reserves_rows[] = array(
          "data" => $row_data, 
          "class" => $classes, 
          "data-id" => $reserve->id, 
          "data-name" => $reserve->name, 
          "data-role" => $reserve->role);
      }
    }
    
    $reserves_rows[] = array("data" => array(""), "class" => array("empty-row"));
    
    $out_reserves = theme_table(array(
      "header" => array(), 
      "rows" => $reserves_rows, 
      "attributes" => array(
        "id" => "lineup-reserves", 
        "class" => array("table", "table-responsive", "lineup-group"), 
        "data-position" => 2), 
      "sticky" => TRUE, 
      "caption" => "", 
      "colgroups" => array(), 
      "empty" => ""));
  }
  
  // return array("squad" => "", "regulars" => "", "reserves" => "", "squad_mobile" => "");
  return array(
    "squad" => $out_squad, 
    "regulars" => $out_regulars, 
    "reserves" => $out_reserves, 
    "squad_mobile" => $squad_mobile);
}

function fantacalcio_lineup_insert_players($type = 'new', $t_id, $c_id, $round, $other_c_id = '', $other_round = '', $status = 1) {
  global $user;
  
  $status = ($status + 1) % 2;
  
  if ($type == 'new') {
    // registrazione lineup
    // $sql = "INSERT INTO {fanta_lineups_inserts} (t_id, c_id, round, status, timestamp) VALUES ('%d', '%d', '%d', 1, '%d')";
    // $result = db_query($sql, $t_id, $c_id, $round, time());
    // $teams = get_teams();
    
    $query = db_insert("fanta_lineups_inserts");
    $query->fields(array(
      "t_id" => $t_id, 
      "c_id" => $c_id, 
      "round" => $round, 
      "timestamp" => time(), 
      "status" => 1));
    $query->execute();
    
    // watchdog('fantacalcio', '@team: formazione inserita correttamente', array('@team' => Team::get($t_id)->name), WATCHDOG_NOTICE);
  }
  elseif ($type == 'old') {
    
    if (Lineup::exists($t_id, $c_id, $round)) {
      drupal_set_message(t("Hai già inserito la formazione"));
    }
    else {
      // registrazione lineup

    $query = db_insert("fanta_lineups_inserts");
    $query->fields(array(
      "t_id" => $t_id,
      "c_id" => $c_id,
      "round" => $round,
      "timestamp" => time(),
      "status" => 0));
    $query->execute();
 
  $query = db_delete("fanta_lineups");
  $query->condition("t_id", $t_id);
  $query->condition("c_id", $c_id);
  $query->condition("round", $round);
  $query->execute();
 
  //seleziono l'ultima formazione inserita
  $confirm_lineup = Lineup::getLastForTeam($c_id, $t_id, $round);

  //inserisco la formazione
  foreach ($confirm_lineup->positions as $position => $positions_player) {
    foreach ($positions_player as $pl_id => $player) {
      // insert
      $query = db_insert("fanta_lineups");
      $query->fields(array(
          "t_id" => $t_id,
          "pl_id" => $pl_id,
          "c_id" => $c_id,
          "round" => $round,
          "position" => $position,
          "timestamp" => time(),
          "uid" => $user->uid));
      $query->execute();
    }
  }

       drupal_set_message(t("Userai la formazione della giornata precedente"));
       watchdog('fantacalcio', '@team: formazione inserita in ritardo', array(
         '@team' => Team::get($t_id)->name), WATCHDOG_NOTICE);
    }
  }
  elseif ($type == 'confirm') {
    
    // registrazione lineup
    $sql = "INSERT INTO {fanta_lineups_inserts} (t_id, c_id, round, status, timestamp) VALUES ('%d', '%d', '%d', '%d', '%d')";
    $result = db_query($sql, $t_id, $c_id, $round, $status, time());
    
    // inserimento lineup giornata precedente
    $sql = "DELETE FROM {fanta_lineups} " . "WHERE c_id = '%d' " . "AND round = '%d' " . "AND t_id = '%d'";
    $result = db_query($sql, $c_id, $round, $t_id);
    
    $sql = "SELECT * FROM {fanta_lineups} WHERE c_id = '%d' AND t_id='%d' AND round = '%d'";
    $result = db_query($sql, $other_c_id, $t_id, $other_round);
    while ($row = db_fetch_array($result)) {
      $pl_id = $row['pl_id'];
      $position = $row['position'];
      $sqla = "INSERT INTO {fanta_lineups} (pl_id, c_id, t_id, round, position, uid, timestamp) VALUES " . "(" . $pl_id . ", " . $c_id . ", " . $t_id . ", " . $round . ", " . $position . "," . $user->uid . "," . time() . ")";
      $resulta = db_query($sqla);
    }
    drupal_set_message(t("Formazione confermata"));
    $teams = get_teams();
    watchdog('fantacalcio', '@team: formazione confermata', array(
      '@team' => $teams[$t_id]->name), WATCHDOG_NOTICE);
  }
}

function confirm_lineup_form($form_state, $args) {
  
  // form redirect (rigoristi se previsto il passaggio del turno)
  if (is_passaggio_turno($args['c_id'], $args['round']))
    $form['#redirect'] = 'formazioni/insert/' . get_competition_name($args['c_id']) . '/penalties';
  else
    $form['#redirect'] = 'formazioni/view/' . get_competition_name($args['c_id']) . '/' . $args['round'] . '/' . $args['t_id'];
  
  $form['other_c_id'] = array(
    "#type" => "hidden", 
    "#value" => $args['other_c_id']);
  $form['other_round'] = array(
    "#type" => "hidden", 
    "#value" => $args['other_round']);
  $form['c_id'] = array("#type" => "hidden", "#value" => $args['c_id']);
  $form['round'] = array("#type" => "hidden", "#value" => $args['round']);
  $form['t_id'] = array("#type" => "hidden", "#value" => $args['t_id']);
  
  if (isset($args['is_admin']) && $args['is_admin'] == 1) {
    $form['multa'] = array(
      "#type" => "checkbox", 
      "#title" => "Multa", 
      "#attributes" => array("data-theme" => "e"));
  }
  else
    $form['multa'] = array("#type" => "hidden", "#value" => 0);
  
  $form['confirm']['submit'] = array(
    "#type" => "submit", 
    "#value" => "Ok", 
    "#title" => "conferma la lineup inserita", 
    '#attributes' => array('onclick' => "return confirm_conferma_lineup()"));
  
  return $form;
}

function confirm_lineup_form_submit($form, $form_state) {
  fantacalcio_lineup_insert_players('confirm', $form_state['values']['t_id'], $form_state['values']['c_id'], $form_state['values']['round'], $form_state['values']['other_c_id'], $form_state['values']['other_round'], $form_state['values']['multa']);
}

/**
 * *************************
 */
/* END inserimento */
/**
 * *************************
 */

/**
 * *************************
 */
/* rigoristi */
/**
 * *************************
 */
function fantacalcio_insert_penalties($t_id, $competition_name) {
  global $user;
  
  drupal_add_js(drupal_get_path("module", "fantacalcio") . "/js/jquery.ui.js");
  drupal_add_js(drupal_get_path("module", "fantacalcio") . "/js/jquery.ui.touch-punch.min.js");
  drupal_add_js(drupal_get_path("module", "fantacalcio") . "/js/jquery.mobile.custom.min.js");
  drupal_add_js(drupal_get_path("module", "fantacalcio") . "/js/lineup_penalties_sort.js");
 
  $competition = Competition::getByName($competition_name);
  $_round = Round::getNext();
  
  $round = Round::getCompetitionRound($_round, $competition->id);
  
  if (!$round->next) {
    drupal_set_message(t("Rigoristi non previsti per questa giornata"));
    drupal_goto("formazioni/view/" . $competition_name . "/" . $round->competition_round . "/" . $t_id);
  }
 
  $penalties_rows = array();
  $penalties_order = array();

  $i = 1;
      
  $query = db_select("fanta_lineups", "l");
  $query->join("fanta_players", "p", "l.pl_id = p.pl_id");
  $query->join("fanta_players_rounds", "pr", "pr.pl_id = p.pl_id");
  $query->join("fanta_real_teams", "rt", "pr.rt_id = rt.rt_id");
  $query->condition("l.t_id", $t_id);
  $query->condition("l.c_id", $competition->id);
  $query->condition("l.round", $round->competition_round);
  $query->condition("pr.round", Round::getByCompetitionRound($round->competition_round, $competition->id)->round);
  $query->condition("l.position", 1);
  $query->orderBy("l.position");
  $query->fields("l");
  $query->fields("p");
  $query->addField("rt", "name", "team");
      
  $result = $query->execute();
  foreach ($result as $row) {
  
  	$player_order = ($row->penalty_position == null || $row->penalty_position == 0 ? $i : $row->penalty_position);
    $penalties_order[$row->pl_id] = $player_order;
    
    $penalties_rows[] = array("data" => array(
      "<td><i class='fa fa-arrows'></i></td>",
      fantacalcio_show_role($row->role), 
      $row->name, 
      ucwords($row->team)
      ), 
      "data-id" => $row->pl_id, 
      "data-order" => $player_order);
    
    $i++;
  }
    
  $penalties_form = drupal_get_form('insert_penalties_form', $t_id, $competition->id, $round->competition_round, $penalties_order);
  
  $out = theme("table", array("rows" => $penalties_rows, "attributes" => array("id" => "penalties_order_table", "class" => array("table", "table-responsive"))));
  
  return $out . drupal_render($penalties_form);
}
  
function insert_penalties_form($form, &$form_state, $t_id, $c_id, $round, $penalties_order) {
  
  $form['penalties_order'] = array('#type' => 'hidden', '#default_value' => json_encode($penalties_order), "#attributes" => array("id" => "penalties_order"));
  
  $form['t_id'] = array('#type' => 'hidden', '#value' => $t_id);
  
  $form['c_id'] = array('#type' => 'hidden', '#value' => $c_id);
  
  $form['round'] = array('#type' => 'hidden', '#value' => $round);
  
  $form['submit'] = array('#type' => 'submit', '#value' => t('Conferma'), "#attributes" => array("class" => array("btn-success")));
  
  $form['reset'] = array(
    '#type' => 'submit', 
    '#value' => t('Annulla'), 
    '#validate' => array('insert_penalties_form_clear')
    );
  
  $form['back'] = array(
    '#type' => 'submit', 
    '#value' => t('Indietro'), 
    '#validate' => array('insert_penalties_form_back'),
    "#attributes" => array("class" => array("btn-info"))
    );
    
  return $form;
}

function insert_penalties_form_submit($form, &$form_state) {

  $penalties_order = json_decode($form_state['input']['penalties_order']);

  foreach ($penalties_order as $pl_id => $order) {
    
    db_update("fanta_lineups")
    	->fields(array("penalty_position" => $order))
    	->condition("pl_id", $pl_id)
    	->condition("t_id", $form_state['values']['t_id'])
    	->condition("c_id", $form_state['values']['c_id'])
    	->condition("round", $form_state['values']['round'])
    	->execute();    
  }
  
  drupal_set_message(t("Rigoristi ordinati"));
  
  $competition = Competition::get($form_state['values']['c_id']);
  
  drupal_goto("formazioni/view/" . $competition->sanitized_name . "/" . $form_state['values']['round'] . "/" . $form_state['values']['t_id']);
}

function insert_penalties_form_clear($form, &$form_state) {
  return $form_state['rebuild'] = TRUE;
}

function insert_penalties_form_back($form, &$form_state) {
    $competition = Competition::get($form_state['values']['c_id']);
  
  drupal_goto("formazioni/insert/" . $form_state['values']['t_id'] . "/" . $competition->sanitized_name );
}

/**
 * *****************************
 */
/* END rigoristi */
/**
 * *****************************
 */

/**
 * *****************************
 */
/* multe */
/**
 * *****************************
 */
function fantacalcio_show_payments() {
  $out = '';
  $payments = array();
  
  $teams = Team::allActive();
  
  $fines_sum = 0;
  $default_competition = Competition::getDefault();
  
  foreach ($teams as $t_id => $team) {
    
    $query = db_select("fanta_payments", "p");
    $query->condition("t_id", $t_id);
    $query->fields("p");
    $result = $query->execute();
    
    foreach ($result as $row) {
      $quota = $row->initial_quota;
      $payed = $row->payed;
      $win_quota = $row->win_quota;
      $lost_quota = $row->lost_quota;
      $quota_fine = ($row->initial_quota_fine == 1 ? variable_get("fantacalcio_quota_fine", 5) : 0);
    }
    
    $lineups_fine = 0;
    $lineups_rounds = array();
//     $sql = "SELECT * FROM {fanta_lineups_inserts} WHERE status = 0 AND c_id = '%d' AND t_id = '%d'";
//     $result = db_query($sql, variable_get("fantacalcio_main_competition", 1), $t_id);
//     while ($row = db_fetch_object($result)) {
      
    $query = db_select("fanta_lineups_inserts", "l");
    $query->condition("t_id", $t_id);
    $query->condition("status", 0);
    $query->condition("c_id", $default_competition->id);
    $query->fields("l");
    $result = $query->execute();
      
    foreach ($result as $row) {        
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
      array(
        "data" => $payoff . " &euro;", 
        "class" => "text-right bold " . $payoff_class));
    
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
  
  $out = theme("table", array(
    "header" => $header, 
    "rows" => $rows, 
    "attributes" => array("class" => "text-right")));
  
  $out .= theme("table", array(
    "header" => array(), 
    "rows" => array(
      array(
        array("data" => "Totale multe", "class" => "bold"), 
        "attributes" => array(
          "data" => $fines_sum . " &euro;", 
          "class" => "bold")))));
  
  return $out;
}

function fantacalcio_calculate_fine($num) {
  // calcola la multa totale per ciascuna squadra
  return $num * ($num + 1) / 2;
}

/**
 * *****************************
 */
/* END multe */
/**
 * *****************************
 */

/**
 * *****************
 */
/* utility */
/**
 * *****************
 */

function show_line_up_check($checks, $values) {
  $rows = array();
  $rows[] = array(
    "data" => array(
      t("Numero Titolari"), 
      "<span id=\"regulars_number_value\" class=\"regulars_number_value\">" . $values["regulars_number"] . "</span>", 
      "<span id=\"regulars_number\" class=\"regulars_number\">" . fantacalcio_check_value($checks["regulars_number"]) . "</span>", 
      t("Numero Riserve"), 
      "<span id=\"reserves_number_value\" class=\"reserves_number_value\">" . $values["reserves_number"] . "</span>", 
      "<span id=\"reserves_number\" class=\"reserves_number\">" . fantacalcio_check_value($checks["reserves_number"]) . "</span>"));
  $rows[] = array(
    "data" => array(
      t("Modulo Titolari"), 
      "<span id=\"regulars_module_value\" class=\"regulars_module_value\">" . $values["regulars_module"] . "</span>", 
      "<span id=\"regulars_module\" class=\"regulars_module\">" . fantacalcio_check_value($checks["regulars_module"]) . "</span>", 
      t("Modulo Riserve"), 
      "<span id=\"reserves_module_value\" class=\"reserves_module_value\">" . $values["reserves_module"] . "</span>", 
      "<span id=\"reserves_module\" class=\"reserves_module\">" . fantacalcio_check_value($checks["reserves_module"]) . "</span>"));
  
  return theme("table", array(
    "header" => array(), 
    "rows" => $rows, 
    "attributes" => array("class" => array("table", "well", "table-responsive"))));
}

/********************/
/* END utility      */
/********************/

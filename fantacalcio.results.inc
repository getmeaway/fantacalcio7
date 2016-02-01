<?php

/**
  * @file fantacalcio.risultati.inc
*/

/**
 * *********************
 */
/* GESTIONE RISULTATI */
/**
 * *********************
 */
function fantacalcio_results($round = '', $action = '') {

  $round = (is_numeric($round)) ? $round : Round::getLast() + 1;
  
  $out = '';
  
  $order = 1;
  
  $actions = array(
    'giornata' => array(
      'name' => t('Scegli giornata'), 
      'function' => 'fantacalcio_results_choose_round', 
      'order' => $order++), 
    'formazioni' => array(
      'name' => t('Controlla Formazioni'), 
      'function' => 'fantacalcio_results_lineup_check', 
      'order' => $order++), 
    'voti' => array(
      'name' => t('Carica Voti'), 
      'function' => 'fantacalcio_upload_votes', 
      'order' => $order++), 
    'titolari' => array(
      'name' => t('Trova titolari'), 
      'function' => 'fantacalcio_find_regulars', 
      'order' => $order++), 
    'modificatori' => array(
      'name' => 'Modificatori', 
      'function' => 'fantacalcio_get_modifiers', 
      'order' => $order++), 
    'totali' => array(
      'name' => t('Calcola totali'), 
      'function' => 'fantacalcio_get_totals', 
      'order' => $order++), 
    
    // 'passaggio' => array(
    // 'name' => 'Passaggio Turno',
    // 'function' => 'fantacalcio_get_next',
    // 'order' => $order++
    // ),
    
    'fine' => array(
      'name' => t('Chiudi giornata'), 
      'function' => 'fantacalcio_close_round', 
      'order' => $order++));
  
  if (module_exists('fantasfide'))
    $actions['sfide'] = array(
      'name' => 'Aggiorna sfide', 
      'function' => 'fantacalcio_update_challenges', 
      'order' => $order++);
  
  if (module_exists('simplenews'))
    $actions['newsletter'] = array(
      'name' => 'Invia newsletter', 
      'function' => 'fantacalcio_send_newsletter', 
      'order' => $order++);
  
  $out = "\n<div id='actions'>";
  $out .= get_actions_list($actions, $action, $round);
  $out .= "\n</div>";
  
  $actions_keys = array_keys($actions);
  $action = !empty($action) ? $action : $actions_keys[0];
  
  $function = $actions[$action]['function'];
  
  $out .= "\n<div id='data'>";
  $out .= $function($round);
  $out .= "\n</div>";
  
  return $out;
}

function get_actions_list($actions, $action = '', $round) {
  $actions_keys = array_keys($actions);
  $action = !empty($action) ? $action : $actions_keys[0];
  
  foreach ($actions as $act) {
    $order[] = $act["order"];
  }
  
  array_multisort($order, $actions);
  
  $this_order = $actions[$action]['order'];
  
  $items = array();
  foreach ($actions as $key => $act) {
    if ($key == $action)
      array_push($items, array(
        "data" => $act['name'], 
        "class" => array("list-group-item", "selected")));
    elseif ($act['order'] == ($this_order + 1))
      
      // $out .= "\n<li><a href='" . base_path() . arg(0) . "/" . $round . "/" . $key . "'>" . $act['name'] . "</a></li>";
      array_push($items, array(
        "data" => l($act['name'], arg(0) . "/" . $round . "/" . $key), 
        "class" => array("list-group-item")));
    elseif ($act['order'] < ($this_order))
      
      // $out .= "\n<li class='done'>" . $act['name'] . "</li>";
      array_push($items, array(
        "data" => $act['name'], 
        "class" => array("done", "list-group-item")));
    else
      // $out .= "\n<li>" . $act['name'] . "</li>";
      array_push($items, array(
        "data" => $act['name'], 
        "class" => array("list-group-item")));
  }
  
  // $out .= "\n</ul>";
  
  return theme("item_list", array(
    "items" => $items, 
    "type" => "ul", 
    "attributes" => array("class" => array("list-group"))));
}

function fantacalcio_risultati_calcolati() {
  $leagues = get_competitions();
  $calcolati = 0;
  
  foreach ($competitions as $c_id => $competition) {
    
    $last_round = get_last_formazioni($c_id);
    
    $n_played = get_competition_played($c_id);
    
    if ($n_played == $last_round)
      $calcolati++;
  }
  if ($calcolati == 0)
    return FALSE;
  else
    return TRUE;
}

/**
 * *********************
 */
/* scegli giornata */
/**
 * *********************
 */
function fantacalcio_results_choose_round($round) {
  drupal_set_title(t('Risultati: Giornata #@round', array("@round" => $round)));
  $out = drupal_get_form('results_choose_round_form', $round);
  
  return drupal_render($out);
}

function results_choose_round_form($form, $form_state, $round) {
  $rounds = Round::allWithStatus();
  
  $rounds_options = array();
  foreach ($rounds as $current_round)
    $rounds_options[$current_round->round] = $current_round->round . " (" . $current_round->status . ")";
  
  $form['round'] = array(
    '#type' => 'select', 
    '#default_value' => $round, 
    '#options' => $rounds_options,  // array_reduce($rounds, function($item) {return $item->round . " (" . $item->status . ")";}),
    '#title' => t('Giornata'));
  
  $form['submit'] = array('#type' => 'submit', '#value' => t('Scegli'));
  
  return $form;
}

function results_choose_round_form_submit($form, &$form_state) {
  $round = $form_state["values"]["round"];
  drupal_goto('risultati/' . $round . '/giornata');
}

/**
 * *********************
 */
/* END scegli giornata */
/**
 * *********************
 */

/**
 * *********************
 */
/* formazioni */
/**
 * *********************
 */
function fantacalcio_results_lineup_check($vote_round) {

  drupal_set_title(t('Risultati: Giornata #@round', array(
    "@round" => $vote_round)));
  
  $teams = Team::all();
  $competitions = Competition::allForRound($vote_round);
  
  $round = Round::getByRound($vote_round);

  //importo formazioni se il termine di invio è scaduto
  //if (time() > $round->date)
  //  import_lineups($vote_round);
  
  $lineups = array();

  $lineups_import_form = drupal_get_form("import_lineups_form", $vote_round);
  
  $out = drupal_render($lineups_import_form);
  
  foreach ($competitions as $c_id => $competition) {

    // squadre impegnate nella giornata    
    $teams = Team::getTeamsForRound($competition->id, $round->competitions[$competition->id]->competition_round, $competition->type);

    $rows = array();
    
    if (count($teams) > 0) {
      
      $out .= "<h3>" . $competition->name . "</h3>";
      
      foreach ($teams as $t_id => $team) {
        
        $lineup = Lineup::get($competition->id, $team->id, $competition->competition_round); // ($lineups[$t_id]);
        
        if ($lineup != null) {
          
          $module = array();
          
          if ($lineup->check())
            $status = "<span style='color: green;'><i class='fa fa-check-circle'></i></span>";
          else
            $status = "<span style='color: red; font-weight:bold;'><i class='fa fa-minus-circle'></i></span>";
          
          $insert_time = $lineup->getInsertTime();
          
          if ($insert_time > $round->date)
            $insert_time_out = "<span style='color: red; font-weight:bold;'>" 
                                . date("d/m/Y H:i:s", $lineup->getInsertTime())
                                . "</span>";
          else
            $insert_time_out = date("d/m/Y H:i:s", $lineup->getInsertTime());
          
          $rows[$t_id] = array(
            l($team->name, "formazioni/admin/" . $t_id), 
            $status, 
            implode("-", $lineup->getModuleRegulars()) . " / " . implode("-", $lineup->getModuleReserves()),
            $insert_time_out
            );
        }
        
        else
          $rows[$t_id] = array(
            l($team->name, "formazioni/admin/" . $t_id), 
            array(
              "data" => "<span style='color: red; font-weight:bold;'>" . t("Non inserita") . "</span>", 
              "colspan" => 3));
      }
      
      $out .= theme("table", array(
        "rows" => $rows, 
        "header" => array(
          t("Squadra"), 
          t("Valida"), 
          t("Modulo"), 
          t("Data inserimento"))));
    }
  }
  
  
  return $out;
}

function fantacalcio_admin_formazione($t_id = "", $competition_name = '') {
  if (!team_exists($t_id))
    return choose_team_formazione_admin();
  
  else {
    drupal_set_title(filter_xss("Amministra formazioni - " . get_team_name($t_id)));
    return fantacalcio_formazione_insert_start_admin($competition_name, $t_id);
  }
}

function fantacalcio_admin_penalties($t_id = "", $competition_name = '') {
  if (!team_exists($t_id))
    return choose_team_formazione_admin_penalties();
  
  else
    return fantacalcio_formazione_admin_penalties($competition_name, $t_id);
}

function choose_team_formazione_admin() {
  $out = '<br/>' . l("Amministra rigoristi &raquo;", "formazioni/admin/penalties", array(
    "html" => TRUE)) . '<br/><br/>';
  
  $teams = get_teams();
  
  foreach ($teams as $t_id => $team) {
    $team_names[] = $team->name;
  }
  
  array_multisort($team_names, $teams);
  
  $out .= "<ul>";
  foreach ($teams as $t_id => $team) {
    $out .= "<li>" . l($teams[$t_id]->name, "formazioni/admin/" . $teams[$t_id]->t_id) . "</li>";
  }
  $out .= "</ul>";
  
  return $out;
}

function choose_team_formazione_admin_penalties() {
  $out = '<br/>' . l("Amministra formazioni &raquo;", "formazioni/admin", array(
    "html" => TRUE)) . '<br/><br/>';
  
  $teams = get_teams();
  
  foreach ($teams as $t_id => $team) {
    $team_names[] = $team->name;
  }
  
  array_multisort($team_names, $teams);
  
  $out .= "<ul>";
  foreach ($teams as $t_id => $team) {
    $out .= "<li>" . l($teams[$t_id]->name, "formazioni/admin/penalties/" . $teams[$t_id]->t_id) . "</li>";
  }
  $out .= "</ul>";
  
  return $out;
}

function fantacalcio_insert_formazione_admin($competition_name = '', $t_id) {
  global $user;
  
  $competitions = get_competitions();
  
  if (empty($competition_name) || !is_competition_name($competition_name)) {
    // controllo quante formazioni vanno inserite per la giornata
    $vote_round = get_last_votes() or 0;
    $vote_round++;
    $sql = "SELECT * FROM {fanta_rounds_competitions} WHERE round = '%d'";
    $result = db_query($sql, $vote_round);
    while ($row = db_fetch_array($result)) {
      $sqla = "SELECT * FROM {fanta_matches} " . "WHERE g_id IN (SELECT g_id FROM {fanta_groups} WHERE c_id =  '%d') " . "AND round = '%d' " . "AND (t1_id = '%d' OR t2_id = '%d')";
      $resulta = db_query($sqla, $row['c_id'], $row['competition_round'], $t_id, $t_id);
      while ($rowa = db_fetch_array($resulta)) {
        $formazioni[$row['c_id']] = $rowa['round'];
      }
    }
    
    if (count($formazioni) > 1) {
      
      $teams = get_teams();
      $out = "<h3>" . $teams[$t_id]->name . "</h3>";
      
      $out .= "<p>Inserire una formazione per ogni partita</p>";
      
      $out .= "<ul>";
      foreach ($formazioni as $c_id => $round) {
        $competition = $competitions[$c_id];
        $out .= "\n<li><a href='" . base_path() . "formazioni/admin/" . $t_id . "/" . $competition->name . "'>" . $competition->name . " (" . $round . "&ordf; Giornata)</a></li>";
      }
      $out .= "</ul>";
      return $out;
    }
    elseif (count($formazioni) == 1) {
      
      foreach ($formazioni as $c_id => $round)
        $competition_name = $competitions[$c_id]->name;
      
      return insert_formazione_admin($competition_name, $t_id);
    }
  }
  
  else {
    
    return insert_formazione_admin($competition_name, $t_id);
  }
}

function fantacalcio_formazione_admin_penalties($competition_name = '', $t_id) {
  global $user;
  
  $competitions = get_competitions();
  
  if (empty($competition_name) || !is_competition_name($competition_name)) {
    // controllo quante formazioni vanno inserite per la giornata
    $vote_round = get_last_votes() or 0;
    $vote_round++;
    $sql = "SELECT * FROM {fanta_rounds_competitions} WHERE round = '%d' AND next = 1";
    $result = db_query($sql, $vote_round);
    while ($row = db_fetch_array($result)) {
      $sqla = "SELECT * FROM {fanta_matches} " . "WHERE g_id IN (SELECT g_id FROM {fanta_groups} WHERE c_id =  '%d') " . "AND round = '%d' " . "AND (t1_id = '%d' OR t2_id = '%d')";
      $resulta = db_query($sqla, $row['c_id'], $row['competition_round'], $t_id, $t_id);
      while ($rowa = db_fetch_array($resulta)) {
        $penalties[$row['c_id']] = $rowa['round'];
      }
    }
    
    if (count($penalties) > 1) {
      
      $teams = get_teams();
      $out = "<h3>" . $teams[$t_id]->name . "</h3>";
      
      $out .= "<p>Inserire una formazione per ogni partita</p>";
      
      $out .= "<ul>";
      foreach ($penalties as $c_id => $round) {
        $competition = $competitions[$c_id];
        $out .= "\n<li><a href='" . base_path() . "formazioni/admin/penalties/" . $t_id . "/" . $competition->name . "'>" . $competition->name . " (" . $round . "&ordf; Giornata)</a></li>";
      }
      $out .= "</ul>";
      return $out;
    }
    elseif (count($penalties) == 1) {
      
      foreach ($penalties as $c_id => $round)
        $competition_name = $competitions[$c_id]->name;
        
        // return insert_formazione_admin($competition_name, $t_id);
      return drupal_get_form('insert_penalties_form', $t_id, $c_id, $round);
    }
    
    else {
      return "La squadra non deve inserire rigoristi<br>" . l("Indietro", "formazioni/admin/penalties");
    }
  }
  
  else {
    
    // return insert_formazione_admin($competition_name, $t_id);
    return drupal_get_form('insert_penalties_form', $t_id, $c_id, $round);
  }
}

function insert_formazione_admin($competition_name, $t_id) {
  $c_id = get_competition_id($competition_name);
  $round = get_next_round_formazione($c_id);
  $teams = get_teams();
  
  $out = '';
  
  $ts_match = get_timestamp_match($c_id, $round);
  
  $out .= "\n<h3>" . ucwords($competition_name) . "</h3>";
  $out .= "\n<h4>" . $round . "&#7491; giornata</h4>";
  $out .= "\n<h3>" . $teams[$t_id]->name . "</h3>";
  
  $args = array('t_id' => $t_id, 'c_id' => $c_id, 'round' => $round);
  
  $formazione_form = drupal_get_form('fantacalcio_formazione_form', $args);
  
  $args_confirm = get_args_confirm($args);
  
  $out .= confirm_formazione($args_confirm);
  
  $validate = show_line_up_check(fantacalcio_formazione_check(get_line_up($c_id, $t_id, $round)));
  
  $out .= "\n<div id='validate'>" . $validate . "</div>";
  
  $out .= "\n<div id='formazione_form'>" . $formazione_form . "</div>";
  
  $out .= "\n<div class='clear' style='height:10px;'></div>";
  
  $out .= "\n<div id='formazione'>" . make_formazione($t_id, $c_id, $round, TRUE) . "\n</div>";
  
  return $out;
}

function import_lineups_form($form, &$form_state, $round) {
  $form['round'] = array(
    '#type' => 'hidden', 
    '#default_value' => $round, 
    '#value' => $round); 
  
  $form['submit'] = array('#type' => 'submit', '#value' => t('Importa formazioni'));

  return $form;
}

function import_lineups_form_submit($form, &$form_state) {
  $round = $form_state["values"]["round"];

  import_lineups_lineups($round);
}

function import_lineups_lineups($vote_round) {
  
  // elenco competizioni
  $round = Round::getByRound($vote_round);
  
  // per ogni competizione
  foreach (Competition::all() as $competition) {
    
    // foreach($competition->groups as $group) {
    if (isset($round->competitions[$competition->id])) {
      
      // formazioni mancanti
      if ($competition->type == COMPETITION_TYPE_SD)
        $teams = Team::getTeamsForRound($competition->id, $round->competitions[$competition->id]->competition_round, $competition->type);
      if ($competition->type == COMPETITION_TYPE_GP)
        $teams = Team::allByCompetition($competition->id);
      
      $lineups = Lineup::allForRound($round->competitions[$competition->id]->competition_round, $competition);
      // echo $group->name. " " . count($teams) . " " . count($lineups) . "<br>";
      
      $missing_lineups = array();
      foreach ($teams as $team) {
        if (!array_key_exists($team->id, $lineups))
          array_push($missing_lineups, $team);
      }
      
      // per ogni formazione
      $i = 0;
      foreach ($missing_lineups as $team) {
        $i++;
        // importo formazione (giornata precedente / default)
        Lineup::import($team, $competition, $round->competitions[$competition->id]->competition_round);
      }

    }
  }
}

/**
 * *********************
 */
/* END formazioni */
/**
 * *********************
 */

/**
 * *****************
 */
/* voti */
/**
 * *****************
 */
function fantacalcio_load_votes($round) {
  drupal_set_title(t('Risultati: Giornata #@round', array(
    "@round" => $vote_round)));
  
  $providers = get_vote_providers();
  $votes = get_votes_html_data($round, $providers);
  
  foreach ($providers as $p_id => $provider) {
    insert_votes($votes[$p_id], $round, $p_id);
  }
  return "";
}

function fantacalcio_upload_votes($vote_round) {
  drupal_set_title(t('Risultati: Giornata #@round', array(
    "@round" => $vote_round)));
  
  $out = "";
  if (variable_get('fantacalcio_votes_url', '') != "")
    $out = "<p>" . l("Voti", variable_get('fantacalcio_votes_url', '')) . "</p>";
  
  $upload_form = drupal_get_form('votes_upload_form', $vote_round);
  
  return $out . drupal_render($upload_form);
}

function votes_upload_form($form, $form_state, $round) {
  
  global $providers;

  // $form['#redirect'] = 'risultati/titolari';
  $last_round = 0;
  $all_real_teams = RealTeam::all();
  
  $real_teams = array();
  foreach ($all_real_teams as $real_team)
    $real_teams[$real_team->id] = $real_team->name;
    
    // $sql = "SELECT round FROM {fanta_rounds} WHERE status = 1 ORDER BY round";
    // $result = db_query($sql);
    // while ($row = db_fetch_object($result)) {
    // $rounds[$row->round] = $row->round;
    // $last_round = $row->round;
    // }
    
  // $rounds[$last_round + 1] = $last_round + 1;
  
  $form["voto_default_1"] = array(
    "#type" => "fieldset", 
    "#collapsed" => TRUE, 
    "#collapsible" => TRUE, 
    "#title" => "Voto default #1");
  
  $form["voto_default_1"]["teams_default_1"] = array(
    "#type" => "checkboxes", 
    "#title" => "Squadre", 
    "#default_value" => array(), 
    "#description" => "Squadre che non hanno giocato nella giornata (Voto default: " . variable_get("fantacalcio_default_vote_1", "6") . ")", 
    "#options" => $real_teams);
  
  $form["voto_default_2"] = array(
    "#type" => "fieldset", 
    "#collapsed" => TRUE, 
    "#collapsible" => TRUE, 
    "#title" => "Voto default #2");
  
  $form["voto_default_2"]["teams_default_2"] = array(
    "#type" => "checkboxes", 
    "#title" => "Squadre", 
    "#default_value" => array(), 
    "#description" => "Squadre che non hanno giocato nella giornata (Voto default: " . variable_get("fantacalcio_default_vote_2", "6") . ")", 
    "#options" => $real_teams);
  
  $form['provider'] = array(
    '#type' => 'checkboxes', 
    '#options' => $providers,
    '#default_value' => array(variable_get('fantacalcio_votes_provider', 1)), 
    '#title' => t('Provider'));
  
  $form['round'] = array(
    '#type' => 'hidden', 
    '#default_value' => $round, 
    '#value' => $round, 
    '#title' => t('Giornata'));
  
  $form["file"] = array(
    "#type" => "fieldset", 
    "#collapsed" => TRUE, 
    "#collapsible" => TRUE, 
    "#title" => t("File di testo esterno"));
  
  $form["file"]['text'] = array(
    '#type' => 'textarea', 
    '#rows' => 20, 
    '#cols' => 80, 
    '#title' => t('File'));
  
  $form['submit'] = array('#type' => 'submit', '#value' => t('Carica'));
  
  $form['clear'] = array(
    '#type' => 'submit', 
    '#value' => 'Annulla', 
    '#validate' => array('upload_form_clear'));

  
  return $form;
}

function votes_upload_form_clear($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
}

function votes_upload_form_submit($form, &$form_state) {

  $text = $form_state["values"]["text"];
  $round = $form_state["values"]["round"];
  
  $votes_url = DATA_SOURCE_URL . "/votes/" . $round . "-" . variable_get("fantacalcio_votes_provider", 1) . ".json";
  
  $votes = json_decode(file_get_contents($votes_url));

  foreach ($form_state["values"]["provider"] as $provider) {
  	if ($provider > 0) {
      $out = insert_votes($votes, $round, $provider);
      insert_default_votes($form_state["values"]["teams_default_1"], $round, 1, $provider);
      insert_default_votes($form_state["values"]["teams_default_2"], $round, 2, $provider);
  	}
  }

  update_players($round + 1);
  
  // risultati provvisori
  db_update("fanta_rounds")->condition("round", $round)->fields(array(
    "status" => -1))->execute();
  
  drupal_set_message('Voti caricati');
  drupal_set_message(check_plain($out));
}

function fantacalcio_upload_votes_cron($round) {
  $providers = get_vote_providers();
  // $votes = get_votes_html_data($round, $providers);
  
  $votes_url = DATA_SOURCE_URL . "/votes/" . $round . "-" . variable_get("fantacalcio_votes_provider", 1) . ".json";
  
  $votes = json_decode(file_get_contents($votes_url));
  
  foreach ($providers as $provider => $vote_provider) {
  	if ($provider > 0) {
      $out = insert_votes($votes[$provider], $round, $provider);
      insert_default_votes($form_state["values"]["teams_default_1"], $round, 1, $provider);
      insert_default_votes($form_state["values"]["teams_default_2"], $round, 2, $provider);
  	}
  }
  
  update_players($round + 1);
  
  // risultati provvisori
  db_query("UPDATE {fanta_rounds} SET status = -1 WHERE round = '%d'", $round);
  
  // drupal_set_message('Voti caricati');
  // drupal_set_message(check_plain($out));
}

function parse_votes_text($text, $round, $provider) {
  
  // spezzo il file in righe
  $file_righe = explode("\n", $text);
  $query_values = array();
  
  // n° righe
  $count_1 = count($file_righe);
  
  // inserisco ogni riga come un nuovo record della tabella 'fanta_votes'
  foreach ($file_righe as $key => $riga) {
    $riga = trim($riga);
    if (is_numeric(substr($riga, 0, 1))) {
      // separo i campi della riga con il carattere \t "tab"
      $elems = explode(chr(9), $riga);
      
      // se il ruolo è P,D,C,A => ok
      if (in_array($elems[1], array("P", "D", "C", "A"))) {
        foreach ($elems as $id_elem => $elem) {
          $elem = trim($elem);
        }
        // controllo se ha preso SV
        if (!is_numeric($elems[3]))
          $elems[3] = strtr($elems[3], ',', '.');
          
          // giocatori senza voti ma con bonus e/o malus
        if (!is_numeric($elems[3]) || $elems[3] == 0) {
          // portiere SV
          if ($elems[1] == 'P') {
            $elems[3] = 6;
          }
          // non portiere con bonus/malus
          elseif (($elems[4] + $elems[5] + $elems[6] + $elems[7] + $elems[8] + $elems[9] + $elems[10] + $elems[11] + $elems[12] + $elems[13]) != 0) {
            $elems[3] = 6;
          }
          // non inserire
          else {
            unset($file_righe[$key]);
          }
        }
        
        // elimino il ruolo del giocatore dal voto
        if (isset($file_righe[$key]))
          unset($elems[1]);
          
          // sommo assist da azione con assist da fermo
          // $elems[12] = $elems[12] + $elems[13];
          
        // elimino nome, assist da fermo, gol_vittoria e gol_pareggio
        unset($elems[2]);
        unset($elems[13]);
        unset($elems[14]);
        unset($elems[15]);
        
        // trasformo il voto a un solo decimale
        $elems[3] = (strlen($elems[3]) == 4) ? substr($elems[3], 0, 3) : $elems[3];
        // echo $elems[3]." - ";
        
        if (isset($file_righe[$key])) {
          $query_values[$key] = implode(',', $elems);
          $votes[$elems[0]][$provider] = array(
            "vote" => $elems[3], 
            "gol_fatto" => $elems[4], 
            "gol_subito" => $elems[5], 
            "rig_parato" => $elems[6], 
            "rig_sbagliato" => $elems[7], 
            "rig_fatto" => $elems[8], 
            "autogol" => $elems[9], 
            "amm" => $elems[10], 
            "esp" => $elems[11], 
            "assist" => $elems[12], 
            "assist_fermo" => $elems[13]);
        }
      }
    }
    else
      unset($file_righe[$key]);
  }
  
  sort($query_values);
  array_unique($query_values);
  
  $tot = count($file_righe);
}

function insert_votes($votes, $round, $provider) {
  global $roles;

  $tot = 0;

  // VOTI della GIORNATA  
  $query = db_delete("fanta_votes");
  $query->condition("round", $round);  
  $query->condition("provider", $provider);
  
  $result = $query->execute();
  
  $players_ids = Player::getIdList();

// real teams
    $real_teams = RealTeam::allNames();
    $real_teams = array_flip($real_teams);

    $roles = array_flip($roles);

    foreach ($votes->votes as $name => $vote) {
      if (!in_array($name, array_keys($players_ids))) {
      	$pl_id = db_insert("fanta_players")->fields(array("name" => strtoupper($name), "role" => $roles[strtoupper($vote->role)]))->execute();
      	$rt_id = $real_teams[strtolower($vote->team)];
      	db_insert("fanta_players_rounds")->fields(array("pl_id" => $pl_id, "rt_id" => $rt_id, "round" => $round, "quotation" => 0, "not_rounded_quotation" => 0, "active" => 1))->execute();
      }
      else {
      	$pl_id = $players_ids[$name];
      }
      
      $total = $vote->vote + ($vote->goals_for * variable_get("fantacalcio_points_goals_for", "3")) + ($vote->penalty_goals * variable_get("fantacalcio_points_penalty_goals", "3")) + ($vote->assists * variable_get("fantacalcio_points_assists", "1")) + ($vote->saved_penalties * variable_get("fantacalcio_points_saved_penalties", "3")) + ($vote->goals_against * variable_get("fantacalcio_points_goals_against", "-1")) + ($vote->red_cards * variable_get("fantacalcio_points_red_card", "-1")) + ($vote->yellow_cards * variable_get("fantacalcio_points_yellow_card", "-0.5")) + ($vote->own_goals * variable_get("fantacalcio_points_own_goal", "-2")) + ($vote->missed_penalties * variable_get("fantacalcio_points_missed_penalties", "-3")) + ($vote->draw_goals * variable_get("fantacalcio_points_draw_goals", "1")) + ($vote->win_goals * variable_get("fantacalcio_points_win_goals", "3"));
      
      $query = db_insert("fanta_votes");
      $query->fields(array(
        "round" => $round, 
        "pl_id" => $pl_id, 
        "provider" => $provider, 
        "total" => $total, 
        "vote" => $vote->vote, 
        "goals_for" => $vote->goals_for, 
        "goals_against" => $vote->goals_against, 
        "saved_penalties" => $vote->saved_penalties, 
        "missed_penalties" => $vote->missed_penalties, 
        "penalty_goals" => $vote->penalty_goals, 
        "own_goals" => $vote->own_goals, 
        "yellow_cards" => $vote->yellow_cards, 
        "red_cards" => $vote->red_cards, 
        "assists" => $vote->assists, 
        "win_goals" => $vote->win_goals, 
        "draw_goals" => $vote->draw_goals,
        "regular" => $vote->regular,
        "substituted" => $vote->substituted,
        "has_vote" => $vote->has_vote
      ));
      // print $query->arguments();
      $query->execute();
      
      $tot++;
    
  }
  
  return $tot . " Voti Inseriti";
}

function insert_default_votes($teams = array(), $round, $number_default, $provider_id) {
  if ($teams) {
    $checked_teams = array();
    foreach ($teams as $t_id => $value) {
      if ($value != "0")
        array_push($checked_teams, $value);
    }
    
    if ($checked_teams) {
      $sqla = "INSERT INTO {fanta_votes} (pl_id, provider, round, total, vote) VALUES ";
      
      $message = "Voto di default #" . $number_default . " (" . variable_get('fantacalcio_default_vote_' . $number_default, '6') . ") per i giocatori di";
      
      $team_sql = "";
      foreach ($checked_teams as $team) {
        $team_sql .= "'" . $team . "',";
        $message .= " " . $team . ",";
      }
      $team_sql = substr($team_sql, 0, -1);
      $message = substr($message, 0, -1);
      drupal_set_message(check_plain($message));
      
      $sql = "SELECT DISTINCT pl_id FROM {fanta_players_rounds} WHERE rt_id in (" . implode(",", $checked_teams) . ") AND round = " . ($round - 1);
      echo $sql;$resulta = db_query($sql);
      foreach ($resulta as $row) {
        $sqla .= "\n(" . $row->pl_id . ", " . $provider_id . ", " . $round . ", " . variable_get('fantacalcio_default_vote_' . $number_default, '6') . ", " . variable_get('fantacalcio_default_vote_' . $number_default, '6') . "),";
      }
      $sqla = substr($sqla, 0, -1);
      
      $resulta = db_query($sqla);
    }
  }
}

function update_players($round) {
  $i = 0;
  
  // cancello i giocatori della giornata per evitare doppioni
  db_delete("fanta_players_rounds")->condition("round", $round)->execute();
  
  $query = db_select("fanta_players_rounds", "pr");
  $query->condition("round", $round - 1);
  $query->fields("pr"); // "SELECT DISTINCT pl_id, rt_id as team, active FROM {fanta_players_teams} WHERE round = '%d'";
  $result = $query->execute(); // ($sql, ($round - 1));
  
  foreach ($result as $row) {
    $i++;
    
    db_insert("fanta_players_rounds")
    ->fields(array("pl_id" => $row->pl_id, "round" => $round, "rt_id" => $row->rt_id, "quotation" => $row->quotation, "not_rounded_quotation" => $row->not_rounded_quotation, "active" => $row->active))
    ->execute();
  }
  
  $message = "Giocatori aggiornati: " . $i . " (Giornata #" . $round . ")";
  drupal_set_message(check_plain($message));
}

function get_teams_serieA() {
  $sql = "SELECT rt_id, long_name FROM {fanta_real_teams} ORDER BY long_name";
  $result = db_query($sql);
  while ($row = db_fetch_array($result)) {
    $teams[$row["rt_id"]] = ucwords($row["long_name"]);
  }
  return $teams;
}

/**
 * *****************
 */
/* END voti */
/**
 * *****************
 */

/**
 * *********************
 */
/* calcola modificatori */
/**
 * *********************
 */
function fantacalcio_get_modifiers($vote_round) {
  drupal_set_title(t('Risultati: Giornata #@round', array(
    "@round" => $vote_round)));
  
  if (variable_get('fantacalcio_modifier_role_0', '0') || variable_get('fantacalcio_modifier_role_1', '0') || variable_get('fantacalcio_modifier_role_2', '0') || variable_get('fantacalcio_modifier_role_3', '0')) {
    
    $out = "";
    
    // $vote_round = get_last_votes();
    $teams = Team::all();
    $round = Round::getByRound($vote_round); // print_r($round);die();
    $votes = $round->getVotes(variable_get("fantacalcio_votes_provider", 1));
    $competitions = Competition::all();
    
    $matches = Match::getMatchesByRound($vote_round);
    
    foreach ($matches as $m_id => $match) {
      $t1_id = $match->t1_id;
      $t2_id = $match->t2_id;
      $competition_round = $match->round;
      
      if (variable_get('fantacalcio_modifier_role_0', '0')) {
        $mod_1_role_0 = Result::getModifierRole_0($t1_id, $match->c_id, $vote_round, $competition_round, variable_get("fantacalcio_votes_provider", 1));
        $mod_2_role_0 = Result::getModifierRole_0($t2_id, $match->c_id, $vote_round, $competition_round, variable_get("fantacalcio_votes_provider", 1));
      }
      else {
        $mod_1_role_0 = 0;
        $mod_2_role_0 = 0;
      }
      
      if (variable_get('fantacalcio_modifier_role_1', '0')) {
        $mod_1_role_1 = Result::getModifierRole_1($t2_id, $match->c_id, $vote_round, $competition_round, variable_get("fantacalcio_votes_provider", 1));
        $mod_2_role_1 = Result::getModifierRole_1($t1_id, $match->c_id, $vote_round, $competition_round, variable_get("fantacalcio_votes_provider", 1));
      }
      else {
        $mod_1_role_1 = 0;
        $mod_2_role_1 = 0;
      }
      
      if (variable_get('fantacalcio_modifier_role_2', '0')) {
        $mod_role_2 = Result::getModifierRole_2($t1_id, $t2_id, $match->c_id, $vote_round, $competition_round, variable_get("fantacalcio_votes_provider", 1));
        $mod_1_role_2 = $mod_role_2[1];
        $mod_2_role_2 = $mod_role_2[2];
      }
      else {
        $mod_1_role_2 = 0;
        $mod_2_role_2 = 0;
      }
      
      if (variable_get('fantacalcio_modifier_role_3', '0')) {
        $mod_1_role_3 = Result::getModifierRole_3($t1_id, $match->c_id, $vote_round, $competition_round, variable_get("fantacalcio_votes_provider", 1));
        $mod_2_role_3 = Result::getModifierRole_3($t2_id, $match->c_id, $vote_round, $competition_round, variable_get("fantacalcio_votes_provider", 1));
      }
      else {
        $mod_1_role_3 = 0;
        $mod_2_role_3 = 0;
      }
      
      $sql = "UPDATE {fanta_matches} SET 
          mod_1_role_0 = '%f',
          mod_2_role_0  = '%f',
          mod_1_role_1 = '%f',
          mod_2_role_1  = '%f',
          mod_1_role_2 = '%f',
          mod_2_role_2  = '%f',
          mod_1_role_3 = '%f',
          mod_2_role_3  = '%f'
          WHERE m_id = '%d'";
      // $result = db_query($sql, $mod_1_role_0, $mod_2_role_0, $mod_1_role_1, $mod_2_role_1, $mod_1_role_2, $mod_2_role_2, $mod_1_role_3, $mod_2_role_3, $match->m_id);
      
      $query = db_update("fanta_matches");
      $query->fields(array(
        "mod_1_role_0" => $mod_1_role_0, 
        "mod_1_role_1" => $mod_1_role_1, 
        "mod_1_role_2" => $mod_1_role_2, 
        "mod_1_role_3" => $mod_1_role_3, 
        "mod_2_role_0" => $mod_2_role_0, 
        "mod_2_role_1" => $mod_2_role_1, 
        "mod_2_role_2" => $mod_2_role_2, 
        "mod_2_role_3" => $mod_2_role_3));
      $query->condition("m_id", $m_id);
      
      $query->execute();
    }
    
    // output
    foreach ($round->competitions as $round_competition) {
      
      // $sql = "SELECT * FROM {fanta_matches} "
      // . "WHERE g_id IN (SELECT g_id FROM {fanta_groups} WHERE c_id = '%d') "
      // . "AND round = '%d' ";
      // $result = db_query($sql, $c_id, $competition_round);
      
      $subquery = db_select('fanta_groups', 'g');
      $subquery->condition("c_id", $round_competition->competition_id);
      $subquery->fields('g', array("g_id"));
      
      $query = db_select("fanta_matches", "m");
      $query->condition("g_id", $subquery, "IN");
      $query->condition("round", $round_competition->competition_round);
      
      $query->fields("m");
      
      $result = $query->execute();
      
      $out .= "<h3>" . check_plain($competitions[$round_competition->competition_id]->name) . "</h3>";
      
      $header = array(
        t("Squadra"), 
        t("Portiere"), 
        t("Difesa"), 
        t("Centrocampo"), 
        t("Attacco"));
      $rows = array();
      
      foreach ($result as $row) {
        $rows[$row->m_id . "_1"][] = $teams[$row->t1_id]->name;
        $rows[$row->m_id . "_1"][] = $row->mod_1_role_0;
        $rows[$row->m_id . "_1"][] = $row->mod_1_role_1;
        $rows[$row->m_id . "_1"][] = $row->mod_1_role_2;
        $rows[$row->m_id . "_1"][] = $row->mod_1_role_3;
        $rows[$row->m_id . "_2"][] = $teams[$row->t2_id]->name; // squadra 2
        $rows[$row->m_id . "_2"][] = $row->mod_2_role_0;
        $rows[$row->m_id . "_2"][] = $row->mod_2_role_1;
        $rows[$row->m_id . "_2"][] = $row->mod_2_role_2;
        $rows[$row->m_id . "_2"][] = $row->mod_2_role_3;
        
        $rows[$row->m_id . "_3"][] = array("data" => "", "colspan" => 5); // separatore
      }
      
      $out .= theme("table", array(
        "header" => $header, 
        "rows" => $rows, 
        "attributes" => array("class" => array("table", "table-responsive"))));
    }
  }
  else
    $out = t("No modificatori");
  
  return $out;
}



/**
 * *********************
 */
/* END modificatori */
/**
 * *********************
 */

/**
 * *********************
 */
/* titolari */
/**
 * *********************
 */
function fantacalcio_find_regulars($vote_round) {

  drupal_set_title(t('Risultati: Giornata #@round', array(
    "@round" => $vote_round)));
  
  $out = "";
  
  // $vote_round = get_last_votes();
  
  $teams = Team::all();
  
  $round = Round::getByRound($vote_round); // print_r($round);die();
  $votes = $round->getVotes(variable_get("fantacalcio_votes_provider", 1)); // get_votes($vote_round);
//  print_r($round);die();
  $pl_votes = array();
  
  foreach ($votes as $vote) {
    if ($vote->has_vote == 1) 
      $pl_votes[] = $vote->pl_id;
  }
  
    // print_r($pl_votes);die();
    
  // $pl_ids_list = implode(',', $pl_votes);
    
  // $sqlx = "SELECT * FROM {fanta_rounds_competitions} " .
    // "WHERE round = '%d'";
    // $resultx = db_query($sqlx, $vote_round);
    // while ($row = db_fetch_array($resultx)) {
  
  foreach ($round->competitions as $round_competition) {

    $c_id = $round_competition->competition_id;
    $competition_round = $round_competition->competition_round;
    
    $out .= "<h3>" . Competition::get($c_id)->name . "</h3>";
    
    // resetto i valori
    $query = db_update("fanta_lineups");
    $query->fields(array("has_played" => 0));
    $query->condition("round", $competition_round);
    $query->condition("c_id", $c_id);
    
    $query->execute();
    
    // titolari con voto
    $query = db_update("fanta_lineups");
    $query->fields(array("has_played" => 1));
    $query->condition("round", $competition_round);
    $query->condition("c_id", $c_id);
    $query->condition("position", 1);
    $query->condition("pl_id", $pl_votes, "IN");
    
    $result = $query->execute();
    
    // trovo riserve da far entrare
    $query = db_select("fanta_lineups", "l");
    $query->condition("round", $competition_round);
    $query->condition("c_id", $c_id);
    $query->distinct();
    $query->fields("l", array("t_id"));
    
    $result = $query->execute();
    
    foreach ($result as $row) {
      Result::getRegularsTeam($row->t_id, $pl_votes, $competition_round, $c_id);
    }
    
    // report
    $query = db_select("fanta_lineups", "l");
    $query->addExpression("count(*)", "n");
    $query->condition("has_played", 1);
    $query->condition("c_id", 1);
    $query->condition("round", 1);
    $query->fields("l");
    $query->groupBy("t_id");
    
    $result = $query->execute();
    
    $played = array();
    $i = 0;
    
    foreach ($result as $row) {
      $i++;
      $played[$i]['t_id'] = $row->t_id;
      $played[$i]['n'] = $row->n;
    }
    
    $query = db_select("fanta_lineups", "l");
    $query->join("fanta_players", "p", "p.pl_id = l.pl_id");
    $query->condition("c_id", $c_id);
    $query->condition("round", $competition_round);
    $query->fields("l");
    $query->fields("p");
    
    $result = $query->execute();
    
    $lineups = array();
    foreach ($result as $row) {
      
      if (!isset($lineups[$row->t_id]["regulars"]))
        $lineups[$row->t_id]["regulars"] = array(0, 0, 0, 0);
      if (!isset($lineups[$row->t_id]["played"]))
        $lineups[$row->t_id]["played"] = array(0, 0, 0, 0);
      
      if ($row->position == 1)
        $lineups[$row->t_id]["regulars"][$row->role]++;
      if ($row->has_played == 1)
        $lineups[$row->t_id]["played"][$row->role]++;
    }
    
    // riepilogo titolari squadre
    $header = array(
      "Squadra", 
      "N&deg; Titolari", 
      "Modulo Titolari", 
      "Modulo Formazione");
    
    $rows = array();
    foreach ($lineups as $key => $value) {
      $n_titolari = array_sum($lineups[$key]["played"]);
      
      $style = ($n_titolari == 11) ? "" : "font-weight: bold; color: red;";
      
      ksort($lineups[$key]["played"]);
      ksort($lineups[$key]["regulars"]);
      
      $rows[$key][] = $teams[$key]->name;
      $rows[$key][] = array("data" => $n_titolari, "style" => $style);
      $rows[$key][] = implode("-", $lineups[$key]["played"]);
      $rows[$key][] = implode("-", $lineups[$key]["regulars"]);
    }
    
    $out .= theme("table", array("header" => $header, "rows" => $rows));
  }
  
  return $out;
}



/**
 * *********************
 */
/* END titolari */
/**
 * *********************
 */

/**
 * *********************
 */
/* totali */
/**
 * *********************
 */
function fantacalcio_get_totals($vote_round) {
  drupal_set_title(t('Risultati: Giornata #@round', array(
    "@round" => $vote_round)));
  
  // $vote_round = get_last_votes();
  // $teams = get_teams();
  $teams = Team::all();
  
  $round = Round::getByRound($vote_round);
  $votes = $round->getVotes(variable_get("fantacalcio_votes_provider", 1));
  
  $matches_competitions = array();
  
  $out = '';
  
  foreach ($round->competitions as $round_competition) {
    
    $c_id = $round_competition->competition_id;
    $competition = Competition::get($c_id);
    $competition_round = $round_competition->competition_round;
    
    if ($competition->type == COMPETITION_TYPE_SD) {
      
      $matches = $round->getMatches($c_id);
      
      $header = array(
        t("Squadra 1"), 
        t("Voti"), 
        t("Modificatori"), 
        t("Bonus"), 
        t("Tot"), 
        t("Gol"), 
        t(""), 
        t("Squadra 2"), 
        t("Voti"), 
        t("Modificatori"), 
        t("Bonus"), 
        t("Tot"), 
        t("Gol"));
      $rows = array();
      
      foreach ($matches as $m_id => $match) {
        
        $t1_id = $match->t1_id;
        $t2_id = $match->t2_id;
        $competition_round = $match->round;
        
        $mod_1 = array(
          $match->mod_1_role_0, 
          $match->mod_1_role_1, 
          $match->mod_1_role_2, 
          $match->mod_1_role_3);
        $mod_2 = array(
          $match->mod_2_role_0, 
          $match->mod_2_role_1, 
          $match->mod_2_role_2, 
          $match->mod_2_role_3);
        
        $bonus_t1 = $match->bonus_t1;
        $bonus_t2 = $match->bonus_t2;
        
        $tot_votes_1 = Result::getTotal($t1_id, $competition_round, $vote_round, $c_id, variable_get("fantacalcio_votes_provider", 1));
        $tot_votes_2 = Result::getTotal($t2_id, $competition_round, $vote_round, $c_id, variable_get("fantacalcio_votes_provider", 1));
        $tot_1 = $tot_votes_1 + array_sum($mod_1) + $bonus_t1;
        $tot_2 = $tot_votes_2 + array_sum($mod_2) + $bonus_t2;
        
        $goals_1 = floor(($tot_1 - 60) / 6);
        $goals_2 = floor(($tot_2 - 60) / 6);
        $goals_1 = ($goals_1 >= 0) ? $goals_1 : 0;
        $goals_2 = ($goals_2 >= 0) ? $goals_2 : 0;
        
        // vittoria con scarto
        if (variable_get('fantacalcio_scarto', '0') && variable_get('fantacalcio_scarto_punti', '0') > 0) {
          if (($goals_1 == $goals_2) && ($tot_1 - $tot_2) > variable_get('fantacalcio_scarto_punti', '0'))
            $goals_1++;
          if (($goals_1 == $goals_2) && ($tot_2 - $tot_1) > variable_get('fantacalcio_scarto_punti', '0'))
            $goals_2++;
        }
        
        if ($goals_1 > $goals_2)
          $winner_id = $t1_id;
        else 
          if ($goals_1 < $goals_2)
            $winner_id = $t2_id;
          else 
            if ($goals_1 == $goals_2)
              $winner_id = -1;
          
          // aggiorno partite
        $query_update = db_update("fanta_matches");
        $query_update->fields(array(
          "pt_1" => $tot_votes_1, 
          "pt_2" => $tot_votes_2, 
          "tot_1" => $tot_1, 
          "tot_2" => $tot_2, 
          "goals_1" => $goals_1, 
          "goals_2" => $goals_2, 
          "played" => 1, 
          "winner_id" => $winner_id));
        $query_update->condition("m_id", $match->m_id);
        
        $result = $query_update->execute();
        
        $classes_1 = $t1_id == $winner_id ? "bold" : "";
        $classes_2 = $t2_id == $winner_id ? "bold" : "";
        
        $rows[] = array(
          array("data" => Team::get($t1_id)->name, "class" => array($classes_1)), 
          $tot_votes_1, 
          implode("/", $mod_1), 
          $bonus_t1, 
          $tot_1, 
          $goals_1, 
          "", 
          array("data" => Team::get($t2_id)->name, "class" => array($classes_2)), 
          $tot_votes_2, 
          implode("/", $mod_2), 
          $bonus_t2, 
          $tot_2, 
          $goals_2);
      }
      
      $out .= "<h3>" . $competition->name . "</h3>";
      $out .= theme("table", array(
        "header" => $header, 
        "rows" => $rows, 
        "empty" => t("Nessuna partita"), 
        "attributes" => array(
          "classes" => array("table", "table-responsive", "table-striped"))));
    }
    
    if ($competition->type == COMPETITION_TYPE_GP) {
      
      $header = array(
        t("Squadra"), 
        t("Tot"), 
        t("Posizione giornata"), 
        t("Posizione Stagione"));
      
      $rows = array();
      
      $competition->groups = Group::allByCompetition($competition->id);
      
      $round = Round::getCompetitionRound($vote_round, $competition->id);
      
      $query = db_delete("fanta_teams_rounds");
      $query->condition("round", $competition_round);
      $query->condition("c_id", $competition->id);
      $query->execute();
      
      // squadre per competizione
      foreach ($competition->groups as $group) {
        
        $teams = Team::allByGroup($group->id);
        
        if ($teams) {
          
          $total = array();
          $round_positions = array();
          $season_positions = array();
          
          foreach ($teams as $team) {
            $total = Result::getTotal($team->id, $competition_round, $vote_round, $competition->id, variable_get("fantacalcio_votes_provider", 1));
            
            $totals[$team->id] = $total;
            $round_positions[$team->id] = $total;
            $season_positions[$team->id] = $team->seasonPoints($competition->id) + $total;
          }
          
          arsort($round_positions);
          arsort($season_positions);
          
          foreach ($teams as $team) {
            
            if ($totals[$team->id] > 0) {
              
              $round_position = array_search($team->id, array_keys($round_positions)) + 1;
              $season_position = array_search($team->id, array_keys($season_positions)) + 1;
              
              $query = db_insert("fanta_teams_rounds");
              $query->fields(array(
                "t_id" => $team->id, 
                "c_id" => $competition->id, 
                "round" => $competition_round, 
                "mode" => 1, 
                "points" => $totals[$team->id], 
                "round_position" => $round_position, 
                "season_position" => $season_position));
              
              $query->execute();
              
              $rows[] = array(
                $team->name, 
                $totals[$team->id], 
                $round_position, 
                $season_position);
            }
          }
        }
      }
      
      $out .= "<h3>" . $competition->name . "</h3>";
      $out .= theme("table", array(
        "header" => $header, 
        "rows" => $rows, 
        "empty" => t("Nessuna partita"), 
        "attributes" => array(
          "classes" => array("table", "table-responsive", "table-striped"))));
    }
  }
  
  return $out;
}

/**
 * *****************
 */
/* passaggio turno */
/**
 * *****************
 */
function fantacalcio_passaggio_turno($vote_round) {
  drupal_set_title(filter_xss('Risultati ' . $vote_round . '&ordf; giornata'));
  
  $teams = get_teams();
  
  $matches = get_matches_next($vote_round);
  
  if ($matches) {
    $header = array(
      "Label", 
      "Team 1", 
      "Team 2", 
      "Goal 1", 
      "Goal 2", 
      "OT 1", 
      "OT 2", 
      "Penalty 1", 
      "Penalty 2", 
      "Vincente");
    
    $rows = array();
    foreach ($matches as $label => $matches_label) {
      $rows[$label][] = $label;
      
      // sommo i goals della squadra (andata + ritorno)
      foreach ($matches_label as $m_id => $match) {
        $goals[$match->t1_id]["h"] += $match->goals_1;
        $goals[$match->t2_id]["a"] += $match->goals_2;
        
        $t1 = $match->t1_id;
        $t2 = $match->t2_id;
      }
      
      // ///table
      
      // squadre
      $rows[$label][] = $teams[$match->t1_id]->name;
      $rows[$label][] = $teams[$match->t2_id]->name;
      
      // gol (tempi regolamentari)
      $rows[$label][] = "<b>" . array_sum($goals[$t1]) . "</b> (" . $goals[$t1]["h"] . "+" . $goals[$t1]["a"] . ")";
      $rows[$label][] = "<b>" . array_sum($goals[$t2]) . "</b> (" . $goals[$t2]["h"] . "+" . $goals[$t2]["a"] . ")";
      
      // tempi regolamentari in parità --> tempi supplementari
      if (array_sum($goals[$t1]) == array_sum($goals[$t2]) && (($goals[$t1]["a"] == $goals[$t2]["a"]) || count($matches_label) == 1)) {
        
        $overtime = get_overtime($match, variable_get("fantacalcio_votes_provider", 1)); // array(0=>array(t_id, pt_ot, goals_ot), 1=>array(t_id, pt_ot, goals_ot))
                                                                                         
        // tempi supplementari
        $rows[$label][] = "<b>" . $overtime[0]['goals_ot'] . "</b> (" . $overtime[0]['pt_ot'] . ")";
        $rows[$label][] = "<b>" . $overtime[1]['goals_ot'] . "</b> (" . $overtime[1]['pt_ot'] . ")";
        
        // tempi supplementari in parità --> rigori
        if ($overtime[0]['goals_ot'] == $overtime[1]['goals_ot']) {
          $penalties = get_penalties($match); // ar(0=>ar(t_id, goals_penalties=>array(i, gol)),1=>ar(t_id,goals_penalties=>array(i=>gol)))
          $last_penalty_index = get_penalties_last_index($penalties);
          // rigori
          $goals_penalties_1 = get_goals_penalties($penalties[0]['goals_penalties'], $last_penalty_index);
          $goals_penalties_2 = get_goals_penalties($penalties[1]['goals_penalties'], $last_penalty_index);
          $rows[$label][] = "<b>" . $goals_penalties_1 . "/" . $last_penalty_index . "</b>";
          $rows[$label][] = "<b>" . $goals_penalties_2 . "/" . $last_penalty_index . "</b>";
          
          update_goals_penalties($match, $goals_penalties_1, $goals_penalties_2);
        }
        else {
          // rigori
          $rows[$label][] = "-";
          $rows[$label][] = "-";
        }
      }
      else {
        // tempi supplementari
        $rows[$label][] = "-";
        $rows[$label][] = "-";
        
        // rigori
        $rows[$label][] = "-";
        $rows[$label][] = "-";
      }
      
      // winner
      // //team 1
      if (array_sum($goals[$t1]) > array_sum($goals[$t2]))
        $next[$label] = $t1;
        
        // //team 2
      elseif (array_sum($goals[$t1]) < array_sum($goals[$t2]))
        $next[$label] = $t2;
        
        // //draw
      elseif (array_sum($goals[$t1]) == array_sum($goals[$t2])) {
        // winner is home team
        if ($goals[$t1]["a"] > $goals[$t2]["a"])
          $next[$label] = $t1;
          // winner is away team
        elseif ($goals[$t1]["a"] < $goals[$t2]["a"])
          $next[$label] = $t2;
          // draw ==> overtime
        elseif ($goals[$t1]["a"] == $goals[$t2]["a"])
          $next[$label] = get_winner_overtime($overtime, $penalties);
      }
      
      $rows[$label][] = ($next[$label] >= 0) ? array(
        "data" => $teams[$next[$label]]->name, 
        "style" => "font-weight:bold;") : array(
        "data" => "**", 
        "style" => "color: red;");
    }
    
    // inserisco il vincitore nella partita successiva
    foreach ($next as $label => $t_id) {
      if ($t_id >= 0) {
        $sql = "UPDATE {fanta_matches} SET t1_id = '%d' WHERE t1_label = '%s'";
        $result = db_query($sql, $t_id, $label);
        $sql = "UPDATE {fanta_matches} SET t2_id = '%d' WHERE t2_label = '%s'";
        $result = db_query($sql, $t_id, $label);
      }
    }
    return theme_table($header, $rows);
  }
  else
    return t("Passaggio turno non previsto in questa giornata");
}

function get_overtime($match, $votes_provider) {
  $vote_round = get_round_by_match(get_competition_group($match->g_id), $match->round);
  $votes = get_votes($vote_round);
  
  $players = get_players();
  
  // voti riserve non entrate
  $riserve_team_1 = get_riserve_overtime($match, $match->t1_id, $votes_provider);
  $riserve_team_2 = get_riserve_overtime($match, $match->t2_id, $votes_provider);
  
  // if ($match->t2_id == 7) {echo "<pre>"; print_r($riserve_team_1);die("a");}
  
  foreach ($riserve_team_1 as $pl_id => $player) {
    $tot_1 += $votes[$pl_id]->tot;
  }
  
  foreach ($riserve_team_2 as $pl_id => $player) {
    $tot_2 += $votes[$pl_id]->tot;
  }
  
  // 1° goal: 20pts; poi un goal ogni 4pts
  $goals_ot_1 = floor(($tot_1 - 16) / 4);
  $goals_ot_2 = floor(($tot_2 - 16) / 4);
  
  $goals_ot_1 = ($goals_ot_1 < 0) ? 0 : $goals_ot_1;
  $goals_ot_2 = ($goals_ot_2 < 0) ? 0 : $goals_ot_2;
  
  $sql = "UPDATE {fanta_matches} SET 
        pt_ot_1 = " . $tot_1 . ", 
        pt_ot_2 = " . $tot_2 . ", 
        goals_ot_1 = " . $goals_ot_1 . ", 
        goals_ot_2 = " . $goals_ot_2 . " 
        WHERE m_id = " . $match->m_id;
  db_query($sql);
  
  return array(
    0 => array(
      't_id' => $match->t1_id, 
      'pt_ot' => $tot_1, 
      'goals_ot' => $goals_ot_1), 
    1 => array(
      't_id' => $match->t1_id, 
      'pt_ot' => $tot_2, 
      'goals_ot' => $goals_ot_2));
}

function get_penalties($match) {
  // elenco rigoristi (in ordine)
  $penalties_players_1 = get_penalties_team($match, $match->t1_id);
  $penalties_players_2 = get_penalties_team($match, $match->t2_id);
  
  foreach ($penalties_players_1 as $penalty_position => $penalties_player) {
    $goals_penalties_1[$penalty_position] = array(
      "pl_id" => $penalties_player->pl_id, 
      'penalty' => $penalties_player->penalty);
  }
  foreach ($penalties_players_2 as $penalty_position => $penalties_player) {
    $goals_penalties_2[$penalty_position] = array(
      "pl_id" => $penalties_player->pl_id, 
      'penalty' => $penalties_player->penalty);
  }
  
  return array(
    0 => array('t_id' => $match->t1_id, 'goals_penalties' => $goals_penalties_1), 
    1 => array('t_id' => $match->t2_id, 'goals_penalties' => $goals_penalties_2));
}

function get_winner_overtime($overtime, $penalties) {
  if ($overtime[0]['goals_ot'] > $overtime[1]['goals_ot'])
    return $overtime[0]['t_id'];
  elseif ($overtime[1]['goals_ot'] > $overtime[0]['goals_ot'])
    return $overtime[1]['t_id'];
  if ($overtime[0]['goals_ot'] == $overtime[1]['goals_ot'])
    return get_winner_penalties($penalties);
}

function get_winner_penalties($penalties) {
  $goals_1 = 0;
  $goals_2 = 0;
  for ($i = 1; $i <= 5; $i++) {
    $goals_1 += $penalties[0]['goals_penalties'][$i]['penalty'];
    $goals_2 += $penalties[1]['goals_penalties'][$i]['penalty'];
  }
  
  if ($goals_1 > $goals_2)
    return $penalties[0]['t_id'];
  elseif ($goals_2 > $goals_1)
    return $penalties[1]['t_id'];
  elseif ($goals_2 == $goals_1)
    return get_winner_penalties_oltranza($penalties, 6);
}

function get_winner_penalties_oltranza($penalties, $start) {
  // rigori a oltranza
  if ($start == 18)
    return 0; // ferma la ricorsione
  
  if ($penalties[0]['goals_penalties'][$start]['penalty'] > $penalties[1]['goals_penalties'][$start]['penalty'])
    return $penalties[0]['t_id'];
  if ($penalties[1]['goals_penalties'][$start]['penalty'] > $penalties[0]['goals_penalties'][$start]['penalty'])
    return $penalties[1]['t_id'];
  if ($penalties[0]['goals_penalties'][$start]['penalty'] == $penalties[1]['goals_penalties'][$start]['penalty'])
    return get_winner_penalties_oltranza($penalties, ($start + 1));
}

function get_penalties_last_index($penalties) {
  $goals_1 = 0;
  $goals_2 = 0;
  for ($i = 1; $i <= 5; $i++) {
    $goals_1 += $penalties[0]['goals_penalties'][$i]['penalty'];
    $goals_2 += $penalties[1]['goals_penalties'][$i]['penalty'];
  }
  
  if ($goals_1 > $goals_2)
    return 5;
  elseif ($goals_2 > $goals_1)
    return 5;
  elseif ($goals_2 == $goals_1)
    return get_penalties_last_index_oltranza($penalties, 6);
}

function get_penalties_last_index_oltranza($penalties, $start) {
  // rigori a oltranza
  if ($start == 18)
    return 0; // ferma la ricorsione
  
  if ($penalties[0]['goals_penalties'][$start]['penalty'] > $penalties[1]['goals_penalties'][$start]['penalty'])
    return $start;
  if ($penalties[1]['goals_penalties'][$start]['penalty'] > $penalties[0]['goals_penalties'][$start]['penalty'])
    return $start;
  if ($penalties[0]['goals_penalties'][$start]['penalty'] == $penalties[1]['goals_penalties'][$start]['penalty'])
    return get_penalties_last_index_oltranza($penalties, ($start + 1));
}

function get_goals_penalties($penalties_team, $last_penalty_index) {
  $goals_penalties = 0;
  for ($i = 1; $i <= $last_penalty_index; $i++)
    $goals_penalties += $penalties_team[$i]['penalty'];
  
  return $goals_penalties;
}

function update_goals_penalties($match, $goals_penalties_1, $goals_penalties_2) {
  $sql = "UPDATE {fanta_matches} SET penalties_1 = " . $goals_penalties_1 . ", penalties_2 = " . $goals_penalties_2 . " 
      WHERE m_id = " . $match->m_id;
  $result = db_query($sql);
}

function get_riserve_overtime($match, $t_id, $votes_provider) {
  $roles = array(0, 0, 0);
  
  // riserve non entrate
  $sql = "SELECT f.pl_id, f.round, f.t_id, f.c_id, p.role FROM {fanta_lineups} f, {fanta_players} p, {fanta_votes} v
      WHERE f.pl_id = p.pl_id
      AND f.pl_id = v.pl_id
      AND f.c_id = '%d' 
      AND f.round = '%d' 
      AND f.t_id = '%d'
      AND f.position >= 2 
      AND f.has_played = 0 
      AND p.role != 0
      AND v.round = '%d'
	  AND v.provider = '%d'
      ORDER BY position";
  $result = db_query($sql, get_cid_by_gid($match->g_id), $match->round, $t_id, get_round_by_competition_round($match->round, get_cid_by_gid($match->g_id)), $votes_provider);
  while ($row = db_fetch_object($result)) {
    $roles[$row->role]++;
    if ($roles[$row->role] == 1) {
      $riserve[$row->pl_id] = $row;
      $sql_a = "UPDATE {fanta_lineups} SET has_played = 2 
            WHERE pl_id = '%d' 
            AND t_id = '%d' 
            AND round = '%d' 
            AND c_id = '%d'";
      $result_a = db_query($sql_a, $row->pl_id, $t_id, $row->round, $row->c_id);
    }
  }
  
  // riserve e voti
  return $riserve;
}

function get_penalties_team($match, $t_id, $votes_provider) {
  $vote_round = get_round_by_match(get_cid_by_gid($match->g_id), $match->round);
  $votes = get_votes($vote_round);
  $i = 1;
  // rigoristi ordinati
  $sql = "SELECT * FROM {fanta_lineups} f, {fanta_votes} v
      WHERE f.pl_id = v.pl_id
      AND f.c_id = '%d' 
      AND v.round = '%d'
      AND f.round = '%d' 
      AND f.t_id = '%d'
	  AND v.provider = '%d'
      ORDER BY penalty_position";
  $result = db_query($sql, get_cid_by_gid($match->g_id), $match->round, $match->round, $t_id, $votes_provider);
  while ($row = db_fetch_object($result)) {
    $penalties[$i] = $row;
    $penalties[$i]->penalty = ($row->vote >= 6) ? 1 : 0;
    $i++;
  }
  
  return $penalties;
}

function get_matches_vote_round($vote_round) {
  $matches = array();
  
  $sql = "SELECT * FROM {fanta_rounds_competitions} WHERE round = '%d'";
  $result = db_query($sql, $vote_round);
  while ($row = db_fetch_array($result)) {
    $c_id = $row['c_id'];
    $competition_round = $row['competition_round'];
    $matches_competitions = get_round_matches($competition_round, '', $c_id);
    $matches = array_merge($matches, $matches_competitions);
  }
  
  return $matches;
}

function get_matches_next($vote_round) {
  $matches = get_matches_vote_round($vote_round);
  
  $sql = "SELECT match_label, c.next
      FROM {fanta_matches} m, {fanta_rounds_competitions} c 
      WHERE m.round = c.competition_round 
      AND c.c_id = (SELECT c_id FROM {fanta_groups} WHERE g_id = m.g_id)
      AND c.round = '%d'";
  $result = db_query($sql, $vote_round);
  while ($row = db_fetch_object($result)) {
    if ($row->match_label != "" && $row->next == 1)
      $match_labels[] = "'" . $row->match_label . "'";
  }
  
  if ($match_labels) {
    $sql = "SELECT * FROM {fanta_matches} WHERE match_label IN (" . implode(",", $match_labels) . ")";
    $result = db_query($sql);
    while ($row = db_fetch_object($result)) {
      $matches_next[$row->match_label][$row->m_id] = $row;
    }
  }
  return $matches_next;
}

/**
 * *********************
 */
/* END passaggio turno */
/**
 * *********************
 */

/**
 * *********************
 */
/* news */
/**
 * *********************
 */
function fantacalcio_close_round($vote_round) {
  drupal_set_title(t('Risultati: Giornata #@round', array(
    "@round" => $vote_round)));
  
  Result::closeRound($vote_round);
  
  return "News creata";
}

/**
 * *********************
 */
/* END news */
/**
 * *********************
 */

/**
 * *********************
 */
/* newsletter */
/**
 * *********************
 */
function fantacalcio_send_newsletter($vote_round) {
  drupal_set_title(filter_xss('Risultati ' . $vote_round . '&ordf; giornata'));
  
  global $user;
  
  $sql = "SELECT * FROM {fanta_rounds_competitions} WHERE round = '%d' AND c_id IN (SELECT c_id FROM {fanta_competitions} WHERE newsletter = 1)";
  $result = db_query($sql, $vote_round);
  while ($row = db_fetch_object($result)) {
    $rounds[$row->c_id] = $row;
  }
  
  // newsletters
  // TODO //Administation
  $tids = array(1 => 13, 2 => 16);
  
  foreach ($rounds as $c_id => $round) {
    
    $tid = $tids[$c_id];
    
    $body = newsletter_create_body($c_id, $round->competition_round);
    $title = "Riepilogo " . (!empty($round->round_label) ? $round->round_label : $round->competition_round . "a Giornata");
    
    // create node
    $newsletter = array();
    $newsletter = (object) array(
      'title' => $title, 
      'body' => $body, 
      'type' => 'simplenews', 
      'uid' => $user->uid, 
      'language' => 'it');
    
    // taxonomy
    $newsletter->taxonomy[$tid] = taxonomy_get_term($tid);
    $newsletter->simplenews = array(
      'tid' => $tid, 
      's_format' => 'html', 
      'send' => SIMPLENEWS_COMMAND_SEND_NOW);
    
    node_save($newsletter);
    
    module_invoke('simplenews', 'simplenews_nodeapi', $newsletter, 'insert');
  }
  
  return "Newsletter inviate";
}

function newsletter_create_body($c_id, $round) {
  $body = competition_situation($c_id, $round);
  
  return $body;
}

function newsletter_create_title($tid, $round) {
  $sql = "SELECT name FROM {term_data} WHERE tid = '%d'";
  $result = db_query($sql, $tid);
  $title = "[" . db_result($result) . "] " . $round . "a Giornata";
  
  return $title;
}

function competition_situation($c_id, $round) {
  $sql = "SELECT * FROM {fanta_groups} 
                WHERE c_id = '%d' 
                AND active = 1 
                AND (newsletter_standings > 0 OR newsletter_matches > 0)";
  $result = db_query($sql, $c_id);
  while ($row = db_fetch_object($result)) {
    $groups[$row->g_id] = $row;
  }
  
  $out = "";
  
  if ($groups) {
    
    foreach ($groups as $g_id => $group) {
      
      $out .= "<h2>$group->name</h2>";
      
      if ($round > 0)
        $out .= "<h3>" . $round . "&ordf; giornata</h3>";
      
      if ($group->newsletter_matches > 0)
        $out .= home_last_matches($g_id, $round);
      if ($group->newsletter_standings > 0)
        $out .= home_standings($g_id);
      if ($group->newsletter_matches > 0)
        $out .= home_next_matches($g_id, $round);
      
      $out .= "<div class=\"clear\"></div>";
      $out .= "<hr>";
      $out .= "<div class=\"clear\"></div>";
    }
  }
  
  return $out;
}

/**
 * *********************
 */
/* END newsletter */
/**
 * *********************
 */

/**
 * *********************
 */
/* Sfide */
/**
 * *********************
 */
function fantacalcio_aggiorna_sfide($vote_round) {
  drupal_set_title(filter_xss('Risultati ' . $vote_round . '&ordf; giornata'));
  
  $sql = "UPDATE {fanta_sfide} SET status = 3 WHERE status = 1";
  $result = db_query($sql);
  
  $sql = "DELETE FROM {fanta_sfide} WHERE (status = 2 OR status = 0)";
  $result = db_query($sql);
  return "Sfide aggiornate";
}

/**
 * *********************
 */
/* END Sfide */
/**
 * *********************
 */
function fantacalcio_parse_voti($data) {
  $f = file_get_contents("./html_voti.txt");
  
  echo $f;
}

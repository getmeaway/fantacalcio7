<?php

/**
 * @file fantamercato.form.inc
 * functions for Fantamercato.
 *
 */
function group_buy_player_form($form, &$form_state, $g_id) {
  //$form['#theme'] = "group_buy_player";
  
  $teams = Team::allByGroup($g_id);
  $teams_options = array();
  foreach ($teams as $team)
    $teams_options[$team->id] = $team->name;
  
  $form['g_id'] = array('#type' => 'hidden', '#value' => $g_id);
  $form['player'] = array(
    '#type' => 'textfield', 
    '#size' => 50, 
    '#autocomplete_path' => 'mercato/autocomplete/group/' . $g_id, 
    '#size' => 40)
  // "#title" => t("Giocatore")
  ;
  $form['cost'] = array(
    '#type' => 'textfield', 
    '#size' => 5,
    "#autocomplete" => "off",
    "#default_value" => 1)
  // "#title" => t("Costo")
  
  ;
  $form['t_id'] = array('#type' => 'select', '#options' => $teams_options)
  // "#title" => t("Squadra")
  
  ;
  $form['submit'] = array(
    '#type' => 'submit', 
    '#title' => 'Compra', 
    '#value' => "Compra", 
    '#validate' => array('group_buy_player_form_validate'));
  
  return $form;
}

function group_buy_player_form_submit($form, $form_state) {
//   $sql = "INSERT INTO {fanta_squads} (pl_id, t_id, status, cost) VALUES ('%d', '%d', 1, '%d')";
//   $exploded_player = explode(',', $form_state['values']['player']);
//   $result = db_query($sql, $exploded_player[0], $form_state['values']['t_id'], $form_state['values']['cost']);
//   drupal_set_message('Acquisto effettuato');
  
  $exploded_player = explode(',', $form_state['values']['player']);
  
  db_insert("fanta_squads")->fields(array(
  "pl_id" => $exploded_player[0],
  "t_id" => $form_state['values']['t_id'],
  "cost" => $form_state['values']['cost'],
  "timestamp" => time(),
  "status" => 1))->execute();
  
  //$result = db_query($sql, $exploded_player[0], $form_state['input']['t_id'], $form_state['input']['cost']);
  drupal_set_message('Acquisto effettuato');
}

function group_buy_player_form_validate($form, $form_state) {
  $validation = buy_player_validate($form_state['values']['t_id'], $form_state['values']['player'], $form_state['values']['cost']);
  
  if ($validation == "cost")
    form_set_error("", t("Costo massimo superato. "));
  
  if ($validation == "players")
    form_set_error("", t("Numero massimo di giocatori superato. "));
  
  if ($validation == "role")
    form_set_error("", t("Numero massimo di giocatori superato per il ruolo. "));
}

function team_buy_player_form($form, &$form_state, $g_id, $t_id, $role) {
  // $form['#theme'] = "team_buy_player";
  $form['t_id'] = array(
    '#type' => 'hidden', 
    '#value' => $t_id, 
    "#prefix" => '<div style="float:left;">', 
    "#suffix" => '</div>');
  $form['player'] = array(
    '#type' => 'textfield', 
    '#autocomplete_path' => 'mercato/autocomplete/team/' . $g_id . "/" . $role, 
    '#size' => 30, 
    "#prefix" => '<div style="float:left;">', 
    "#suffix" => '</div>');
  $form['cost'] = array(
    '#type' => 'textfield', 
    '#size' => 3, 
    "#prefix" => '<div style="float:left;">', 
    "#suffix" => '</div>');
  $form['submit'] = array(
    '#type' => 'submit', 
    '#title' => 'Compra', 
    '#value' => "Compra", 
    '#validate' => array('team_buy_player_form_validate'), 
    "#attributes" => array("data-mini" => "true", "data-inline" => "true"));
  
  return $form;
}

function team_buy_player_form_submit($form, $form_state) {
  //$sql = "INSERT INTO {fanta_squads} (pl_id, t_id, status, cost) VALUES ('%d', '%d', 1, '%d')";
  $exploded_player = explode(',', $form_state['input']['player']);
  
  db_insert("fanta_squads")->fields(array(
    "pl_id" => $exploded_player[0], 
    "t_id" => $form_state['input']['t_id'], 
    "cost" => $form_state['input']['cost'], 
    "timestamp" => time(), 
    "status" => 1))->execute();
  
  //$result = db_query($sql, $exploded_player[0], $form_state['input']['t_id'], $form_state['input']['cost']);
  drupal_set_message('Acquisto effettuato');
}

function team_buy_player_form_validate($form, $form_state) {
  $validation = buy_player_validate($form_state['input']['t_id'], $form_state['input']['player'], $form_state['input']['cost']);
  
  if ($validation == "cost")
    form_set_error("", t("Costo massimo superato. "));
  
  if ($validation == "players")
    form_set_error("", t("Numero massimo di giocatori superato. "));
  
  if ($validation == "role")
    form_set_error("", t("Numero massimo di giocatori superato per il ruolo. "));
}

function buy_player_validate($t_id, $player, $cost) {
  $exploded_player = explode(',', $player);
  $player = Player::get($exploded_player[0]);
  $player_role = $player->role;
  $num_roles = array(0, 0, 0, 0);
  $max_roles = get_max_number_for_roles();
  $num_players = 0;
  $spesa = 0; 
  
  // controllo numero giocatori
  $sql = "SELECT * 
          FROM {fanta_squads} r, {fanta_players} p
          WHERE r.pl_id = p.pl_id
          AND r.t_id = '%d'";
  
  $query = db_select("fanta_squads", "s");
  $query->join("fanta_players", "p", "p.pl_id = s.pl_id");
  $query->condition("t_id", $t_id);
  $query->fields("s");
  $query->fields("p");
  
  $result = $query->execute();
  foreach ($result as $row) {
    if ($row->status == -1) {
      $spesa += floor($row->cost / 2);
    }
    else
      $spesa += $row->cost;
    
    if ($row->status == 1) {
      $num_players++;
      $num_roles[$row->role]++;
    }
  }
  
  //print_r($num_roles);die();
  
  $credits = Team::get($t_id)->credits;//variable_get('fantacalcio_credits', 350);
  $rimasti = $credits - $spesa + get_swaps_money($t_id);
  
  $spesa_max = $rimasti - variable_get('fantacalcio_number_players', 25) + $num_players + 1;
  
  if ($cost > $spesa_max)
    return "cost";
  
  if ($num_players >= array_sum($max_roles))
    return "players";
  
  if ($num_roles[$player_role] >= $max_roles[$player_role])
    return "role";
  
  return "ok";
}

function team_action_player_form($form, &$form_state, $t_id, $pl_id, $action) {
  $action_values = array(
    "restore" => t("Ripristina"), 
    "delete" => t("Annulla"), 
    "sell" => t("Vendi"), 
    "regain" => t("Recupera"));
  
  $form['t_id'] = array('#type' => 'hidden', '#value' => $t_id);
  $form['pl_id'] = array('#type' => 'hidden', '#value' => $pl_id);
  $form['action'] = array('#type' => 'hidden', '#value' => $action);
  
  $form['button'] = array(
    '#type' => 'submit', 
    '#attributes' => array("class" => array("btn-sm")),
    '#value' => $action_values[$action]);
  
  $form['#prefix'] = "<div style='float:left'>";
  $form['#suffix'] = "</div>";
  
  return $form;
}

function team_action_player_form_submit($form, &$form_state) {
//   print_r($form_state);die();
  switch ($form_state['input']['action']) {
    case 'restore' :
//       $sql = "UPDATE {fanta_squads} SET status = 1 WHERE pl_id = '%d' AND t_id = '%d'";
      db_update("fanta_squads")->fields(array("status" => 1))->condition("pl_id", $form_state['input']['pl_id'])->condition("t_id", $form_state['input']['t_id'])->execute();
      break;
    case 'sell' :
//       $sql = "UPDATE {fanta_squads} SET status = 0 WHERE pl_id = '%d' AND t_id = '%d'";
      db_update("fanta_squads")->fields(array("status" => 0))->condition("pl_id", $form_state['input']['pl_id'])->condition("t_id", $form_state['input']['t_id'])->execute();
      break;
    case 'regain' :
//       $sql = "UPDATE {fanta_squads} SET status = -1 WHERE pl_id = '%d' AND t_id = '%d'";
      db_update("fanta_squads")->fields(array("status" => -1))->condition("pl_id", $form_state['input']['pl_id'])->condition("t_id", $form_state['input']['t_id'])->execute();
      break;
    case 'delete' :
//       $sql = "DELETE FROM {fanta_squads} WHERE pl_id = '%d' AND t_id = '%d'";
      db_delete("fanta_squads")->condition("pl_id", $form_state['input']['pl_id'])->condition("t_id", $form_state['input']['t_id'])->execute();
      break;
  }
  
//   $result = $query->execute();//($sql, $form_state['input']['pl_id'], $form_state['input']['t_id']);
  
  drupal_set_message('Operazione effettuata');
}

function players_swap_form($form, &$form_state, $g_id) {
  $form['swap'] = array('#type' => 'fieldset', '#title' => t('Scambia giocatori'));
  $form['swap']['player_1'] = array(
    '#type' => 'textfield', 
    '#autocomplete_path' => 'mercato/autocomplete/swaps/' . $g_id, 
    '#size' => 50, 
    '#title' => 'Da');
  $form['swap']['player_2'] = array(
    '#type' => 'textfield', 
    '#autocomplete_path' => 'mercato/autocomplete/swaps/' . $g_id, 
    '#size' => 50, 
    '#title' => 'A');
  $form['swap']['submit'] = array(
    '#type' => 'submit', 
    '#title' => 'Scambia', 
    '#value' => "Scambia giocatori");
  
  return $form;
}

function players_swap_form_submit($form, $form_state) {
  $sql = "UPDATE {fanta_squads} SET pl_id = '%d' WHERE pl_id = '%d' AND t_id = '%d' AND status = 1";
  $exploded_player_1 = explode(',', $form_state['values']['player_1']);
  $exploded_player_2 = explode(',', $form_state['values']['player_2']);
  
  $pl1_id = $exploded_player_1[0];
  $pl2_id = $exploded_player_2[0];
  $t1_id = $exploded_player_1[1];
  $t2_id = $exploded_player_2[1];
  
  db_update("fanta_squads")->fields(array("pl_id" => $pl2_id))->condition("t_id", $t1_id)->condition("pl_id", $pl1_id)->execute();
  db_update("fanta_squads")->fields(array("pl_id" => $pl1_id))->condition("t_id", $t2_id)->condition("pl_id", $pl2_id)->execute();
  
  //db_query($sql, $exploded_player_2[0], $exploded_player_1[0], $exploded_player_1[1]);
  //db_query($sql, $exploded_player_1[0], $exploded_player_2[0], $exploded_player_2[1]);
  
  drupal_set_message(t('Scambio effettuato'));
}

function team_money_transfer_form($form, &$form_state, $g_id) {

$teams = Team::allByGroup($g_id);
  $teams_options = array();
  foreach ($teams as $team)
    $teams_options[$team->id] = $team->name;

  $form['transfer'] = array(
    '#type' => 'fieldset', 
    '#title' => t('Trasferisci soldi'));
  $form['transfer']['t1_id'] = array(
    '#type' => 'select', 
    '#options' => $teams_options, 
    '#title' => 'Da');
  $form['transfer']['t2_id'] = array(
    '#type' => 'select', 
    '#options' => $teams_options, 
    '#title' => 'A');
  $form['transfer']['money'] = array(
    '#type' => 'textfield', 
    '#title' => 'Importo', 
    '#size' => 3);
  $form['transfer']['submit'] = array(
    '#type' => 'submit', 
    '#title' => 'Trasferisci soldi', 
    '#value' => "Trasferisci", 
    '#validate' => array('team_money_transfer_form_validate'));
  
  return $form;
}

function team_money_transfer_form_submit($form, $form_state) {
  $sql = "INSERT INTO {fanta_squad_changes} (t1_id, t2_id, money, active) 
          VALUES ('%d', '%d', '%d', 1)";
//  $result = db_query($sql, $form_state['values']['t1_id'], $form_state['values']['t2_id'], $form_state['values']['money']);
  
  db_insert("fanta_squad_changes")->fields(array("t1_id" => $form_state['values']['t1_id'], "t2_id" => $form_state['values']['t2_id'], "money" => $form_state['values']['money'], "active" => 1))->execute();
  
  drupal_set_message(t('Trasferimento effettuato'));
}

function team_money_transfer_form_validate($form, $form_state) {
  // controllo spesa
  
  $spent = 0;
  $num_players = 0;
  
  $sql = "SELECT * 
          FROM {fanta_squads} r
          WHERE r.t_id = '%d'";
//  $result = db_query($sql, $form_state['values']['t1_id']);
  
  $query = db_select("fanta_squads", "s");
  $query->condition("t_id", $form_state['values']['t1_id']);
  $query->fields("s");
  
  $result = $query->execute();
  
  foreach ($result as $row) {
    if ($row->status == -1) {
      $spent += floor($row->cost / 2);
    }
    else
      $spent += $row->cost;
    
    if ($row->status == 1) {
      $num_players++;
      
    }
  }
  
  $credits = Team::get($form_state['values']['t1_id'])->credits;
  $available = $credits - $spent + get_swaps_money($form_state['values']['t1_id']);
  
  $max_offer = $available - variable_get('fantacalcio_num_rosa', 25) + $num_players + 1;
  
  if ($form_state['values']['money'] > $available)
    form_set_error("", "Crediti insufficienti. ");
}

function team_money_transfer_delete_form($form, &$form_state, $s_id) {
  $form['s_id'] = array('#type' => 'hidden', '#value' => $s_id);
  $form['submit'] = array(
    '#type' => 'submit', 
    '#title' => 'Annulla', 
    '#value' => "Annulla");
  
  return $form;
}

function team_money_transfer_delete_form_submit($form, $form_state) {
  $sql = "DELETE FROM {fanta_squad_changes} WHERE s_id = '%d'";
//  $result = db_query($sql, $form_state['values']['s_id']);
  
  db_delete("fanta_squad_changes")->condition("s_id", $form_state['values']['s_id'])->execute();
  
  drupal_set_message('Trasferimento annullato');
}

function theme_group_buy_player($variables) {
  $form = $variables["form"];
  
  drupal_add_js('sites/all/modules/fantamercato/js/mercato.js', 'module');
  
  $rows[0] = array(t("Giocatore"), t("Costo"), t("Squadra"), "");
  $rows[1] = array(
    array("data" => drupal_render($form["player"]), "width" => 600), 
    array("data" => drupal_render($form["cost"])), 
    array("data" => drupal_render($form["t_id"])), 
    array("data" => drupal_render_children($form), "colspan" => 2));
  
  return theme("table", array("header" => array(), "rows" => $rows));
}

function theme_team_buy_player($variables) {
  $form = $variables["form"];
  
  $row = array(
//     drupal_render($form["player"]), 
//     drupal_render($form["cost"]), 
//     drupal_render_children($form)
      
  );
  
  return $row;
  
  // return theme("table", array("header" => array(), "rows" => $rows));
}

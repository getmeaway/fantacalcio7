<?php

/**
 * @file fantamercato.form.inc
 * functions for Fantamercato.
 *
 */

function group_buy_player_form($form_state, $g_id) {

  $form['#theme'] = "group_buy_player";

  $form['g_id'] = array('#type' => 'hidden', '#value' => $g_id);
  $form['player'] = array('#type' => 'textfield', '#size' => 50, '#autocomplete_path' => 'mercato/autocomplete/group/' . $g_id, '#size' => 40);
  $form['cost'] = array('#type' => 'textfield', '#size' => 5, "#attributes" => array("class" => "mobile-hidden"), "#default_value" => 1);
  $form['t_id'] = array('#type' => 'select', '#options' => get_teams_group_options($g_id));
  $form['submit'] = array('#type' => 'submit', '#title' => 'Compra', '#value' => "Compra", '#validate' => array('group_buy_player_form_validate'));

  return $form;
}

function group_buy_player_form_submit($form, $form_state) {
  $sql = "INSERT INTO {fanta_squads} (pl_id, t_id, in_team, cost) VALUES ('%d', '%d', 1, '%d')";
  $exploded_player = explode(',', $form_state['values']['player']);
  $result = db_query($sql, $exploded_player[0], $form_state['values']['t_id'], $form_state['values']['cost']);
  drupal_set_message('Acquisto effettuato');
}

function group_buy_player_form_validate($form, $form_state) {
  buy_player_validate($form_state['values']['t_id'], $form_state['values']['player'], $form_state['values']['cost']);
}

function team_buy_player_form($form_state, $g_id, $t_id, $role) {
  $form['t_id'] = array('#type' => 'hidden', '#value' => $t_id, "#prefix" => '<div style="float:left;">', "#suffix" => '</div>');
  $form['player'] = array('#type' => 'textfield', '#autocomplete_path' => 'mercato/autocomplete/team/' .$g_id . "/" . $role, '#size' => 30, "#prefix" => '<div style="float:left;">', "#suffix" => '</div>');
  $form['cost'] = array('#type' => 'textfield', '#size' => 3, "#prefix" => '<div style="float:left;">', "#suffix" => '</div>');
  $form['submit'] = array('#type' => 'submit', '#title' => 'Compra', '#value' => "Compra", '#validate' => array('team_buy_player_form_validate'), "#attributes" => array("data-mini" => "true", "data-inline" => "true"));

  return $form;
}

function team_buy_player_form_submit($form, $form_state) {
  $sql = "INSERT INTO {fanta_squads} (pl_id, t_id, in_team, cost) VALUES ('%d', '%d', 1, '%d')";
  $exploded_player = explode(',', $form_state['clicked_button']['#post']['player']);

  $result = db_query($sql, $exploded_player[0], $form_state['clicked_button']['#post']['t_id'], $form_state['clicked_button']['#post']['cost']);
  drupal_set_message('Acquisto effettuato');
}

function team_buy_player_form_validate($form, $form_state) {
  buy_player_validate($form_state['clicked_button']['#post']['t_id'], $form_state['clicked_button']['#post']['player'], $form_state['clicked_button']['#post']['cost']);
}

function buy_player_validate($t_id, $player, $cost) {
  $exploded_player = explode(',', $player);
  $players = Player::all();
  $player_role = $players[$exploded_player[0]]->role;
  $num_roles = array(0, 0, 0, 0);
  $max_roles = get_max_number_for_roles();
  $num_players = 0;

  #controllo numero giocatori
  $sql = "SELECT * 
          FROM {fanta_squads} r, {fanta_players} p
          WHERE r.pl_id = p.pl_id
          AND r.t_id = '%d'";
  $result = db_query($sql, $t_id);
  while($row = db_fetch_object($result)){
    if ($row->in_team == -1) {
      $spesa += floor($row->cost / 2);
    }
    else $spesa += $row->cost;
		
    if ($row->in_team == 1) {
      $num_players++;
      $num_roles[$row->role]++;
    }    
  }

  $crediti = variable_get('fantacalcio_credits', 350);
  $rimasti = $crediti - $spesa + get_swaps_money($t_id);

  $spesa_max = $rimasti - variable_get('fantacalcio_num_rosa', 25) + $num_players + 1;

  if($cost > $spesa_max)
    form_set_error("", "cost massimo superato. ");	

  if($num_players == array_sum($max_roles)) 
    form_set_error("", "Numero massimo di giocatori superato. ");

  if($num_roles[$player_role] == $max_roles[$player_role]) 
    form_set_error("", "Numero massimo di giocatori superato per il ruolo. ");	
}

function team_action_player_form($form_state, $t_id, $pl_id, $action) {
  $action_values = array("restore" => t("Ripristina"), "delete" => t("Annulla"), "sell" => t("Vendi"), "regain" => t("Recupera"));

  $form['t_id'] = array( '#type' => 'hidden', '#value' => $t_id);
  $form['pl_id'] = array( '#type' => 'hidden', '#value' => $pl_id);
  $form['action']  = array( '#type' => 'hidden', '#value' => $action);

  $form['button'] = array('#type' => 'submit', '#value' => $action_values[$action]);

  $form['#prefix'] = "<div style='float:left'>";
  $form['#suffix'] = "</div>";

  return $form;
}

function team_action_player_form_submit($form, $form_state) {
  switch($form_state['clicked_button']['#post']['action']) {
    case 'restore': 
      $sql = "UPDATE {fanta_squads} SET in_team = 1 WHERE pl_id = '%d' AND t_id = '%d'";
      break;
    case 'sell': 
      $sql = "UPDATE {fanta_squads} SET in_team = 0 WHERE pl_id = '%d' AND t_id = '%d'";
      break;
    case 'regain': 
      $sql = "UPDATE {fanta_squads} SET in_team = -1 WHERE pl_id = '%d' AND t_id = '%d'";
      break;
    case 'delete': 
      $sql = "DELETE FROM {fanta_squads} WHERE pl_id = '%d' AND t_id = '%d'";
      break;
  }

  $result = db_query($sql, $form_state['clicked_button']['#post']['pl_id'], $form_state['clicked_button']['#post']['t_id']);

  drupal_set_message('Operazione effettuata');
}

function players_swap_form($form_state, $g_id) {
  $form['swap'] = array('#type' => 'fieldset', '#title' => 'Scambia giocatori');
  $form['swap']['player_1'] = array('#type' => 'textfield', '#autocomplete_path' => 'mercato/autocomplete/swaps/' . $g_id, '#size' => 50, '#title' => 'Da');
  $form['swap']['player_2'] = array('#type' => 'textfield', '#autocomplete_path' => 'mercato/autocomplete/swaps/' . $g_id, '#size' => 50, '#title' => 'A');
  $form['swap']['submit'] = array('#type' => 'submit', '#title' => 'Scambia', '#value' => "Scambia giocatori");

  return $form;
}

function players_swap_form_submit($form, $form_state) {
  $sql = "UPDATE {fanta_squads} SET pl_id = '%d' WHERE pl_id = '%d' AND t_id = '%d' AND in_team = 1";
  $exploded_player_1 = explode(',', $form_state['values']['player_1']);
  $exploded_player_2 = explode(',', $form_state['values']['player_2']);
  db_query($sql, $exploded_player_2[0], $exploded_player_1[0], $exploded_player_1[1]);
  db_query($sql, $exploded_player_1[0], $exploded_player_2[0], $exploded_player_2[1]);

  drupal_set_message('Scambio effettuato');
}

function team_money_transfer_form($form_state, $g_id) {
  $form['transfer'] = array('#type' => 'fieldset', '#title' => 'Trasferisci soldi');
  $form['transfer']['t1_id'] = array('#type' => 'select', '#options' => get_teams_group_options($g_id), '#title' => 'Da');
  $form['transfer']['t2_id'] = array('#type' => 'select', '#options' => get_teams_group_options($g_id), '#title' => 'A');
  $form['transfer']['money'] = array('#type' => 'textfield', '#title' => 'Importo', '#size' => 3);
  $form['transfer']['submit'] = array('#type' => 'submit', '#title' => 'Trasferisci soldi', '#value' => "Trasferisci", '#validate' => array('team_money_transfer_form_validate'));

  return $form;
}

function team_money_transfer_form_submit($form, $form_state) {
  $sql = "INSERT INTO {fanta_squad_changes} (t1_id, t2_id, money, active) 
          VALUES ('%d', '%d', '%d', 1)";
  $result = db_query($sql, $form_state['values']['t1_id'], $form_state['values']['t2_id'], $form_state['values']['money'] );

  drupal_set_message('Trasferimento effettuato');
}

function team_money_transfer_form_validate($form, $form_state) {
  #controllo spesa
  $sql = "SELECT * 
          FROM {fanta_squads} r
          WHERE r.t_id = '%d'";
  $result = db_query($sql, $form_state['values']['t1_id']);
  while($row = db_fetch_object($result)){
    if ($row->in_team == -1) {
      $spesa += floor($row->cost / 2);
    }
    else $spesa += $row->cost;
		
    if ($row->in_team == 1) {
      $num_players++;
      $num_roles[$row->role]++;
    }    
  }

  $crediti = variable_get('fantacalcio_credits', 350);
  $rimasti = $crediti - $spesa + get_swaps_money($t_id);

  $spesa_max = $rimasti - variable_get('fantacalcio_num_rosa', 25) + $num_players + 1;

  if($form_state['values']['money'] > $rimasti)
    form_set_error("", "Crediti insufficienti. ");
}

function team_money_transfer_delete_form($form_state, $s_id) {
  $form['s_id'] = array('#type' => 'hidden', '#value' => $s_id);
  $form['submit'] = array('#type' => 'submit', '#title' => 'Annulla', '#value' => "Annulla");

  return $form;
}

function team_money_transfer_delete_form_submit($form, $form_state) {
  $sql = "DELETE FROM {fanta_squad_changes} WHERE s_id = '%d'";
  $result = db_query($sql, $form_state['values']['s_id'] );

  drupal_set_message('Trasferimento annullato');
}


function theme_group_buy_player($form) {
  
  drupal_add_js('sites/all/modules/fantamercato/js/mercato.js', 'module');
  
  $rows[0] = array(t("Giocatore"), drupal_render($form["player"]) );
  $rows[1] = array(t("cost"), (is_mobile() ? "<input data-theme='a' type='range' name='range_cost' id='range_cost' value='1' min='1' max='" . variable_get('fantacalcio_credits', 350) . "'><input type='button' value='+' id='cost_plus' data-mini='true' data-inline='true'><input type='button' value='-' id='cost_minus' data-mini='true' data-inline='true'>" : "") . drupal_render($form["cost"]));
  $rows[2] = array(t("Squadra"), drupal_render($form["t_id"]));
  $rows[3][] = array("data" => drupal_render($form), "colspan" => 2);
        
  return theme_table(array(), $rows);
  
}

function theme_team_buy_player($form) {
  
  $rows[] = array(  
          drupal_render($form["player"]),
          drupal_render($form["cost"]),
          drupal_render($form)
        );  

  return theme_table(array(), $rows);
}

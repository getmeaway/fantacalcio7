	<?php

/**
 * @file fantamercato.inc
 * functions for Fantamercato.
 *
 */
	
	require_once("fantacalcio.draft.autocomplete.inc");
	require_once("fantacalcio.draft.form.inc");

function draft_is_enabled() {
  if (variable_get('fantacalcio_enable_draft', '1') == 1)
    return TRUE;
  else
    return FALSE;
}

function fantamercato_group($g_id = '') {
  if ($g_id == '' || !Group::exists($g_id))
    return fantamercato_choose_group('group');
  else {
    return fantamercato_show_group($g_id);
  }
}

function fantamercato_team($g_id = '', $t_id = '') {
  if ($g_id == '' || !Group::exists($g_id))
    return fantamercato_choose_group('group');
  else {
    if ($t_id == '' || !Team::exists($t_id))
      return fantamercato_choose_team($g_id, 'team');
    else
      return fantamercato_show_team($g_id, $t_id);
  }
}

function fantamercato_swaps($g_id = '') {
  if ($g_id == '' || !Group::exists($g_id))
    return fantamercato_choose_group('group');
  else {
    return fantamercato_swaps_group($g_id);
  }
}

function fantamercato_view($g_id = '') {
  if ($g_id == '' || !Group::exists($g_id))
    return fantamercato_choose_group('group');
  else {
    return fantamercato_view_group($g_id);
  }
}

function fantamercato_choose_group($link) {
  if (draft_is_enabled()) {
    $groups = get_groups(variable_get('fantacalcio_main_competitions', 1));
    foreach ($groups as $g_id => $group) {
      $items[] = l($group->name, 'mercato/' . $link . '/' . $g_id, array("attributes" =>  array("data-ajax" => "false")) );
    }

    return theme_item_list($items, NULL, "ul", array("class" => "float"));
  }
  else
    return t("Mercato disabilitato");
}

function fantamercato_choose_team($g_id, $link) {
  $teams = get_group_teams($g_id);
  foreach ($teams as $t_id => $team) {
    $items[] = l($team->name, 'mercato/' . $g_id . '/' . $t_id);
  }
  return theme_item_list($items);
}

function fantamercato_get_teams_options() {
  $sql = "SELECT * FROM {fanta_teams} WHERE active = 1 ORDER BY name";
  $result = db_query($sql);
  while ($row = db_fetch_array($result)) {
    $teams[$row['t_id']] = $row['name'];
  }

  return $teams;
}

function get_group_teams_list($g_id) {
  $teams = get_group_teams($g_id);

  return implode(",", array_keys($teams));
}

function group_summary($g_id) {
  $players = get_all_players();

  $header = array("Squadra", "N° Gioc", "Spesa", "Crediti", "Max", "P", "D", "C", "A");

  $teams = get_group_teams($g_id);
	
  foreach ($teams as $t_id => $team) {
    $spesa = 0;$n_giocatori = 0; $n_roles = array(0,0,0,0);
		
    $sql = "SELECT * FROM {fanta_squads} WHERE t_id = '%d'";
    $result = db_query($sql, $t_id);
    while($row = db_fetch_object($result)){
      if ($row->in_team == -1) {
        $spesa += floor($row->cost / 2);
      }
      else $spesa += $row->cost;
			
      if ($row->in_team == 1) {
        $n_giocatori++;
        $n_roles[$players[$row->pl_id]->role]++;
      }
    }

    $crediti = variable_get('fantacalcio_credits', 350);
    $rimasti = $crediti - $spesa + get_swaps_money($t_id);

    $spesa_max = $rimasti - variable_get('fantacalcio_num_rosa', 25) + $n_giocatori + 1;

    $row_class = ($n_giocatori == variable_get('fantacalcio_num_rosa', 25)) ? "finished": "";

    $row_data = array();
    $row_data[] = array("data" => l($team->name, "mercato/team/" . $g_id . "/" .$t_id));
    $row_data[] = array("data" => $n_giocatori, "class" => "num");
    $row_data[] = array("data" => $spesa, "class" => "num");
    $row_data[] = array("data" => $rimasti, "class" => "num");
    $row_data[] = array("data" => (($n_giocatori == variable_get('fantacalcio_num_rosa', 25)) ? "-": $spesa_max), "class" => "num max");
    $row_data[] = array("data" => $n_roles[0] ." / " . 3, "class" => get_riepilogo_class($n_roles[0],3));
    $row_data[] = array("data" => $n_roles[1] ." / " . 8, "class" => get_riepilogo_class($n_roles[1],8));
    $row_data[] = array("data" => $n_roles[2] ." / " . 8, "class" => get_riepilogo_class($n_roles[2],8));
    $row_data[] = array("data" => $n_roles[3] ." / " . 6, "class" => get_riepilogo_class($n_roles[3],6));

    $rows[$t_id] = array("class" => $row_class, 'data' => $row_data);
  }

  return theme_table($header, $rows);
}

function team_players_list($g_id, $t_id) {
  $players = get_all_players();
  $real_teams = get_real_teams();

  $header = array("Ruolo", "Giocatore", "Squadra", "Costo", "", "");

  $spesa = 0; 
  $n_giocatori = 0; 
  $n_roles = array(0, 0, 0, 0);
  $max_roles = get_max_number_for_roles();
		
  $sql = "SELECT * 
          FROM {fanta_squads} r 
          INNER JOIN {fanta_players} p 
          ON r.pl_id = p.pl_id
          INNER JOIN {fanta_players_teams} t 
          ON p.pl_id = t.pl_id
          WHERE r.t_id = '%d' 
          AND r.in_team = 1
          AND t.round = '%d'";
  $result = db_query($sql, $t_id, get_last_votes(variable_get('fantacalcio_main_competition', 1)) );
  while($row = db_fetch_object($result)){
    $team_players[$row->role][$row->pl_id] = $row;
  }

  //ciclo per ogni ruolo
  for($i = 0; $i < 4; $i++) {
    if ($team_players[$i]) {
      foreach($team_players[$i] as $pl_id => $player) {
        $data = array();
        $data[] = array("data" => convert_role($player->role), "class" => "role");
        $data[] = array("data" => $player->name);
        $data[] = array("data" => ucwords($real_teams[$player->rt_id]->long_name));
        $data[] = array("data" => $player->cost, "class" => "num");
        $data[] = drupal_get_form('team_action_player_form', $t_id, $pl_id, 'sell') . drupal_get_form('team_action_player_form', $t_id, $pl_id, 'regain') . drupal_get_form('team_action_player_form', $t_id, $pl_id, 'delete');
        $data[] = l("<img src='" . base_path() . drupal_get_path("module", "fantacalcio") . "/images/stats.png'>", 
                  "scheda/giocatore/" . $pl_id, 
                  array("html" => "true", "attributes" => array("class" => "thickbox", "rel" => "lightframe")));


        $rows[$i . "_" . $pl_id] = array("data" => $data, "class" => "role_" . $i);
      }
    }
    for ($j = count($team_players[$i]); $j < $max_roles[$i]; $j++) {
      $data = array();
      $data[] = array("data" => convert_role($i), "class" => "role");
      $data[] = array("data" => drupal_get_form('team_buy_player_form', $g_id, $t_id, $i, $j), "colspan" => 5);
      $rows[$i . "__" . $j] = array("data" => $data, "class" => "role_" . $i);
    }
  }

  return theme_table($header, $rows);
}

function team_summary($g_id, $t_id){
  $players = get_all_players();

  $spesa = 0; $n_giocatori = 0; $n_roles = array(0, 0, 0, 0);
  $sql = "SELECT * FROM {fanta_squads} WHERE t_id = '%d'";
  $result = db_query($sql, $t_id);
  while($row = db_fetch_object($result)){
    if($row->in_team == -1) {
      $spesa += floor($row->cost / 2);
    }
    else $spesa += $row->cost;
		
    if ($row->in_team == 1) {
      $n_giocatori++;
      $n_roles[$players[$row->pl_id]->role]++;
    }
  }
	
  $crediti = variable_get('fantacalcio_credits', 350);
  $rimasti = $crediti - $spesa + get_swaps_money($t_id);

  $spesa_max = (variable_get('fantacalcio_num_rosa', 25) == $n_giocatori) ? "-" : ($rimasti - variable_get('fantacalcio_num_rosa', 25) + $n_giocatori + 1);

  $rows[] = array(array("data" => "N&deg; giocatori", "class" => "label"), array("data" => $n_giocatori, "class" => "num"), array("data" => "Portieri", "class" => "label"), array("data" => $n_roles[0], "class" => "num"));
  $rows[] = array(array("data" => "Crediti spesi", "class" => "label"), array("data" => $spesa, "class" => "num"), array("data" => "Difensori", "class" => "label"), array("data" => $n_roles[1], "class" => "num"));
  $rows[] = array(array("data" => "Crediti rimasti", "class" => "label"), array("data" => $rimasti, "class" => "num imp"), array("data" => "Centrocampisti", "class" => "label"), array("data" => $n_roles[2], "class" => "num"));
  $rows[] = array(array("data" => "Costo massimo", "class" => "label"), array("data" => $spesa_max, "class" => "num max"), array("data" => "Attaccanti", "class" => "label"), array("data" => $n_roles[3], "class" => "num"));

  return theme_table(array(), $rows);
}

function team_ex_players($g_id, $t_id){
  $players = get_all_players();
  $real_teams = get_real_teams();
  $team_ex_players = array();
	
  $sql = "SELECT * 
          FROM {fanta_squads} r 
          INNER JOIN {fanta_players} p 
          ON r.pl_id = p.pl_id
          INNER JOIN {fanta_players_teams} t 
          ON p.pl_id = t.pl_id
          WHERE r.t_id = '%d' 
          AND r.in_team != 1
          AND t.round = '%d'";
  $result = db_query($sql, $t_id, get_next_round_formazione(variable_get('fantacalcio_main_competition', 1)));
  while($row = db_fetch_object($result)){
    $team_ex_players[$row->pl_id] = $row;
  }

  if ($team_ex_players) {
    foreach ($team_ex_players as $pl_id => $player) {
      $data = array();
      $data[] = array("data" => convert_role($player->role), "class" => "role");
      $data[] = array("data" => $player->name);
      $data[] = array("data" => ucwords($real_teams[$player->rt_id]->long_name));
      $data[] = array("data" => $player->cost, "class" => "num");
      $data[] = drupal_get_form('team_action_player_form', $t_id, $pl_id, 'restore');
      $data[] = l("<img src='" . base_path() . drupal_get_path("module", "fantacalcio") . "/images/stats.png'>", 
                  "scheda/giocatore/" . $pl_id, 
                  array("html" => "true", "attributes" => array("class" => "thickbox", "rel" => "lightframe")));

      $rows[] = array("data" => $data, "class" => "role_" . $player->role);
    }
    return "<h3>Ex</h3>" . theme_table(array(), $rows);
  }

  return "";
}


function swaps_summary($g_id) {
  $teams = get_teams();

  $header = array("Da", "A", "Importo", "");

  $sql = "SELECT * FROM {fanta_squad_changes} 
          WHERE t1_id IN (SELECT t_id FROM {fanta_teams_groups} WHERE g_id = '%d')
          AND t2_id IN (SELECT t_id FROM {fanta_teams_groups} WHERE g_id = '%d')";
  $result = db_query($sql, $g_id, $g_id);
  while ($row = db_fetch_object($result) ) {
    $rows[] = array($teams[$row->t1_id]->name, $teams[$row->t2_id]->name, $row->money, drupal_get_form("team_money_transfer_delete_form", $row->s_id));
  }
 
  return theme_table($header, $rows);
}

function get_last_boughts($g_id, $n) {
  $players = get_all_players();
  $real_teams = get_real_teams();
  $bought_players = array();
  $teams = get_teams();
  $teams_list = get_group_teams_list($g_id);
	
  $sql = "SELECT * 
          FROM {fanta_squads} r 
          INNER JOIN {fanta_players} p 
          ON r.pl_id = p.pl_id
          INNER JOIN {fanta_players_teams} t 
          ON p.pl_id = t.pl_id
          WHERE r.in_team = 1
          AND r.t_id IN (" . $teams_list . ")
          AND t.round = '%d' 
          ORDER BY r.r_id DESC";
  $result = db_query_range($sql, get_next_round_formazione(variable_get('fantacalcio_main_competition', 1)), 0, $n);
  while($row = db_fetch_object($result)){
    $bought_players[$row->pl_id] = $row;
  }

  if ($bought_players) {
    foreach ($bought_players as $pl_id => $player) {
      $data = array();
      $data[] = array("data" => convert_role($player->role), "class" => "role");
      $data[] = array("data" => $player->name . " (" . ucwords($real_teams[$player->rt_id]->long_name) . ")");
      $data[] = array("data" => $teams[$player->t_id]->name);
      $data[] = array("data" => $player->cost, "class" => "num");
      $data[] = drupal_get_form('team_action_player_form', $player->t_id, $player->pl_id, 'delete');

      $rows[] = array("data" => $data, "class" => "role_" . $player->role);
    }
    return "<h3>Ultimi acquisti</h3>" . theme_table(array("Ruolo", "Giocatore", "Squadra", "Costo", ""), $rows);
  }

  return "";
}

function get_riepilogo_class($n, $limit){
  if ($n < $lim)
    return "";
  elseif ($n == $limit)
    return "stop";
  elseif ($n > $limit)
    return "alert";
}

function get_swaps_money($t_id){
  $money = 0;
  $sql = "SELECT * FROM {fanta_squad_changes} WHERE (t1_id = '%d' OR t2_id = '%d') ";
  $result = db_query($sql, $t_id, $t_id);
  while($row = db_fetch_object($result)){
    if ($t_id == $row->t1_id) $money -= $row->money;
    elseif ($t_id == $row->t2_id) $money += $row->money;
  }

  return $money;
}

function get_max_number_for_roles() {
  for($i = 0; $i < 4; $i++)
    $max[$i] = variable_get("fantacalcio_number_role_" . $i, 0);

  return $max;
}

function fantamercato_links($g_id) {
  $items[] = l("Riepilogo", "mercato/group/" . $g_id);
  $items[] = l("Scambi", "mercato/swaps/" . $g_id);
  $items[] = l("Visualizzazione", "mercato/view/" . $g_id);

  return theme_item_list($items) . "<div class=clear></div><br>";
}

/* funzioni per la gestione del mercato - Girone */
function fantamercato_show_group($g_id) {
  if (draft_is_enabled()) {
    drupal_set_title(filter_xss("Mercato - " .  Group::get($g_id)->name . " - Riepilogo"));
    return //fantamercato_links($g_id)
		drupal_get_form('group_buy_player_form', $g_id)
      . "<br>" . group_summary($g_id)
      . "<br>" . get_last_boughts($g_id, 5);
  }
  else
    return t("Mercato disabilitato");
}
/* END- funzioni per la gestione del mercato - Girone */


/* funzioni per la gestione del mercato - Squadra */
function fantamercato_show_team($g_id, $t_id) {
  if (draft_is_enabled()) {
    drupal_set_title(filter_xss("Mercato - " . Group::get($g_id)->name . " - " . get_team_name($t_id)));
    return fantamercato_links($g_id)
      . "<br>" . team_summary($g_id, $t_id)
      . "<br>" . team_players_list($g_id, $t_id)
      . "<br>" . team_ex_players($g_id, $t_id);
  }
  else
    return t("Mercato disabilitato");
}
/* END- funzioni per la gestione del mercato - Squadra */


/* funzioni per la visualizzazione del mercato - Girone */
function fantamercato_view_group($g_id) {
  if (draft_is_enabled()) {
    drupal_set_title(filter_xss("Mercato - " . Group::get($g_id)->name . " - Visualizzazione"));
    return fantamercato_links($g_id).group_summary($g_id);
  }
  else
    return t("Mercato disabilitato");
}
/* END- funzioni per la visualizzazione del mercato - Girone */


/* funzioni per gli scambi del mercato - Girone */
function fantamercato_swaps_group($g_id) {
  if (draft_is_enabled()) {
    drupal_set_title(filter_xss("Mercato - " .  Group::get($g_id)->name . " - Scambi"));
    return fantamercato_links($g_id)
    . drupal_get_form('players_swap_form', $g_id) 
    . "<br>" 
    . drupal_get_form('team_money_transfer_form', $g_id)
    . "<br>"
    . swaps_summary($g_id) ;
  }
  else
    return t("Mercato disabilitato");
}
/* END- funzioni per gli scambi del mercato - Girone */

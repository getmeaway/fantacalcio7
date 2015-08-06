	<?php

/**
 * @file fantamercato.inc
 * functions for Fantamercato.
 *
 */
require_once ("fantacalcio.draft.autocomplete.inc");
require_once ("fantacalcio.draft.form.inc");

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
    $groups = Group::allByCompetition(Competition::getDefault()->id);
    foreach ($groups as $g_id => $group) {
      $items[] = l($group->name, 'mercato/' . $link . '/' . $g_id, array(
        "attributes" => array("data-ajax" => "false")));
    }
    
    return theme("item_list", array("items" => $items, "type" => "ul", "attributes" => array("class" => array("float"))));
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
  
  $header = array(
    t("Squadra"), 
    t("N° Gioc"), 
    t("Spesa"), 
    t("Crediti"), 
    t("Max"), 
    t("P"), 
    t("D"), 
    t("C"), 
    t("A"));
  
  $teams = Team::allByGroup($g_id);
  
  foreach ($teams as $t_id => $team) {
    $spesa = 0;
    $n_giocatori = 0;
    $n_roles = array(0, 0, 0, 0);
    
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
        $n_giocatori++;
        $n_roles[$row->role]++;
      }
    }
    
    $crediti = variable_get('fantacalcio_credits', 350);
    $rimasti = $crediti - $spesa + get_swaps_money($t_id);
    
    $spesa_max = $rimasti - variable_get('fantacalcio_num_rosa', 25) + $n_giocatori + 1;
    
    $row_class = ($n_giocatori == variable_get('fantacalcio_num_rosa', 25)) ? "finished" : "";
    
    $row_data = array();
    $row_data[] = array(
      "data" => l($team->name, "mercato/team/" . $g_id . "/" . $t_id));
    $row_data[] = array("data" => $n_giocatori, "class" => "num");
    $row_data[] = array("data" => $spesa, "class" => "num");
    $row_data[] = array("data" => $rimasti, "class" => "num");
    $row_data[] = array(
      "data" => (($n_giocatori == variable_get('fantacalcio_number_players', 25)) ? "-" : $spesa_max), 
      "class" => "num max");
    $row_data[] = array(
      "data" => $n_roles[0] . " / " . variable_get('fantacalcio_number_role_0', 25), 
      "class" => role_number_get_class($n_roles[0], variable_get('fantacalcio_number_role_0', 25)));
    $row_data[] = array(
      "data" => $n_roles[1] . " / " . variable_get('fantacalcio_number_role_1', 25), 
      "class" => role_number_get_class($n_roles[1], variable_get('fantacalcio_number_role_1', 25)));
    $row_data[] = array(
      "data" => $n_roles[2] . " / " . variable_get('fantacalcio_number_role_2', 25), 
      "class" => role_number_get_class($n_roles[2], variable_get('fantacalcio_number_role_2', 25)));
    $row_data[] = array(
      "data" => $n_roles[3] . " / " . variable_get('fantacalcio_number_role_3', 25), 
      "class" => role_number_get_class($n_roles[3], variable_get('fantacalcio_number_role_3', 25)));
    
    $rows[$t_id] = array("class" => $row_class, 'data' => $row_data);
  }
  
  return theme("table", array("header" => $header, "rows" => $rows, "attributes" => array("class" => array("table", "table-responsive"))));
}

function team_players_list($g_id, $t_id) {
  //$players = get_all_players();
  //$real_teams = get_real_teams();
  
  $header = array(t("Ruolo"), t("Giocatore"), t("Squadra"), t("Costo"), "");
  
  $spesa = 0;
  $n_giocatori = 0;
  $n_roles = array(0, 0, 0, 0);
  $max_roles = get_max_number_for_roles();
  $team_players = array();
  
  $query = db_select("fanta_squads", "s");
  $query->join("fanta_players", "p", "p.pl_id = s.pl_id");
  $query->join("fanta_players_rounds", "pr", "pr.pl_id = p.pl_id");
  $query->join("fanta_real_teams", "rt", "rt.rt_id = pr.rt_id");
  $query->condition("s.t_id", $t_id);
  $query->condition("pr.round", Round::getLast() + 1);
  $query->condition("s.status", 1);

  $query->fields("s");
  $query->fields("p");
  $query->addField("rt", "name", "team");
    
  $result = $query->execute();
  
  foreach ($result as $row) {
    $team_players[$row->role][$row->pl_id] = $row;
  }
  
  // ciclo per ogni ruolo
  for ($i = 0; $i < 4; $i++) {
    if (isset($team_players[$i])) {
      
      $role_count = count($team_players[$i]);
      
      foreach ($team_players[$i] as $pl_id => $player) {
        
        $sell_form = drupal_get_form('team_action_player_form', $t_id, $pl_id, 'sell');
        $regain_form = drupal_get_form('team_action_player_form', $t_id, $pl_id, 'regain');
        $delete_form = drupal_get_form('team_action_player_form', $t_id, $pl_id, 'delete');
        
        $data = array();
        $data[] = array(
          "data" => fantacalcio_show_role($player->role), 
          "class" => "role");
        $data[] = array("data" => $player->name);
        $data[] = array(
          "data" => ucwords($player->team));
        $data[] = array("data" => $player->cost, "class" => array("num"));
        $data[] = drupal_render($sell_form) . drupal_render($regain_form) . drupal_render($delete_form);
        
        $rows[$i . "_" . $pl_id] = array(
          "data" => $data, 
          "class" => array("role_" . $i));
      }
    }
    else 
      $role_count = 0;
    
    for ($j = $role_count; $j < $max_roles[$i]; $j++) {
      $buy_player_form = drupal_get_form('team_buy_player_form', $g_id, $t_id, $i, $j);
      $data = array();
      $data[] = array("data" => fantacalcio_show_role($i), "class" => array("role"));
      $data[] = array(
        "data" => drupal_render($buy_player_form), 
        "colspan" => 4);
      $rows[$i . "__" . $j] = array("data" => $data, "class" => array("role_" . $i));
    }
  }
  
  return theme("table", array("header" => $header, "rows" => $rows, "attributes" => array("class" => array("table", "table-responsive"))));
}

function team_summary($g_id, $t_id) {
    
  $spesa = 0;
  $n_giocatori = 0;
  $n_roles = array(0, 0, 0, 0);
  
  $query = db_select("fanta_squads", "s");
  $query->join("fanta_players", "p", "p.pl_id = s.pl_id");
  $query->condition("t_id", $t_id);
  $query->fields("s");
  $query->fields("p");
  $result = $query->execute();
  
  foreach($result as $row) {
    if ($row->status == -1) {
      $spesa += floor($row->cost / 2);
    }
    else
      $spesa += $row->cost;
    
    if ($row->status == 1) {
      $n_giocatori++;
      $n_roles[$row->role]++;
    }
  }
  
  $crediti = variable_get('fantacalcio_credits', 350);
  $rimasti = $crediti - $spesa + get_swaps_money($t_id);
  
  $spesa_max = (variable_get('fantacalcio_num_rosa', 25) == $n_giocatori) ? "-" : ($rimasti - variable_get('fantacalcio_num_rosa', 25) + $n_giocatori + 1);
  
  $rows[0] = array(
    array("data" => t("N&deg; giocatori"), "class" => "-label"), 
    array("data" => $n_giocatori, "class" => "num"), 
    array("data" => t("Portieri"), "class" => "-label"), 
    array("data" => $n_roles[0], "class" => "num"));
  $rows[1] = array(
    array("data" => t("Crediti spesi"), "class" => "-label"), 
    array("data" => $spesa, "class" => "num"), 
    array("data" => t("Difensori"), "class" => "-label"), 
    array("data" => $n_roles[1], "class" => "num"));
  $rows[2] = array(
    array("data" => t("Crediti rimasti"), "class" => "-label"), 
    array("data" => $rimasti, "class" => "num imp"), 
    array("data" => t("Centrocampisti"), "class" => "-label"), 
    array("data" => $n_roles[2], "class" => "num"));
  $rows[3] = array(
    array("data" => t("Costo massimo"), "class" => "-label"), 
    array("data" => $spesa_max, "class" => "num max"), 
    array("data" => t("Attaccanti"), "class" => "-label"), 
    array("data" => $n_roles[3], "class" => "num"));
  
  return theme("table", array("header" => array(), "rows" => $rows, "attributes" => array("class" => array("table", "table-responsive"))));
}

function team_ex_players($g_id, $t_id) {
//   $players = get_all_players();
//   $real_teams = get_real_teams();
  $team_ex_players = array();
  
  $query = db_select("fanta_squads", "s");
  $query->join("fanta_players", "p", "p.pl_id = s.pl_id");
  $query->join("fanta_players_rounds", "pr", "pr.pl_id = p.pl_id");
  $query->join("fanta_real_teams", "rt", "rt.rt_id = pr.rt_id");
  $query->condition("s.t_id", $t_id);
  $query->condition("pr.round", Round::getLast());
  $query->condition("s.status", 1, "!=");

  $query->fields("s");
  $query->fields("p");
  $query->addField("rt", "name", "team");
    
  $result = $query->execute();
  
  $result = $query->execute();
  
  foreach ($result as $row) {
    $team_ex_players[$row->pl_id] = $row;
  }
  
  if ($team_ex_players) {
    foreach ($team_ex_players as $pl_id => $player) {
      $data = array();
      $data[] = array("data" => fantacalcio_show_role($player->role), "class" => "role");
      $data[] = array("data" => $player->name);
      $data[] = array("data" => ucwords($real_teams[$player->rt_id]->long_name));
      $data[] = array("data" => $player->cost, "class" => "num");
      $data[] = drupal_get_form('team_action_player_form', $t_id, $pl_id, 'restore');
      $data[] = l("<img src='" . base_path() . drupal_get_path("module", "fantacalcio") . "/images/stats.png'>", "scheda/giocatore/" . $pl_id, array(
        "html" => "true", 
        "attributes" => array("class" => "thickbox", "rel" => "lightframe")));
      
      $rows[] = array("data" => $data, "class" => "role_" . $player->role);
    }
    return "<h3>Ex</h3>" . theme("table", array("header" => array(), "rows" => $rows, "attributes" => array("class" => array("table", "table-responsive"))));
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
  while ($row = db_fetch_object($result)) {
    $rows[] = array(
      $teams[$row->t1_id]->name, 
      $teams[$row->t2_id]->name, 
      $row->money, 
      drupal_get_form("team_money_transfer_delete_form", $row->s_id));
  }
  
  return theme("table", array("header" => $header, "rows" => $rows, "attributes" => array("class" => array("table", "table-responsive"))));
}

function get_last_boughts($g_id, $n) {
  
  $bought_players = array();
  $teams = Team::all();
  
  $query = db_select("fanta_squads", "s");
  $query->join("fanta_players", "p", "p.pl_id = s.pl_id");
  $query->join("fanta_players_rounds", "r", "p.pl_id = r.pl_id");
  $query->join("fanta_real_teams", "rt", "rt.rt_id = r.rt_id");
  $query->condition("s.status", 1);
  $query->condition("s.t_id", array_keys($teams), "IN");
  $query->condition("r.round", Round::getLast() + 1);
  
  $query->fields("s");
  $query->fields("p");
  $query->addField("rt", "name", "team");
  
  $query->orderBy("s.s_id", "DESC");
  $query->range(0, $n);
  
  $result = $query->execute();
  
  foreach ($result as $row) {
    $bought_players[$row->pl_id] = $row;
  }
  
  if ($bought_players) {
    foreach ($bought_players as $pl_id => $player) {
      
      $delete_form = drupal_get_form('team_action_player_form', $player->t_id, $player->pl_id, 'delete');
      
      $data = array();
      $data[] = array("data" => fantacalcio_show_role($player->role), "class" => array("role"));
      $data[] = array(
        "data" => $player->name . " (" . ucwords($player->team) . ")");
      $data[] = array("data" => $teams[$player->t_id]->name);
      $data[] = array("data" => $player->cost, "class" => array("num"));
      $data[] = drupal_render($delete_form);
      
      $rows[] = array("data" => $data, "class" => array("role_" . $player->role));
    }
    $header = array(
      t("Ruolo"), 
      t("Giocatore"), 
      t("Squadra"), 
      t("Costo"), 
      "");
    
    return "<h3>" . t("Ultimi acquisti") . "</h3>" . theme("table", array("header" => $header, "rows" => $rows, "attributes" => array("class" => array("table", "table-responsive"))));
  }
  
  return "";
}

function role_number_get_class($n, $limit) {
  if ($n < $limit)
    return "";
  elseif ($n == $limit)
    return "stop";
  elseif ($n > $limit)
    return "alert";
}

function get_swaps_money($t_id) {
  $money = 0;
  $sql = "SELECT * FROM {fanta_squad_changes} WHERE (t1_id = '%d' OR t2_id = '%d') ";
  
  $query = db_select("fanta_squad_changes", "c");
  $or = db_or()->condition('t1_id', $t_id)->condition('t2_id', $t_id);
  $query->condition($or);
  $query->fields("c");
  
  $result = $query->execute();
  foreach($result as $row) {
    if ($t_id == $row->t1_id)
      $money -= $row->money;
    elseif ($t_id == $row->t2_id)
      $money += $row->money;
  }
  
  return $money;
}

function get_max_number_for_roles() {
  for ($i = 0; $i < 4; $i++)
    $max[$i] = variable_get("fantacalcio_number_role_" . $i, 0);
  
  return $max;
}

function fantamercato_links($g_id) {
  $items[] = l("Riepilogo", "mercato/group/" . $g_id);
  $items[] = l("Scambi", "mercato/swaps/" . $g_id);
  $items[] = l("Visualizzazione", "mercato/view/" . $g_id);
  
  return theme("item_list",array("items" => $items)) . "<div class=clear></div><br>";
}

/* funzioni per la gestione del mercato - Girone */
function fantamercato_show_group($g_id) {
  if (draft_is_enabled()) {
    drupal_set_title(filter_xss("Mercato - " . Group::get($g_id)->name . " - Riepilogo"));
    $buy_form = drupal_get_form('group_buy_player_form', $g_id);
    return drupal_render($buy_form)
 . "<br>" . group_summary($g_id) . "<br>" . get_last_boughts($g_id, 5);
  }
  else
    return t("Mercato disabilitato");
}
/* END- funzioni per la gestione del mercato - Girone */

/* funzioni per la gestione del mercato - Squadra */
function fantamercato_show_team($g_id, $t_id) {
  if (draft_is_enabled()) {
    drupal_set_title(filter_xss("Mercato - " . Group::get($g_id)->name . " - " . Team::get($t_id)->name));
    return fantamercato_links($g_id) . "<br>" . team_summary($g_id, $t_id) . "<br>" . team_players_list($g_id, $t_id) . "<br>" . team_ex_players($g_id, $t_id);
  }
  else
    return t("Mercato disabilitato");
}
/* END- funzioni per la gestione del mercato - Squadra */

/* funzioni per la visualizzazione del mercato - Girone */
function fantamercato_view_group($g_id) {
  if (draft_is_enabled()) {
    drupal_set_title(filter_xss("Mercato - " . Group::get($g_id)->name . " - Visualizzazione"));
    return fantamercato_links($g_id) . group_summary($g_id);
  }
  else
    return t("Mercato disabilitato");
}
/* END- funzioni per la visualizzazione del mercato - Girone */

/* funzioni per gli scambi del mercato - Girone */
function fantamercato_swaps_group($g_id) {
  if (draft_is_enabled()) {
    drupal_set_title(filter_xss("Mercato - " . Group::get($g_id)->name . " - Scambi"));
    return fantamercato_links($g_id) . drupal_get_form('players_swap_form', $g_id) . "<br>" . drupal_get_form('team_money_transfer_form', $g_id) . "<br>" . swaps_summary($g_id);
  }
  else
    return t("Mercato disabilitato");
}
/* END- funzioni per gli scambi del mercato - Girone */

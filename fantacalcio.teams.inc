<?php

/**
* @file fantacalcio.inc
* 
*/
function fantacalcio_team_form($form, $form_state, $id) {
  global $user;
  
  if ($id == null)
    $team = null;
  else
    $team = Team::get($id);
  
  if ($team == null || $team->user !== $user->uid)
    drupal_goto(drupal_access_denied());
  
  $action = ($team) ? "update" : "insert";
  $title = ($team) ? t("Gestione squadra") . " - " . $team->name : t("Crea squadra");
  drupal_set_title(filter_xss($title));
  
  $form["title"] = array("#title" => check_plain($title));
  
  // if (variable_get("fantacalcio_team_change_name", 0) == 1 || $team == null) {
  $form["name"] = array(
    "#type" => "textfield", 
    "#title" => "Nome squadra", 
    "#default_value" => $team ? check_plain($team->name) : "", 
    "#required" => TRUE, 
    "#disabled" => (variable_get("fantacalcio_team_change_name", 0) == 1 || $team != null) ? true : false);
  // }
  // else {
  // $form ["name"] = array(
  // "#type" => "hidden",
  // "#title" => "Nome squadra",
  // "#suffix" => "<h4>" . check_plain($team->name) . "</h4>",
  // "#default_value" => check_plain($team->name),
  // "#required" => TRUE
  // );
  // }
  
  $form["coach"] = array(
    "#type" => "textfield", 
    "#title" => "Allenatore", 
    "#default_value" => $team ? check_plain($team->coach) : "");
  
  $form["stadium"] = array(
    "#type" => "textfield", 
    "#title" => "Stadio", 
    "#default_value" => $team ? check_plain($team->stadium) : "");
  
  // $form ["shirt"] = array(
  // "#type" => "fieldset",
  // "#title" => "Divisa",
  // "#ahah" => array(
  // "wrapper" => "prova",
  
  // // default so not really needed, but included for clarity
  // "method" => "replace",
  // "event" => "change",
  // "path" => "js/divisa.js"
  // )
  
  // );
  
  // $form ["shirt"] ["type"] = array(
  // "#type" => "select",
  // "#options" => array(
  // 0 => "solo bordi",
  // 1 => "righe verticali",
  // 2 => "righe orizzontali",
  // 3 => "banda diagonale"
  // ),
  // "#title" => "Tipo",
  // "#default_value" => $team ? check_plain(substr($team->shirt, -9, 1)) : "",
  // "#ahah" => array(
  // "wrapper" => "shirt_preview",
  
  // // default so not really needed, but included for clarity
  // "method" => "replace",
  // "event" => "change",
  // "path" => "team/js"
  // )
  
  // );
  
  // $form ["shirt"] ["color_1"] = array(
  // "#type" => "select",
  // "#options" => array(
  // 0 => "Bianco",
  // 1 => "Nero",
  // 2 => "Rosso",
  // 3 => "Blu",
  // 4 => "Verde",
  // 5 => "Giallo"
  // ),
  // "#title" => "Colore 1",
  // "#default_value" => $team ? check_plain(substr($team->shirt, -7, 1)) : "",
  // "#ahah" => array(
  // "wrapper" => "shirt_preview",
  
  // // default so not really needed, but included for clarity
  // "method" => "replace",
  // "event" => "change",
  // "path" => "team/js"
  // )
  
  // );
  
  // $form ["shirt"] ["color_2"] = array(
  // "#type" => "select",
  // "#options" => array(
  // 0 => "Bianco",
  // 1 => "Nero",
  // 2 => "Rosso",
  // 3 => "Blu",
  // 4 => "Verde",
  // 5 => "Giallo"
  // ),
  // "#title" => "Colore 2",
  // "#default_value" => $team ? check_plain(substr($team->shirt, -5, 1)) : "",
  // "#ahah" => array(
  // "wrapper" => "shirt_preview",
  
  // // default so not really needed, but included for clarity
  // "method" => "replace",
  // "event" => "change",
  // "path" => "team/js"
  // )
  
  // );
  
  // $form ["shirt"] ["preview"] = array(
  // "#prefix" => "<div id=\"shirt_preview\" style=\"float:left;\">",
  // "#suffix" => "</div>",
  // "#type" => "markup",
  // "#value" => $team ? "<img src=\"" . base_path() . drupal_get_path("module", "fantacalcio") . "/images/shirts/" . $team->shirt . "\">" : ""
  // );
  
  $form["t_id"] = array(
    "#type" => "hidden", 
    "#value" => $team ? check_plain($team->id) : "");
  
  $form["action"] = array("#type" => "hidden", "#value" => check_plain($action));
  
  $form["submit"] = array("#type" => "submit", "#value" => "Ok");
  
  $form["clear"] = array(
    "#type" => "submit", 
    "#value" => "Annulla", 
    "#validate" => array("fantacalcio_team_form_clear"));
  
  return $form;
}

function fantacalcio_team_form_ahah() {
  $type = $_POST["type"];
  $color_1 = $_POST["color_1"];
  $color_2 = $_POST["color_2"];
  
  $output = "<img src=\"" . base_path() . drupal_get_path("module", "fantacalcio") . "/images/shirts/shirt_" . $type . "_" . $color_1 . "_" . $color_2 . ".png\" >";
  print drupal_to_js(array("data" => $output, "status" => true));
  exit();
}

function fantacalcio_team_form_clear($form, &$form_state) {
  $form_state["rebuild"] = TRUE;
}

function fantacalcio_team_form_validate($form, &$form_state) {
  $name = $form_state["values"]["name"];
  if (!$name || Team::getByName($name) != null)
    form_set_error("name", t("Inserisci un nome per la squadra"));
  if (Team::getByName($name) != null)
    form_set_error("name", t("Il nome della squadra è già stata utilizzato"));
}

function fantacalcio_team_form_submit($form, &$form_state) {
  global $user;
  
  $team_name = $form_state["values"]["name"];
  $team_coach = $form_state["values"]["coach"];
  $team_stadium = $form_state["values"]["stadium"];
  $team_shirt = ""; // shirt_" . $form_state ["values"] ["type"] . "_" . $form_state ["values"] ["color_1"] . "_" . $form_state ["values"] ["color_2"] . ".png";
  $t_id = $form_state["values"]["t_id"];
  $action = $form_state["values"]["action"];
  
  if ($action == "update") {
    $query = db_update('fanta_teams');
    $query->fields(array(
      'name' => $team_name, 
      'coach' => $team_coach, 
      'stadium' => $team_stadium, 
      "shirt" => $team_shirt));
    $query->condition('t_id', $t_id);
    $query->execute();
    
    // $sql = "UPDATE {fanta_teams} " . "SET name = \"%s\", " . "coach = \"%s\", " . "stadium = \"%s\", " . "shirt = \"%s\" " . "WHERE t_id = \"%d\"";
    // $result = db_query($sql, $team_name, $team_coach, $team_stadium, $team_shirt, $t_id);
    return drupal_set_message(t("Squadra modificata con successo"));
  }
  elseif ($action == "insert") {
    
    $query = db_insert('fanta_teams');
    $query->fields(array(
      'name' => $team_name, 
      'coach' => $team_coach, 
      'stadium' => $team_stadium, 
      "shirt" => $team_shirt, 
      "active" => 1, 
      "uid" => $user->uid));
    $query->execute();
    
    // $sql = "INSERT INTO {fanta_teams} (name, coach, stadium, shirt, active, uid) " . "VALUES (\"%s\", \"%s\", \"%s\", \"%s\", 1, \"%d\")";
    // $result = db_query($sql, $team_name, $team_coach, $team_stadium, $team_shirt, $user->uid);
    return drupal_set_message(t("Squadra creata con successo"));
  }
  
  drupal_goto("mie");
}

function fantacalcio_get_title_team() {
  $team = get_team_data();
  
  if (count($team) == 0)
    return "Crea Squadra";
  else
    return "Modifica Squadra";
}

/**
 * *************
 */
/* squadre */
/**
 * *************
 */
function fantacalcio_teams_manager() {
  global $user;
  
  $teams = Team::allByUser($user->uid);
  
  $own_teams_rows = array();
  
  foreach ($teams as $t_id => $team) {
    $classes = array();
    $row = array();
    
    array_push($row, l($team->name, "mie/" . $team->id));
    array_push($row, l(t("Dati"), "mie/" . $team->id . "/dati", array(
      "attributes" => array("class" => array("btn", "btn-sm", "btn-info")))) . " " . l(t("Rosa"), "mie/" . $team->id . "/rosa", array(
      "attributes" => array("class" => array("btn", "btn-sm", "btn-info")))) . " " . l(t("Movimenti"), "mie/" . $team->id . "/movimenti", array(
      "attributes" => array("class" => array("btn", "btn-sm", "btn-info")))) . " " . l(t("Lista movimenti"), "mie/" . $team->id . "/lista-movimenti", array(
      "attributes" => array("class" => array("btn", "btn-sm", "btn-info")))));
    array_push($own_teams_rows, array(
      "data" => $row, 
      "class" => $classes, 
      "data-name" => $team->name));
  }
  
  $prefix = "";
  if (variable_get("fantacalcio_user_can_create_team", 1) == 1 && (count($teams) < variable_get("fantacalcio_user_teams_limit", 0) || variable_get("fantacalcio_user_teams_limit", 0) == 0))
    $prefix = "<a href=\"mie/add\" class=\"pull-left btn btn-sm btn-success\">" . t("Aggiungi squadra") . "</a>";
  
  return $prefix . theme("table", array(
    "rows" => $own_teams_rows, 
    "attributes" => array(
      "class" => array(
        "collapse-table", 
        "collapse-table-no-header", 
        "table", 
        "table-responsive")), 
    "header" => array("", ""), 
    "empty" => t("Non hai ancora nessuna squadra")));
}

function fantacalcio_team_manager_add() {
  if (variable_get("fantacalcio_user_can_create_team", 0) == 1 && fantacalcio_user_can_add_team()) {
    return drupal_get_form("fantacalcio_team_form", null);
  }
  else {
    $out = "<div class=\"well\">" . "<p>" . t("Per creare un'altra squadra") . " " . l(t("contatta un amministratore"), variable_get("fantacalcio_admin_contact_url", "")) . "</p>" . "</div>";
    
    return $out;
  }
}

function fantacalcio_team_manager($team_id) {
  global $user;
  
  $team = Team::get($team_id);
  
  if ($team == null || $team->user !== $user->uid)
    drupal_goto(drupal_access_denied());
  
  $content["team_form"] = drupal_get_form("fantacalcio_team_form", $team_id);
  $content["t_id"] = $team_id;
  
  return theme("team_form", $content);
}

function fantacalcio_show_squad($t_id) {
  $content = array(
    "squad" => array(
      "#type" => "markup", 
      "#markup" => theme_table(show_team_squad($t_id))), 
    "t_id" => $t_id);
  
  return theme("squad", $content);
}

function fantacalcio_show_teams($t_id = "") {
  global $user;
  
  $competition = Competition::getDefault();
  
  // $args = array("t_id" => NULL, "c_id" => variable_get("fantacalcio_main_competition", 1), "c_name" => "", "selected_competition" => "");
  
  // if (!empty($t_id) && team_exists($t_id))
  // $args = array("t_id" => $t_id, "c_id" => variable_get("fantacalcio_main_competition", 1), "c_name" => "", "selected_competition" => "");
  
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
          "data" => l($team->name, "squadre/" . $team->id), 
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
    
    // print_r($teams_list);
    
    // $output = array(
    // "teams_list" => array(
    // "#items" => $teams_list,
    // "#theme" => "item_list",
    // ));
    
    $output["teams_list"] = $teams_list;
    
    if (isset($t_id) && Team::get($t_id) != null) {
      $output["main_output"] = show_team_data($t_id);
    }
    
    return theme("columns", $output);
  }
  else {
    return t("Nessuna competizione");
  }
}

function show_team_data($t_id) {
  global $user;
  
  drupal_set_title(check_plain("Squadre - " . Team::get($t_id)->name));
  
  drupal_add_js(drupal_get_path("module", "fantacalcio") . "/js/responsive-tabs.js");
  drupal_add_js(" (function($) { fakewaffle.responsiveTabs(['xs', 'sm']);  })(jQuery);", "inline");
  
  $output = array(
    "squad" => array(
      "#type" => "markup", 
      "#markup" => theme_table(show_team_squad($t_id))), 
    
    "credits" => array(
      "#type" => "markup", 
      "#markup" => theme_table(show_team_credits($t_id))), 
    
    "details" => array(
      "#type" => "markup", 
      "#markup" => theme_table(show_team_details($t_id))), 
    
    "rounds" => array(
      "#type" => "markup", 
      "#markup" => theme_table(show_team_rounds($t_id))), 
    
    "stats" => array(
        "#type" => "markup", 
        "#markup" => show_team_stats($t_id)), 
    
    "is_own_team" => Team::get($t_id)->user == $user->uid, 
    "team" => Team::get($t_id));
  
  return theme("team", $output);
}

function show_team_squad($t_id) {
  global $user;
  
  $team = Team::get($t_id);
  
  if (!$team)
    return t("Squadra inesistente");
  
  $squad = $team->getSquad();
  
  $header = array(t("Ruolo"), t("Nome"), t("Squadra"), t("Costo"));
  
  if (variable_get("fantacalcio_free_movements", 1) == 1) {
    array_push($header, t("Quotazione"));
  }
  array_push($header, "");
  
  $squad_rows = array();
  foreach ($squad as $pl_id => $player) {
    $squad_rows[$pl_id] = array(
      fantacalcio_show_role($player->role), 
      $player->name, 
      ucfirst($player->team), 
      $player->cost);
    if (variable_get("fantacalcio_free_movements", 1) == 1)
      array_push($squad_rows[$pl_id], $player->quotation);
    array_push($squad_rows[$pl_id], l("<i class=\"fa fa-bar-chart\"></i>", "scheda/giocatore/" . $pl_id, array(
      "html" => true, 
      "attributes" => array(
        "data-toggle" => "modal", 
        "data-target" => "#player-stats-modal"))));
  }
  
  $squad_table = array(
    "header" => $header, 
    "rows" => $squad_rows, 
    "attributes" => array(
      "class" => array("table table-responsive table-striped")), 
    "caption" => "", 
    "empty" => t("Nessun giocatore"), 
    "sticky" => TRUE, 
    "colgroups" => array(), 
    "theme" => "table");
  
  return $squad_table;
}

function show_team_credits($t_id) {
  $spent = 0;
  $credits = variable_get("fantacalcio_credits", 1000);
  
  $query = db_select("fanta_squads", "s");
  $query->condition("t_id", $t_id);
  $query->fields("s");
  
  $result = $query->execute();
  
  foreach ($result as $row) {
    $spent += $row->status == 1 ? $row->cost : ceil($row->cost / 2);
  }
  
  $credits_rows = array();
  $credits_rows[0] = array(t("Crediti"), $credits);
  $credits_rows[1] = array(t("Crediti spesi"), $spent);
  $credits_rows[2] = array(t("Crediti rimasti"), ($credits - $spent));
  
  $credits_table = array(
    "header" => array(), 
    "rows" => $credits_rows, 
    "attributes" => array(
      "class" => array("table table-responsive table-striped")), 
    "caption" => "", 
    "empty" => "", 
    "sticky" => TRUE, 
    "colgroups" => array(), 
    "theme" => "table");
  
  return $credits_table;
}

function show_team_details($t_id = "") {
  $team = Team::get($t_id);
  
  $rows = array();
  
  if (!empty($t_id) && $team != null) {
    $rows[] = array("<h4>" . t("Allenatore") . "</h4>", $team->coach);
    $rows[] = array("<h4>" . t("Stadio") . "</h4>", $team->stadium);
    // $rows [] = array(
    // "<h4>" . t("Trofei") . "</h4>",
    // theme_team_history($team->honours)
    // );
    
    return array(
      "header" => array(), 
      "rows" => $rows, 
      "caption" => "", 
      "empty" => "", 
      "colgroups" => array(), 
      "sticky" => false, 
      "attributes" => array("class" => array("table", "table-responsive")));
  }
  
  return array();
}

function show_team_rounds($t_id) {
  if (Competition::getDefault()->type == COMPETITION_TYPE_GP) {
    
    $rounds = array();
    
    $query = db_select("fanta_teams_rounds", "tr");
    $query->join("fanta_teams", "t", "t.t_id = tr.t_id");
    $query->join("fanta_rounds", "r", "r.round = tr.round");
    $query->join("fanta_rounds_competitions", "rc", "rc.round = r.round");
    $query->condition("tr.t_id", $t_id);
    $query->condition("rc.c_id", Competition::getDefault()->id);
    $query->fields("tr");
    $query->fields("r");
    $query->fields("rc");
    $query->addField("t", "name");
    $query->orderBy("r.round");
    
    $result = $query->execute();
    
    foreach ($result as $row) {
      
      $rounds[$row->round] = array(
        $row->round, 
        $row->points, 
        $row->round_position, 
        $row->season_position);
      
      if ($row->round == Round::getNextForCompetition($row->c_id) || $row->status == 1) {
        array_push($rounds[$row->round], l("<i class=\"fa fa-bar-chart\"></i>", "scheda/partita/" . $row->round, array(
          "html" => "true", 
          "attributes" => array(
            "class" => "_thickbox", 
            "rel" => "lightframe[\"matches_" . $t_id . "\"|width:970px;]"))));
      }
      else
        array_push($rounds[$row->round], array(
          "data" => "&nbsp;", 
          "colspan" => 1));
    }
    
    return (array(
      "header" => array(
        t("Giornata"), 
        t("Punteggio"), 
        t("Posizione parziale"), 
        t("Posizione stagionale"), 
        ""), 
      "rows" => $rounds, 
      "attributes" => array(
        "class" => array("squad", "table", "table-responsive")), 
      "caption" => "", 
      "empty" => t("Nessuna giornata giocata"), 
      "sticky" => TRUE, 
      "colgroups" => array()));
  }
  else {
    $query = db_select("fanta_matches", "m");
    $query->join("fanta_teams", "t1", "t1.t_id = m.t1_id");
    $query->join("fanta_teams", "t2", "t2.t_id = m.t2_id");
    $or = db_or();
    $or->condition("m.t1_id", $t_id);
    $or->condition("m.t2_id", $t_id);
    $query->condition($or);
    $query->condition("g_id", array_keys(Competition::getDefault()->groups), "IN");
    $query->fields("m");
    $query->addField("t1", "name", "t1_name");
    $query->addField("t2", "name", "t2_name");
    $query->orderBy("m.round");
    
    $result = $query->execute();
    
    $rows = array();
    foreach ($result as $row) {
      $rows[$row->m_id][] = $row->round;
      $rows[$row->m_id][] = $row->t1_name . " - " . $row->t2_name;
      $rows[$row->m_id][] = ($row->played == 1) ? $row->goals_1 . " - " . $row->goals_2 : "";
      
      if ($row->round == Round::getNextForCompetition(Group::get($row->g_id)->competition_id) || $row->played == 1) {
        $rows[$row->m_id][] = l("<i class=\"fa fa-bar-chart\"></i>", "scheda/partita/" . $row->m_id, array(
          "html" => "true", 
          "attributes" => array(
            "class" => "_thickbox", 
            "rel" => "lightframe[\"matches_" . $t_id . "\"|width:970px;]")));
      }
      else
        $rows[$row->m_id][] = array("data" => "&nbsp;", "colspan" => 1);
    }
    
    return (array(
      "header" => array(t("Giornata"), t("Partita"), t("Risultati"), ""), 
      "rows" => $rows, 
      "attributes" => array(
        "class" => array("squad", "table", "table-responsive")), 
      "caption" => "", 
      "empty" => t("Nessuna giornata giocata"), 
      "sticky" => TRUE, 
      "colgroups" => array()));
  }
}

function show_team_stats($t_id) {
  if (count(Round::allPlayed()) > 0) {
    drupal_add_js(drupal_get_path("module", "fantacalcio") . "/js/Chart.min.js");
    
    $team = Team::get($t_id);
    
    $default_competition = Competition::getDefault();
    
    $rows = array();
    
    $position = $team->statPosition($default_competition);
    
    $rows[] = array(t("Posizione attuale"), $position);
    $rows[] = array(
      t("Punteggio attuale"), 
      $team->statPoints($default_competition));
    
    if ($position == 1)
      $rows[] = array(
        t("Distacco dal secondo"), 
        $team->statGap($default_competition));
    else
      $rows[] = array(
        t("Distacco dal primo"), 
        $team->statGap($default_competition));
    
    $rows[] = array(
      t("Miglior punteggio"), 
      $team->statMaxPoints($default_competition));
    $rows[] = array(
      t("Peggior punteggio"), 
      $team->statMinPoints($default_competition));
    $rows[] = array(
      t("Punteggio medio"), 
      $team->statAvgPoints($default_competition));
    
    $stats = array(
      "header" => array("", ""), 
      "rows" => $rows, 
      "attributes" => array("class" => array("table", "table-responsive")));
    
    $points = $team->getRoundsPoints($default_competition);
    
    $points_chart = "<div class=\"row\">
			<div class=\"cols-xs-12\">
			<label>" . t("Punteggi") . "</label><br>
			<canvas id=\"pointsChart\" style='width:100%; height: 200px'></canvas>
			</div>
			</div>";
    
    $points_chart_js = "jQuery(document).on('ready', function() {
		var ctx = jQuery('#pointsChart').get(0).getContext('2d');
		var data = {
			labels: [" . implode(", ", array_keys($points)) . "],
			datasets: [
			{
				label: '" . t("Punteggi") . "',
				fillColor: 'rgba(255,100,40,0.2)',
				strokeColor: 'rgba(255,100,40,0.5)',
				pointColor: 'rgba(255,100,40,0.5)',
				pointStrokeColor: '#fff',
				pointHighlightFill: '#fff',
				pointHighlightStroke: 'rgba(220,220,220,1)',
				data: [" . implode(", ", $points) . "]
			},		
			]
		};
		
		new Chart(ctx).Line(data);
		});";
    
    drupal_add_js($points_chart_js, "inline");
    
    $round_positions = $team->getRoundsPositions($default_competition);
    
    $round_positions_chart = "<div class=\"row\">
			<div class=\"cols-xs-12\">
			<label>" . t("Posizioni giornata") . "</label><br>
			<canvas id=\"positionsChart\" style='width:100%; height: 200px'></canvas>
			</div>
			</div>";
    
    $round_positions_chart_js = "jQuery(document).ready(function() {
		var ctx = jQuery('#positionsChart').get(0).getContext('2d');
		var data = {
			labels: [" . implode(", ", array_keys($round_positions)) . "],
			datasets: [
			{
				label: '" . t("Punteggi") . "',
				fillColor: 'rgba(20,20,220,0.2)',
				strokeColor: 'rgba(20,20,220,0.5)',
				pointColor: 'rgba(20,20,220,0.5)',
				pointStrokeColor: '#fff',
				pointHighlightFill: '#fff',
				pointHighlightStroke: 'rgba(220,220,220,1)',
				data: [" . implode(", ", $round_positions) . "]
			},
			]
		};
	
		new Chart(ctx).Line(data);
		});";
    
    drupal_add_js($round_positions_chart_js, "inline");
    
    $season_position_chart = "";
    $matches_chart = "";
    
    if ($default_competition->type == COMPETITION_TYPE_GP) {
      $season_position_chart = "<div class=\"row\">
				<div class=\"cols-xs-12\">
				<label>" . t("Posizioni stagione") . "</label><br>
				<canvas id=\"seasonPositionsChart\" style='width:100%; height: 200px'></canvas>
				</div>
				</div>";
      
      $season_positions = $team->getRoundsPositions($default_competition);
      
      $season_positions_js = "jQuery(document).ready(function() {
			var ctx = jQuery('#seasonPositionsChart').get(0).getContext('2d');
			var data = {
				labels: [" . implode(", ", array_keys($season_positions)) . "],
				datasets: [
				{
					label: '" . t("Punteggi") . "',
					fillColor: 'rgba(20,220,20,0.2)',
					strokeColor: 'rgba(20,220,20,0.5)',
					pointColor: 'rgba(220,220,220,0.5)',
					pointStrokeColor: '#fff',
					pointHighlightFill: '#fff',
					pointHighlightStroke: 'rgba(220,220,220,1)',
					data: [" . implode(", ", $season_positions) . "]
				},
				]
			};
		
			new Chart(ctx).Line(data);
			});";
      
      drupal_add_js($season_positions_js, "inline");
    }
    
    else 
      if ($default_competition->type == COMPETITION_TYPE_SD) {
        $matches_chart = "<div class=\"row\">
				<div class=\"cols-xs-12\">
				<label>" . t("Partite") . "</label><br>
				<canvas id=\"matchesChart\" style='width:100%; height: 200px'></canvas>
				</div>
				</div>";
        
        $matches = $team->getMatchesResults($default_competition);
        
        $matches_js = "jQuery(document).ready(function() {
			var ctx = jQuery('#matchesChart').get(0).getContext('2d');
			var data = [
			    {
			        value: " . $matches->losts . ",
			        color:\"#F7464A\",
			        highlight: \"#FF5A5E\",
			        label: \"" . t("Sconfitte") . "\"
			    },
			    {
			        value: " . $matches->wins . ",
			        color: \"#46BF5D\",
			        highlight: \"#5AD351\",
			        label: \"" . t("Vittorie") . "\"
			    },
			    {
			        value: " . $matches->draws . ",
			        color: \"#FDB45C\",
			        highlight: \"#FFC870\",
			        label: \"" . t("Pareggi") . "\"
			    }
			]
		
			new Chart(ctx).Pie(data);
			});";
        
        drupal_add_js($matches_js, "inline");
      }
    
    return theme("table", $stats);// . $points_chart . $round_positions_chart . $season_position_chart . $matches_chart;
  }
  else {
    return t("Nessuna giornata giocata");
  }
}

function theme_team_history($history) {
  if ($history) {
    $data = unserialize($history);
    $out = "";
    $out = "<ul class=\"team-history\">";
    if ($data) {
      foreach ($data as $key => $value)
        $out .= "<li>" . $key . " <img src=\"" . base_path() . drupal_get_path("module", "fantacalcio") . "/images/" . variable_get($value, "") . "\"></li>";
    }
    $out .= "</ul>";
    return $out;
  }
}

function theme_team_last_year($last_year) {
  if ($last_year) {
    $data = unserialize($last_year);
    $out = "";
    foreach ($data as $key => $value)
      $out .= "<img src=\"" . base_path() . drupal_get_path("module", "fantacalcio") . "/images/" . variable_get($value, "") . "\">";
    
    return $out;
  }
}

function fantacalcio_squad_show_favourites() {
  global $user;
  
  $team = get_team_by_user($user->uid);
  
  if (!$team)
    drupal_goto("crea-squadra");
  
  drupal_set_title($team->name . " - Lista movimenti", $output = CHECK_PLAIN);
  
  $rows = $team->getMovements();
  
  $header = array(
    t("Ruolo"), 
    t("Nome"), 
    t("Squadra"), 
    t("Movimento"), 
    t("Crediti"));
  
  $out = theme_table(array(
    "header" => $header, 
    "rows" => $rows, 
    "attributes" => array(
      "class" => array("table table-responsive table-striped")), 
    "caption" => "", 
    "empty" => "Nessun movimento", 
    "sticky" => TRUE, 
    "colgroups" => array()));
  
  $content["raw_markup"] = array("#type" => "markup", "#markup" => $out);
  
  return $content;
}

function fantacalcio_squad_edit($t_id) {
  // solo per leghe di tipo Gran Premio
  if (variable_get("fantacalcio_league_type", 1) != COMPETITION_TYPE_GP)
    drupal_goto("mie/" . $t_id . "/lista-movimenti");
  
  drupal_add_js(drupal_get_path("module", "fantacalcio") . "/js/squad-list.js");
  drupal_add_js(drupal_get_path("module", "fantacalcio") . "/js/jquery.tablesorter.js");
  drupal_add_js(drupal_get_path("module", "fantacalcio") . "/js/jquery.metadata.js");
  drupal_add_js("jQuery(window).ready(function() {jQuery(\"#players-list\").tablesorter({cssAsc: \"sort-asc\", cssDesc: \"sort-desc\", cssHeader: \"sort-header\"});})", "inline");
  
  global $user;
  
  $team = Team::get($t_id);
  
  if ($team->user != $user->uid)
    drupal_goto(drupal_access_denied());
  
  drupal_set_title($team->name . " - " . t("Gestione Rosa"), $output = CHECK_PLAIN);
  
  // $squad_list_form = drupal_get_form("fantacalcio_squad_players_list_form");
  // $out_left = drupal_render($squad_list_form);
  
  // lista giocatori
  $players_list = Player::allWithQuotation();
  
  // rosa
  $squad = $team->getSquad();
  $squad_rows = array();
  $bought_players_role = array("0" => 0, "1" => 0, "2" => 0, "3" => 0);
  $outflow = 0;
  
  foreach ($squad as $pl_id => $player) {
    $sell_player_form = drupal_get_form("fantacalcio_sell_player_form", $t_id, $pl_id, $player->quotation);
    
    $squad_rows[$pl_id] = array(
      "data" => array(
        "<span class='fa-stack'>
						<i class='fa fa-square fa-stack-2x squad-player-role-" . $player->role . "'></i>
						<i class='fa fa-stack-1x' style='color: white;'><span class='font-normal'>" . Player::convertRole($player->role) . "</span></i>
					</span>", 
        $player->name, 
        ucfirst($player->team), 
        $player->cost, 
        $player->quotation, 
        "<a href=\"#\" data-toggle=\"modal\" data-target=\"#player-stats-modal\" class=\"player-stats\" id=\"player-stat-" . $pl_id . "\"><i class=\"fa fa-bar-chart\"></i></a>", 
        drupal_render($sell_player_form)), 
      "class" => array("squad-player-role-" . $player->role));
    
    $outflow += $player->cost;
    $bought_players_role[$player->role]++;
  }
  
  $list_header = array(
    array("data" => t("Ruolo"), "data-id" => "role"), 
    array("data" => t("Nome"), "data-id" => "name"), 
    array("data" => t("Squadra"), "data-id" => "team"), 
    array("data" => t("Quotazione"), "data-id" => "quotation"), 
    "", 
    "");
  $list_rows = Player::listForSquad($players_list, $squad, $team->id, $team->isConfirmed());
  
  $content["players_list"] = array(
    "#type" => "markup", 
    "#markup" => theme_table(array(
      "type" => "table", 
      "header" => $list_header, 
      "rows" => $list_rows, 
      "attributes" => array(
        "id" => "players-list", 
        "class" => array("table table-responsive table-striped")), 
      "caption" => "", 
      "empty" => t("Nessun giocatore"), 
      "sticky" => TRUE, 
      "colgroups" => array())));
  
  $real_teams_items = array(array("data" => "", "id" => "", "class" => array()));
  
  $real_teams = RealTeam::all();
  array_push($real_teams_items, array(
    "data" => l(" - ", "", array('fragment' => "rt-0", 'external' => TRUE)), 
    "id" => "rt-0", 
    "class" => array(), 
    "role" => "presentation"));
  foreach ($real_teams as $real_team) {
    array_push($real_teams_items, array(
      "data" => l($real_team->name, "", array(
        'fragment' => "rt-" . $real_team->id, 
        'external' => TRUE, 
        "attributes" => array("id" => "link-rt-" . $real_team->id))), 
      "id" => "rt-" . $real_team->id, 
      "class" => array(), 
      "role" => "presentation"));
  }
  
  $content["real_teams_list"] = array(
    "#items" => $real_teams_items, 
    "#theme" => "item_list", 
    "#type" => "ul", 
    "#attributes" => array(
      "id" => "real-teams-list", 
      "class" => array("dropdown-menu"), 
      "role" => "menu"));
  
  $content["squad_data"] = array(
    "#type" => "markup", 
    "#markup" => theme_table(array(
      "type" => "table", 
      "header" => array(
        t("Ruolo"), 
        t("Nome"), 
        t("Squadra"), 
        t("Costo"), 
        t("Quotazione"), 
        "", 
        ""), 
      "rows" => $squad_rows, 
      "attributes" => array(
        "id" => "my-squad", 
        "class" => array("table table-responsive table-striped")), 
      "caption" => "", 
      "empty" => t("Nessun giocatore"), 
      "sticky" => false, 
      "colgroups" => array())));
  
  $content["credits"] = variable_get("fantacalcio_credits", 100);
  $content["outflow"] = $outflow;
  $content["expected_players"] = array(
    "0" => variable_get("fantacalcio_number_role_0", 1), 
    "1" => variable_get("fantacalcio_number_role_1", 1), 
    "2" => variable_get("fantacalcio_number_role_2", 1), 
    "3" => variable_get("fantacalcio_number_role_3", 1));
  $content["bought_players"] = $bought_players_role;
  
  $content["movements"] = variable_get("fantacalcio_max_movements", 0);
  $content["left_movements"] = variable_get("fantacalcio_max_movements", 0) - $team->getMovementsCount();
  
  $squad_confirm_form = drupal_get_form("fantacalcio_complete_squad_form", $t_id);
  $content["squad_confirm_form"] = drupal_render($squad_confirm_form);
  
  $content["is_team_complete"] = $team->canConfirm();
  
  $content["t_id"] = $t_id;
  
  $query_parameters = drupal_get_query_parameters();
  
  $content["search_string"] = "";
  if (isset($query_parameters["s"]))
    $content["search_string"] = $query_parameters["s"];
  
  return theme("movements", $content);
}

function fantacalcio_complete_squad_form($form, &$form_state, $t_id) {
  $form["pl_ids"] = array(
    "#type" => "hidden", 
    "#value" => "", 
    "#attributes" => array("id" => array("squad-confirm-pl-ids")));
  $form["t_id"] = array("#type" => "hidden", "#value" => $t_id);
  $form["confirm"] = array(
    "#title" => t("Conferma"), 
    "#value" => t("Conferma"), 
    "#type" => "submit", 
    "#attributes" => array(
      "class" => array("squad-confirm", "btn", "btn-success")));
  $form["reset"] = array(
    "#title" => t("Annulla"), 
    "#value" => t("Annulla"), 
    "#type" => "reset", 
    "#attributes" => array("class" => array("squad-confirm", "btn")));
  
  return $form;
}

function fantacalcio_complete_squad_form_validate($form, &$form_state) {
  $pl_ids = JSON_decode($form_state["input"]["pl_ids"]);
  
  $team = Team::get($form_state["input"]["t_id"]);
  
  if (!$team->isSquadComplete()) {
    form_set_error("", t("Errore nella compilazione della rosa."));
  }
}

function fantacalcio_complete_squad_form_submit($form, &$form_state) {
  // $pl_ids = JSON_decode($form_state["input"]["pl_ids"]);
  $team = Team::get($form_state["input"]["t_id"]);
  
  $team->setConfirmed();
  
  drupal_set_message(t("Squadra confermata. Ora puoi cominciare a giocare"));
  
  drupal_goto("mie/" . $team->id . "/movimenti");
  
  // foreach($pl_ids as $pl_id) {
  // $player = Player::get($pl_id);
  
  // if (!$team->hasPlayer($pl_id) && $player != null) {
  
  // //vendo giocatore - elimino il giocatore dalla rosa
  // $query = db_insert("fanta_squads");
  // $query->fields(array("t_id" => $form_state["input"]["t_id"], "pl_id" => $pl_id, "cost" => $player->getQuotation(), "status" => 1, "timestamp" => time()));
  // $query->execute();
  
  // //aggiungo il movimento alla lista movimenti
  // $query = db_insert("fanta_squads_movements");
  // $query->fields(array("t_id" => $form_state["input"]["t_id"], "pl_id" => $pl_id, "status" => 1, "timestamp" => time(), "value" => $player->getQuotation(), "temporary" => !($team->isConfirmed())));
  // $query->execute();
  
  // drupal_set_message(t("Acquisto completato"), "status");
  
  // watchdog("fantacalcio", t("%team: Acquisto di %player (-%quotation crediti)"), array("%team" => $team->name, "%player" => $player->name, "%quotation" => $player->getQuotation()), WATCHDOG_INFO);
  // }
  // }
}

function fantacalcio_sell_player_form($form, &$form_state, $t_id, $pl_id) {
  $form["pl_id"] = array(
    "#type" => "hidden", 
    "#value" => $pl_id, 
    "#attributes" => array("class" => array("squad-sell-pl-id")));
  // $form["quotation"] = array("#type" => "hidden", "#value" => $quotation);
  $form["t_id"] = array("#type" => "hidden", "#value" => $t_id);
  $form["sell"] = array(
    "#title" => t("Vendi"), 
    "#value" => t("Vendi"), 
    "#type" => "submit", 
    "#attributes" => array(
      "class" => array("squad-sell-button", "btn", "btn-sm", "btn-warning")));
  
  $form["#suffix"] = "<div class='squad-sell hidden' style='display: none'><i class='fa fa-check-circle fa-2x' style='color: #3C3'></i></div>";
  
  return $form;
}

function fantacalcio_sell_player_form_validate($form, &$form_state) {
  // TODO controllare squadra appartenenza, crediti rimasti, numero giocatori, numero movimenti
  global $user;
  
  $team = Team::get($form_state["input"]["t_id"]);
  $player = Player::get($form_state["input"]["pl_id"]);
  
  $credits = $team->getCredits();
  $movements_count = $team->getMovementsCount();
  $num_players = $team->getNumberPlayers();
  
  $max_movements = variable_get("fantacalcio_max_movements", 0) > 0 ? variable_get("fantacalcio_max_movements", 0) : PHP_INT_MAX;
  
  // verifica squadra appartenente all'utente
  if ($user->uid != $team->user)
    form_set_error('', t("Impossibile completare l'operazione.") . " " . t('La squadra non appartiene all\'utente'));
    
    // verifico numero movimenti rimasti
  $needed_movements = variable_get("fantacalcio_number_players", 25) - array_sum($num_players) - 1;
  // echo $needed_movements ."$$";echo $max_movements."$$"; echo $movements_count."";die();
  if ($needed_movements > ($max_movements - $movements_count))
    form_set_error('', t("Impossibile completare l'operazione.") . " " . t('I movimenti a disposizione non sono sufficienti'));
}

function fantacalcio_sell_player_form_submit($form, &$form_state) {
  fantacalcio_sell_player_function($form_state["input"]["t_id"], $form_state["input"]["pl_id"]);
}

function fantacalcio_sell_player_function($t_id, $pl_id) {
  global $user;
  
  // print_r($form_state["values"]);die();
  
  $team = Team::get($t_id);
  $player = Player::get($pl_id);
  
  if ($team != null && $team->user == $user->uid) {
    // verifico che il giocatore sia presente in rosa
    if ($team->hasPlayer($pl_id) && $player != null) {
      // vendo giocatore - elimino il giocatore dalla rosa
      
      $temporary = $team->isConfirmed() ? 0 : 1;
      
      $query = db_delete("fanta_squads");
      $query->condition("t_id", $t_id);
      $query->condition("pl_id", $pl_id);
      $query->execute();
      
      // aggiungo il movimento alla lista movimenti
      $query = db_insert("fanta_squads_movements");
      $query->fields(array(
        "t_id" => $t_id, 
        "pl_id" => $pl_id, 
        "status" => -1, 
        "timestamp" => time(), 
        "value" => $player->getQuotation(), 
        "temporary" => $temporary));
      // print_r($query->fields());die();
      $query->execute();
      
      drupal_set_message(t("Cessione completata"), "status");
      
      watchdog("fantacalcio", t("%team: Cessione di %player (+%quotation crediti)"), array(
        "%team" => $team->name, 
        "%player" => $player->name, 
        "%quotation" => $player->getQuotation()), WATCHDOG_INFO);
    }
  }
}

function fantacalcio_buy_player_form($form, &$form_state, $t_id, $pl_id) {
  $form["pl_id"] = array(
    "#type" => "hidden", 
    "#value" => $pl_id, 
    "#attributes" => array("class" => array("squad-sell-pl-id")));
  $form["search_string"] = array(
    "#type" => "hidden", 
    "#attributes" => array("class" => array("search-string")));
  // $form["quotation"] = array("#type" => "hidden", "#value" => $quotation);
  $form["t_id"] = array("#type" => "hidden", "#value" => $t_id);
  $form["sell"] = array(
    "#title" => t("Compra"), 
    "#value" => t("Compra"), 
    "#type" => "submit", 
    "#attributes" => array(
      "class" => array("squad-buy-button", "btn", "btn-sm", "btn-info")));
  
  // $form["#suffix"] = "<div class='squad-sell hidden' style='display: none'><i class='fa fa-check-circle fa-2x' style='color: #3C3'></i></div>";
  
  return $form;
}

function fantacalcio_buy_player_form_validate($form, &$form_state) {
  // TODO controllare squadra appartenenza, crediti rimasti, numero giocatori, numero movimenti
  global $user;
  
  $team = Team::get($form_state["input"]["t_id"]);
  $player = Player::get($form_state["input"]["pl_id"]);
  
  $credits = $team->getCredits();
  $movements_count = $team->getMovementsCount();
  $num_players = $team->getNumberPlayers();
  
  $max_movements = variable_get("fantacalcio_max_movements", 0) > 0 ? variable_get("fantacalcio_max_movements", 0) : PHP_INT_MAX;
  
  // verifica squadra appartenente all'utente
  if ($user->uid != $team->user)
    form_set_error('', t("Impossibile completare l'operazione.") . " " . t('La squadra non appartiene all\'utente'));
    
    // verifico numero movimenti rimasti
  if ($movements_count > $max_movements)
    form_set_error('', t("Impossibile completare l'operazione.") . " " . t('I movimenti a disposizione sono terminati'));
    
    // verifico crediti
  if ($player->getQuotation() > $credits)
    form_set_error('', t("Impossibile completare l'operazione.") . " " . t('I crediti rimasti non sono sufficienti a completare l\'acquisto'));
    
    // verifico numero giocatori in squadra
  if (array_sum($num_players) >= variable_get("fantacalcio_number_players", 25))
    form_set_error('', t("Impossibile completare l'operazione.") . " " . t('La squadra &egrave; gi&agrave; al completo'));
    
    // verifico numero giocatori del ruolo
  if (isset($num_players[$player->role]) && $num_players[$player->role] >= variable_get("fantacalcio_number_role_" . $player->role, 0))
    form_set_error('', t("Impossibile completare l'operazione.") . " " . t('Il ruolo &egrave; gi&agrave; al completo'));
  // echo $form_state["input"]["search_string"]; die();
  // drupal_goto("mie/" . $form_state["input"]["t_id"] . "/movimenti" , array("query" => array("s" => $form_state["input"]["search_string"])));
}

function fantacalcio_buy_player_form_submit($form, &$form_state) {
  fantacalcio_buy_player_function($form_state["input"]["t_id"], $form_state["input"]["pl_id"], $form_state["input"]["search_string"]);
}

function fantacalcio_buy_player_ajax($t_id, $pl_id, $s) {
  fantacalcio_buy_player_function($t_id, $pl_id, $s);
  
  return drupal_json_output("ok");
}

function fantacalcio_buy_player_function($t_id, $pl_id, $s) {
  // echo $team->isConfirmed();die();
  global $user;
  
  $team = Team::get($t_id);
  $player = Player::get($pl_id);
  
  if ($team != null && $team->user == $user->uid) {
    
    // verifico che il giocatore non sia presente in rosa
    if (!$team->hasPlayer($pl_id) && $player != null) {
      
      $temporary = $team->isConfirmed() ? 0 : 1;
      
      // vendo giocatore - inserisco il giocatore nella rosa
      $query = db_insert("fanta_squads");
      $query->fields(array(
        "t_id" => $t_id, 
        "pl_id" => $pl_id, 
        "cost" => $player->getQuotation(), 
        "status" => 1, 
        "timestamp" => time()));
      // $query->condition("t_id", $form_state["input"]["t_id"]);
      // $query->condition("pl_id", $form_state["input"]["pl_id"]);
      $query->execute();
      
      // aggiungo il movimento alla lista movimenti
      $query = db_insert("fanta_squads_movements");
      $query->fields(array(
        "t_id" => $t_id, 
        "pl_id" => $pl_id, 
        "status" => 1, 
        "timestamp" => time(), 
        "value" => $player->getQuotation(), 
        "temporary" => $temporary));
      $query->execute();
      
      drupal_set_message(t("Acquisto completato"), "status");
      
      drupal_goto("mie/" . $t_id . "/movimenti", array(
        "query" => array("s" => $s)));
      
      watchdog("fantacalcio", t("%team: Acquisto di %player (-%quotation crediti)"), array(
        "%team" => $team->name, 
        "%player" => $player->name, 
        "%quotation" => $player->getQuotation()), WATCHDOG_INFO);
    }
  }
}

function fantacalcio_squad_movements($t_id) {
  global $user;
  
  $team = Team::get($t_id);
  
  if ($team == null || $team->user !== $user->uid)
    drupal_goto(drupal_access_denied());
  
  drupal_set_title($team->name . " - Lista movimenti", $output = CHECK_PLAIN);
  
  $rows = $team->getMovements();
  
  $header = array(
    t("Ruolo"), 
    t("Nome"), 
    t("Squadra"), 
    t("Data"), 
    t("Movimento"), 
    t("Crediti"));
  
  $content["t_id"] = $t_id;
  $content["movements"] = array(
    "#type" => "markup", 
    "#markup" => theme_table(array(
      "header" => $header, 
      "rows" => $rows, 
      "attributes" => array(
        "class" => array("table table-responsive table-striped")), 
      "caption" => "", 
      "empty" => "Nessun movimento", 
      "sticky" => TRUE, 
      "colgroups" => array())));
  
  return theme("movements-list", $content);
}

function fantacalcio_user_can_add_team() {
  global $user;
  
  $max_teams_for_user = variable_get("fantacalcio_user_teams_limit", 0) > 0 ? variable_get("fantacalcio_user_teams_limit", 0) : PHP_INT_MAX;
  $max_teams_for_user = min(Team::getMaxNumberForUser($user->uid), $max_teams_for_user);
  
  $teams_for_user = count(Team::allByUser($user->uid));
  
  if ($teams_for_user < $max_teams_for_user)
    return TRUE;
  else
    return FALSE;
}
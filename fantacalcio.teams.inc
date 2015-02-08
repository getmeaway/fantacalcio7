<?php

/**
* @file fantacalcio.inc
* 
*/

function fantacalcio_team_form ($form_state) {
	$id = 7;//TODO
	$team = Team::get($id);
	$action = ($team) ? "update" : "insert";
	$title = ($team) ? t("Gestione squadra") . " - " . $team->name : t("Crea squadra");
	drupal_set_title(filter_xss($title));
	
	$form ["title"] = array(
			"#title" => check_plain($title) 
	);
	
	$form ["name"] = array(
			"#type" => "textfield",
			"#title" => "Nome squadra",
			"#default_value" => check_plain($team->name),
			"#required" => TRUE 
	)
	;
	
	$form ["coach"] = array(
			"#type" => "textfield",
			"#title" => "Allenatore",
			"#default_value" => check_plain($team->coach) 
	)
	;
	
	$form ["stadium"] = array(
			"#type" => "textfield",
			"#title" => "Stadio",
			"#default_value" => check_plain($team->stadium) 
	)
	;
	
	$form ["shirt"] = array(
			"#type" => "fieldset",
			"#title" => "Divisa",
			"#ahah" => array(
					"wrapper" => "prova",
					
					// default so not really needed, but included for clarity
					"method" => "replace",
					"event" => "change",
					"path" => "js/divisa.js" 
			)
			 
	)
	;
	
	$form ["shirt"] ["type"] = array(
			"#type" => "select",
			"#options" => array(
					0 => "solo bordi",
					1 => "righe verticali",
					2 => "righe orizzontali",
					3 => "banda diagonale" 
			),
			"#title" => "Tipo",
			"#default_value" => check_plain(substr($team->shirt, -9, 1)),
			"#ahah" => array(
					"wrapper" => "shirt_preview",
					
					// default so not really needed, but included for clarity
					"method" => "replace",
					"event" => "change",
					"path" => "team/js" 
			)
			 
	);
	
	$form ["shirt"] ["color_1"] = array(
			"#type" => "select",
			"#options" => array(
					0 => "Bianco",
					1 => "Nero",
					2 => "Rosso",
					3 => "Blu",
					4 => "Verde",
					5 => "Giallo" 
			),
			"#title" => "Colore 1",
			"#default_value" => check_plain(substr($team->shirt, -7, 1)),
			"#ahah" => array(
					"wrapper" => "shirt_preview",
					
					// default so not really needed, but included for clarity
					"method" => "replace",
					"event" => "change",
					"path" => "team/js" 
			)
			 
	);
	
	$form ["shirt"] ["color_2"] = array(
			"#type" => "select",
			"#options" => array(
					0 => "Bianco",
					1 => "Nero",
					2 => "Rosso",
					3 => "Blu",
					4 => "Verde",
					5 => "Giallo" 
			),
			"#title" => "Colore 2",
			"#default_value" => check_plain(substr($team->shirt, -5, 1)),
			"#ahah" => array(
					"wrapper" => "shirt_preview",
					
					// default so not really needed, but included for clarity
					"method" => "replace",
					"event" => "change",
					"path" => "team/js" 
			)
			 
	);
	
	$form ["shirt"] ["preview"] = array(
			"#prefix" => "<div id=\"shirt_preview\" style=\"float:left;\">",
			"#suffix" => "</div>",
			"#type" => "markup",
			"#value" => "<img src=\"" . base_path() . drupal_get_path("module", "fantacalcio") . "/images/shirts/" . $team->shirt . "\">" 
	);
	
	$form ["t_id"] = array(
			"#type" => "hidden",
			"#value" => check_plain($team->t_id) 
	)
	;
	
	$form ["action"] = array(
			"#type" => "hidden",
			"#value" => check_plain($action) 
	)
	;
	
	$form ["submit"] = array(
			"#type" => "submit",
			"#value" => "Ok" 
	);
	
	$form ["clear"] = array(
			"#type" => "submit",
			"#value" => "Annulla",
			"#validate" => array(
					"fantacalcio_squadra_form_clear" 
			) 
	)
	;
	
	return $form;
}

function fantacalcio_team_form_ahah () {
	$type = $_POST ["type"];
	$color_1 = $_POST ["color_1"];
	$color_2 = $_POST ["color_2"];
	
	$output = "<img src=\"" . base_path() . drupal_get_path("module", "fantacalcio") . "/images/shirts/shirt_" . $type . "_" . $color_1 . "_" . $color_2 . ".png\" >";
	print drupal_to_js(array(
			"data" => $output,
			"status" => true 
	));
	exit();
}

function fantacalcio_team_form_clear ($form, &$form_state) {
	$form_state ["rebuild"] = TRUE;
}

function fantacalcio_team_form_validate ($form, &$form_state) {
	$name = $form_state ["values"] ["name"];
	if (!$name)
		form_set_error("name", "Inserisci un nome per la squadra");
}

function fantacalcio_team_form_submit ($form, &$form_state) {
	
	global $user;
	
	$team_name = $form_state ["values"] ["name"];
	$team_coach = $form_state ["values"] ["coach"];
	$team_stadium = $form_state ["values"] ["stadium"];
	$team_shirt = "shirt_" . $form_state ["values"] ["type"] . "_" . $form_state ["values"] ["color_1"] . "_" . $form_state ["values"] ["color_2"] . ".png";
	$t_id = $form_state ["values"] ["t_id"];
	$action = $form_state ["values"] ["action"];
	
	if ($action == "update") {
		$sql = "UPDATE {fanta_teams} " . "SET name = \"%s\", " . "coach = \"%s\", " . "stadium = \"%s\", " . "shirt = \"%s\" " . "WHERE t_id = \"%d\"";
		$result = db_query($sql, $team_name, $team_coach, $team_stadium, $team_shirt, $t_id);
		return drupal_set_message("Squadra modificata con successo");
	} elseif ($action == "insert") {
		$sql = "INSERT INTO {fanta_teams} (name, coach, stadium, shirt, active, uid) " . "VALUES (\"%s\", \"%s\", \"%s\", \"%s\", 1, \"%d\")";
		$result = db_query($sql, $team_name, $team_coach, $team_stadium, $team_shirt, $user->uid);
		return drupal_set_message("Squadra creata con successo");
	}
}

function fantacalcio_get_title_team () {
	
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


function fantacalcio_teams_manager () {
	global $user;

	$teams = Team::allByUser($user->uid);

	//zero squadre --> Avviso
	if (count($teams) == 0)
		return t("Non hai ancora nessuna squadra");

	//1 squadra --> redirect
	if (count($teams) == 1)
		drupal_goto("mie/" . $teams[0]->id);

	//+1 squadre --> scelta
	if (count($teams) >= 1) {
		$own_teams_list = array();

		foreach($teams as $t_id => $team) {
			$classes = array();
			if ($team->isSquadComplete())
				array_push($classes, "bold");
			array_push($own_teams_list, array("data" => l($team->name, "mie/" . $team->id), "class" => $classes, "data-name" => $team->name));
		}

		return array(
				"#items" => $own_teams_list,
				"#theme" => "item_list",
		);
	}
}

function fantacalcio_team_manager () {
	return drupal_get_form("fantacalcio_team_form");
}

function fantacalcio_show_squad($t_id) {
	$content =  array(
			"squad" => array(
					"#type" => "markup",
					"#markup" => theme_table(show_team_squad($t_id)),
			),
			"t_id" => $t_id);
	
	return theme("squad", $content);
}

function fantacalcio_show_teams($t_id = "") {
	
	global $user;
	
// 	$args = array("t_id" => NULL, "c_id" => variable_get("fantacalcio_main_competition", 1), "c_name" => "", "selected_competition" => "");

// 	if (!empty($t_id) && team_exists($t_id))
// 		$args = array("t_id" => $t_id, "c_id" => variable_get("fantacalcio_main_competition", 1), "c_name" => "", "selected_competition" => "");

	$teams = Team::allActive();

	$teams_list = array();
	foreach ($teams as $team_id => $team) {
		$classes = array("list-group-item");
		if (isset($array_args["t_id"]) && $team_id == $t_id ) 
			array_push($classes, "active");
		if ($team->user == $user->uid)
			array_push($classes, "mine");
		
		$teams_list[] = array("data" => l($team->name, "squadre/" . $team->id), "class" => $classes, "data-name" => $team->name);
	}

	$output =  array(
			"teams_list" => array(
					"#items" => $teams_list,
					"#theme" => "item_list",
			));
	
	if(isset($t_id) && Team::get($t_id) != null) {
		$output = array_merge($output, show_team_data($t_id));
	}
	
	return theme("columns", $output);
}

function show_team_data ($t_id) {
	
	global $user;
	
	drupal_set_title(check_plain("Squadre - " . Team::get($t_id)->name));
	
	$output =  array(
			"squad" => array(
						"#type" => "markup",
						"#markup" => theme_table(show_team_squad($t_id)),
					),
			"details" => array(
					"#type" => "markup",
					"#markup" => theme_table(show_team_squad($t_id)),//TODO details					
			),
			
			"rounds" => array(
					"#type" => "markup",
					"#markup" => theme_table(show_team_rounds($t_id)),//TODO rounds
			),

			"stats" => array(
					"#type" => "markup",
					"#markup" => theme_table(show_team_squad($t_id)),//TODO stats
			),
			"is_own_team" => Team::get($t_id)->user == $user->uid,
			"team" => Team::get($t_id),
	);
	
	return $output;
}

function show_team_squad ($t_id) {
	
	global $user;
	
	$team = Team::get($t_id);
	
	if (!$team)
		return t("Squadra inesistente");
	
	$squad = $team->getSquad();
	
	$header = array(
			t("Ruolo"),
			t("Nome"),
			t("Squadra"),
			t("Costo"),			 
	);
	
	if(variable_get("fantacalcio_free_movements", 1) == 1) {
		array_push($header, t("Quotazione"));
	}
	array_push($header, "");
	
	$squad_rows = array();
	foreach ( $squad as $pl_id => $player ) {
		$squad_rows [$pl_id] = array(
				"<span class=\"fa-stack\">
					<i class=\"fa fa-square fa-stack-2x role-" . $player->role . "\"></i>
					<i class=\"fa fa-stack-1x\" style=\"color: white;\"><span class=\"font-normal\">" . Player::convertRole($player->role) . "</span></i>
				</span>",
				$player->name,
				ucfirst($player->team),
				$player->cost,				
		);
		if (variable_get("fantacalcio_free_movements", 1) == 1) 
			array_push($squad_rows[$pl_id], $player->quotation);
		array_push($squad_rows[$pl_id], "<a href=\"#\" data-toggle=\"modal\" data-target=\"#player-stats-modal\" class=\"player-stats\" id=\"player-stat-" . $pl_id . "\"><i class=\"fa fa-bar-chart\"></i></a>");
	}
	
	$squad = array(
			"header" => $header,
			"rows" => $squad_rows,
			"attributes" => array(
					"class" => array(
							"table table-responsive table-striped" 
					) 
			),
			"caption" => "",
			"empty" => "Nessun giocatore",
			"sticky" => TRUE,
			"colgroups" => array(),
			"theme" => "table"
	);
	
	return $squad;
}

function dettagli_squadra ($t_id = "") {
	
	$teams = array();
	$sql = "SELECT * FROM {fanta_teams} WHERE active = 1 AND t_id = :t_id";
	$result = db_query($sql, array(
			":t_id" => $t_id 
	));
	foreach ( $result as $row ) {
		$team = $row;
	}
	
	if (!empty($t_id) && team_exists($t_id)) {
		$rows [] = array(
				"<h4>" . t("Allenatore") . "</h4>",
				$team->coach 
		);
		$rows [] = array(
				"<h4>" . t("Stadio") . "</h4>",
				$team->stadium 
		);
		$rows [] = array(
				"<h4>" . t("Trofei") . "</h4>",
				theme_team_history($team->honours) 
		);
		
		$out = theme_table(array(
				"header" => array(),
				"rows" => $rows,
				"attributes" => array(
						"class" => array(
								"rosa",
								"table",
								"table-responsive" 
						) 
				),
				"caption" => "",
				"empty" => "",
				"sticky" => TRUE,
				"colgroups" => array() 
		));
	}
	
	return $out;
}

function show_team_rounds ($t_id) {
	
	if(variable_get("fantacalcio_league_type", 1) == LEAGUE_TYPE_GP || TRUE) {

		$rounds = array();
		
		$query = db_select("fanta_teams_rounds", "tr");
		$query->join("fanta_teams", "t", "t.t_id = tr.t_id");
		$query->join("fanta_rounds", "r", "r.round = tr.round");
		$query->join("fanta_rounds_competitions", "rc", "rc.round = r.round");
		$query->condition("tr.t_id", $t_id);
		$query->fields("tr");
		$query->fields("r");
		$query->fields("rc");
		$query->addField("t", "name");		
		$query->orderBy("r.round");
		
		$result = $query->execute();
		
		foreach ( $result as $row ) {
			
			$rounds[$row->round] = array($row->round, $row->points, $row->round_position, $row->season_position);
							
			if ($row->round == Round::getNextForCompetition($row->c_id) || $row->status == 1) {
				array_push($rounds[$row->round], l("<i class=\"fa fa-bar-chart\"></i>", "scheda/partita/" . $row->round, array(
						"html" => "true",
						"attributes" => array("class" => "_thickbox", "rel" => "lightframe[\"matches_" . $t_id . "\"|width:970px;]")
				)));
			} else
				array_push($rounds[$row->round], array("data" => "&nbsp;", "colspan" => 1));
		}
		
		return (array(
				"header" => array(t("Giornata"), t("Punteggio"), t("Posizione parziale"), t("Posizione stagionale"), ""),
				"rows" => $rounds,
				"attributes" => array("class" => array("squad", "table", "table-responsive")),
				"caption" => "",
				"empty" => "",
				"sticky" => TRUE,
				"colgroups" => array()
		));
	}
	else {
		$query = db_select("fanta_matches", "m");
		$query->join("fanta_teams", "t1", "t1.t_id = m.t1_id");
		$query->join("fanta_teams", "t2", "t2.t_id = m.t2_id");
		$or = db_or();
		$or->condition("m.t1_id", $t_id);
		$or->condition("m.t2_id", $t_id);
		$query->condition($or);
		$query->fields("m");
		$query->addField("t1", "name", "t1_name");
		$query->addField("t2", "name", "t2_name");
		$query->orderBy("m.round");
		
		$result = $query->execute();
		
		foreach ( $result as $row ) {
			$rows [$row->m_id] [] = $row->round;
			$rows [$row->m_id] [] = $row->t1_name . " - " . $row->t2_name;
			$rows [$row->m_id] [] = ($row->played == 1) ? $row->goals_1 . " - " . $row->goals_2 : "";
			
			if ($row->round == get_next_competition_round(get_cid_by_gid($row->g_id)) || $row->played == 1) {
				$rows [$row->m_id] [] = l("<img src=\"" . base_path() . drupal_get_path("module", "fantacalcio") . "/images/details.png\">", "scheda/partita/" . $row->m_id, array(
						"html" => "true",
						"attributes" => array("class" => "_thickbox", "rel" => "lightframe[\"matches_" . $t_id . "\"|width:970px;]") 
				));
			} else
				$rows [$row->m_id] [] = array(
						"data" => "&nbsp;",
						"colspan" => 1 
				);
		}
		
		return (array(
				"header" => array(t("Giornata"), t("Partita"), t("Risultati"), ""),
				"rows" => $rows,
				"attributes" => array("class" => array("squad", "table", "table-responsive")),
				"caption" => "",
				"empty" => "",
				"sticky" => TRUE,
				"colgroups" => array() 
		));
	}
}

function theme_team_history ($history) {
	
	if ($history) {
		$data = unserialize($history);
		$out = "";
		$out = "<ul class=\"team-history\">";
		if ($data) {
			foreach ( $data as $key => $value )
				$out .= "<li>" . $key . " <img src=\"" . base_path() . drupal_get_path("module", "fantacalcio") . "/images/" . variable_get($value, "") . "\"></li>";
		}
		$out .= "</ul>";
		return $out;
	}
}

function theme_team_last_year ($last_year) {
	if ($last_year) {
		$data = unserialize($last_year);
		$out = "";
		foreach ( $data as $key => $value )
			$out .= "<img src=\"" . base_path() . drupal_get_path("module", "fantacalcio") . "/images/" . variable_get($value, "") . "\">";
		
		return $out;
	}
}


function fantacalcio_squad_show_favourites()
{
	global $user;

	$team = get_team_by_user($user->uid);

	if (!$team)
		drupal_goto("crea-squadra");

	drupal_set_title($team->name . " - Lista movimenti", $output = CHECK_PLAIN);

	$rows = $team->getMovements();

	$header = array(t("Ruolo"), t("Nome"), t("Squadra"), t("Movimento"), t("Crediti"));

	$out = theme_table(array("header" => $header, "rows" => $rows, "attributes" => array("class" => array("table table-responsive table-striped")), "caption" => "", "empty" => "Nessun movimento", "sticky" => TRUE, "colgroups" => array()));

	$content["raw_markup"] = array(
			"#type" => "markup",
			"#markup" => $out,
	);

	return $content;
}

function fantacalcio_squad_edit($t_id)
{
	//solo per leghe di tipo Gran Premio
	if(variable_get("fantacalcio_league_type", 1) != LEAGUE_TYPE_GP)
		drupal_goto("mie/" . $t_id . "/lista-movimenti");
	
	drupal_add_js(drupal_get_path("module", "fantacalcio"). "/js/squad_list.js");

	global $user;

	$team = Team::get($t_id);

	if ($team->user != $user->uid)
		drupal_goto(drupal_access_denied());

	drupal_set_title($team->name . " - " . t("Gestione Rosa"), $output = CHECK_PLAIN);

// 	$squad_list_form = drupal_get_form("fantacalcio_squad_players_list_form");
// 	$out_left = drupal_render($squad_list_form);

	//lista giocatori
	$players_list = Player::allWithQuotation();

	//rosa
	$squad = $team->getSquad();
	$squad_rows = array();
	$bought_players_role = array("0" => 0, "1" => 0, "2" => 0, "3" => 0);
	$outflow = 0;

	foreach($squad as $pl_id => $player)
	{
// 		$sell_player_form = drupal_get_form("fantacalcio_sell_player_form", $t_id, $pl_id, $player->quotation);
	
		$squad_rows[$pl_id] = array("data" => array(
				"<span class='fa-stack'>
						<i class='fa fa-square fa-stack-2x role-" . $player->role . "'></i>
						<i class='fa fa-stack-1x' style='color: white;'><span class='font-normal'>" . Player::convertRole($player->role) . "</span></i>
					</span>",
				$player->name,
				ucfirst($player->team),
				$player->cost,
				$player->quotation,
				//drupal_render($sell_player_form)
				),
				"class" => array("squad-player-role-" . $player->role)
								
		);
		
		$outflow += $player->cost; 
		$bought_players_role[$player->role]++;
	}

	$list_header = array(t("Ruolo"), t("Nome"), t("Squadra"), t("Quotazione"), "");
	$list_rows = Player::listForSquad($players_list, $squad, $team->id);

	$content["players_list"] = array(
			"#type" => "markup",
			"#markup" => theme_table(array(
								"type" => "table",
								"header" => $list_header, 
								"rows" => $list_rows, 
								"attributes" => array("id" => "players-list", "class" => array("table table-responsive table-striped")), 
								"caption" => "", 
								"empty" => t("Nessun giocatore"), 
								"sticky" => TRUE, 
								"colgroups" => array()								
						))
	);
	
	$real_teams_items = array(array("data" => "", "id" => "", "class" => array()));
	
	$real_teams = RealTeam::all();
	array_push($real_teams_items, array("data" => l(" - ", "", array('fragment' => "rt-0", 'external' => TRUE)), "id" => "rt-0", "class" => array(), "role" => "presentation"));
	foreach($real_teams as $real_team) {
		array_push($real_teams_items, array("data" => l($real_team->name, "", array('fragment' => "rt-" . $real_team->id, 'external' => TRUE)), "id" => "rt-" . $real_team->id, "class" => array(), "role" => "presentation"));
	}
	
	$content["real_teams_list"] = array(
			"#items" => $real_teams_items,
			"#theme" => "item_list",
			"#type" => "ul",
			"#attributes" => array("id" => "real-teams-list", "class" => array("dropdown-menu"), "role" => "menu")
	);
	
	$content["squad_data"] = array(
			"#type" => "markup",
			"#markup" => theme_table(array(
					"type" => "table",
					"header" => array(t("Ruolo"), t("Nome"), t("Squadra"), t("Costo"), t("Quotazione"), ""),
					"rows" => $squad_rows,
					"attributes" => array("id" => "players-list", "class" => array("table table-responsive table-striped")),
					"caption" => "",
					"empty" => t("Nessun giocatore"),
					"sticky" => TRUE,
					"colgroups" => array()
			))
		);
	
	$content["credits"] = variable_get("fantacalcio_credits", 100);
	$content["outflow"] = $outflow;
	$content["expected_players"] = array("0" => variable_get("fantacalcio_number_role_0", 1), "1" => variable_get("fantacalcio_number_role_1", 1), "2" => variable_get("fantacalcio_number_role_2", 1), "3" => variable_get("fantacalcio_number_role_3", 1));
	$content["bought_players"] = $bought_players_role;
	$content["t_id"] = $t_id;

	return theme("movements", $content);
}

function fantacalcio_squad_movements($t_id)
{
	global $user;

	$team = Team::get($t_id);

	if (!$team)
		drupal_goto(drupal_access_denied());

	drupal_set_title($team->name . " - Lista movimenti", $output = CHECK_PLAIN);

	$rows = $team->getMovements();

	$header = array(t("Ruolo"), t("Nome"), t("Squadra"), t("Data"), t("Movimento"), t("Crediti"));
	
	$content["t_id"] = $t_id;
	$content["movements"] = array(
			"#type" => "markup",
			"#markup" => theme_table(array("header" => $header, "rows" => $rows, "attributes" => array("class" => array("table table-responsive table-striped")), "caption" => "", "empty" => "Nessun movimento", "sticky" => TRUE, "colgroups" => array()))
	);

	return theme("movements-list", $content);
}
<?php

/**
* @file fantacalcio.inc
* 
*/
function fantacalcio_team_form($form, $form_state, $id) {
  global $user;
  
    drupal_add_js(drupal_get_path("module", "fantacalcio") . "/js/team.shirt.js");
    
  if ($id == null)
    $team = null;
  else
    $team = Team::get($id);
  
  if ($team != null && $team->user !== $user->uid)
    drupal_goto(drupal_access_denied());
  
  $action = ($team) ? "update" : "insert";
  $title = ($team) ? t("Gestione squadra") . " - " . $team->name : t("Crea squadra");
  drupal_set_title(filter_xss($title));
  
  $form["#theme"] = "fantacalcio_team_form";
  
    $form["title"] = array("#title" => check_plain($title));
  
  // if (variable_get("fantacalcio_team_change_name", 0) == 1 || $team == null) {
  $form["name"] = array(
    "#type" => "textfield", 
    "#title" => "Nome squadra", 
    "#default_value" => $team ? check_plain($team->name) : "", 
    "#required" => TRUE, 
    
  );
    
    if ((variable_get("fantacalcio_team_change_name", 0) == 0 && $team != null)) {
    $form["name"]["#attributes"] = array("readonly" =>  "readonly");
    }
  
  /*$form["coach"] = array(
    "#type" => "textfield", 
    "#title" => "Allenatore", 
    "#default_value" => $team ? check_plain($team->coach) : "");
  
  $form["stadium"] = array(
    "#type" => "textfield", 
    "#title" => "Stadio", 
    "#default_value" => $team ? check_plain($team->stadium) : "");*/
  
   $form ["shirt"] = array(
   "#type" => "fieldset",
   "#title" => t("Divisa") 
   );
    
    $form ["shirt"] ["metadata"] = array(
        "#type" => "hidden",
        "#attributes" => array("id" => "metadata"),
   "#default_value" => $team ? $team->shirt_metadata : "",
   );
        
    $form ["shirt"] ["image"] = array(
        "#type" => "hidden",
        "#attributes" => array("id" => "image"),
   "#default_value" => $team ? json_decode($team->shirt) : "",
   );
   
    $form ["shirt"] ["type_0"] = array(
        "#type" => "radio",
        "#default_value" => $team && $team->shirt_metadata ? $team->shirt_metadata->type : "",
        //"#value" => "<i class='fa fa-user'></i>",
        "#prefix" => "<table><tr><td>" . t("Tipo") .': </td><td><div class="btn-group" data-toggle="buttons">' .  '<label class="btn btn-default color-type">' ,
        "#suffix" => '<img src="' . base_path(). drupal_get_path("module", "fantacalcio") . '/images/tipo_0.png" width="30"></label>',
        "#attributes" => array("class" => array("form-inline"), "id" => "type")
   );
    
    $form ["shirt"] ["type_1"] = array(
        "#type" => "radio",
   "#default_value" => $team && $team->shirt_metadata ? $team->shirt_metadata->type : "",
        "#prefix" => '<label class="btn btn-default color-type">',
        "#suffix" => '<img src="' . base_path(). drupal_get_path("module", "fantacalcio") . '/images/tipo_1.png" width="30"></label>',
                "#attributes" => array("class" => array("btn", "btn-default"), "id" => "type-1")
   );
    $form ["shirt"] ["type_2"] = array(
        "#type" => "radio",
   "#default_value" => $team && $team->shirt_metadata ? $team->shirt_metadata->type : "",
        "#prefix" => '<label class="btn btn-default color-type">',
        "#suffix" => '<img src="' . base_path(). drupal_get_path("module", "fantacalcio") . '/images/tipo_2.png" width="30"></label>',
                "#attributes" => array("class" => array("btn", "btn-default"), "id" => "type-2")
   );
    
    $form ["shirt"] ["type_3"] = array(
        "#type" => "radio",
   "#default_value" => $team && $team->shirt_metadata ? $team->shirt_metadata->type : "",
        "#prefix" => '<label class="btn btn-default color-type">',
        "#suffix" => '<img src="' . base_path(). drupal_get_path("module", "fantacalcio") . '/images/tipo_3.png" width="30"></label>',
                "#attributes" => array("class" => array("btn", "btn-default"), "id" => "type-3")
   );
    $form ["shirt"] ["type_4"] = array(
        "#type" => "radio",
   "#default_value" => $team && $team->shirt_metadata ? $team->shirt_metadata->type : "",
        "#prefix" => '<label class="btn btn-default color-type">',
        "#suffix" => '<img src="' . base_path(). drupal_get_path("module", "fantacalcio") . '/images/tipo_4.png" width="30"></label>',
                "#attributes" => array("class" => array("btn", "btn-default"), "id" => "type-4")
   );
    
    $form ["shirt"] ["type_5"] = array(
        "#type" => "radio",
   "#default_value" => $team && $team->shirt_metadata ? $team->shirt_metadata->type : "",
        "#prefix" => '<label class="btn btn-default color-type">',
        "#suffix" => '<img src="' . base_path(). drupal_get_path("module", "fantacalcio") . '/images/tipo_5.png" width="30"></label>',
                "#attributes" => array("class" => array("btn", "btn-default"), "id" => "type-5")
   );
    $form ["shirt"] ["type_6"] = array(
        "#type" => "radio",
   "#default_value" => $team && $team->shirt_metadata ? $team->shirt_metadata->type : "",
        "#prefix" => '<label class="btn btn-default color-type">',
        "#suffix" => '<img src="' . base_path(). drupal_get_path("module", "fantacalcio") . '/images/tipo_6.png" width="30"></label>',
                "#attributes" => array("class" => array("btn", "btn-default"), "id" => "type-6")
   );
    
    $form ["shirt"] ["type_7"] = array(
        "#type" => "radio",
   "#default_value" => $team && $team->shirt_metadata ? $team->shirt_metadata->type : "",
        "#prefix" => '<label class="btn btn-default color-type">',
        "#suffix" => '<img src="' . base_path(). drupal_get_path("module", "fantacalcio") . '/images/tipo_7.png" width="30"></label>',
                "#attributes" => array("class" => array("btn", "btn-default"), "id" => "type-7")
   );
    $form ["shirt"] ["type_8"] = array(
        "#type" => "radio",
   "#default_value" => $team && $team->shirt_metadata ? $team->shirt_metadata->type : "",
        "#prefix" => '<label class="btn btn-default color-type">',
        "#suffix" => '<img src="' . base_path(). drupal_get_path("module", "fantacalcio") . '/images/tipo_8.png" width="30"></label>',
                "#attributes" => array("class" => array("btn", "btn-default"), "id" => "type-8")
   );
    
    $form ["shirt"] ["type_9"] = array(
        "#type" => "radio",
   "#default_value" => $team && $team->shirt_metadata ? $team->shirt_metadata->type : "",
        "#prefix" => '<label class="btn btn-default color-type">',
        "#suffix" => '<img src="' . base_path(). drupal_get_path("module", "fantacalcio") . '/images/tipo_9.png" width="30"></label></td></tr>',
                "#attributes" => array("class" => array("btn", "btn-default"), "id" => "type-9")
   );
          
    $colors_list = array("#FFFFFF", "#FFF0F5", "#FFDEAD", "#FFB6C1", "#DEB887", "#F4A460", "#FFA07A", "#EE82EE", "#D8BFD8", "#FF7F50", "#DB7093", "#C71585", "#FF00FF", "#FF1493", "#DC143C", "#7B68EE", "#8A2BE2", "#483D8B", "#4B0082", "#800080", "#800000", "#FF0000", "#FF4500", "#B22222", "#8B4513", "#A0522D", "#D2691E", "#B8860B", "#FFA500", "#DAA520", "#FFFF00", "#00FF00", "#7FFF00", "#F0E68C", "#BDB76B", "#8FBC8F", "#9ACD32", "#90EE90", "#ADFF2F", "#32CD32", "#228B22", "#006400", "#556B2F", "#6B8E23", "#808000", "#00FF7F", "#00FA9A", "#66CDAA", "#3CB371", "#2E8B57", "#008080", "#20B2AA", "#00FFFF", "#AFEEEE", "#7FFFD4", "#5F9EA0", "#B0C4DE", "#87CEEB", "#1E90FF", "#6495ED", "#4169E1", "#191970", "#00008B", "#0000FF", "#00BFFF", "#B0E0E6", "#E6E6FA", "#D3D3D3", "#808080", "#778899", "#2F4F4F", "#000000");
    
    $colors = '<ul class="dropdown-menu">
													
							<li>

								<div class="panel panel-default">
																	   
									<div class="panel-body">
										
										<ul class="list-inline">';
    
                                        $i = 1;
                                        foreach($colors_list as $color) {
                                            if (!empty($color)) {
                                                $colors .= '<li><div id="color-' . $color . '" class="swatch swatch-clickable" style="background-color:' . $color . '"></div></li>';
                                                if ($i % 8 == 0) {
                                                    $colors .= "</ul><ul class='list-inline'>";
                                                }

                                                $i++;
                                            }
                                        }
											
										$colors .= '</ul>

									</div>
										
								</div>
								
							</li>
								
						</ul>';
   
    $form ["shirt"] ["bgColor"] = array(
        "#type" => "hidden",
   "#title" => t("Colore 1"),
    "#prefix" => "<tr><td>" . t("Colore 1") . "</td><td>",
        "#suffix" =>  "<div class='form-group'><div class='input-group-btn group-color'><button type='button' class='btn btn-default btn-box change-color dropdown-toggle' data-toggle='dropdown'>
							<span class='fa fa-square preview'></span> 
						</button>" . $colors . '</div></div></td></tr>',
   "#default_value" => $team && $team->shirt_metadata ? $team->shirt_metadata->bgColor : "",
    "#attributes" => array("class" => array("btn", "btn-default", "color-val"), "id" => "bg-color")
   );
    
    $form ["shirt"] ["mainColor"] = array(
            "#type" => "hidden",
   "#title" => t("Colore 2"),
    "#prefix" => "<tr><td>" . t("Colore 2") . "</td><td>",
        "#suffix" =>  '<div class="form-group"><div class="input-group-btn group-color"><button type="button" class="btn btn-default btn-box change-color dropdown-toggle" data-toggle="dropdown">
							<span class="fa fa-square preview"></span> 
						</button>' . $colors . '</td></tr>',
   "#default_value" => $team && $team->shirt_metadata ? $team->shirt_metadata->mainColor : "",
        "#attributes" => array("class" => array("btn", "btn-default", "color-val"), "id" => "main-color")
   );
    
    $form ["shirt"] ["border"] = array(
        "#type" => "checkbox",
   /*"#title" => t("Bordo"),*/
        "#prefix" => "<tr><td>" . t("Bordo") . "</td><td>",
        "#suffix" => "</td></tr>",
           "#default_value" => $team && $team->shirt_metadata ? $team->shirt_metadata->border : "",
        "#attributes" => array("id" => "border")
   );
    
    $form ["shirt"] ["borderColor"] = array(
        "#type" => "hidden",
   "#title" => t("Colore bordo"),
    "#prefix" => "<tr><td>" . t("Colore bordo") . "</td><td>",
        "#suffix" =>  '<div class="form-group"><div class="input-group-btn group-color"><button type="button" class="btn btn-default btn-box change-color dropdown-toggle" data-toggle="dropdown">
							<span class="fa fa-square preview"></span> 
						</button>' . $colors . '</td></tr></table><canvas id="myCanvas" width="350" height="350"></canvas>',
   "#default_value" => $team && $team->shirt_metadata ? $team->shirt_metadata->borderColor : "",
        "#attributes" => array("class" => array("btn", "btn-default", "color-val"), "id" => "border-color")
   );
  
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

function fantacalcio_team_form_clear($form, &$form_state) {
  $form_state["rebuild"] = TRUE;
}

function fantacalcio_team_form_validate($form, &$form_state) {
    //update
    if ($form_state["values"]["action"] == "update") {
        $name = $form_state["values"]["name"];
        if (!$name && Team::getByName($name) != null)
            form_set_error("name", t("Inserisci un nome per la squadra"));
        if ($form_state["values"]["action"] == "insert" && Team::getByName($name) != null)
            form_set_error("name", t("Il nome della squadra è già stata utilizzato"));
    }
    //create
    else {
        $name = $form_state["values"]["name"];
        if (!$name && Team::getByName($name) != null)
            form_set_error("name", t("Inserisci un nome per la squadra"));
        if ($form_state["values"]["action"] == "insert" && Team::getByName($name) != null)
            form_set_error("name", t("Il nome della squadra è già stata utilizzato"));
    }
    
}

function fantacalcio_team_form_submit($form, &$form_state) {
  global $user;
  
  $team_name = $form_state["values"]["name"];
  $team_coach = "";//$form_state["values"]["coach"];
  $team_stadium = "";//$form_state["values"]["stadium"];
  $team_shirt = $form_state["values"]["image"]; // shirt_" . $form_state ["values"] ["type"] . "_" . $form_state ["values"] ["color_1"] . "_" . $form_state ["values"] ["color_2"] . ".png";
  $team_shirt_metadata = $form_state["values"]["metadata"]; // shirt_" . $form_state ["values"] ["type"] . "_" . $form_state ["values"] ["color_1"] . "_" . $form_state ["values"] ["color_2"] . ".png";
  $t_id = $form_state["values"]["t_id"];
  $action = $form_state["values"]["action"];
    
    $fields = array(
      'coach' => $team_coach, 
      'stadium' => $team_stadium, 
      "shirt_metadata" => $team_shirt_metadata,
      "shirt" => $team_shirt);
    
    if (variable_get("fantacalcio_team_change_name", 0) == 1)
        $fields["name"] = $team_name;
  
  if ($action == "update") {
    $query = db_update('fanta_teams');
    $query->fields($fields);
    $query->condition('t_id', $t_id);
    $query->execute();
    
    // $sql = "UPDATE {fanta_teams} " . "SET name = \"%s\", " . "coach = \"%s\", " . "stadium = \"%s\", " . "shirt = \"%s\" " . "WHERE t_id = \"%d\"";
    // $result = db_query($sql, $team_name, $team_coach, $team_stadium, $team_shirt, $t_id);
    return drupal_set_message(t("Squadra modificata con successo"));
  }
  elseif ($action == "insert") {
    
    $t_id = db_insert('fanta_teams')->fields(array(
      "name" => $team_name, 
      "coach" => $team_coach, 
      "stadium" => $team_stadium, 
      "shirt" => $team_shirt, 
      "shirt_metadata" => "", 
      "credits" => variable_get("fantacalcio_credits", 0), 
      "active" => 1, 
      "uid" => $user->uid))->execute();
    
    $group_keys = array_keys(Competition::getDefault()->groups);
    $g_id = $group_keys[0];
    
    db_insert("fanta_teams_groups")->fields(array("t_id" => $t_id, "g_id" => $g_id, "active" => 1))->execute();
    
    drupal_set_message(t("Squadra creata con successo"));
  }
  
  drupal_goto("mie");
}

function theme_fantacalcio_team_form($variables) {
    
    $form = $variables["form"];
    
    $out = "";
    
    return $out;
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
    $prefix = l(t("Aggiungi squadra"), "mie/add", array("attributes" => array("class" => array("pull-left", "btn", "btn-sm", "btn-success"))));
  
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
  $credits = Team::get($t_id)->credits;//variable_get("fantacalcio_credits", 1000);
  
  $query = db_select("fanta_squads", "s");
  $query->condition("t_id", $t_id);
  $query->fields("s");
  
  $result = $query->execute();
  
  foreach ($result as $row) {
    $spent += $row->status == 1 ? $row->cost : ceil($row->cost / 2);
  }
  
  //scambi
  $query = db_select("fanta_squad_changes", "s");
  $query->condition("t1_id", $t_id);
  $query->fields("s");
  
  $result = $query->execute();
  
  foreach ($result as $row) {
    $spent += $row->money;
  }

  $query = db_select("fanta_squad_changes", "s");
  $query->condition("t2_id", $t_id);
  $query->fields("s");
  
  $result = $query->execute();
  
  foreach ($result as $row) {
    $spent -= $row->money;
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
        array_push($rounds[$row->round], l("<i class=\"fa fa-bar-chart\"></i>", "scheda/giornata/" . Competition::getDefault()->sanitized_name . "/" . $row->round . "/" . $t_id, array(
          "html" => "true", 
          "attributes" => array( 
            "data-toggle" => "modal",
	    "data-target" => "#match-details-modal"))));
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
            "data-toggle" => "modal",
	    "data-target" => "#match-details-modal")));
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
    
    $rows[] = array(t("Posizione attuale"), $position . "&deg;");
    $rows[] = array(
      t("Punteggio attuale"), 
      $team->statPoints($default_competition) . " pt");
    
    if ($position == 1)
      $rows[] = array(
        t("Distacco dal secondo"), 
        $team->statGap($default_competition) . " pt");
    else
      $rows[] = array(
        t("Distacco dal primo"), 
        $team->statGap($default_competition) . " pt");
    
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
    
    $points_chart_js = "jQuery(document).ready(function () {
    	jQuery('a[href=\"#stats\"], #stats').on('shown.bs.tab', function () {

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
		});
		});";
    
    drupal_add_js($points_chart_js, "inline");
    
    $round_positions_chart = "";
    if ($default_competition->type == COMPETITION_TYPE_GP) {
    $round_positions = $team->getRoundsPositions($default_competition);
    array_reverse($round_positions);
    
    $round_positions_chart = "<div class=\"row\">
			<div class=\"cols-xs-12\">
			<label>" . t("Posizioni giornata") . "</label><br>
			<canvas id=\"positionsChart\" style='width:100%; height: 200px'></canvas>
			</div>
			</div>";
    
    $round_positions_chart_js = "jQuery(document).ready(function () {
    	jQuery('a[href=\"#stats\"], #stats').on('shown.bs.tab', function () {
		var ctx = jQuery('#positionsChart').get(0).getContext('2d');
		var data = {
			labels: [" . implode(", ", array_keys($round_positions)) . "],
			datasets: [
			{
				label: '" . t("Punteggio") . "',
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
		});
		});";
    
    drupal_add_js($round_positions_chart_js, "inline");
    }
    
    $season_position_chart = "";
    $matches_chart = "";
    
   // if ($default_competition->type == COMPETITION_TYPE_GP) {
      $season_position_chart = "<div class=\"row\">
				<div class=\"cols-xs-12\">
				<label>" . t("Posizioni stagione") . "</label><br>
				<canvas id=\"seasonPositionsChart\" style='width:100%; height: 200px'></canvas>
				</div>
				</div>";
      
      $season_positions = $team->getRoundsPositions($default_competition);
//       $season_positions = array_reverse($season_positions);
      
      $season_positions_js = "jQuery(document).ready(function () {
      	jQuery('a[href=\"#stats\"], #stats').on('shown.bs.tab', function () {
			var ctx = jQuery('#seasonPositionsChart').get(0).getContext('2d');
			var data = {
				labels: [" . implode(", ", array_keys($season_positions)) . "],
				datasets: [
				{
					label: '" . t("Punteggi") . "',
					yAxisID: 'y-axis',
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
			});
			});";
      
      drupal_add_js($season_positions_js, "inline");
    //}
    
    //else 
    $matches_chart = "";
      if ($default_competition->type == COMPETITION_TYPE_SD) {
        $matches_chart = "<div class=\"row\">
				<div class=\"cols-xs-12\">
				<label>" . t("Partite") . "</label><br>
				<canvas id=\"matchesChart\" style='width:100%; height: 200px'></canvas>
				</div>
				</div>";
        
        $matches = $team->getMatchesResults($default_competition);
        
        $matches_js = "jQuery(document).ready(function () {
        	jQuery('a[href=\"#stats\"], #stats').on('shown.bs.tab', function () {
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
			});
			});";
        
        drupal_add_js($matches_js, "inline");
      }
    
    return theme("table", $stats) . $points_chart . $round_positions_chart . $season_position_chart . $matches_chart;
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
        "<button class=\"btn btn-sm btn-warning sell-player\" onclick=\"sellPlayer(" . $pl_id . ", " . $t_id . ");\" id=\"sell-" . $pl_id . "\">" . t("Vendi") . "</button>"), 
      "class" => array("player", "squad-player-role-" . $player->role));
    
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
  
  $check = fantacalcio_sell_player_form_validate_inner($form_state["input"]["t_id"], $form_state["input"]["pl_id"]);
  
      if(!$check)
         form_set_error('', $check);
}

function fantacalcio_sell_player_form_submit($form, &$form_state) {
  fantacalcio_sell_player_function($form_state["input"]["t_id"], $form_state["input"]["pl_id"]);
    
      drupal_set_message(t("Cessione completata"), "status");
}

function fantacalcio_sell_player_form_validate_inner($t_id, $player) {
  
  global $user;
  
  $team = Team::get($t_id);
  
  // verifica squadra appartenente all'utente
  if ($user->uid != $team->user)
    return array("success" => false, "error" => t("Impossibile completare l'operazione.") . " " . t('La squadra non appartiene all\'utente'));
    
  $credits = $team->getCredits();
  $movements_count = $team->getMovementsCount();
  $num_players = $team->getNumberPlayers();
  
  $max_movements = 10000;//variable_get("fantacalcio_max_movements", 0) > 0 ? variable_get("fantacalcio_max_movements", 0) : PHP_INT_MAX;
    
  // verifico numero movimenti rimasti
  $needed_movements = variable_get("fantacalcio_number_players", 25) - array_sum($num_players) - 1;
  
  /*if ($needed_movements > intval($max_movements - $movements_count));
    return array("success" => false, "error" => t("Impossibile completare l'operazione.") . " " . t('I movimenti a disposizione non sono sufficienti'));*/
    
  return array("success" => true);
}

function fantacalcio_sell_player_function($t_id, $pl_id) {
  global $user;
  
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
            
      watchdog("fantacalcio", t("%team: Cessione di %player (+%quotation crediti)"), array(
        "%team" => $team->name, 
        "%player" => $player->name, 
        "%quotation" => $player->getQuotation()), WATCHDOG_INFO);
    return array("success" => true);
    }
      else
      return array("success" => false, "error" => t("Giocatore non presente in rosa"));
  }
    else
    return array("success" => false, "error" => t("Modifiche non consentite su questa squadra"));
}

function fantacalcio_sell_player_ajax($t_id, $pl_id) {
    
    $result = fantacalcio_sell_player_form_validate_inner($t_id, $pl_id);
    if ($result["success"] == true)
        $result = fantacalcio_sell_player_function($t_id, $pl_id);
        
    return drupal_json_output(array_merge($result, fantacalcio_get_squad($t_id)));
}

function fantacalcio_get_squad($t_id) {
   
    $team = Team::get($t_id);
    
    $squad = $team->getSquad();
  $squad_rows = array();
  $bought_players_role = array("0" => 0, "1" => 0, "2" => 0, "3" => 0);
  $outflow = 0;
  
  foreach ($squad as $pl_id => $player) {
    
    $squad_rows[] = array(
        "pl_id" => $pl_id, 
        "name" => $player->name, 
        "team" => ucfirst($player->team), 
          "role" => $player->role,
        "cost" => $player->cost, 
        "quotation" => $player->quotation
      );
    
    $outflow += $player->cost;
    $bought_players_role[$player->role]++;
  }
  
  $content["squad_data"] = $squad_rows;
  
  $content["max_credits"] = variable_get("fantacalcio_credits", 100);
  
    $content["outflow"] = $outflow;
  
    $content["credits"] = variable_get("fantacalcio_credits", 100) - $outflow;
    
  $content["expected_players"] = array(
    "0" => variable_get("fantacalcio_number_role_0", 1), 
    "1" => variable_get("fantacalcio_number_role_1", 1), 
    "2" => variable_get("fantacalcio_number_role_2", 1), 
    "3" => variable_get("fantacalcio_number_role_3", 1));
  $content["bought_players"] = $bought_players_role;
  
  $content["movements"] = variable_get("fantacalcio_max_movements", 0);
  $content["left_movements"] = variable_get("fantacalcio_max_movements", 0) - $team->getMovementsCount();
  
  $content["is_team_complete"] = $team->canConfirm();
  
  return $content;
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
    
$check = fantacalcio_buy_player_form_validate_inner($form_state["input"]["t_id"], $form_state["input"]["pl_id"]);

    if(!$check)
        form_set_error('', $check);
}

function fantacalcio_buy_player_form_validate_inner($t_id, $pl_id) {
  // TODO controllare squadra appartenenza, crediti rimasti, numero giocatori, numero movimenti
  global $user;
  
  $team = Team::get($t_id);
  $player = Player::get($pl_id);
  
  $credits = $team->getCredits();
  $movements_count = $team->getMovementsCount();
  $num_players = $team->getNumberPlayers();
  
  $max_movements = 10000;//variable_get("fantacalcio_max_movements", 0) > 0 ? variable_get("fantacalcio_max_movements", 0) : PHP_INT_MAX;
  
  // verifica squadra appartenente all'utente
  if ($user->uid != $team->user)
    return array("success" => false, "error" => t("Impossibile completare l'operazione.") . " " . t('La squadra non appartiene all\'utente'));
    
    // verifico numero movimenti rimasti
  if ($movements_count > $max_movements)
    return array("success" => false, "error" => t("Impossibile completare l'operazione.") . " " . t('I movimenti a disposizione sono terminati'));
    
    // verifico crediti
  if ($player->getQuotation() > $credits)
    return array("success" => false, "error" => t("Impossibile completare l'operazione.") . " " . t('I crediti rimasti non sono sufficienti a completare l\'acquisto'));
    
    // verifico numero giocatori in squadra
  if (array_sum($num_players) >= variable_get("fantacalcio_number_players", 25))
    return array("success" => false, "error" => t("Impossibile completare l'operazione.") . " " . t('La squadra &egrave; gi&agrave; al completo'));
    
    // verifico numero giocatori del ruolo    
  if (isset($num_players[$player->role]) && $num_players[$player->role] >= variable_get("fantacalcio_number_role_" . $player->role, 0))
    return array("success" => false, "error" => t("Impossibile completare l'operazione.") . " " . t('Il ruolo &egrave; gi&agrave; al completo'));
  
    return array("success" => true);
}

function fantacalcio_buy_player_form_submit($form, &$form_state) {
  fantacalcio_buy_player_function($form_state["input"]["t_id"], $form_state["input"]["pl_id"], $form_state["input"]["search_string"]);
    drupal_set_message(t("Acquisto completato"), "status");
}

function fantacalcio_buy_player_function($t_id, $pl_id) {
  
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
      
      //drupal_goto("mie/" . $t_id . "/movimenti", array(
        //"query" => array("s" => $s)));
      
      watchdog("fantacalcio", t("%team: Acquisto di %player (-%quotation crediti)"), array(
        "%team" => $team->name, 
        "%player" => $player->name, 
        "%quotation" => $player->getQuotation()), WATCHDOG_INFO);
        
        return array("success" => true);
    }
      else
    return array("success" => false, "error" => t("Giocatore già presente in rosa"));
  }
    else
    return array("success" => false, "error" => t("Modifiche non consentite su questa squadra"));
}

function fantacalcio_buy_player_ajax($t_id, $pl_id) {
    
    $result = fantacalcio_buy_player_form_validate_inner($t_id, $pl_id);
    if ($result["success"] === true)
        $result = fantacalcio_buy_player_function($t_id, $pl_id);    
            
    return drupal_json_output(array_merge($result, fantacalcio_get_squad($t_id)));
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
 // $max_teams_for_user = min(Team::getMaxNumberForUser($user->uid), $max_teams_for_user);

  $teams_for_user = count(Team::allByUser($user->uid));
  
  if ($teams_for_user < $max_teams_for_user)
    return TRUE;
  else
    return FALSE;
}
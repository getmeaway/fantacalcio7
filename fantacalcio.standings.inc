  <?php
/**
* @file fantacalcio.standings.inc
* 
*/

function fantacalcio_standings($name = '', $type = null, $round = null) {

	$name = str_replace("-", " ", $name);
	
	$competition = Competition::getByName($name);
	
  if ($competition == null || $competition->has_standings != 1) { 
    $items = Competition::choose(array("has_standings" => 1, "active" => 1), "classifica");
     
    return theme_item_list(array("items" => $items, "attributes" => array("class" => array("list-group")), "type" => "ul", "title" => ""));
  }
  else {
    drupal_set_title(check_plain(t("Classifica") . " - " . ucwords($name)));
    
    if ($type == 'f') {
		drupal_set_title(check_plain(t("Classifica") . " - " . ucwords($name) . " - Final 8"));
    	return get_standing_table_f($competition, 1, 14, 8);
	}
	else if ($competition->type == COMPETITION_TYPE_GP)
    	return get_standing_table_gp($competition, $round);
    else if($competition->type == COMPETITION_TYPE_SD)
    	return get_standing_table_sd($competition);
	
  }

}

function get_standing_table_sd($competition) {
	
	drupal_add_js(drupal_get_path("module", "fantacalcio") . "/js/jquery.tablesorter.js");
	drupal_add_js(drupal_get_path("module", "fantacalcio") . "/js/jquery.metadata.js");
	drupal_add_js("jQuery(window).ready(function() {jQuery(\".standings	\").tablesorter({cssAsc: \"sort-asc\", cssDesc: \"sort-desc\", cssHeader: \"sort-header\"});})", "inline");

  global $user;
  
  $output = array();
  $index = 0;
  $already_active = false;

  $standing_f8 = array();
//$i = 0;
  foreach($competition->groups as $g_id => $group) {
    $teams = Team::allByGroup($g_id);
//$j = 0;
    foreach ($teams as $t_id => $team) {
	$standing_f8[$t_id] = $team->pointsForF8($g_id, 1, 14);
//$standing_f8[$t_id] = $i * 10 + $j;
//$j++;
    }
//$i++;
  }

  asort($standing_f8);
  $standing_f8 = array_reverse($standing_f8, true);

  $_standing_f8 = array();
  $index = 1;
  foreach($standing_f8 as $t_id => $points) {
	$_standing_f8[$t_id] = $index;
        $index++;
  }

  $standing_f8 = $_standing_f8;
//print_r($standing_f8);  
  foreach($competition->groups as $g_id => $group) {
  	
  	if ($group->standings_order > 0) {
  	
	  	$index++;
	
	  	$group = Group::get($g_id);
	  
	  	$teams = Team::allByGroup($g_id);
	  	$points = array();
	  	$goals_for = array();
	  	$points_for = array();
	  	
	  	$header = array();
	  	$rows = array();
	  	
	  	if ($teams) {
	
		    foreach ($teams as $t_id => $team) {
		
			    #$teams[$t_id]['n_played'] = 0 + get_num_played($t_id, $g_id);
			    $teams[$t_id]->n_win = 0 + $team->numWin($g_id);//get_num_win($t_id, $g_id);
			    $teams[$t_id]->n_lost = 0 + $team->numLost($g_id);//get_num_lost($t_id, $g_id);
			    $teams[$t_id]->n_draw = 0 + $team->numDraw($g_id);//get_num_draw($t_id, $g_id);
			    $teams[$t_id]->goals_in = 0 + $team->goalsFor($g_id);//get_goals_in($t_id, $g_id);
			    $teams[$t_id]->goals_against = 0 + $team->goalsAgainst($g_id);//get_goals_out($t_id, $g_id);
			    $teams[$t_id]->points_in = 0 + $team->pointsFor($g_id);//get_points_in($t_id, $g_id);
			    $teams[$t_id]->points_in_f8 = 0 + $team->pointsForF8($g_id, 1, 14);//get_points_in($t_id, $g_id);
			    $teams[$t_id]->points_against = 0 + $team->pointsAgainst($g_id);//get_points_out($t_id, $g_id);
			    $teams[$t_id]->pts = $teams[$t_id]->n_win * 3 + $teams[$t_id]->n_draw * 1;
			    $points[$t_id] = $teams[$t_id]->n_win * 3 + $teams[$t_id]->n_draw * 1;
			    $goals_for[$t_id] = $teams[$t_id]->goals_in;
			    $points_for[$t_id] = $teams[$t_id]->points_in;
			}
	
			array_multisort($points, SORT_DESC, $goals_for, SORT_DESC, $points_for, SORT_DESC, $teams);
		  
			$header = array("", t("Squadra"), t("Pts"), t("Vinte"), t("Perse"), t("Pari"), t("Gol f."), t("Gol s."), t("Diff Gol"), t("Pti f."), t("Pti s."), t("Diff Pti"), t("Pti F8"), t("Posizione F8"));
		
			$i = 0;
			$rows = array();
	
			foreach ($teams as $t_id => $team) {
		
		    	$i++;
		    	
		    	$team_class = array();
		    	if ($team->user == $user->uid)
		    	   array_push($team_class, "bold");
		
		    	$rows[$t_id][] = array("data" => $i, "data-title" => t("Posizione"));
			    $rows[$t_id][] = array("data" => l($team->name, "squadre/" . $team->id), "data-title" => t("Squadra"), "class" => $team_class);
			    $rows[$t_id][] = array("data" => $team->pts, "data-title" => t("Punti"));
			    $rows[$t_id][] = array("data" => $team->n_win, "data-title" => t("Vinte"));
			    $rows[$t_id][] = array("data" => $team->n_lost, "data-title" => t("Perse"));
			    $rows[$t_id][] = array("data" => $team->n_draw, "data-title" => t("Pari"));
			    $rows[$t_id][] = array("data" => $team->goals_in, "data-title" => t("gol f."));
			    $rows[$t_id][] = array("data" => $team->goals_against, "data-title" => t("Gol s."));
			    $rows[$t_id][] = array("data" => ($team->goals_in - $team->goals_against), "data-title" => t("Diff. Gol"));
			    $rows[$t_id][] = array("data" => $team->points_in, "data-title" => t("Pti f."));
			    $rows[$t_id][] = array("data" => $team->points_against, "data-title" => t("Pti s."));
			    $rows[$t_id][] = array("data" => ($team->points_in - $team->points_against), "data-title" => t("Diff. Pti"));
			    $rows[$t_id][] = array("data" => $team->points_in_f8, "data-title" => t("Pti F8"));
			    $rows[$t_id][] = array("data" => $standing_f8[$team->id], "data-title" => t("Posizione F8"));
		
			}
	
		}
	  	
		$group->output = theme("table", array("header" => $header, "rows" => $rows, "attributes" => array("class" => "collapse-table table table-responsive standings", "id" => "classifica_" . $g_id), "sticky" => "", "empty" => ""));
	  	
		if (isset($user->groups)) {
		  if (in_array($g_id, $user->groups[$competition->id]) && !$already_active) {
		    $group->active = true;
		    $already_active = true;
		  }
		  else
		    $group->active = false;
		}
		else if ($index == 1)
		  $group->active = true;
		else
		  $group->active = false;
	  		
	  	$groups[$g_id] = $group;
	  	
  		}
	}
   
	$output["groups"] = $groups;
    
	return theme("standings", $output);
}

function get_standing_table_gp($competition, $round = '') {

	drupal_add_js(drupal_get_path("module", "fantacalcio") . "/js/jquery.tablesorter.js");
	drupal_add_js(drupal_get_path("module", "fantacalcio") . "/js/jquery.metadata.js");
	drupal_add_js("jQuery(window).ready(function() {jQuery(\".standings	\").tablesorter({cssAsc: \"sort-asc\", cssDesc: \"sort-desc\", cssHeader: \"sort-header\"});})", "inline");
	
	global $user;
	
	$index = 0;
	
	foreach($competition->groups as $g_id => $group) {
		
		if ($group->standings_order > 0) {
		
			$index++;
			
			$points = array();
			$register_date = array();
			$positions = array();
	
			$standing_round = FALSE;
			if($round != '' && Round::exists($round))
				$standing_round = TRUE;
			 
			if ($standing_round)
				drupal_set_title(t("Classifica") . " - " . $round . "&ordf; giornata", PASS_THROUGH);
		
			$teams = Team::allByGroup($g_id);
		
			$out = "";
// 			$out .= "<div class='navbar navbar-default navbar-static-top'>" 
// 					. show_rounds_list($rounds_list, "classifica/" . $selection, array("key" => "season", "value" => t("Classifica completa")))
// 					. "</div>";
		
			//if ($selection == ALL_TEAMS)
				//$out .= search_teams_box("classifica/all" . ((!empty($round) && is_numeric($round)) ? "/" . $round : ""), $search);
		
			if ($standing_round)
				$header = array(t("Posizione"), t("Squadra"), t("Punti"), t("Posizione Stagione"));
			else
				$header = array(t("Posizione"), t("Squadra"), t("Punti"), t("Punteggio Massimo"), t("Punteggio Minimo"));
		
			$rows = array();
		
			if ($teams) {
		
				foreach ($teams as $t_id => $team) {
					$teams[$t_id]->points = (!empty($round) && Round::exists($round)) ? $team->points($competition->id, $round) : $team->points($competition->id, null);
					$teams[$t_id]->points_max = $team->pointsMax($competition->id);
					$teams[$t_id]->points_min = $team->pointsMin($competition->id);
					$teams[$t_id]->register_date = $team->register_date;
					$teams[$t_id]->season_position = $team->getSeasonPosition($competition->id);
					$teams[$t_id]->round_position = $team->getRoundPosition($competition->id, $round);
		
					$points[$t_id] = $teams[$t_id]->points;
					$register_date[$t_id] = $teams[$t_id]->register_date;
					$positions[$t_id] = (!empty($round) && is_numeric($round)) ? $teams[$t_id]->round_position : $teams[$t_id]->season_position;
				}
				
				array_multisort($positions, SORT_ASC, $register_date, SORT_DESC, $teams);
		
				$i = 0;
		
				$rows = array();
		
				foreach ($teams as $t_id => $team) {
		
					$i++;
		
					$team_class = (isset($user) && $team->user == $user->uid) ? " class=bold" : "";
		
					if ($standing_round) {
						$rows[$t_id][] = array("data" => $teams[$t_id]->round_position, "data-title" => t("Posizione"));
						$rows[$t_id][] = array("data" => l($team->name, "squadre/" . $team->id), "class" =>$team_class, "data-title" => t("Squadra"));
						$rows[$t_id][] = array("data" => $team->points, "data-title" => t("Punti"));
						$rows[$t_id][] = array("data" => $teams[$t_id]->season_position, "data-title" => t("Posizione Stagione"));
						$rows[$t_id][] = array("data" => l("<i class='fa fa-bar-chart'></i>", "scheda/giornata/" . $competition->sanitized_name . "/" . $round . "/" . $team->id, array("html" => true, "attributes" => array("data-toggle" => "modal", "data-target" => "#match-details-modal"))), "data-title" => t("Scheda giornata"));		
					}
					else {
						$rows[$t_id][] = array("data" => $teams[$t_id]->season_position, "data-title" => t("Posizione"));
						$rows[$t_id][] = array("data" => l($team->name, "squadre/" . $team->id), "attributes" => array("class" => $team_class), "data-title" => t("Squadra"));
						$rows[$t_id][] = array("data" => $team->points, "data-title" => t("Punti"));
						$rows[$t_id][] = array("data" => $team->points_max, "data-title" => t("Punteggio Massimo"));
						$rows[$t_id][] = array("data" => $team->points_min, "data-title" => t("Punteggio Minimo"));
					}
		
				}
		
			}
			
			$group->output = theme_table(array("header" => $header, "rows" => $rows, "attributes" => array("class" => "collapse-table table table-responsive standings", "id" => "standings-" . $g_id), "caption" => "", "sticky" => "", "empty" => "", "colgroups" => array()));
			$group->active = (isset($user_group) && $g_id == $user_group) || $index == 1;
			
			$groups[$g_id] = $group;
		}
	}
	
	$output["groups"] = $groups;
	$output["rounds_list"] = Round::listForStandings($competition->id);
	$output["url"] = "classifica/" . $competition->name; 
	$output["first_item"] = array("key" => "", "value" => t("Classifica completa"));
    
	return theme("standings", $output);
// 	return $out . theme_table(array("header" => $header, "rows" => $rows, "attributes" => array("class" => array("table", "table-responsive", "table-striped", "-tablesorter", "-tablesorter-blue", "-classifica"), "id" => "classifica" ), "caption" => "", "sticky" => "", "empty" => t("Nessuna squadra presente"), "colgroups" => array()));
}

function get_standing_table_f($competition, $round_start, $round_end, $limit) {

	drupal_add_js(drupal_get_path("module", "fantacalcio") . "/js/jquery.tablesorter.js");
	drupal_add_js(drupal_get_path("module", "fantacalcio") . "/js/jquery.metadata.js");
	drupal_add_js("jQuery(window).ready(function() {jQuery(\".standings	\").tablesorter({cssAsc: \"sort-asc\", cssDesc: \"sort-desc\", cssHeader: \"sort-header\"});})", "inline");

  global $user;
  
  $output = array();
    
  $all_teams = array();
  $points = array();
  $goals_for = array();

  foreach($competition->groups as $g_id => $group) {
  	  	$group = Group::get($g_id);
	  
	  	$teams = Team::allByGroup($g_id);
	  	
		
		if ($teams) {
	
		    foreach ($teams as $t_id => $team) {
				$all_teams[$t_id] = $team;
				$pts = $team->pointsForF8($g_id, $round_start, $round_end);
				$goals = $team->goalsFor($g_id);
				$all_teams[$t_id]->points_in_f8 = $pts;
				$all_teams[$t_id]->goals_in = 
			    $points[$t_id] = $pts;
				$goals_for[$t_id] = $goals;
			}
		}
	  	
	  	
}

	array_multisort($points, SORT_DESC, $goals_for, SORT_DESC, $all_teams);
  
	$header = array("", t("Squadra"), t("Punti"));

	$i = 0;
	$rows = array();

	foreach ($all_teams as $t_id => $team) {

		$i++;
		
		$team_class = array();
		if($i <= $limit) {
			array_push($team_class, "bg-success");
		}
		
		if ($team->user == $user->uid)
		   array_push($team_class, "bold");

		$rows[$t_id][] = array("data" => $i, "data-title" => t("Posizione"), "class" => $team_class);
		$rows[$t_id][] = array("data" => l($team->name, "squadre/" . $team->id), "data-title" => t("Squadra"), "class" => $team_class);
		$rows[$t_id][] = array("data" => $team->points_in_f8, "data-title" => t("Pti F8"), "class" => $team_class);

	}
	  	
	$group->output = theme("table", array("header" => $header, "rows" => $rows, "attributes" => array("class" => "collapse-table table table-responsive standings", "id" => "classifica_" . $g_id), "sticky" => "", "empty" => ""));
	  			
 	$groups[$g_id] = $group; 	
  		   
	$output["groups"] = $groups;
    
	return theme("standings", $output);
}

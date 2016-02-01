<?php

/**
 * @file fantastats.players.inc
 *
 */

/**
 * ricerca giocatore
 * *
 */
function statistiche_autocomplete_players($s) {
  $players = array();
  $roles = array("P", "D", "C", "A");
  $round = Round::getNext();
  
  $query = db_select("fanta_players", "p");
  $query->join("fanta_players_rounds", "r", "r.pl_id = p.pl_id");
  $query->join("fanta_real_teams", "rt", "rt.rt_id = r.rt_id");
  $query->fields("p");
  $query->addField("rt", "name", "team");
  $query->condition("r.round", $round);
  $query->condition("p.name", db_like(addslashes($s)) . '%', 'LIKE');
  $query->orderBy("p.name");
  $query->range(0, 10);
  
  $result = $query->execute(); 
  
  foreach ($result as $row) {
    $id = $row->pl_id . ", " . htmlspecialchars(stripslashes($row->name)) . " (" . ucwords($row->team) . ") - " . $roles[$row->role];
    $players[$id] = $id;
  }
  print drupal_json_output($players);
  exit();
}

function player_details($pl_id = '') {
  $choose_player_form = drupal_get_form("choose_pl_id_form");
  
  $out = drupal_render($choose_player_form);
  
  if (!empty($pl_id)) {
    if (Player::exists($pl_id))
      $out .= player_stats($pl_id);
    else
      $out .= "Nessun giocatore con questo id";
  }
  
  return $out;
}

function player_stats($pl_id) {
  global $roles;
  
  // $teams_groups = Team::allByGroup();
  // ksort($teams_groups);
  // $groups = Group::all();
  $player = Player::get($pl_id);
  
  $player_rounds_stats = player_rounds_stats($pl_id);
  
  $out = "<div class='row'>";
  $out .= "<div class='col-xs-12 col-sm-6'><h3>" . $player->name . " (" . $roles[$player->role] . ")</h3></div>";
  $out .= "<div class='col-xs-12 col-sm-6'>&nbsp;</div>";
  $out .= "<div class='col-xs-12 col-sm-6'>&nbsp;</div>";
  
  if (count($player_rounds_stats['rounds']) > 0) {

	  $player_rounds_rows = array();
  
	  $player_rounds_rows[] = array_merge($player_rounds_stats['summary']['caps'], $player_rounds_stats['summary']['yellow_cards']);
	  $player_rounds_rows[] = array_merge($player_rounds_stats['summary']['vote'], $player_rounds_stats['summary']['red_cards']);
	  $player_rounds_rows[] = array_merge($player_rounds_stats['summary']['total'], $player_rounds_stats['summary']['saved_penalties']);
	  $player_rounds_rows[] = array_merge($player_rounds_stats['summary']['goals_for'], $player_rounds_stats['summary']['missed_penalties']);
	  $player_rounds_rows[] = array_merge($player_rounds_stats['summary']['goals_against'], $player_rounds_stats['summary']['own_goals']);
	  $player_rounds_rows[] = array_merge($player_rounds_stats['summary']['assists'], array("", ""));

	  $player_summary = theme("table", array("header" => array(), "rows" => $player_rounds_rows, "attributes" => array("class" => array("table", "table-responsive"))));
	  $presences = player_get_presences($pl_id);
	  	  
	  $script = "jQuery(document).ready(function() {
			var presences_ctx = jQuery('#presences-chart').get(0).getContext('2d');
			var votes_ctx = jQuery('#votes-chart').get(0).getContext('2d');
			var presences = [
				{
					value: " . $presences['regular'] . ",
					color: '#46BFBD',
					highlight: '#5AD3D1',
					label: '" . t("Titolare") . "'
				},
				{
					value: " . $presences['reserve'] . ",
					color: '#FDB45C',
					highlight: '#FFC870',
					label: '" . t("Riserva") . "'
				},
				{
                                        value: " . $presences['not_played'] . ",
                                        color:'#F7464A',
                                        highlight: '#FF5A5E',
                                        label: '" . t("Non entrato") . "'
                                }
			];

var votes = {
    labels: [" . implode(", ", array_map(function ($item) {return $item['round'];}, $player_rounds_stats['rounds'])) . "],//rounds
    datasets: [
        {
            label: 'Voto',
            fillColor: 'rgba(220,220,220,0.2)',
            strokeColor: 'rgba(220,220,220,1)',
            pointColor: 'rgba(220,220,220,1)',
            pointStrokeColor: '#fff',
            pointHighlightFill: '#fff',
            pointHighlightStroke: 'rgba(220,220,220,1)',
            data: [" . implode(", ", array_map(function ($item) {return $item['vote'];}, $player_rounds_stats['chart'])) . "]
        },
        {
            label: 'Fantavoto',
            fillColor: 'rgba(151,187,205,0.2)',
            strokeColor: 'rgba(151,187,205,1)',
            pointColor: 'rgba(151,187,205,1)',
            pointStrokeColor: '#fff',
            pointHighlightFill: '#fff',
            pointHighlightStroke: 'rgba(151,187,205,1)',
            data: [" . implode(", ", array_map(function ($item) {return $item['total'];}, $player_rounds_stats['chart'])) . "]
        }
    ]
};

var presences_options = {legendTemplate : '<ul class=\"<%=name.toLowerCase()%>-legend\"><% for (var i=0; i<segments.length; i++){%><li><span style=\"background-color:<%=segments[i].fillColor%>\"></span><%if(segments[i].label){%><%=segments[i].label%><%}%></li><%}%></ul>'
};
var votes_options = {legendTemplate : '<ul class=\"<%=name.toLowerCase()%>-legend\"><% for (var i=0; i<datasets.length; i++){%><li><span style=\"background-color:<%=datasets[i].strokeColor%>\"></span><%if(datasets[i].label){%><%=datasets[i].label%><%}%></li><%}%></ul>'
};

			var presences_chart = new Chart(presences_ctx).Doughnut(presences, presences_options);
			jQuery('#presences-chart-legend').html(presences_chart.generateLegend());
			var votes_chart = new Chart(votes_ctx).Line(votes, votes_options);
			jQuery('#votes-chart-legend').html(votes_chart.generateLegend());
		})";

	  drupal_add_js(drupal_get_path("module", "fantacalcio") . "/js/Chart.js", "file");
	  drupal_add_js($script, "inline");
	  drupal_add_css(".chart-legend ul {list-style-type: none;} .chart-legend li span{
    display: inline-block;
    width: 12px;
    height: 12px;
    margin-right: 5px;
}", "inline");
  
	  $out .= "<div class='col-xs-12 col-sm-4 pull-right'>" . $player_summary . "</div>";
	  $out .= "<div class='col-xs-12 col-sm-8 pull-left'>";
	  $out .= "<h4>" . t("Presenze e voti") . "</h4>";
	 $out .= "<div class='row'>";
	  $out .= "<div class='col-xs-12 col-sm-5'><canvas class='pull-left' id='presences-chart'></canvas><div id='presences-chart-legend' class='chart-legend pull-right'></div></div>";
	  $out .= "<div class='col-xs-12 col-sm-7'><canvas class='pull-left' id='votes-chart'></canvas><div id='votes-chart-legend' class='chart-legend pull-right'></div></div>";
	  $out .= "</div>";
	  $out .= "</div>";
  }
  
  $out .= "</div>";
  
  $fanta_teams = player_get_fantateams($pl_id);
  
  if (count($fanta_teams) > 0) {
  	 $out .= "<div class='row'><div class='col-xs-12'>";
  	 
	foreach($fanta_teams as $group => $fanta_team) {
		$out .= "<p>" . $group . ": " . $fanta_team . "</p>";
	}
	  
	  $out .= "</div></div>";
  }
  
  // $out .= "<h3>" . $roles[$player->role] . "</h3><br>";
  // $out .= "<h4>" . ucwords($player->team) . "</h4><br>";
  
  // $out .= "<div class=clear>" . get_player_fantateams($player->pl_id, $groups, $teams_groups) . "</div><br/>";
  if (count($player_rounds_stats['rounds']) > 0) {
  
	  $header = array(
		  t("Giornata"), 
		  t("Squadra"), 
		  t("Totale"), 
		  t("Voto"), 
		  t("Gol Fatti"), 
		  t("Gol Subiti"), 
		  t("Assist"), 
		  t("Amm"), 
		  t("Esp"), 
		  t("Rig Parati"), 
		  t("Rig Sbagliati"), 
		  t("Autogol"));
  
	  $out .= "<div class='row'>&nbsp;</div>";
	  $out .= "<div class='row'>";
	  $out .= "<div class='col-xs-12'>";
	  $out .= "<h4>" . t("Dettagli giornate") . "</h4>";
	  $out .= theme("table", array("header" => $header, "rows" => $player_rounds_stats['rounds'], "attributes" => array("class" => array("table", "table-responsive"))));
	  $out .= "</div>";
	  $out .= "</div>"; 
  } 
  else {
  	$out .= t("Nessuna presenza");
  }

  return $out;
}

function choose_pl_id_form() {
  $form['players'] = array("#type" => "fieldset");
  
  $form['players']['player'] = array(
    "#title" => "Giocatore", 
    "#type" => "textfield", 
    "#autocomplete_path" => "statistiche/autocomplete/players");
  
  $form['players']['submit'] = array("#type" => "submit", "#value" => t("Ok"));
  
  return $form;
}

function choose_pl_id_form_submit($form, $form_state) {
  $player = $form_state['values']['player'];
  $player_exploded = explode(',', $player);
  $pl_id = $player_exploded[0];
  
  drupal_goto("statistiche/giocatore/" . $pl_id);
}

function player_get_fantateams($pl_id) {
	$query = db_select("fanta_squads", "s");
	$query->join("fanta_teams", "t", "s.t_id = t.t_id");
	$query->join("fanta_teams_groups", "tg", "t.t_id = tg.t_id");
	$query->join("fanta_groups", "g", "g.g_id = tg.g_id");
	$query->condition("pl_id", $pl_id);
	$query->condition("g.c_id", Competition::getDefault()->id); 
	$query->fields("t");
	$query->addField("g", "name", "group_name");
	
	$result = $query->execute();
	
	$fanta_teams = array();
	foreach($result as $row) {
		$fanta_teams[$row->group_name] = $row->name;
	}
	
	return $fanta_teams;
}

function player_rounds_stats($pl_id) {
  $votes = array();
  $chart = array();
  $rounds = 0;
  $summary = array(
    "total" => 0, 
    "vote" => 0, 
    "goals_for" => 0, 
    "goals_against" => 0, 
    "assists" => 0, 
    "yellow_cards" => 0, 
    "red_cards" => 0, 
    "own_goals" => 0, 
    "saved_penalties" => 0, 
    "missed_penalties" => 0);
 
  $query = db_select("fanta_players_rounds", "pr");
  $query->join("fanta_real_teams", "rt", "rt.rt_id = pr.rt_id");
  $query->condition("pl_id", $pl_id);
  $query->condition("pr.round", Round::getNext(), "!="); 
  $query->fields("pr");
  $query->addField("rt", "name", "team");

  $result = $query->execute();

  foreach ($result as $row) {
    $votes[$row->round] = array('round' => $row->round, 'team' => $row->team, 'total' => array("data" => "-", "colspan" => 10));
    $chart[$row->round] = array('round' => $row->round, 'total' => 0, 'vote' => 0);
  }

 
  $query = db_select("fanta_votes", "v");
  $query->join("fanta_players_rounds", "r", "r.pl_id = v.pl_id AND r.round = v.round");
  $query->join("fanta_real_teams", "rt", "rt.rt_id = r.rt_id");
  $query->condition("v.pl_id", $pl_id);
  $query->condition("v.provider", variable_get("fantacalcio_votes_provider", 1));
  $query->fields("v");
  $query->addField("rt", "name", "team");
  $query->orderBy("v.round");
 
  $result = $query->execute();
  
  foreach ($result as $row) {
    $votes[$row->round] = array();
    $chart[$row->round] = array();
    $votes[$row->round]['round'] = $row->round;
    $votes[$row->round]['team'] = $row->team;
    
    if ($row->has_vote == 1) {
    $votes[$row->round]['total'] = $row->total;
    $votes[$row->round]['vote'] = $row->vote;
    $votes[$row->round]['goals_for'] = $row->goals_for + $row->penalty_goals;
    $votes[$row->round]['goals_against'] = $row->goals_against;
    $votes[$row->round]['assists'] = $row->assists;
    $votes[$row->round]['yellow_cards'] = $row->yellow_cards;
    $votes[$row->round]['red_cards'] = $row->red_cards;
    $votes[$row->round]['saved_penalties'] = $row->saved_penalties;
    $votes[$row->round]['missed_penalties'] = $row->missed_penalties;
    $votes[$row->round]['own_goals'] = $row->own_goals;
    unset($votes[$row->round]['v_id']);
    unset($votes[$row->round]['provider']);
    unset($votes[$row->round]['pl_id']);
    unset($votes[$row->round]['penalty_goals']);
    unset($votes[$row->round]['win_goals']);
    unset($votes[$row->round]['draw_goals']);

    $summary['total'] += $row->total;
    $summary['vote'] += $row->vote;
    $summary['goals_for'] += $row->goals_for;
    $summary['goals_against'] += $row->goals_against;
    $summary['assists'] += $row->assists;
    $summary['yellow_cards'] += $row->yellow_cards;
    $summary['red_cards'] += $row->red_cards;
    $summary['saved_penalties'] += $row->saved_penalties;
    $summary['missed_penalties'] += $row->missed_penalties;
    $summary['own_goals'] += $row->own_goals;

    $chart[$row->round]['total'] = $row->total;
    $chart[$row->round]['vote'] = $row->vote;

    $rounds++;
    }
    else {
	$votes[$row->round]['total'] = array("data" => "-", "colspan" => 9);
   	$chart[$row->round]['total'] = 0;
	$chart[$row->round]['vote'] = 0;
    }


  }
  
  if ($rounds != 0) {
    
    $summary['caps'] = array(t("Presenze"), $rounds);
    $summary['vote'] = array(t("Media Voto"), round($summary['vote'] / $rounds, 2));
    $summary['total'] = array(t("Media FantaVoto"), round($summary['total'] / $rounds, 2));
    $summary['goals_for'] = array(t("Goal fatti"), $summary['goals_for']);
    $summary['goals_against'] = array(t("Goal subiti"), $summary['goals_against']);
    $summary['assists'] = array(t("Assist"), $summary['assists']);
    $summary['yellow_cards'] = array(t("Ammonizioni"), $summary['yellow_cards']);
    $summary['red_cards'] = array(t("Espulsioni"), $summary['red_cards']);
    $summary['saved_penalties'] = array(t("Rigori parati"), $summary['saved_penalties']);
    $summary['missed_penalties'] = array(t("Rigori sbagliati"), $summary['missed_penalties']);
    $summary['own_goals'] = array(t("Autogol"), $summary['own_goals']);
    
    ksort($votes);
    ksort($chart);
    //$votes[$rounds] = array("data" => $summary, "class" => array("bold"));
    
//    return theme("table", array("header" => $header, "rows" => $votes));
    return array("summary" => $summary, "rounds" => $votes, "chart" => $chart);
  }
  else
    return array("summary" => array(), "rounds" => array(), "chart" => array());
}

function player_get_presences($pl_id) {
	
	$presences = array('not_played' => 0, 'regular' => 0, 'reserve' => 0);
	
	$query = db_select('fanta_players_rounds', 'pr');
	$query->condition("pl_id", $pl_id);
	$query->condition("active", 1);
	$query->condition("round", Round::getNext(), "!=");
	$query->fields("pr");
	
	$result = $query->execute();

	$all_rounds = $result->rowCount();

	$query = db_select("fanta_votes", "v");
	$query->condition("v.pl_id", $pl_id);
	$query->condition("v.provider", variable_get('fantacalcio_votes_provider', 1));
	$query->fields("v", array("regular"));
	
	$result = $query->execute();
	
	foreach ($result as $row) {
		if ($row->regular == 1)
			$presences['regular']++;
		else if ($row->regular == 0)
			$presences['reserve']++;
	}

	$presences['not_played'] = $all_rounds - $presences['regular'] - $presences['reserve'];
	
	return $presences;
}

/**
 * ricerca avanzata
 * *
 */
function choose_players_stats($form, &$form_state) {
  $played_rounds = Round::allPlayed();
  
  $rounds = array();
  foreach ($played_rounds as $round) {
    $rounds[$round->round] = $round->round;
  }
  
  $default_competition = Competition::getDefault();
  $groups = Group::allByCompetition($default_competition->id);
  $group_options = array();
  foreach ($groups as $g_id => $group)
    $group_options[$g_id] = $group->name;
  
  $form['#theme'] = "fantastats_choose_stats_form";
  
  $form['type'] = array(
    "#type" => "select", 
    "#title" => "Tipo statistica", 
    '#options' => get_stats_types(), 
    "#default_value" => arg(2), 
    "#attributes" => array("class" => array("form-control", "input-sm")));
  
  if ($rounds)
    $form['round_start'] = array(
      "#type" => "select", 
      "#title" => "Giornata inizio", 
      '#options' => $rounds, 
      "#default_value" => arg(3), 
      "#attributes" => array("class" => array("form-control", "input-sm")));
  
  if ($rounds)
    krsort($rounds);
  
  if ($rounds)
    $form['round_stop'] = array(
      "#type" => "select", 
      "#title" => "Giornata fine", 
      "#default_value" => arg(4), 
      '#options' => $rounds, 
      "#attributes" => array("class" => array("form-control", "input-sm")));
  
  $form['roles'] = array(
    "#type" => "checkboxes", 
    "#options" => array("0" => "P", "1" => "D", "2" => "C", "3" => "A"), 
    "#default_value" => explode("-", arg(6)), 
    "#title" => "Ruoli");
  
  $form['presences'] = array(
    "#type" => "select", 
    "#title" => "Presenze<br/>minime", 
    "#default_value" => arg(5), 
    '#options' => ($rounds), 
    "#attributes" => array("class" => array("form-control", "input-sm")));
  
  $form['rows'] = array(
    "#type" => "select", 
    "#title" => "Giocatori da<br/>visualizzare", 
    '#options' => array(
      "10" => 10, 
      "20" => 20, 
      "30" => 30, 
      "40" => 40, 
      "50" => 50), 
    "#default_value" => arg(7), 
    "#attributes" => array("class" => array("form-control", "input-sm")));
  
  $form['sort'] = array(
    "#type" => "select", 
    "#title" => "Ordinamento", 
    '#options' => array("desc" => "Decrescente", "asc" => "Cresecente"), 
    "#default_value" => arg(8), 
    "#attributes" => array("class" => array("form-control", "input-sm")));
  
  $form['group'] = array(
    "#type" => "select", 
    "#title" => t("Girone"), 
    '#options' => $group_options,  // get_group_options(variable_get("fantacalcio_main_competition", 1)),
    "#default_value" => arg(9), 
    "#attributes" => array("class" => array("form-control", "input-sm")));
  
  $form['in_team'] = array(
    "#type" => "select", 
    "#default_value" => arg(10), 
    "#title" => "Tesserato", 
    '#options' => array(
      "1" => t("Tesserato"), 
      "-1" => t("Non tesserato"), 
      "0" => t("Indifferente")), 
    "#attributes" => array("class" => array("form-control", "input-sm")));
  
  $form['submit'] = array(
    "#type" => "submit", 
    "#value" => "Ok", 
    "#attributes" => array("class" => array("btn-success")));
  
  $form['clear'] = array(
    '#type' => 'submit', 
    '#value' => 'Annulla', 
    "#attributes" => array("class" => array("btn-default")), 
    '#validate' => array('choose_players_stats_clear'));
  
  return $form;
}

function choose_players_stats_submit($form, &$form_state) {
  $type = $form_state["values"]["type"];
  $round_start = $form_state["values"]["round_start"];
  $round_stop = $form_state["values"]["round_stop"];
  $presences = $form_state["values"]["presences"];
  $rows = $form_state["values"]["rows"];
  $sort = $form_state["values"]["sort"];
  $g_id = $form_state["values"]["group"];
  $in_team = $form_state["values"]["in_team"];
  
  $roles_list = "";
  if ($form_state['values']['roles']) {
    foreach ($form_state['values']['roles'] as $key => $value) {
      if ($value != '')
        $roles_list .= $key . "-";
    }
  }
  if (strlen($roles_list) > 0)
    $roles_list = substr($roles_list, 0, -1);
  else
    $roles_list = "0-1-2-3";
  drupal_goto("statistiche/players/" . $type . "/" . $round_start . "/" . $round_stop . "/" . $presences . "/" . $roles_list . "/" . $rows . "/" . $sort . "/" . $g_id . "/" . $in_team);
}

function choose_players_stats_clear($form, &$form_state) {
  drupal_goto("statistiche/players");
}

function players_stats($type, $round_start, $round_stop, $presences, $roles_list, $number, $sort, $g_id, $in_team) {
  $roles = explode("-", $roles_list);
  
  $avg_types = array("vote", "total");
  
  $query = db_select("fanta_players", "p");
  $query->join("fanta_votes", "v", "v.pl_id = p.pl_id");
  $query->join("fanta_players_rounds", "pr", "pr.pl_id = p.pl_id");
  $query->join("fanta_real_teams", "rt", "rt.rt_id = pr.rt_id");
  $query->condition("v.provider", variable_get('fantacalcio_votes_provider', 1));
  $query->condition("pr.round", Round::getLast());
  $query->fields("v");
  $query->fields("p");
  $query->addField("rt", "name", "team");
  
  if (count($roles) > 0 && count($roles) < 4)
    $query->condition("role", $roles, "IN");
    
  // tesserati  
  $subquery2 = db_select("fanta_teams_groups", "g");
  $subquery2->condition("g_id", $g_id);
  $subquery2->addField("g", "t_id", "t_id");
  
  $subquery1 = db_select("fanta_squads", "s");
  $subquery1->condition("status", 1);
  $subquery1->condition("t_id", $subquery2, "IN");
  $subquery1->addField("s", "pl_id", "pl_id");
  
  if ($in_team == 1 && $g_id > 0) {
    $query->condition("p.pl_id", $subquery1, "IN");
    // $sql .= " AND p.pl_id IN (SELECT pl_id FROM {fanta_squads} WHERE in_team = 1 AND t_id IN (SELECT t_id FROM {fanta_teams_groups} WHERE g_id = '%d'))";
  }
  // non tesserati
  if ($in_team == -1 && $g_id > 0)
    $query->condition("p.pl_id", $subquery1, "NOT IN");
    // $sql .= " AND p.pl_id NOT IN (SELECT pl_id FROM {fanta_squads} WHERE in_team = 1 AND t_id IN (SELECT t_id FROM {fanta_teams_groups} WHERE g_id = '%d'))";
    
  // $result = db_query($sql, variable_get('fantacalcio_votes_provider', 1), $g_id);
  $result = $query->execute();
  
  $stats = array();
  
  foreach ($result as $row) {
    if (!isset($stats[$row->pl_id]))
      $stats[$row->pl_id] = array("name" => "", "pl_id" => "", "value" => 0, "votes" => 0, "presences" => 0);
    
    if ($row->round >= $round_start && $row->round <= $round_stop) {
      $stats[$row->pl_id]["pl_id"] = $row->pl_id;
      $stats[$row->pl_id]["name"] = $row->name;
      $stats[$row->pl_id]["team"] = $row->team;
      $stats[$row->pl_id]["role"] = $row->role;
      $stats[$row->pl_id]["value"] += $row->$type;
      $stats[$row->pl_id]["votes"]++;
    }
    $stats[$row->pl_id]["presences"]++;
  }
  
  if (count($stats) > 0) {
    
    $values = array();
    $rows = array();
    
    foreach ($stats as $pl_id => $stat) {
      if ($stat["presences"] >= $presences && $stat['votes'] > 0) {
        if (in_array($type, $avg_types))
          $stats[$pl_id]['value'] = round(($stat['value'] / $stat['votes']), 2);
        $values[$pl_id] = $stats[$pl_id]['value'];
      }
      else
        unset($stats[$pl_id]);
    }
    
    $flag = ($sort == "asc") ? SORT_ASC : SORT_DESC;
    array_multisort($values, $flag, $stats);
    $i = 0;
    
    foreach ($stats as $pl_id => $stat) {
      
      if ($i < $number) {
        $rows[] = array(
          fantacalcio_show_role($stat['role']), 
          $stat['name'],
          ucwords(strtolower($stat['team'])), 
          $stat['votes'], 
          $stat['value'], 
          get_player_fantateams($stat['pl_id']), 
          l("<i class='fa fa-bar-chart'></i>", "scheda/giocatore/" . $stat['pl_id'], array(
            "html" => TRUE, 
            "attributes" => array(
              "title" => "Dettagli giocatore", 
              "data-toggle" => "modal", 
              "data-target" => "#player-stats-modal"))));
      }
      $i++;
    }
    
    $types = get_stats_types();
    
    $header = array(
      t("Ruolo"),
      t("Giocatore"), 
      t("Squadra"), 
      t("Presenze"), 
      ucwords($types[$type]), 
      t("Tesserato"), 
      "");
    
    $choose_players_stats = drupal_get_form('choose_players_stats');
    
    return drupal_render($choose_players_stats) . theme("table", array(
      "header" => $header, 
      "rows" => $rows));
  }
  
  else {
    $choose_players_stats = drupal_get_form('choose_players_stats');
    return drupal_render($choose_players_stats) . t("Non è stato trovato nessun giocatore corrispondente ai criteri di ricerca");
  }
}

function theme_fantastats_choose_stats_form($variables) {
  $form = $variables["form"];
  
  $rows[0] = array(
    drupal_render($form["type"]), 
    drupal_render($form["round_start"]) . drupal_render($form["round_stop"]), 
    drupal_render($form["presences"]), 
    drupal_render($form["roles"]), 
    drupal_render($form["rows"]), 
    drupal_render($form["sort"]), 
    drupal_render($form["group"]), 
    drupal_render($form["in_team"]));
  
  $rows[1][] = array(
    "data" => drupal_render($form["submit"]) . drupal_render($form["clear"]) . drupal_render_children($form), 
    "colspan" => 8);
  
  return theme("table", array(
    "header" => array(), 
    "rows" => $rows, 
    "empty" => t("Nessun giocatore")));
}

function get_stats_types() {
  $types = array(
    "total" => "Media FantaVoto", 
    "vote" => "Media Voto");

  if (variable_get("fantacalcio_points_goals_for") != 0) 
    $types["goals_for"] = t("Goal fatti");
    
if (variable_get("fantacalcio_points_goals_against") != 0)
    $types["goals_against"] = t("Goal subiti"); 
    
if (variable_get("fantacalcio_points_assists") != 0)
    $types["assists"] = t("Assist");
    
if (variable_get("fantacalcio_points_yellow_card") != 0)
    $types["yellow_card"] = t("Ammonizioni"); 
    
if (variable_get("fantacalcio_points_red_card") != 0)
    $types["red_card"] = t("Espulsioni");
    
if (variable_get("fantacalcio_points_penalty_goals") != 0)
    $types["penalty_goals"] = t("Gol rigore"); 
    
if (variable_get("fantacalcio_points_saved_penalties") != 0)
    $types["saved_penalties"] = t("Rigore parato"); 
    
if (variable_get("fantacalcio_points_missed_penalties") != 0)
    $types["missed_penalties"] = t("Rigore sbagliato"); 
    
if (variable_get("fantacalcio_points_goals_for") != 0)
    $types["own_goals"] = t("Autogol");

if (variable_get("fantacalcio_points_draw_goal") != 0)
    $types["draw_goal"] = t("Gol pareggio");

if (variable_get("fantacalcio_points_win_goals") != 0)
    $types["win_goal"] = t("Gol vittoria");
  return $types;
}

function get_player_fantateams($pl_id) {
  $fanta_teams = array();
  $query = db_select("fanta_squads", "s")
    ->fields("s")
    ->condition("pl_id", $pl_id)
    ->condition("status", 1);
  $result = $query->execute();
  foreach ($result as $row) {
    $fanta_teams[] = $row->t_id;
  }
  
  $groups = Group::allByCompetition(Competition::getDefault()->id);
  
  $items = array();
  foreach ($groups as $g_id => $group) {
    
    $groups_teams = Team::allByGroup($g_id);
    
    $team_name = "-";
    
    foreach ($groups_teams as $t_id => $team) {
      if (in_array($t_id, $fanta_teams)) {
        $team_name = $team->name;
        continue;
      }
    }
    $items[$g_id] = $groups[$g_id]->name . ": " . $team_name;
  }
  
  return theme("item_list", array("items" => $items));
}

/**
 * giocatori liberi
 * *
 */
function fantacalcio_free_players($g_id = "") {
  global $roles_long;
  $roles = $roles_long;
  
  $competition_groups = Group::allByCompetition(Competition::getDefault()->id);
  $groups = array();
  foreach ($competition_groups as $group) {
    $groups[$group->id] = $group->name;
  }
  
  $choose_group = drupal_get_form("choose_group_form", $groups, $g_id);
  $out = drupal_render($choose_group);
  
  if (!empty($g_id) && Group::exists($g_id)) {
    $out .= "<h3>" . $competition_groups[$g_id]->name . "</h3>";
    
    $nonfree_players = array();
    
    $query = db_select("fanta_squads", "s");
    $query->condition("status", 1);
    $query->condition("t_id", array_keys(Team::allByGroup($g_id)), "IN");
    $query->fields("s", array("pl_id"));
    $result = $query->execute();
    foreach ($result as $row) {
      $nonfree_players[] = $row->pl_id;
    }
    
    $items = array();
    $i = 0;
    foreach ($roles as $role_id => $role) {
      $items[$role_id] = array(
        "data" => '<a aria-controls="r_' . $role_id . '" role="tab" data-toggle="tab" href="#r_' . $role_id . '">' . $role . '</a  >', 
        "role" => "presentation");
      
      if ($i == 0)
        $items[$role_id]["class"] = array("active");
      
      $i++;
    }
    
    $out .= theme("item_list", array(
      "items" => $items, 
      "type" => "ul", 
      "attributes" => array(
        "class" => array("nav", "nav-pills"), 
        "role" => "tablist")));
    
    $out .= "\n<div class='tab-content'>";
    $i = 0;
    foreach ($roles as $role_id => $role) {
      $active = "";
      if ($i == 0)
        $active = "active";
      
      $out .= "\n<div role='tabpanel' class='tab-pane " . $active . "' id='r_" . $role_id . "'>" . get_free_players_role($role_id, $g_id, $nonfree_players) . "\n</div>";
      $i++;
    }
    
    $out .= "\n</div>";
  }
  
  return $out;
}

function choose_group_form($form, &$form_state, $groups, $g_id) {
  $form['groups'] = array("#type" => "fieldset");
  
  $form['groups']['groups'] = array(
    "#type" => "select", 
    "#title" => "Scegli un girone", 
    "#options" => $groups, 
    "#default_value" => $g_id, 
    "#attributes" => array("class" => array("form-control")));
  
  $form['groups']['submit'] = array(
    "#value" => "Ok", 
    "#type" => "submit", 
    "#attributes" => array("class" => array("form-control btn-default")));
  return $form;
}

function choose_group_form_submit($form, $form_state) {
  $g_id = $form_state['values']['groups'];
  drupal_goto("statistiche/giocatori-liberi/" . $g_id);
}

function get_free_players_role($role, $g_id, $nonfree_players) {
  $header = array(t("Ruolo"), t("Nome"), t("Squadra"), "");
  $players = Player::allWithRound();
  
  $rows = array();
  
  foreach ($players as $pl_id => $player) {
    $names[$pl_id] = $player->name;
    $teams[$pl_id] = $player->team;
  }
  
  array_multisort($names, SORT_ASC, $teams, SORT_ASC, $players);
  
  foreach ($players as $pl_id => $player) {
    if (!in_array($player->id, $nonfree_players) && $player->role == $role) {
      $link_pl_id = l("<i class='fa fa-bar-chart'></i>", "scheda/giocatore/" . $player->id, array(
        "html" => true, "attributes" => array("data-toggle" => "modal", "data-target" => "#player-stats-modal")));
      
      $class = "active";
      $rows[$pl_id] = array(
        "data" => array(
          fantacalcio_show_role($player->role), 
          $player->name, 
          $player->team, 
          $link_pl_id), 
        "class" => array($class));
    }
  }
  
  return theme("table", array(
    "header" => $header, 
    "rows" => $rows, 
    "attributes" => array("class" => array("table-responsive", "table"))));
}


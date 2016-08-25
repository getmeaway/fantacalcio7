<?php


class Result {

    static function getPoints() {
	$points = array(
			"yellow_cards" => array(
				"title" => t("Amm."),
				"type" => "checkbox"
				),
				"red_cards" => array(
				"title" => t("Esp."),
				"type" => "checkbox"
				),
				"goals_for" => array(
				"title" => t("Gol fatti"),
				"type" => "numberfield"
				),
				"penalty_goals" => array(
				"title" => t("Gol su rigore"),
				"type" => "numberfield"
				),
				"goals_against" => array(
				"title" => t("Gol subiti"),
				"type" => "numberfield"
				),
				"assists" => array(
				"title" => t("Assist"),
				"type" => "numberfield"
				),
				"own_goals" => array(
				"title" => t("Autogol"),
				"type" => "numberfield"
				),
				"missed_penalties" => array(
				"title" => t("Rigori sbagliati"),
				"type" => "numberfield"
				),
				"saved_penalties" => array(
				"title" => t("Rigori parati"),
				"type" => "numberfield"
				),
				"draw_goals" => array(
				"title" => t("Gol vittoria"),
				"type" => "numberfield"
				),
				"win_goals" => array(
				"title" => t("Gol pareggio"),
				"type" => "numberfield"
				),
				
				);
				
	return $points;
}
    
  static function importLineups($vote_round) {

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
  
  static function insertVotes($vote_round) {
      
    global $roles;
 
    $votes_url = DATA_SOURCE_URL . "/votes/" . $vote_round . "-" . variable_get("fantacalcio_votes_provider", 1) . ".json";
    
    $votes = json_decode(file_get_contents($votes_url));
    
    //cancello voti già inseriti
    db_delete("fanta_votes")->condition("round", $vote_round)->execute();
    
    $players_ids = Player::getIdList();

    // cancello i giocatori della giornata per evitare doppioni
    if ($vote_round > 1) {
        db_delete("fanta_players_rounds")->condition("round", $vote_round)->execute();

        $query = db_select("fanta_players_rounds", "pr");
        $query->condition("round", $vote_round - 1);
        $query->fields("pr");
        $result = $query->execute();

        foreach ($result as $row) {
          db_insert("fanta_players_rounds")
          ->fields(array("pl_id" => $row->pl_id, "round" => $vote_round, "rt_id" => $row->rt_id, "quotation" => $row->quotation, "not_rounded_quotation" => $row->not_rounded_quotation, "active" => $row->active))
          ->execute();
        }
    }
    
    // real teams
    $real_teams = RealTeam::allNames();
    $real_teams = array_flip($real_teams);
    
    $roles = array_flip($roles);

    foreach ($votes->votes as $name => $vote) {
      if (!in_array($name, array_keys($players_ids))) {
      	$pl_id = db_insert("fanta_players")->fields(array("name" => strtoupper($name), "role" => $roles[strtoupper($vote->role)]))->execute();
      	$rt_id = $real_teams[strtolower($vote->team)];
      	db_insert("fanta_players_rounds")->fields(array("pl_id" => $pl_id, "rt_id" => $rt_id, "round" => $vote_round, "quotation" => 0, "not_rounded_quotation" => 0, "active" => 1))->execute();
      }
      else {
      	$pl_id = $players_ids[$name];
      }
      
        $total = $vote->vote + ($vote->goals_for * variable_get("fantacalcio_points_goals_for", "3")) + ($vote->penalty_goals * variable_get("fantacalcio_points_penalty_goals", "3")) + ($vote->assists * variable_get("fantacalcio_points_assists", "1")) + ($vote->saved_penalties * variable_get("fantacalcio_points_saved_penalties", "3")) + ($vote->goals_against * variable_get("fantacalcio_points_goals_against", "-1")) + ($vote->red_cards * variable_get("fantacalcio_points_red_card", "-1")) + ($vote->yellow_cards * variable_get("fantacalcio_points_yellow_card", "-0.5")) + ($vote->own_goals * variable_get("fantacalcio_points_own_goal", "-2")) + ($vote->missed_penalties * variable_get("fantacalcio_points_missed_penalties", "-3")) + ($vote->draw_goals * variable_get("fantacalcio_points_draw_goals", "1")) + ($vote->win_goals * variable_get("fantacalcio_points_win_goals", "3"));
    
        $query = db_insert("fanta_votes");
        $query->fields(array(
            "round" => $vote_round,
            "pl_id" => $pl_id,
            "provider" => variable_get("fantacalcio_votes_provider", "1"),
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

        $query->execute();
    
      
    }
  }
 
  static function updatePlayers($round) {
  /*
  $i = 0;

  // cancello i giocatori della giornata per evitare doppioni
  db_delete("fanta_players_rounds")->condition("round", $round + 1)->execute();

  $query = db_select("fanta_players_rounds", "pr");
  $query->condition("round", $round);
  $query->fields("pr"); 
  $result = $query->execute();

  foreach ($result as $row) {
    $i++;

    db_insert("fanta_players_rounds")
    ->fields(array("pl_id" => $row->pl_id, "round" => $round + 1, "rt_id" => $row->rt_id, "quotation" => $row->quotation, "not_rounded_quotation" => $row->not_rounded_quotation, "active" => $row->active))
    ->execute();
  }

  $message = "Giocatori aggiornati: " . $i . " (Giornata #" . $round . ")";
  drupal_set_message(check_plain($message));
  */
}

 
  static function getRegularsTeam($t_id, $pl_votes, $competition_round, $c_id) {
    $teams = array();
  
    $max_substitutions = variable_get("fantacalcio_max_substitutions", 0) == -1 ? PHP_INT_MAX : variable_get("fantacalcio_max_substitutions", 0);
  
    $substitutions = 0;
    
    //$module = Lineup::getModule($t_id, $c_id, $competition_round);
  
    // titolari senza voto da sostituire
    $query = db_select("fanta_lineups", "l");
    $query->join("fanta_players", "p", "p.pl_id = l.pl_id");
    $query->condition("has_played", 0);
    $query->condition("position", 1);
    $query->condition("round", $competition_round);
    $query->condition("c_id", $c_id);
    $query->condition("t_id", $t_id);
    $query->fields("p");
    $query->fields("l");
  
    $result = $query->execute();
  
    foreach ($result as $row) {
  
      if ($substitutions <= $max_substitutions) {
  
        $role = $row->role;
        $position = $row->position + 1; // echo "=".$position."<br>";
  
        $search = true;
  
        while ($search && $substitutions <= $max_substitutions) {
          $query_search = db_select("fanta_lineups", "l");
          $query_search->join("fanta_players", "p", "p.pl_id = l.pl_id");
          
          if(variable_get("fantacalcio_reserves_mode", 0) == 0)
	          $query_search->condition("role", $role);
	          
          $query_search->condition("position", $position);
          $query_search->condition("round", $competition_round);
          $query_search->condition("c_id", $c_id);
          $query_search->condition("t_id", $t_id);
          $query_search->fields("p");
          $query_search->fields("l");
  
          $result_search = $query_search->execute();
  
          if ($result_search->rowCount() > 0) {
  
            foreach ($result_search as $row_search) {
  
              $pl_id = $row_search->pl_id;
              
              $new_module_ok = true;
              if(variable_get("fantacalcio_reserves_mode", 0) == 1) {
              	$module[$role]--;
              	$module[$row_search->role]++;
              	
              	$new_module_ok =  in_array(implode("-", $module), $modules);              		
              }
  
              // se con il voto entra la riserva
              if (in_array($pl_id, $pl_votes) && $row_search->has_played == 0 && $new_module_ok) {
                $query_update = db_update("fanta_lineups");
                $query_update->fields(array("has_played" => 1));
                $query_update->condition("pl_id", $pl_id);
                $query_update->condition("t_id", $t_id);
                $query_update->condition("c_id", $c_id);
                $query_update->condition("round", $competition_round);
  
                $query_update->execute();
                
                $substitutions++;
                $search = false;
              }
              else {
                $position++;
              }
            }
          }
  
          // nessuna prima riserva ancora non entrata
          else {
            $position++;
            $search = false;
          }
        }
      }
    }
  }
  
  static function findRegulars($vote_round) {
  
    $teams = Team::all();
    
    $round = Round::getByRound($vote_round);
    $votes = $round->getVotes(variable_get("fantacalcio_votes_provider", 1));
   
    $pl_votes = array();

    foreach ($votes as $vote) {
      if ($vote->has_vote == 1)
        $pl_votes[] = $vote->pl_id;
    }  
    foreach ($round->competitions as $round_competition) {
  
      $c_id = $round_competition->competition_id;
      $competition_round = $round_competition->competition_round;
      
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
        self::getRegularsTeam($row->t_id, $pl_votes, $competition_round, $c_id);
      }
    }
  }
  
  static function getModifiers($vote_round) {
    
    if (variable_get('fantacalcio_modifier_role_0', '0') || variable_get('fantacalcio_modifier_role_1', '0') || variable_get('fantacalcio_modifier_role_2', '0') || variable_get('fantacalcio_modifier_role_3', '0')) {
      
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
          $mod_1_role_0 = self::getModifierRole_0($t1_id, $match->c_id, $vote_round, $competition_round, variable_get("fantacalcio_votes_provider", 1));
          $mod_2_role_0 = self::getModifierRole_0($t2_id, $match->c_id, $vote_round, $competition_round, variable_get("fantacalcio_votes_provider", 1));
        }
        else {
          $mod_1_role_0 = 0;
          $mod_2_role_0 = 0;
        }
        
        if (variable_get('fantacalcio_modifier_role_1', '0')) {
          $mod_1_role_1 = self::getModifierRole_1($t2_id, $match->c_id, $vote_round, $competition_round, variable_get("fantacalcio_votes_provider", 1));
          $mod_2_role_1 = self::getModifierRole_1($t1_id, $match->c_id, $vote_round, $competition_round, variable_get("fantacalcio_votes_provider", 1));
        }
        else {
          $mod_1_role_1 = 0;
          $mod_2_role_1 = 0;
        }
        
        if (variable_get('fantacalcio_modifier_role_2', '0')) {
          $mod_role_2 = self::getModifierRole_2($t1_id, $t2_id, $match->c_id, $vote_round, $competition_round, variable_get("fantacalcio_votes_provider", 1));
          $mod_1_role_2 = $mod_role_2[1];
          $mod_2_role_2 = $mod_role_2[2];
        }
        else {
          $mod_1_role_2 = 0;
          $mod_2_role_2 = 0;
        }
        
        if (variable_get('fantacalcio_modifier_role_3', '0')) {
          $mod_1_role_3 = self::getModifierRole_3($t1_id, $match->c_id, $vote_round, $competition_round, variable_get("fantacalcio_votes_provider", 1));
          $mod_2_role_3 = self::getModifierRole_3($t2_id, $match->c_id, $vote_round, $competition_round, variable_get("fantacalcio_votes_provider", 1));
        }
        else {
          $mod_1_role_3 = 0;
          $mod_2_role_3 = 0;
        }
        
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
      
    }

  }
  
  static function getTotals($vote_round) {
    
    $teams = Team::all();
  
    $round = Round::getByRound($vote_round);
    $votes = $round->getVotes(variable_get("fantacalcio_votes_provider", 1));
  
    $matches_competitions = array();
  
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
  
          $tot_votes_1 = self::getTotal($t1_id, $competition_round, $vote_round, $c_id, variable_get("fantacalcio_votes_provider", 1));
          $tot_votes_2 = self::getTotal($t2_id, $competition_round, $vote_round, $c_id, variable_get("fantacalcio_votes_provider", 1));
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
  
      }
  
      if ($competition->type == COMPETITION_TYPE_GP) {
  
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
              $total = get_total($team->id, $competition_round, $vote_round, $competition->id, variable_get("fantacalcio_votes_provider", 1));
  
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
  
              }
            }
          }
        }
  
      }
    }
  
  }
  
  static function updateRealTeamsMatches($vote_round) {

		//recupero voti
		$query = db_select("fanta_votes", "v");
		$query->join("fanta_players_rounds", "pr", "v.pl_id = pr.pl_id AND v.round = pr.round");
		$query->join("fanta_players", "p", "p.pl_id = v.pl_id AND pr.pl_id = p.pl_id");
		$query->condition("v.provider", variable_get("fantacalcio_votes_provider", 1));
		$query->condition("v.round", $vote_round);
		$query->fields("v");
		$query->fields("pr");

		$result = $query->execute();

		$teams_goals = array();
		foreach($result as $row) {
			$goals_against = 0;
			if (isset($teams_goals[$row->rt_id])) {
				$goals_against = $teams_goals[$row->rt_id];
			}
			$teams_goals[$row->rt_id] = $goals_against + $row->goals_against;
		}

		foreach($teams_goals as $rt_id => $goals) {
			//goals_1
			$query = db_update("fanta_real_teams_matches")->fields(array("goals_1" => $goals))->condition("rt2_id", $rt_id)->condition("round", $vote_round)->execute();
	
			//goals_2
			$query = db_update("fanta_real_teams_matches")->fields(array("goals_2" => $goals))->condition("rt1_id", $rt_id)->condition("round", $vote_round)->execute();
		}

		//winner_id
		db_update("fanta_real_teams_matches")->fields(array("winner_id" => "rt1_id"))->condition("goals_1", "goals_2", ">")->condition("round", $vote_round)->execute();
		db_update("fanta_real_teams_matches")->fields(array("winner_id" => "rt2_id"))->condition("goals_1", "goals_2", "<")->condition("round", $vote_round)->execute();
		db_update("fanta_real_teams_matches")->fields(array("winner_id" => -1))->condition("goals_1", "goals_2", "=")->condition("round", $vote_round)->execute();

		//played
		db_update("fanta_real_teams_matches")->fields(array("played", 1))->condition("round", $vote_round)->execute();

	}
  
  static function closeRound($vote_round) {

    global $user;
  
    $competitions = Competition::all();
  
    $round = Round::getByRound($vote_round);
  
    $body = "";
    foreach ($round->competitions as $c_id => $competition_round) {
      $body .= l($competition_round->label . " " . $competitions[$c_id]->name , "calendario/" . $competitions[$c_id]->name) . ", ";
    }
  
    $main_c_id = variable_get("fantacalcio_main_competition", 1);
    $main_competition = Competition::getDefault();//get_competition_name(variable_get("fantacalcio_main_competition", 1));
    $body = "Risultati calcolati per: " . substr($body, 0, -2);
    //   $body .= "<br/>" . base_path() . "calendario/" . $main_competition->sanitized_name ;
  
    $title = "Risultati " . $vote_round . "ª Giornata";
  
    $node = new stdClass(); // We create a new node object
    $node->type = "news"; // Or any other content type you want
  
    node_object_prepare($node);
  
    $node->title = $title;
    $node->language = LANGUAGE_NONE; // Or any language code if Locale module is enabled. More on this below *
    //   $node->path = array('alias' => 'news/risultati-' . $vote_round); // Setting a node path
    $node->uid = 1;//$user->uid; // Or any id you wish
  
    $node->body[$node->language][0]['value'] = $body;
    $node->body[$node->language][0]['summary'] = $body;
    $node->body[$node->language][0]['format']  = 'full_html'; 
 
    $node->status = 1;   // (1 or 0): published or unpublished
    $node->promote = 1;  // (1 or 0): promoted to front page or not
    $node->sticky = 0;  // (1 or 0): sticky at top of lists or not
    $node->comment = 0;  // 2 = comments open, 1 = comments closed, 0 = comments hidden
  
    $node = node_submit($node); // Prepare node for saving
    node_save($node);
  
    $nid = $node->nid;
    $node = node_load($nid);
  
    // Make this change a new revision
    $node->revision = 1;
  
    node_save($node);
  
    // rimuovi "risultati provvisori"
    db_update("fanta_rounds")->fields(array("status" => 1))->condition("round", $vote_round)->execute();
  
  }
  
  static function getModifierRole_0($t_id, $c_id, $vote_round, $competition_round, $votes_provider) {
  
    $query = db_select("fanta_lineups", "l");
    $query->join("fanta_players", "p", "p.pl_id = l.pl_id");
    $query->condition("p.role", 0);
    $query->condition("l.c_id", $c_id);
    $query->condition("l.round", $competition_round);
    $query->condition("l.t_id", $t_id);
    $query->condition("l.has_played", 1);
  
    $query->fields("p");
  
    $result = $query->execute();
  
    foreach ($result as $row) {
      $pl_id = $row->pl_id;
    }
  
    if (isset($pl_id)) {
      $sql = "SELECT * FROM {fanta_votes} " . "WHERE pl_id = '%d' " . "AND round = '%d' " . "AND provider = '%d'";
  
      $query = db_select("fanta_votes", "v");
      $query->condition("pl_id", $pl_id);
      $query->condition("round", $vote_round);
      $query->condition("provider", $votes_provider);
  
      $query->fields("v");
  
      $result = $query->execute();
  
      foreach ($result as $row) {
        if ($row->saved_penalties == 0 && $row->vote > 6)
          return $row->vote - 6;
        else
          return 0;
      }
    }
    else
      return 0;
  }
  
  static function getModifierRole_1($t_id, $c_id, $vote_round, $competition_round, $votes_provider) {
    $pl_ids = array();
    $role_0_lineup_number = 0;
    $role_0_played_number = 0;
    $sql = "SELECT * FROM {fanta_lineups} f, {fanta_players} p " . "WHERE f.pl_id = p.pl_id " . "AND p.role = 1 " . "AND f.c_id = '%d' " . "AND f.round = '%d'" . "AND f.t_id = '%d'";
  
    // $result = db_query($sql, $c_id, $competition_round, $t_id);
  
    $query = db_select("fanta_lineups", "l");
    $query->join("fanta_players", "p", "p.pl_id = l.pl_id");
    $query->condition("p.role", 1);
    $query->condition("l.c_id", $c_id);
    $query->condition("l.round", $competition_round);
    $query->condition("l.t_id", $t_id);
  
    $query->fields("p");
    $query->fields("l");
  
    $result = $query->execute();
  
    foreach ($result as $row) {
  
      if ($row->has_played == 1) {
        array_push($pl_ids, $row->pl_id);
        $role_0_played_number++;
      }
  
      if ($row->position == 1)
        $role_0_lineup_number++;
    }
  
    $elenco_dif_ids = implode(',', $pl_ids);
  
    $tot = 0;
    $sql = "SELECT DISTINCT pl_id, vote FROM {fanta_votes} " . "WHERE pl_id IN ($elenco_dif_ids) " . "AND round = '%d' " . "AND provider = '%d'";
    // $result = db_query($sql, $vote_round, $votes_provider);
  
    if (count($pl_ids) > 0) {
  
      $query = db_select("fanta_votes", "v");
  
      $query->fields("v");
  
      $query->condition("pl_id", $pl_ids, "IN");
      $query->condition("round", $vote_round);
      $query->condition("provider", $votes_provider);
  
      $result = $query->execute();
  
      foreach ($result as $row) {
        $tot += $row->vote;
      }
  
      $tot += 5 * ($role_0_lineup_number - $role_0_played_number);
      $avg = ($tot > 0) ? round($tot / $role_0_lineup_number, 2) : 0;
  
      $modifier = 27 - $role_0_lineup_number - floor($avg * 4);
  
      // echo "$t_id : $tot_difesa : $avg_difesa : $role_0_lineup_number : $mod_difesa <br>";
  
      return $modifier;
    }
    else {
      return 0;
    }
  }
  
  static function getModifierRole_2($t1_id, $t2_id, $c_id, $vote_round, $competition_round, $votes_provider) {
    $role_2_t1_ids = array();
    $role_2_t1_regulars = 0;
    $role_2_t1_played = 0;
    $role_2_t2_ids = array();
    $role_2_t2_regulars = 0;
    $role_2_t2_played = 0;
  
    // team 1
  
    $query = db_select("fanta_lineups", "l");
    $query->join("fanta_players", "p", "p.pl_id = l.pl_id");
    $query->condition("p.role", 2);
    $query->condition("l.c_id", $c_id);
    $query->condition("l.round", $competition_round);
    $query->condition("l.t_id", $t1_id);
  
    $query->fields("l");
    $query->fields("p");
  
    $result = $query->execute();
    foreach ($result as $row) {
      if ($row->has_played == 1) {
        array_push($role_2_t1_ids, $row->pl_id);
        $role_2_t1_played++;
      }
  
      if ($row->position == 1)
        $role_2_t1_regulars++;
    }
  
    // team 2
    $query = db_select("fanta_lineups", "l");
    $query->join("fanta_players", "p", "p.pl_id = l.pl_id");
    $query->condition("p.role", 2);
    $query->condition("l.c_id", $c_id);
    $query->condition("l.round", $competition_round);
    $query->condition("l.t_id", $t2_id);
  
    $query->fields("l");
    $query->fields("p");
  
    $result = $query->execute();
  
    foreach ($result as $row) {
      if ($row->has_played == 1) {
        array_push($role_2_t2_ids, $row->pl_id);
        $role_2_t2_played++;
      }
  
      if ($row->position == 1)
        $role_2_t2_regulars++;
    }
  
    $role_2_tot_1 = 0;
    $role_2_tot_2 = 0;
  
    if (count($role_2_t1_ids) > 0 && count($role_2_t2_ids)) {
  
      // voti team 1
      $query = db_select("fanta_votes", "v");
      $query->condition("pl_id", $role_2_t1_ids, "IN");
      $query->condition("round", $vote_round);
      $query->condition("provider", $votes_provider);
  
      $query->fields("v", array("vote", "pl_id"));
  
      $result = $query->execute(); // ($sql, $vote_round, $votes_provider);
      foreach ($result as $row) {
        $role_2_tot_1 += $row->vote;
      }
  
      // voti team 2
      $query = db_select("fanta_votes", "v");
      $query->condition("pl_id", $role_2_t2_ids, "IN");
      $query->condition("round", $vote_round);
      $query->condition("provider", $votes_provider);
  
      $query->fields("v", array("vote", "pl_id"));
  
      $result = $query->execute(); // ($sql, $vote_round, $votes_provider);
      foreach ($result as $row) {
        $role_2_tot_2 += $row->vote;
      }
  
      $role_2_tot_1 += 5 * ($role_2_t1_regulars - $role_2_t1_played);
      $role_2_tot_2 += 5 * ($role_2_t2_regulars - $role_2_t2_played);
  
      if ($role_2_t1_regulars > $role_2_t2_regulars)
        $role_2_tot_2 += 5 * ($role_2_t1_regulars - $role_2_t2_regulars);
  
      if ($role_2_t2_regulars > $role_2_t1_regulars)
        $role_2_tot_1 += 5 * ($role_2_t2_regulars - $role_2_t1_regulars);
  
      $scarto = $role_2_tot_1 - $role_2_tot_2;
  
      $scarto = floor(abs($scarto)) / 2;
  
      if ($role_2_tot_1 > $role_2_tot_2)
        $mod_role_2 = array(1 => $scarto, 2 => 0 - $scarto);
  
      if ($role_2_tot_1 < $role_2_tot_2)
        $mod_role_2 = array(1 => 0 - $scarto, 2 => $scarto);
  
      if ($role_2_tot_1 == $role_2_tot_2)
        $mod_role_2 = array(1 => 0, 2 => 0);
    }
    else
      $mod_role_2 = array(1 => 0, 2 => 0);
  
    return $mod_role_2;
  }
  
  static function getModifierRole_3($t_id, $c_id, $vote_round, $competition_round, $votes_provider) {
    $mod_role_3 = 0;
  
    $query = db_select("fanta_lineups", "l");
    $query->join("fanta_players", "p", "p.pl_id = l.pl_id");
    $query->condition("p.role", 3);
    $query->condition("l.c_id", $c_id);
    $query->condition("l.round", $competition_round);
    $query->condition("l.t_id", $t_id);
    $query->condition("l.has_played", 1);
  
    $query->fields("p");
  
    $result = $query->execute();
  
    foreach ($result as $row) {
      $pl_id = $row->pl_id;
  
      if ($pl_id) {
        $sql = "SELECT * FROM {fanta_votes} " . "WHERE pl_id = '%d' " . "AND round = '%d' " . "AND provider = '%d'";
  
        $query2 = db_select("fanta_votes", "v");
        $query2->condition("pl_id", $pl_id);
        $query2->condition("round", $vote_round);
        $query2->condition("provider", $votes_provider);
  
        $query2->fields("v");
  
        $result2 = $query2->execute();
        foreach ($result2 as $row2) {
          if ($row2->assists == 0 && $row2->goals_for == 0 && $row2->vote > 6)
            $mod_role_3 += ($row2->vote - 6);
        }
      }
    }
  
    return $mod_role_3;
  }
  
  static function getTotal($t_id, $competition_round, $vote_round, $c_id) {
    $total = 0;
  
    $query = db_select("fanta_votes", "v");
    $query->join("fanta_lineups", "l", "l.pl_id = v.pl_id");
    $query->fields("l");
    $query->fields("v");
    $query->condition("l.round", $competition_round);
    $query->condition("v.round", $vote_round);
    $query->condition("l.t_id", $t_id);
    $query->condition("l.c_id", $c_id);
    $query->condition("l.has_played", 1);
    $query->condition("v.provider", variable_get("fantacalcio_votes_provider", 1));
  
    $result = $query->execute();
  
    foreach ($result as $row) {
      $total += $row->total;
    }
  
    return $total;
  }
}

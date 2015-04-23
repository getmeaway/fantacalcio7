<?php

class Lineup {
	
	var $round;
	var $team;
	var $competition;
	var $regulars;
	var $reserves;
	
	static function get($competition_id, $team_id, $competition_round) {
		$lineups = array();
		$regulars = array();
		$reserves = array();
		
		$query = db_select("fanta_lineups", "l");
		$query->join("fanta_players", "p", "p.pl_id = l.pl_id");
		$query->join("fanta_players_rounds", "pr", "pr.pl_id = p.pl_id");
		$query->join("fanta_real_teams", "rt", "rt.rt_id = pr.rt_id");
		$query->join("fanta_rounds_competitions", "rc", "rc.round = pr.round AND l.round = rc.competition_round");
		$query->condition("l.t_id", $team_id);
		$query->condition("l.c_id", $competition_id);
		$query->condition("l.round", $competition_round);
		$query->fields("l");
		$query->fields("p");
		$query->addField("rt", "name", "team");
		
		$result = $query->execute();
		
		if ($result->rowCount() > 0) {
			 
			foreach ($result as $row) {
				$pl_id = $row->pl_id;
				$lineups[$pl_id] = $row;
				$player = new Player($row->pl_id, $row->name, $row->role);
				$player->team = $row->team;
				$player->position = $row->position;
				$positions[$row->position][$pl_id] = $player;
				
			}
			
			$lineup = new Lineup(); 
			$lineup->team = Team::get($team_id);
			$lineup->round = Round::get($competition_round, $competition_id);
			$lineup->competition = Competition::get($competition_id);
			$lineup->regulars = $regulars;
			$lineup->reserves = $reserves;
			$lineup->positions = $positions;
			
			return $lineup;
		}
		else 
			return null;
	}
	
	static function getForForm($competition_id, $team_id, $competition_round) {
		
		$squad = Team::get($team_id)->getSquad();
		
		$lineup = array();
		
		foreach ($squad as $player) {
			$pl_id = $player->pl_id;
			$lineup[$pl_id] = array("pl_id" => $pl_id, "role" => $player->role, "position" => 0);
		}
		
		$query = db_select("fanta_lineups", "l");
		$query->join("fanta_players", "p", "p.pl_id = l.pl_id");
		$query->condition("l.t_id", $team_id);
		$query->condition("l.c_id", $competition_id);
		$query->condition("l.round", $competition_round);
		$query->fields("l");
		$query->fields("p");		
	
		$result = $query->execute();
	
		foreach ($result as $row) {
			$pl_id = $row->pl_id;
			$lineup[$pl_id] = array("pl_id" => $pl_id, "role" => $row->role, "position" => $row->position);
		}
		
		return $lineup;
	}
	
	static function allForRound($round, $competition) {
		$lineups = array();
		
		$query = db_select("fanta_lineups", "l");
		$query->condition("c_id", $competition->id);
		$query->condition("round", $round->competition_round);
		$query->fields("l");
		
		$result = $query->execute();
		
		foreach ($result as $row) {
			$lineups[$row->t_id] = self::get($competition->id, $row->t_id, $round->competition_round);
		}
		
		return $lineups;
	}
	
	static function exists($t_id, $c_id, $round) {
	
		$query = db_select("fanta_lineups", "l");
		$query->condition("t_id", $t_id);
		$query->condition("c_id", $c_id);
		$query->condition("round", $round);
		$query->fields("l");
		$result = $query->execute();
				
		if ($result->rowCount() > 0) 
			return TRUE;
		else 
			return FALSE;	
	}
	
	function check(){
		//massima posizione
		$max_position = 0;
		
		foreach($this->positions as $position => $player) {
			if ($position > $max_position)
				$max_position = $position;
		}
	
		//inizializzo gli array
		$positions = array();
		for($j = 0; $j <= $max_position; $j++){
			$positions[$j] = array(0, 0, 0, 0);
		}
	
		//conto gli elementi (per posizione e per ruolo)
		foreach($this->positions as $position => $players) {
			foreach($players as $pl_id => $player) {
// 				$currPosition = $player['position'];
				$currRole = $player->role;
				$positions[$position][$currRole] = $positions[$position][$currRole] + 1;
			}
		}
	
		//moduli consentiti e numero giocatori
		$check_number_1 = TRUE;
		$check_module_1 = TRUE;
		$check_number_2_3 = TRUE;
		$check_module_2_3 = TRUE;
		$modules_1 = array(array(1, 3, 4, 3), array(1, 3, 5, 2), array(1, 4, 3, 3), array(1, 4, 4, 2), array(1, 4, 5, 1), array(1, 5, 3, 2), array(1, 5, 4, 1), array(1, 6, 3, 1)); //TODO prenderli da variable_get
		$modules_2 = array(array(1, 1, 1, 1));
		$modules_3 = array(array(0, 1, 1, 1));
	
		//verifico titolari
		if (isset($positions[1]) && $positions[1] != null ) {
			//numero titolari
			$number_1 = $positions[1][0] + $positions[1][1] + $positions[1][2] + $positions[1][3];
			if($number_1 != 11)
				$check_number_1 = FALSE;
	
			//modulo titolari
			$module_1 = array($positions[1][0], $positions[1][1], $positions[1][2], $positions[1][3]);
			if(!in_array($module_1, $modules_1))
				$check_module_1 = FALSE;
		}
		else {
			$check_number_1 = FALSE;
			$check_module_1 = FALSE;
		}
	
		//verifico riserve
		if (isset($positions[2]) && $positions[2] != null && $positions[3] != null) {
			//numero riserve
			$number_2_3 = $positions[2][0] + $positions[2][1] + $positions[2][2] + $positions[3][3] + $positions[3][0] + $positions[3][1] + $positions[3][2] + $positions[3][3];
			if($number_2_3 != 7)
				$check_number_2_3 = FALSE;
	
			//modulo riserve
			$module_2 = array($positions[2][0], $positions[2][1], $positions[2][2], $positions[2][3]);
			$module_3 = array($positions[3][0], $positions[3][1], $positions[3][2], $positions[3][3]);
			if(!in_array($module_2, $modules_2) || !in_array($module_3, $modules_3))
				$check_module_2_3 = FALSE;
		}
		else {
			$check_number_2_3 = FALSE;
			$check_module_2_3 = FALSE;
		}
	
		return array('regulars_number' => $check_number_1, 'regulars_module' => $check_module_1, 'reserves_number' => $check_number_2_3, 'reserves_module' => $check_module_2_3);
	}
	
	function getModuleRegulars() {
		$module = array();
		
		foreach($this->positions[1] as $regular) {
			if(!isset($module[$regular->role]))
				$module[$regular->role] = 0;
			$module[$regular->role]++;
		}	
		
		ksort($module);
		
		return $module;
	}

	function getModuleReserves() {
		$module = array();
		
		$reserves = array();
		foreach($this->positions as $position => $reserve) {
			if ($position > 1)
			$reserves = array_merge($reserves, $reserve);
		} 
	
		foreach($reserves as $reserve) {
			if(!isset($module[$reserve->role]))
				$module[$reserve->role] = 0;
			$module[$reserve->role]++;
		}
	
		ksort($module);
	
		return $module;
	}
	
	function getInsertTime() {
		
		$query = db_select("fanta_lineups_inserts", "i");
		$query->condition("c_id", $this->competition->id);
		$query->condition("t_id", $this->team->id);
		$query->condition("round", $this->round->competition_round);
		$query->fields("i", array("timestamp"));
		
		$result = $query->execute();
		
		return $result->fetchObject()->timestamp;
		
	}
}
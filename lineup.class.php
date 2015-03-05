<?php

class Lineup {
	
	var $round;
	var $team;
	var $competition;
	
	static function get($competition_round, $team_id, $competition_id) {
		$lineups = array();
		$starting_lineups = array();
		$reserves_lineups = array();
		
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
				$positions[$row->position][$pl_id] = $row;
				if ($row->position == 1)
					$starting_lineups[$pl_id] = $row;
				elseif (in_array($row->position, array(2, 3)))
				$reserves_lineups[$pl_id] = $row;
			}
			
			return array("positions" => $positions, "starting_lineups" => $starting_lineups, "reserves_lineups" => $reserves_lineups);
		}
		else 
			return null;
	}
}
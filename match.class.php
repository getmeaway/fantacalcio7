<?php

class Match {
	
	var $id;
	var $round;
	var $competition;
	var $group;
	var $home_team;
	var $away_team;
	var $home_points;
	var $away_points;
	var $home_modifier_0;
	var $away_modifier_0;
	var $home_modifier_1;
	var $away_modifier_1;
	var $home_modifier_2;
	var $away_modifier_2;
	var $home_modifier_3;
	var $away_modifier_3;
	var $home_bonus;
	var $away_bonus;
	var $home_total;
	var $away_total;
	var $home_goals;
	var $away_goals;
	var $winner;
	var $played;
	
	static function get($id) {
		
	}
	
	static function getByGroup($g_id) {
		$matches = array();
		
		$query = db_select("fanta_matches", "m");
		$query->condition("m.g_id", $g_id);
		$query->join("fanta_teams", "t1", "t1.t_id = m.t1_id");
		$query->join("fanta_teams", "t2", "t2.t_id = m.t2_id");
		$query->join("fanta_rounds_competitions", "rc", "rc.competition_round = m.round");
		$query->join("fanta_rounds", "r", "r.round = rc.round");
		$query->fields("m");
		$query->addField("t1", "name", "home_team");
		$query->addField("t2", "name", "away_team");
		$query->addField("r", "date", "date");
		
		$result = $query->execute();
				
		while ($row = $result->fetchObject()) {
			$match = new Match();
			$match->id = $row->m_id;
			$match->home_team = $row->home_team;
			$match->away_team = $row->away_team;
			$match->date = $row->date;
			$match->round = $row->round;
// 			$m_id = $row->m_id;
// 			$matches[$m_id] = $row;
// 			$c_id = $groups[$row->g_id];
// 			$matches[$m_id]->date = $dates[$c_id][$row->round];
			
			$matches[$row->round][$match->id] = $match;
		}
		
		return $matches;
	}
	
	
}
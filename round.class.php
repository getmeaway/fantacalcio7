<?php

class Round
{

	var $number;
	
	static function exists($round) {
		$query = db_select("fanta_rounds", "r");
		$query->condition("round", $round);
		$query->addField("r", "round");
		$result = $query->execute();
		
		return $result->rowCount() > 0;
	}

	static function getLast () {
		$result = db_select("fanta_votes", "r");
		$result->addExpression("MAX(round)");
		return $result->execute()->fetchField();
	}
	
	static function getNextForCompetition($competition_id) {
		$round = self::getLast();
		
		$query = db_select("fanta_rounds_competitions", "rc");
		$query->condition("c_id", $competition_id);
		$query->condition("round", $round);
		$query->addField("rc", "competition_round");
		$result = $query->execute();
		
		return $result->fetchField();
	}
	
	static function listForStandings($competition_id) {
		$rounds = array();
		
		$query = db_select("fanta_teams_rounds", "tr");
		$query->join("fanta_rounds_competitions", "rc", "rc.round = tr.round");
		$query->condition("tr.c_id", $competition_id);
		$query->fields("tr", array("round"));
		$query->fields("rc", array("round_label"));
		$result = $query->execute();
		
		while($row = $result->fetchObject()) {
			$rounds[$row->round] = (!empty($row->round_label)) ? $row->round_label : $row->round;
		}
		
		return $rounds;
	
	}

}
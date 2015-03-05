<?php

class Round
{

	var $number;
	
	static function getCompetitionRound($round, $competition_id) {
		$query = db_select("fanta_rounds_competitions", "rc");
		$query->condition("round", $round);
		$query->condition("c_id", $competition_id);
		$query->fields("rc");
		$result = $query->execute();
		
		while ($row = $result->fetchObject()) {
			$competition_round = new Round();
			$competition_round->competition_round= $row->competition_round;
			$competition_round->round= $row->round;
			$competition_round->label = (empty($row->round_label) ? $row->competition_round . t("ª giornata") : $row->round_label);
			$competition_round->next = $row->next;

			return $competition_round;
		}
		
		return null;
	}
	
	static function getRoundByCompetitionRound($competition_round, $competition_id) {
		$query = db_select("fanta_rounds_competitions", "rc");
		$query->condition("competition_round", $competition_round);
		$query->condition("c_id", $competition_id);
		$query->addField("rc", "round");
		$result = $query->execute();
		
// 		while($row = $result-)
		
		return $result->fetchField();
	}
	
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
	
	static function getLastLineups($competition_id) {
		$round = self::getLast();
	
		$query = db_select("fanta_lineups", "l");
		$query->condition("c_id", $competition_id);		
		$query->addExpression("MAX(round)", "max");
		$result = $query->execute();
	
		$max_round = $result->fetchField()->max;
		
		return self::get($max_round, $competition_id);
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

	static function listForCalendar($competition_id) {
		$rounds = array();
	
		$query = db_select("fanta_rounds", "r");
		$query->join("fanta_rounds_competitions", "rc", "rc.round = r.round");
		$query->condition("rc.c_id", $competition_id);
		$query->fields("r", array("round"));
		$query->fields("rc", array("round_label"));
		$query->orderBy("round");
		$result = $query->execute();
	
		while($row = $result->fetchObject()) {
			$rounds[$row->round] = (!empty($row->round_label)) ? $row->round_label : $row->round;
		}
	
		return $rounds;
	
	}
}
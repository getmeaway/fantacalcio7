<?php

class Round
{

	var $competition_round;
	var $round;
	var $label;
	var $next;
	var $date;
	
// 	function __construct($round, $competition_round, $label, $next, $date) {
// 		$this->round = $round;
// 		$this->competition_round = $competition_round;
// 		$this->label = $label;
// 		$this->next = $next;
// 		$this->date = $date;
// 	}

	static function all() {
		$rounds = array();
		
		$query = db_select("fanta_rounds_competitions", "rc");
		$query->join("fanta_rounds", "r", "r.round = rc.round");
		$query->fields("rc");
		$query->fields("r");
		$result = $query->execute();
		
		while ($row = $result->fetchObject()) {
			$competition_round = new Round();
			$competition_round->competition_round= $row->competition_round;
			$competition_round->round= $row->round;
			$competition_round->label = (empty($row->round_label) ? $row->competition_round . t("ª giornata") : $row->round_label);
			$competition_round->next = $row->next;
			$competition_round->date = $row->date;
		
			array_push($rounds, $competition_round);
		}
		
		return $rounds;
	}

	static function allWithStatus() {
		$rounds = array();
	
		$query = db_select("fanta_rounds", "r");
		$query->join("fanta_round_statuses", "s", "s.s_id = r.status");
		$query->fields("r", array("round", "date"));
		$query->fields("s", array("status"));
		$query->orderBy("r.round");
		$result = $query->execute();
	
		while ($row = $result->fetchObject()) {
			$competition_round = new Round();
			$competition_round->round= $row->round;
			$competition_round->date = $row->date;
			$competition_round->status = $row->status;
	
			array_push($rounds, $competition_round);
		}
	
		return $rounds;
	}
	
	static function allPlayed() {
		$rounds = array();
	
		$query = db_select("fanta_rounds_competitions", "rc");
		$query->join("fanta_rounds", "r", "r.round = rc.round");
		$query->fields("rc");
		$query->fields("r");
		$query->condition("status", 1);
		$result = $query->execute();
	
		while ($row = $result->fetchObject()) {
			$competition_round = new Round();
			$competition_round->competition_round= $row->competition_round;
			$competition_round->round= $row->round;
			$competition_round->label = (empty($row->round_label) ? $row->competition_round . t("ª giornata") : $row->round_label);
			$competition_round->next = $row->next;
			$competition_round->date = $row->date;
	
			array_push($rounds, $competition_round);
		}
	
		return $rounds;
	}
	
	static function get($round, $competition_id) {
		$query = db_select("fanta_rounds_competitions", "rc");
		$query->join("fanta_rounds", "r", "r.round = rc.round");
		$query->condition("rc.round", $round);
		$query->condition("c_id", $competition_id);
		$query->fields("rc");
		$query->fields("r");
		$result = $query->execute();
	
		while ($row = $result->fetchObject()) {
			$competition_round = new Round();
			$competition_round->competition_round= $row->competition_round;
			$competition_round->round= $row->round;
			$competition_round->label = (empty($row->round_label) ? $row->competition_round . t("ª giornata") : $row->round_label);
			$competition_round->next = $row->next;
			$competition_round->date = $row->date;
	
			return $competition_round;
		}
	
		return null;
	}
	
	static function getByRound($vote_round) {
		
		$round = null;
		
		$query = db_select("fanta_rounds", "r");
		$query->condition("r.round", $vote_round);
		$query->fields("r");
		$result = $query->execute();
	
		while ($row = $result->fetchObject()) {
			
			$round = new Round();
			$round->round= $row->round;
			$round->date = $row->date;
			
			$query2 = db_select("fanta_rounds_competitions", "rc");
			$query2->condition("rc.round", $vote_round);
			$query2->fields("rc");
			
			$result2 = $query2->execute();
			
			$rounds_competitions = array();
			while ($row2 = $result2->fetchObject()) {
				array_push($rounds_competitions, (object) array("c_id" => $row2->c_id, "competition_round" => $row2->competition_round, "round_label" => $row2->round_label));
			}
	
			$round->competitions = $rounds_competitions;
			
		}
	
		return $round;
	}
	
	static function getByCompetitionRound($competition_round, $competition_id) {
		$query = db_select("fanta_rounds_competitions", "rc");
		$query->join("fanta_rounds", "r", "r.round = rc.round");
		$query->condition("rc.competition_round", $competition_round);
		$query->condition("c_id", $competition_id);
		$query->fields("rc");
		$query->fields("r");
		$result = $query->execute();
	
		while ($row = $result->fetchObject()) {
			$competition_round = new Round();
			$competition_round->competition_round= $row->competition_round;
			$competition_round->round= $row->round;
			$competition_round->label = (empty($row->round_label) ? $row->competition_round . t("ª giornata") : $row->round_label);
			$competition_round->next = $row->next;
			$competition_round->date = $row->date;
	
			return $competition_round;
		}
	
		return null;
	}
	
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
			
			//$competition_round = new Round($row->round, $row->competition_round, null, $row->next, null);
			$competition_round->label = (empty($row->round_label) ? $row->competition_round . t("ª giornata") : $row->round_label);

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
		
		return $result->fetchField();
	}
	
	static function exists($round) {
		$query = db_select("fanta_rounds", "r");
		$query->condition("round", $round);
		$query->addField("r", "round");
		$result = $query->execute();
		
		return $result->rowCount() > 0;
	}
	
	static function existsInCompetition($round, $c_id) {
		$query = db_select("fanta_rounds", "r");
		$query->join("fanta_rounds_competitions", "rc", "rc.round = r.round");
		$query->condition("rc.round", $round);
		$query->condition("rc.c_id", $c_id);
		$query->addField("r", "round");
		$result = $query->execute();
	
		return $result->rowCount() > 0;
	}

	static function getLast() {
		$query = db_select("fanta_votes", "r");
		$query->addExpression("MAX(round)");
		$result = $query->execute();
		
		if ($result->rowCount() == 0)
			return 0;
		
		return $result->fetchField();
	}
	
	static function getLastQuotation() {
		$query = db_select("fanta_players_rounds", "pr");
		$query->addExpression("MAX(round)");
		$result = $query->execute();
	
		if ($result->rowCount() == 0)
			return 0;
	
		return $result->fetchField();
	}
	
	static function getLastLineup ($c_id) {
		$query = db_select("fanta_lineups", "l");
		$query->condition("c_id", $c_id);
		$query->addExpression("MAX(round)");
		$result = $query->execute();
	
		if ($result->rowCount() == 0)
			return 0;
	
		return $result->fetchField();
	}
	
	static function getNextForCompetition($competition_id) {
		$round = self::getLast();
		
		$query = db_select("fanta_rounds_competitions", "rc");
		$query->condition("c_id", $competition_id);
		$query->condition("round", $round);
		$query->addField("rc", "competition_round");
		$result = $query->execute();
		
		return self::get($result->fetchField(), $competition_id);
	}
	
	static function getLastLineups($competition_id) {
		$round = self::getLast();
	
		$query = db_select("fanta_lineups", "l");
		$query->condition("c_id", $competition_id);		
		$query->addExpression("MAX(round)", "max");
		$result = $query->execute();
	
		$max_round = $result->fetchField();
		
		return self::getByCompetitionRound($max_round, $competition_id);
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
	
	function getVotes($provider) {
  		$votes = array();
  		
  		$query = db_select("fanta_votes", "v");
  		$query->condition("round", $this->round);
  		$query->condition("provider", $provider);
  		$query->fields("v");
		
		$result = $query->execute();
		
		foreach ($result as $row) {
			$pl_id = $row->pl_id;
			$votes[$pl_id] = $row;
		}
		 
		return $votes;
	}
	
	function getMatches($competition_id) {
		
// 		$sql = "SELECT * FROM {fanta_matches} WHERE g_id IN (SELECT g_id FROM {fanta_groups} WHERE c_id = :c_id) AND round = :round";
		
		$query = db_select("fanta_matches", "m");
		$query->fields("m");
		$query->condition("round", self::getCompetitionRound($this->round, $competition_id)->competition_round);
		$query->condition("g_id", array_map(function($item) {return ($item->id);}, Group::allByCompetition($competition_id)), "IN");
		
		//$result = db_query($sql, array(":c_id" => $c_id, ":round" => $round));
		
		$result = $query->execute();
	
		$matches = array();
		foreach ($result as $row) {
			$m_id = $row->m_id;
			$matches[$m_id] = $row;
			$matches[$m_id]->date = $this->date;
		}
		
		return $matches;
	}
}
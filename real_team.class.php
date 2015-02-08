<?php

class RealTeam
{

	var $id;

	var $name;

	function __construct ($id, $name) {
		$this->id = $id;
		$this->name = $name;
	}

	static function get ($id) {
		$team = null;
		$query = db_select("fanta_real_teams", "r");
		$query->condition("rt_id", $id);
		$query->fields("r");
		$result = $query->execute();
		foreach ( $result as $row ) {
			$team = new RealTeam($row->t_id, $row->name);
		}
		
		return $team;
	}

	static function all () {
		$teams = array();
		$query = db_select("fanta_real_teams", "r");
		$query->fields("r");
		$result = $query->execute();
		foreach ( $result as $row ) {
			array_push($teams, new RealTeam($row->rt_id, $row->name));
		}
		
		return $teams;
	}
	
	static function allActive () {
		$teams = array();
		$query = db_select("fanta_teams", "t");
		$query->condition("active", 1);
		$query->orderBy("name");
		$query->fields("t");
		$result = $query->execute();
		foreach ( $result as $row ) {
			array_push($teams, new Team($row->t_id, $row->name, $row->uid));
		}
	
		return $teams;
	}

	static function allByGroup ($g_id) {
		$teams = array();
		$query = db_select("fanta_teams", "t");
		$query->join("fanta_teams_groups", "g", "g.t_id =  t.t_id");
		$query->fields("t");
		$query->condition("g_id", $g_id);
		$result = $query->execute();
		foreach ( $result as $row ) {
			array_push($teams, new Team($row->t_id, $row->name, $row->uid));
		}
		
		return $teams;
	}
	
	static function allByUser ($u_id) {
		$teams = array();
		$query = db_select("fanta_teams", "t");
		//$query->join("fanta_teams_groups", "g", "g.t_id =  t.t_id");
		$query->fields("t");
		$query->condition("t.uid", $u_id);
		$result = $query->execute();
		foreach ( $result as $row ) {
			array_push($teams, new Team($row->t_id, $row->name, $row->uid));
		}
	
		return $teams;
	}

	function getSquad () {
		$squad = array();
		
		$round = Round::getLast();
		
		$selled_players = array();
		$query = db_select("fanta_squads", "s");
		$query->condition("s.t_id", $this->id);
		$query->condition("s.status", -1);
		$query->fields("s", array(
				"pl_id",
				"timestamp" 
		));
		$result = $query->execute();
		while ( $row = $result->fetchObject() ) {
			if (!isset($selled_players [$row->pl_id]) || $selled_players [$row->pl_id] < $row->timestamp)
				$selled_players [$row->pl_id] = $row->timestamp;
		}
		
		$query = db_select("fanta_squads", "s");
		$query->join("fanta_players", "p", "s.pl_id = p.pl_id");
		$query->join("fanta_players_rounds", "r", "s.pl_id = r.pl_id");
		$query->join("fanta_real_teams", "rt", "rt.rt_id = r.rt_id");
		$query->condition("s.t_id", $this->id);
		$query->condition("r.round", $round);
		$query->fields("s");
		$query->fields("p", array(
				"name",
				"role" 
		));
		$query->fields("r", array(
				"quotation" 
		));
		$query->addField("rt", "name", "team_name");
		$query->orderBy("role", "ASC");
		$query->orderBy("name", "ASC");
		$result = $query->execute();
		while ( $row = $result->fetchObject() ) {
			if (!array_key_exists($row->pl_id, $selled_players) || $selled_players [$row->pl_id] < $row->timestamp)
				$squad [$row->pl_id] = $row;
		}
		
		return $squad;
	}
	
	function isSquadComplete() {
		return TRUE;
	}
	
	function getMovements() {
		$movements = array();

		$query = db_select("fanta_squads", "s");
		$query->join("fanta_players", "p", "p.pl_id = s.pl_id");
		$query->join("fanta_players_rounds", "r", "p.pl_id = r.pl_id");
		$query->join("fanta_real_teams", "rt", "rt.rt_id = r.rt_id");
		$query->fields("s");
		$query->fields("r");
		$query->condition("s.t_id", $this->id);
		$query->condition("r.round", Round::getLast());
		$query->addField("rt", "name", "team_name");
		$query->addField("p", "name", "player_name");
		$query->orderBy("s.timestamp", "DESC");
	
		$result = $query->execute();
	
		while($row = $result->fetchObject()) {
			if ($row->status == 1) {
				$text = t("Acquisto di @name (@team)", array("@name" => $row->player_name , "@team" => $row->team_name));
				$credits = "-" . $row->cost;
			}
			else if ($row->status == -1) {
				$text = t("Cessione di @name (@team)", array("@name" => $row->player_name , "@team" => $row->team_name));
				$credits = "+" . $row->cost;
			}
			array_push($movements, array(date("d/m/Y - H:i", $row->timestamp), $text, $credits));
		}
	
		return $movements;
	}

}
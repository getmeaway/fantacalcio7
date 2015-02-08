<?php

class Team
{

	var $id;
	var $name;
	var $user;
	var $honours;
	
	function __construct ($id, $name, $user) {
		$this->id = $id;
		$this->name = $name;
		$this->user = $user;
	}

	static function get ($id) {
		$team = null;
		$query = db_select("fanta_teams", "t");
		$query->condition("t_id", $id);
		$query->fields("t");
		$result = $query->execute();
		foreach ( $result as $row ) {
			$team = new Team($row->t_id, $row->name, $row->uid);
			$team->register_date = $row->register_date;
		}
		
		return $team;
	}

	static function all () {
		$teams = array();
		$query = db_select("fanta_teams", "t");
		$query->fields("t");
		$result = $query->execute();
		foreach ( $result as $row ) {
			$teams[$row->t_id] = new Team($row->t_id, $row->name, $row->uid);
			$teams[$row->t_id]->register_date = $row->register_date;
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
			$teams[$row->t_id] = new Team($row->t_id, $row->name, $row->uid);
			$teams[$row->t_id]->register_date = $row->register_date;
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
			$teams[$row->t_id] = new Team($row->t_id, $row->name, $row->uid);
			$teams[$row->t_id]->register_date = $row->register_date;
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
			$teams[$row->t_id] = new Team($row->t_id, $row->name, $row->uid);
			$teams[$row->t_id]->register_date = $row->register_date;
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
		$query->fields("p", array("name","role"));
		$query->fields("r", array("quotation"));
		$query->addField("rt", "name", "team");
		$query->orderBy("role", "ASC");
		$query->orderBy("name", "ASC");
		$result = $query->execute();
		while ( $row = $result->fetchObject() ) {
			//if (!array_key_exists($row->pl_id, $selled_players) || $selled_players [$row->pl_id] < $row->timestamp)
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
		$query->addField("rt", "name", "team");
		$query->fields("p");
		$query->orderBy("s.timestamp", "DESC");
	
		$result = $query->execute();
	
		while($row = $result->fetchObject()) {
			if ($row->status == 1) {
				$movement = t("Acquisto");
				$credits = "-" . $row->cost;
			}
			else if ($row->status == -1) {
				$movement = t("Cessione");
				$credits = "+" . $row->cost;
			}
			array_push($movements, array("<span class='fa-stack'>
						<i class='fa fa-square fa-stack-2x role-" . $row->role . "'></i>
						<i class='fa fa-stack-1x' style='color: white;'><span class='font-normal'>" . Player::convertRole($row->role) . "</span></i>
					</span>", $row->name, $row->team, date("d/m/Y - H:i", $row->timestamp), $movement, $credits));
		}
	
		return $movements;
	}
	
	function numWin($group_id) {
		$query = db_select("fanta_matches", "m");
		$query->condition("winner_id", $this->id);
		$query->condition("g_id", $group_id);
		$query->condition("played", 1);
		$query->addExpression("COUNT(m_id)", "n");
		
		$result = $query->execute();
		return $result->fetchObject()->n;
	}
	
	function numLost($group_id) {
		$query = db_select("fanta_matches", "m");
		
		$or = db_or()->condition('t1_id', $this->id)->condition('t2_id', $this->id);
		
		$query->condition($or);
		$query->condition("winner_id", $this->id, "<>");
		$query->condition("winner_id", "-1", "<>");
		$query->condition("g_id", $group_id);
		$query->condition("played", 1);
		$query->addExpression("COUNT(m_id)", "n");
	
		$result = $query->execute();
		
		return $result->fetchObject()->n;
	}
	
	function numDraw($group_id) {
		$query = db_select("fanta_matches", "m");
	
		$or = db_or()->condition('t1_id', $this->id)->condition('t2_id', $this->id);
	
		$query->condition($or);
		$query->condition("winner_id", "-1");
		$query->condition("g_id", $group_id);
		$query->condition("played", 1);
		$query->addExpression("COUNT(m_id)", "n");
	
		$result = $query->execute();
	
		return $result->fetchObject()->n;
	}

	function goalsFor($group_id) {
	  
	  $query = db_select("fanta_matches", "m");
	  $query->condition("t1_id", $this->id);
	  $query->condition("played", 1);
	  $query->condition("g_id", $group_id);
	  $query->addExpression("SUM(goals_1)", "n");
	  
	  $result = $query->execute();
	  $goals_home = $result->fetchObject()->n;
	  
	  $query = db_select("fanta_matches", "m");
	  $query->condition("t2_id", $this->id);
	  $query->condition("played", 1);
	  $query->condition("g_id", $group_id);
	  $query->addExpression("SUM(goals_2)", "n");
	   
	  $result = $query->execute();
	  $goals_away = $result->fetchObject()->n;
	  
	  return $goals_home + $goals_away;
	}
	
	function goalsAgainst($group_id) {
		 
		$query = db_select("fanta_matches", "m");
		$query->condition("t1_id", $this->id);
		$query->condition("played", 1);
		$query->condition("g_id", $group_id);
		$query->addExpression("SUM(goals_2)", "n");
		 
		$result = $query->execute();
		$goals_home = $result->fetchObject()->n;
		 
		$query = db_select("fanta_matches", "m");
		$query->condition("t2_id", $this->id);
		$query->condition("played", 1);
		$query->condition("g_id", $group_id);
		$query->addExpression("SUM(goals_1)", "n");
	
		$result = $query->execute();
		$goals_away = $result->fetchObject()->n;
		 
		return $goals_home + $goals_away;
	}
	

	function pointsFor($group_id) {
		 
		$query = db_select("fanta_matches", "m");
		$query->condition("t1_id", $this->id);
		$query->condition("played", 1);
		$query->condition("g_id", $group_id);
		$query->addExpression("SUM(tot_1)", "n");
		 
		$result = $query->execute();
		$goals_home = $result->fetchObject()->n;
		 
		$query = db_select("fanta_matches", "m");
		$query->condition("t2_id", $this->id);
		$query->condition("played", 1);
		$query->condition("g_id", $group_id);
		$query->addExpression("SUM(tot_2)", "n");
	
		$result = $query->execute();
		$goals_away = $result->fetchObject()->n;
		 
		return $goals_home + $goals_away;
	}
	
	function pointsAgainst($group_id) {
			
		$query = db_select("fanta_matches", "m");
		$query->condition("t1_id", $this->id);
		$query->condition("played", 1);
		$query->condition("g_id", $group_id);
		$query->addExpression("SUM(tot_2)", "n");
			
		$result = $query->execute();
		$goals_home = $result->fetchObject()->n;
			
		$query = db_select("fanta_matches", "m");
		$query->condition("t2_id", $this->id);
		$query->condition("played", 1);
		$query->condition("g_id", $group_id);
		$query->addExpression("SUM(tot_1)", "n");
	
		$result = $query->execute();
		$goals_away = $result->fetchObject()->n;
			
		return $goals_home + $goals_away;
	}
	
	function points($competition_id, $round) {
		$query = db_select("fanta_teams_rounds", "tr");
		$query->condition("t_id", $this->id);		
		$query->condition("c_id", $competition_id);
		
		if ($round != null)
			$query->condition("round", $round);
		
		$query->addExpression("SUM(points)", "n");
			
		$result = $query->execute();
		return $result->fetchObject()->n;
	}

	function pointsMax($competition_id) {
		$query = db_select("fanta_teams_rounds", "tr");
		$query->condition("t_id", $this->id);
		$query->condition("c_id", $competition_id);
		$query->addExpression("MAX(points)", "n");
			
		$result = $query->execute();
		return $result->fetchObject()->n;
	}

	function pointsMin($competition_id) {
		$query = db_select("fanta_teams_rounds", "tr");
		$query->condition("t_id", $this->id);
		$query->condition("c_id", $competition_id);
		$query->addExpression("MIN(points)", "n");
			
		$result = $query->execute();
		return $result->fetchObject()->n;
	}
	
	function getSeasonPosition($competition_id) {
		$round = 1;//Round::getLast();
		
		$query = db_select("fanta_teams_rounds", "tr");
		$query->condition("t_id", $this->id);
		$query->condition("c_id", $competition_id);
		$query->condition("round", $round);
		$query->addField("tr", "season_position");
			
		$result = $query->execute();
		
		while($row = $result->fetchObject()) {
			return $row->season_position;
		}
		
		return null;
	}
	
	function getRoundPosition($competition_id, $round) {
		$query = db_select("fanta_teams_rounds", "tr");
		$query->condition("t_id", $this->id);
		$query->condition("c_id", $competition_id);
		$query->condition("round", $round);
		$query->addField("tr", "round_position");
			
		$result = $query->execute();
		
		while($row = $result->fetchObject()) {
			return $row->round_position;
		}
		
		return null;
	}
}
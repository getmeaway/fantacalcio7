<?php

class Team {

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
			$team->coach = $row->coach;
			$team->stadium = $row->stadium;
			$team->shirt = $row->shirt;
			$team->register_date = $row->register_date;
			$team->completed_date = $row->completed_date;
			$team->active = $row->active;
		}
		
		return $team;
	}
	
	static function getByName ($name) {
		$team = null;
		$query = db_select("fanta_teams", "t");
		$query->condition("name", $name);
		$query->fields("t");
		$result = $query->execute();
		foreach ( $result as $row ) {
			$team = new Team($row->t_id, $row->name, $row->uid);
			$team->coach = $row->coach;
			$team->stadium = $row->stadium;
			$team->shirt = $row->shirt;
			$team->register_date = $row->register_date;
			$team->completed_date = $row->completed_date;
			$team->active = $row->active;
		}
	
		return $team;
	}
	
	static function exists($t_id) {
		return self::get($t_id) != null;
	}

	static function all () {
		$teams = array();
		$query = db_select("fanta_teams", "t");
		$query->fields("t");
		$query->orderBy("t.name");
		$result = $query->execute();
		foreach ( $result as $row ) {
			$teams[$row->t_id] = new Team($row->t_id, $row->name, $row->uid);
			$teams[$row->t_id]->register_date = $row->register_date;
			$teams[$row->t_id]->active = $row->active;
		}
		
		return $teams;
	}
	
	static function allActive () {
		$teams = array();
		$query = db_select("fanta_teams", "t");
		$query->condition("active", 1);
		$query->orderBy("name");
		$query->fields("t");
		$query->orderBy("t.name");
		$result = $query->execute();
		foreach ( $result as $row ) {
			$teams[$row->t_id] = new Team($row->t_id, $row->name, $row->uid);
			$teams[$row->t_id]->register_date = $row->register_date;
			$teams[$row->t_id]->completed_date = $row->completed_date;
			$teams[$row->t_id]->active = $row->active;
		}
	
		return $teams;
	}

	static function allByCompetition ($c_id) {
		$teams = array();
		
		$query = db_select("fanta_teams", "t");
		$query->join("fanta_teams_groups", "g", "g.t_id =  t.t_id");
		$query->fields("t");
		$query->condition("g_id", array_keys(Group::allByCompetition($c_id)), "IN");
		$query->orderBy("t.name");
		
		$result = $query->execute();
		
		foreach ( $result as $row ) {
			$teams[$row->t_id] = new Team($row->t_id, $row->name, $row->uid);
			$teams[$row->t_id]->register_date = $row->register_date;
			$teams[$row->t_id]->completed_date = $row->completed_date;
			$teams[$row->t_id]->active = $row->active;
		}
		
		return $teams;
	}
	
	static function allByGroup ($g_id) {
		$teams = array();
	
		$query = db_select("fanta_teams", "t");
		$query->join("fanta_teams_groups", "g", "g.t_id =  t.t_id");
		$query->fields("t");
		$query->condition("g_id", $g_id);
		$query->orderBy("t.name");
	
		$result = $query->execute();
	
		foreach ( $result as $row ) {
			$teams[$row->t_id] = new Team($row->t_id, $row->name, $row->uid);
			$teams[$row->t_id]->register_date = $row->register_date;
			$teams[$row->t_id]->completed_date = $row->completed_date;
			$teams[$row->t_id]->active = $row->active;
		}
	
		return $teams;
	}
	
	static function allByGroupLineups ($c_id, $g_id, $round) {
		
		$teams = array();
		
		$query = db_select("fanta_teams", "t");
		$query->join("fanta_teams_groups", "g", "g.t_id =  t.t_id");
		$query->join("fanta_lineups", "l", "l.t_id =  t.t_id");
		$query->fields("t");
		$query->condition("g_id", $g_id);
		$query->condition("l.c_id", $c_id);
		$query->condition("l.round", $round);
		$query->orderBy("t.name");
		
		$result = $query->execute();
		
		foreach ( $result as $row ) {
			$teams[$row->t_id] = new Team($row->t_id, $row->name, $row->uid);
			$teams[$row->t_id]->register_date = $row->register_date;
			$teams[$row->t_id]->completed_date = $row->completed_date;
			$teams[$row->t_id]->active = $row->active;
		}
	
		return $teams;
	}
	
	static function allByUser($u_id) {
		$teams = array();
		$query = db_select("fanta_teams", "t");
		//$query->join("fanta_teams_groups", "g", "g.t_id =  t.t_id");
		$query->fields("t");
		$query->condition("t.uid", $u_id);
		$query->orderBy("t.name");
		$result = $query->execute();
		foreach ( $result as $row ) {
			$teams[$row->t_id] = new Team($row->t_id, $row->name, $row->uid);
			$teams[$row->t_id]->register_date = $row->register_date;
			$teams[$row->t_id]->completed_date = $row->completed_date;
			$teams[$row->t_id]->active = $row->active;
		}
	
		return $teams;
	}
	
	static function getTeamsForRound($competition_id, $competition_round) {
		
		$teams = array();
		
		$query = db_select("fanta_matches", "m");
		$query->condition("m.round", $competition_round);
		$query->condition("m.g_id", array_keys(Group::allByCompetition($competition_id)), "IN");
		$query->fields("m", array("t1_id", "t2_id"));
		
		$result = $query->execute();
		
		foreach($result as $row) {
			array_push($teams, Team::get($row->t1_id));
			array_push($teams, Team::get($row->t2_id));
		}
		
		return $teams;
	}
	
	static function getMaxNumberForUser($u_id) {
		$max_teams = 0;
		$query = db_select("fanta_users", "u");
		$query->condition("uid", $u_id);
		$query->fields("u");
		
		$result = $query->execute();
		
		while ( $row = $result->fetchObject() ) {
			$max_teams = $row->allowed_teams;
		}
		
		return $max_teams;
	}

	function getSquad () {
		$squad = array();
		
		$round = Round::getLastQuotation();
		
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
		$query->leftJoin("fanta_real_teams", "rt", "rt.rt_id = r.rt_id");
		$query->condition("s.status", 1);
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
	
	function getNumberPlayers() {
		
		$query = db_select("fanta_squads", "s");
		$query->join("fanta_players", "p", "p.pl_id = s.pl_id");
		$query->condition("t_id", $this->id);
		$query->fields("s");
		$query->fields("p");
		
		$result = $query->execute();
		
		$players = array();
		
		foreach($result as $row) {
			if (!isset($players[$row->role]))
				$players[$row->role] = 0;
			
			$players[$row->role]++;
		}
		
		return $players;
	}
	
	function getExpense() {
		$query = db_select("fanta_squads_movements", "sm");
		$query->condition("t_id", $this->id);
		$query->fields("sm");
		
		$result = $query->execute();
		
		$expense = 0;
		
		foreach($result as $row) {
			$expense += ($row->value * $row->status);
		}
		
		return $expense;
	}
	
	function getCredits() {
		return variable_get("fantacalcio_credits", 0) - $this->getExpense();
	}
	
	function isConfirmed() {
		return $this->completed_date != null && $this->completed_date > 0;
	}
	
	function canConfirm() {
		return ($this->completed_date == null || $this->completed_date == 0) && $this->isSquadComplete();
	}
	
	function setConfirmed() {
		$query = db_update("fanta_teams");
		$query->fields(array("completed_date" => time()));
		$query->condition("t_id", $this->id);
		
		$query->execute();
	}
	
	function isSquadComplete() {
		$squad = $this->getSquad();
		
		$result = false;
		
		$num_players = array();
		foreach($squad as $player) {
			if (!isset($num_players[$player->role]))
				$num_players[$player->role] = 0;
			
			$num_players[$player->role]++;
		}
		
		if (!isset($num_players[0]) || !isset($num_players[1]) || !isset($num_players[2]) || !isset($num_players[3]))
			return false;
		
		return $num_players[0] == variable_get("fantacalcio_number_role_0", 0) 
			&& $num_players[1] == variable_get("fantacalcio_number_role_1", 0)
			&& $num_players[2] == variable_get("fantacalcio_number_role_2", 0)
			&& $num_players[3] == variable_get("fantacalcio_number_role_3", 0);
	}
	
	function getCompetitions() {
		
		$competitions = array();
		
		$query = db_select("fanta_teams_groups", "tg");
		$query->join("fanta_groups", "g", "g.g_id = tg.g_id");
		$query->join("fanta_competitions", "c", "c.c_id = g.c_id");
		$query->fields("tg");
		$query->fields("g");
		$query->addField("c", "name", "competition_name");
		$query->condition("tg.t_id", $this->id);
		$query->condition("tg.active", 1);
		$query->condition("g.active", 1);
	
		$result = $query->execute();
		
		foreach($result as $row) {
			$competitions[$row->g_id] = $row->competition_name . " - " . $row->name;
		}
	
		return $competitions;
	}
	
	function getGroups() {
	  $groups = array();
	  
	  $query = db_select("fanta_teams_groups", "tg");
	  $query->join("fanta_groups", "g", "g.g_id = tg.g_id");
	  $query->join("fanta_competitions", "c", "c.c_id = g.c_id");
	  $query->fields("tg");
	  $query->fields("g");
	  $query->addField("c", "name", "competition_name");
	  $query->condition("tg.t_id", $this->id);
	  $query->condition("tg.active", 1);
	  $query->condition("g.active", 1);
	  
	  $result = $query->execute();
	  
	  foreach($result as $row) {
	    $groups[$row->c_id] = $row;
	  }
	  
	  return $groups;
	}
	
	function inCompetition($c_id) {
		$query = db_select("fanta_teams_groups", "tg");
		$query->join("fanta_groups", "g", "g.g_id = tg.g_id");
		$query->fields("tg");
		$query->condition("tg.t_id", $this->id);
		$query->condition("g.c_id", $c_id);
		$query->condition("tg.active", 1);
		$query->condition("g.active", 1);
		
		$result = $query->execute();
		
		return $result->rowCount() == 1;
	}
	
	function hasMatch($c_id, $round) {
		
		$groups = Competition::get($c_id)->groups;
		$group_ids = array();
		foreach($groups as $g_id => $group)
			array_push($group_ids, $g_id);	
		
		$query = db_select("fanta_matches", "m");
		
		$db_or = db_or()->condition('t1_id', $this->id)->condition('t2_id', $this->id);

		$query->fields("m", array("m_id"));
		$query->condition($db_or);
		$query->condition("round", $round);
		$query->condition("g_id", $group_ids,'IN');
		
		$result = $query->execute();
		
		return $result->rowCount() == 1;
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
						<i class='fa fa-square fa-stack-2x squad-player-role-" . $row->role . "'></i>
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
	
	function seasonPoints($competition_id) {
		$query = db_select("fanta_teams_rounds", "tr");
		$query->condition("t_id", $this->id);
		$query->condition("c_id", $competition_id);
		$query->addExpression("SUM(points)", "sum");
			
		$result = $query->execute();
		return $result->fetchObject()->sum;
	}
	
	function getSeasonPosition($competition_id) {
		$round = Round::getLast();
		
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
	
	function hasPlayer($player_id) {
		$query = db_select("fanta_squads", "s");
		$query->condition("s.pl_id", $player_id);
		$query->condition("s.t_id", $this->id);
		$query->fields("s", array("pl_id"));
		$result = $query->execute();
		
		return $result->rowCount() == 1;
	}
	
	function getMovementsCount() {
		$query = db_select("fanta_squads_movements", "s");
		$query->condition("s.t_id", $this->id);
		$query->condition("s.status", 1);
		$query->condition("s.temporary", 0);
		$query->fields("s", array("pl_id"));
		$result = $query->execute();
		
		return $result->rowCount();
	}

	function statPosition($competition) {
		if ($competition->type == COMPETITION_TYPE_GP)
			return $this->getSeasonPosition($competition->id);
		else {
			return $this->getPosition($competition);
		}
	}

	function statPoints($competition) {
		if ($competition->type == COMPETITION_TYPE_GP)
			return $this->seasonPoints($competition->id);
		else {
			return $this->getPoints($competition);
		}
	}

	function statGap($competition) {
		if ($competition->type == COMPETITION_TYPE_GP) {
			
			$gap_points = 0;
			
			$query = db_select("fanta_teams_rounds", "tr");
			$query->condition("t_id", $this->id, "!=");
			$query->addExpression("SUM(points)", "sum");
			$query->groupBy("t_id");
			
			$result = $query->execute();
			
			foreach($result as $row) {
			  if ($row->t_id != $this->id)
				$gap_points = max($gap_points, $row->sum);
			}
			
			return abs($this->seasonPoints($competition->id) - $gap_points);
		}
		else {
			return $this->getGap($competition);
		}
	}

	function statMaxPoints($competition) {
		if ($competition->type == COMPETITION_TYPE_GP)
			return $this->pointsMax($competition->id);
		else {
			$query = db_select("fanta_matches", "m");
			$query->condition("t1_id", $this->id);
			$query->condition("played", 1);
			$query->condition("g_id", array_keys($competition->groups), "IN");
			$query->addExpression("MAX(tot_1)", "n");
			 
			$result = $query->execute();
			$points_home = $result->fetchObject()->n;
			 
			$query = db_select("fanta_matches", "m");
			$query->condition("t2_id", $this->id);
			$query->condition("played", 1);
			$query->condition("g_id", array_keys($competition->groups), "IN");
			$query->addExpression("MAX(tot_2)", "n");
		
			$result = $query->execute();
			$points_away = $result->fetchObject()->n;
			
			return max($points_home, $points_away);
		}
	}

	function statMinPoints($competition) {
		if ($competition->type == COMPETITION_TYPE_GP)
			return $this->pointsMin($competition->id);
		else {
			$query = db_select("fanta_matches", "m");
			$query->condition("t1_id", $this->id);
			$query->condition("played", 1);
			$query->condition("g_id", array_keys($competition->groups), "IN");
			$query->addExpression("MIN(tot_1)", "n");
			 
			$result = $query->execute();
			$points_home = $result->fetchObject()->n;
			$points_home = $points_home != null ? $points_home : 10000;
			 
			$query = db_select("fanta_matches", "m");
			$query->condition("t2_id", $this->id);
			$query->condition("played", 1);
			$query->condition("g_id", array_keys($competition->groups), "IN");
			$query->addExpression("MIN(tot_2)", "n");
		
			$result = $query->execute();
			$points_away = $result->fetchObject()->n;
			$points_away = $points_away != null ? $points_away : 10000;
			
			return min($points_home, $points_away);
		}
	}

	function statAvgPoints($competition) {
		if ($competition->type == COMPETITION_TYPE_GP) {
			
			$query = db_select("fanta_teams_rounds", "tr");
			$query->condition("t_id", $this->id);
			$query->fields("tr", array("points"));
							
			$result = $query->execute();

			$count = 0;
			$sum = 0;
			foreach($result as $row) {
				$count++;
				$sum += $row->points;
			}
			
			if ($count > 0)
			 return round($sum / $count, 2);
			else 
			  return 0;
		}
		else {
			
			$count = 0;
			$sum = 0;
			
			$query = db_select("fanta_matches", "m");
			$query->condition("t1_id", $this->id);
			$query->condition("played", 1);
			$query->condition("g_id", array_keys($competition->groups), "IN");
			$query->fields("m", array("tot_1"));
			 
			$result = $query->execute();			
			
			foreach($result as $row) {
				$sum += $row->tot_1;
				$count++;
			}
			
			$query = db_select("fanta_matches", "m");
			$query->condition("t2_id", $this->id);
			$query->condition("played", 1);
			$query->condition("g_id", array_keys($competition->groups), "IN");
			$query->fields("m", array("tot_2"));
		
			$result = $query->execute();
			
			foreach($result as $row) {
				$sum += $row->tot_2;
				$count++;
			}
			
			if ($count > 0)
				return round($sum / $count, 2);
			else 
				return 0;
		}
	}
	
	function getRoundsPoints($competition) {
		
		$query = db_select("fanta_teams_rounds", "tr");
		$query->condition("t_id", $this->id);
		$query->condition("c_id", $competition->id);
		$query->fields("tr", array("points", "round"));
		
		$result = $query->execute();
		
		$points = array();
		foreach($result as $row) {
			$points[$row->round] = $row->points;
		}

		return $points;
	}
	
	function getRoundsPositions($competition) {
	
		$query = db_select("fanta_teams_rounds", "tr");
		$query->condition("t_id", $this->id);
		$query->condition("c_id", $competition->id);
		$query->fields("tr", array("round_position", "round"));
	
		$result = $query->execute();
	
		$positions = array();
		foreach($result as $row) {
			$positions[$row->round] = $row->round_position;
		}
	
		return $positions;
	}

	function getRoundsSeasonPositions($competition) {
	
		$query = db_select("fanta_teams_rounds", "tr");
		$query->condition("t_id", $this->id);
		$query->condition("c_id", $competition->id);
		$query->fields("tr", array("season_position", "round"));
	
		$result = $query->execute();
	
		$positions = array();
		foreach($result as $row) {
			$positions[$row->round] = $row->season_position;
		}
	
		return $positions;
	}
	
	function getPosition($competition) {
	  
	  $groups = $this->getGroups();
		
		$query = db_select("fanta_matches", "m");
		$query->condition("g_id", $groups[$competition->id]->g_id);
		$query->condition("played", 1);
		$query->fields("m");
		$result = $query->execute();
		
		$points = array();
		$goals = array();
		$totals = array();
		$teams = array();
		
		foreach($result as $row) {
		  
		    $teams[$row->t1_id] = $row->t1_id;
		    $teams[$row->t2_id] = $row->t2_id;
		    
		    if (!isset($goals[$row->t1_id]))
		      $goals[$row->t1_id] = 0;
		    $goals[$row->t1_id] += $row->goals_1; 
		    if (!isset($goals[$row->t2_id]))
		      $goals[$row->t2_id] = 0;
		    $goals[$row->t2_id] += $row->goals_2; 

		    if (!isset($totals[$row->t1_id]))
		      $totals[$row->t1_id] = 0;
		    $totals[$row->t1_id] += $row->tot_1; 
		    if (!isset($totals[$row->t2_id]))
		      $totals[$row->t2_id] = 0;
		    $totals[$row->t2_id] += $row->tot_2; 
		    
		    if (!isset($points[$row->t1_id]))
		      $points[$row->t1_id] = 0;
		    if (!isset($points[$row->t2_id]))
		      $points[$row->t2_id] = 0;
			
			switch ($row->winner_id) {
				case $row->t1_id:
					if (!isset($points[$row->t1_id]))
						$points[$row->t1_id] = 0;
					$points[$row->t1_id] += 3;
					break;
				case $row->t2_id:
					if (!isset($points[$row->t2_id]))
						$points[$row->t2_id] = 0;
					$points[$row->t2_id] += 3;
					break;
				case -1:
					if (!isset($points[$row->t1_id]))
						$points[$row->t1_id] = 0;
					$points[$row->t1_id] += 1;
					
					if (!isset($points[$row->t2_id]))
						$points[$row->t2_id] = 0;
					$points[$row->t2_id] += 1;
					break;
			}
		}
		
		if ($points) {

			$max_point = 0; $t_id_max_point = 0;
			foreach ($points as $team_id => $point) {
				if ($this->id == $team_id) 
					$position_point = $point;
		
				if ($max_point < $point) {
					$max_point = $point;
					$t_id_max_point = $team_id;
				}
			}
		
			array_multisort($points, SORT_DESC, $goals, SORT_DESC, $totals, SORT_DESC, $teams);
					
			if (isset($position_point) )
				return array_search($this->id, $teams) +1;
			else 
				return null;
		}
	}
	
	function getPoints($competition) {
		
		$query = db_select("fanta_matches", "m");
		$query->condition("g_id", array_keys($competition->groups), "IN");
		$query->condition("played", 1);
		$db_or = db_or();
		
		$db_or->condition('t1_id', $this->id);
		$db_or->condition('t2_id', $this->id);
		$query->condition($db_or);
		
		$query->fields("m");
		$result = $query->execute();
		
		$points = 0;
		
		foreach($result as $row) {
			switch ($row->winner_id) {
				case $this->id:
					$points += 3;
					break;
				case -1:
					$points += 1;
					break;				
			}
		}
		
		return $points;	
	}
	
	function getGap($competition) {
	  
	  $groups = $this->getGroups();
	
		$query = db_select("fanta_matches", "m");
		$query->condition("g_id", $groups[$competition->id]->g_id);
		$query->condition("played", 1);
		
		$db_and = db_and();
		
		$db_and->condition('t1_id', $this->id, '!=');
		$db_and->condition('t2_id', $this->id, '!=');
// 		$query->condition($db_and);
		
		$query->fields("m");
		$result = $query->execute();
	
		$points = array();
	
		foreach($result as $row) {
		  if (!isset($points[$row->t1_id]))
		    $points[$row->t1_id] = 0;
		  if (!isset($points[$row->t2_id]))
		    $points[$row->t2_id] = 0;
		  
		  switch ($row->winner_id) {
				case $row->t1_id:
					$points[$row->t1_id] += 3;
					break;
				case $row->t2_id:
					$points[$row->t2_id] += 3;
					break;
				case -1:
					$points[$row->t1_id] += 1;  							
					$points[$row->t2_id] += 1;
					break;
			}
		}
	
		if ($points) {
		    $team_points = $points[$this->id];
	       unset($points[$this->id]); 
			return abs(max($points) - $team_points);
		}
	}
	
	function getMatchesResults($competition) {
	
		$query = db_select("fanta_matches", "m");
		$query->condition("g_id", array_keys($competition->groups), "IN");
		$query->condition("played", 1);
		$db_or = db_or();
	
		$db_or->condition('t1_id', $this->id);
		$db_or->condition('t2_id', $this->id);
		$query->condition($db_or);
	
		$query->fields("m");
		$result = $query->execute();
	
		$points = 0;
	
		$wins = 0;
		$draws = 0;
		$losts = 0;
		
		foreach($result as $row) {
			switch ($row->winner_id) {
				case $this->id:
					$wins++;
					break;
				case -1:
					$draws++;
					break;
				default:
					$losts++;
					break;
			}
		}
	
		return (object) array("wins" => $wins, "draws" => $draws, "losts" => $losts);
	}
}

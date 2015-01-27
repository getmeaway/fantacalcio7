<?php

class Player {
	
	var $id;
	var $name;
	var $role;
	var $team;
	
	function __construct($id, $name, $role) {
		$this->id = $id;
		$this->name = $name;
		$this->role = $role;
	}
	
	static function get($id) {
		$player = new Player();
		$query = db_select("fanta_players", "p");
		$query->condition("id", $id);
		$query->fields("p");
		$result = $query->execute();
		foreach($result as $row) {
			$player = new Player($row->pl_id, $row->name, $row->role);
		}
		
		return $player;
	}
	
	static function all() {
		$players = array();
		$query = db_select("fanta_players", "p");
		$query->fields("p");
		$result = $query->execute();
		foreach($result as $row) {
			array_push($players, new Player($row->pl_id, $row->name, $row->role));
		}
	
		return $players;
	}
	
	static function allWithRound($round = null) {
		$players = array();
		$round = ($round != null && is_numeric($round)) ? $round : 1;//TODO Round::getNext();
		$query = db_select("fanta_players", "p");
		$query->join("fanta_players_rounds", "r", "r.pl_id = p.pl_id");
		$query->join("fanta_real_teams", "rt", "rt.rt_id = r.rt_id");
		$query->fields("p");
		$query->fields("r");
		$query->condition("r.round", $round);
		$query->addField("rt", "name", "team");
		$result = $query->execute();
		foreach($result as $row) {
			$player = new Player($row->pl_id, $row->name, $row->role);
			$player->team = $row->team;
			array_push($players, $player);
		}
		
		return $players;
	}
	
}
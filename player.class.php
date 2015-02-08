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
	
	static function allWithQuotation($round = null) {
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
			$player->quotation = $row->quotation;
			array_push($players, $player);
		}
	
		return $players;
	}
	
	static function listForSquad($players_list, $squad, $t_id) {
		
		$list_rows = array();//
		
		foreach($players_list as $pl_id => $player)
		{
			if (array_key_exists($pl_id, $squad))
				$available = "<i class=\"fa fa-check-circle fa-2x text-primary\"></i>";
			else {
				$available = "<button class=\"btn btn-sm btn-success buy-player\" id=\"buy-" . $player->id . "\">" . t("Compra") . "</button>";
			}
			$list_rows[$pl_id] = array("data" => array(
					"<span class='fa-stack'>
						<i class='fa fa-square fa-stack-2x role-" . $player->role . "'></i>
						<i class='fa fa-stack-1x' style='color: white;'><span class='font-normal'>" . self::convertRole($player->role) . "</span></i>
					</span>",
					$player->name,
					ucfirst($player->team),
					$player->quotation,
					$available),
					"class" => array("role-" . $player->role, "show-player-role", "show-player-team", "show-player-name"),
					"data-name" => $player->name,
					"data-team" => $player->team,
					
			);
		}
		
		return $list_rows;
	}
	
	static function convertRole($role_id) {
		$roles = array(0 => t("P"), 1 => t("D"), 2 => t("C"), 3 => t("A"));
		return $roles[$role_id];
	}
	
}
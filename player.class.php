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
		$player = null;
		$query = db_select("fanta_players", "p");
		$query->condition("pl_id", $id);
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
		$round = ($round != null && is_numeric($round)) ? $round : Round::getLastQuotation();
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
	
	static function listForSquad($players_list, $squad, $t_id, $is_squad_complete) {
		
		$list_rows = array();//
		
		foreach($players_list as $pl_id => $player)
		{
			if (array_key_exists($player->id, $squad))
				$available = "<i class=\"fa fa-check-circle fa-2x text-primary player-bought\"></i>";
			else {
				$buy_player_form = drupal_get_form("fantacalcio_buy_player_form", $t_id, $player->id);
				$available = drupal_render($buy_player_form);//"<button class=\"btn btn-sm btn-success buy-player\" id=\"buy-" . $player->id . "\">" . t("Compra") . "</button>";
			}
			$list_rows[$player->id] = array("data" => array(
					"<span class='fa-stack'>
						<i class='fa fa-square fa-stack-2x squad-player-role-" . $player->role . "'></i>
						<i class='fa fa-stack-1x' style='color: white;'><span class='font-normal'>" . self::convertRole($player->role) . "</span></i>
					</span>",
					array("data" =>$player->name, "class" => array("player-list-text")),
					array("data" =>ucfirst($player->team), "class" => array("player-list-text")),
					array("data" =>$player->quotation, "class" => array("player-list-text")),
					"<a href=\"#\" data-toggle=\"modal\" data-target=\"#player-stats-modal\" class=\"player-stats\" id=\"player-stat-" . $pl_id . "\"><i class=\"fa fa-bar-chart\"></i></a>",
					$available,
						),
					"class" => array("role-" . $player->role, "show-player-role", "show-player-team", "show-player-name"),
					"data-name" => $player->name,
					"data-team" => $player->team,
					"data-role" => $player->role,
					"data-quotation" => $player->quotation,
					"id" => "pl-" . $player->id
					
			);
		}
		
		return $list_rows;
	}
	
	static function convertRole($role_id) {
		$roles = array(0 => t("P"), 1 => t("D"), 2 => t("C"), 3 => t("A"));
		return $roles[$role_id];
	}
	
	function getPlayerRounds() {
		$sql = "SELECT * FROM {fanta_players_teams} p, {fanta_real_teams} t
      WHERE p.rt_id = t.rt_id
      AND p.pl_id = :pl_id
      ORDER BY p.round" ;
		
		$query = db_select("fanta_players_rounds", "r");
		$query->join("fanta_real_teams", "rt", "r.rt_id = rt.rt_id");
		$query->fields("r");
		$query->fields("rt");
		$query->condition("pl_id", $this->id); 
		
		$result = $query->execute();
		
		foreach ($result as $row) {
			$player[$row->round] = $row;
		}
		return $player;
	} 
	
	
	function getQuotation($round = null) {
		$round = (Round::exists($round)) ? $round : Round::getLastQuotation();
		
		$query = db_select("fanta_players_rounds", "r");
		$query->condition("pl_id", $this->id);
		$query->condition("round", $round);
		$query->fields("r", array("quotation"));
		$result = $query->execute();
		
		while ($row = $result->fetchObject()) {
			return $row->quotation;
		}
		
		return null;
		
	}
	
}
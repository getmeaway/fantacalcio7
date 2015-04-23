<?php

class User {
	
	var $id;
	var $name;
	var $max_teams;
	var $teams;
	var $payed;
	
	static function get($id) {
		$user = null;
		$query = db_select("fanta_users", "fu");
		$query->join("users", "u", "fu.uid = u.uid");
		$query->condition("u.uid", $id);
		$query->fields("fu");
		$query->fields("u", array("name"));
		
		$result = $query->execute();
		
		foreach($result as $row) {
			$user = new User();
			$user->uid = $row->uid;
			$user->name = $row->name;
			$user->teams = count(Team::allByUser($row->uid));
			$user->allowed_teams = $row->allowed_teams;
			$user->payed = $row->payed;
		}
		
		return $user;
	}
	
	static function getByName($name) {
		$user = null;
		$query = db_select("users", "u");
		$query->condition("u.name", $name);
		$query->fields("u", array("uid", "name"));
				
		$result = $query->execute();
		
		foreach($result as $row) {
			$user = new User();
			$user->uid = $row->uid;
			$user->name = $row->name;
		}
		
		return $user;
	}
	
	static function alreadyAdded($name) {
		$query = db_select("fanta_users", "fu");
		$query->join("users", "u", "fu.uid = u.uid");
		$query->condition("u.name", $name);
		$query->fields("fu");
		$query->fields("u", array("name"));
	
		$result = $query->execute();
	
		return $result->rowCount() == 1;
	}
	
	static function exists($name) {
		$query = db_select("users", "u");
		$query->condition("u.name", $name);
		$query->fields("u", array("name"));
				
		$result = $query->execute();
		
		return $result->rowCount() == 1; 
	}
	
	static function all() {
		$users = array();
		$query = db_select("fanta_users", "fu");
		$query->join("users", "u", "fu.uid = u.uid");
		$query->fields("fu");
		$query->fields("u", array("name"));
		
		$result = $query->execute();
		
		foreach($result as $row) {
			$user = new User();
			$user->uid = $row->uid;
			$user->name = $row->name;
			$user->teams = Team::allByUser($row->uid);
			$user->allowed_teams = $row->allowed_teams;
			$user->payed = $row->payed;
			
			array_push($users, $user);
		}
		
		return $users;
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
	
	static function listForSquad($players_list, $squad, $t_id, $is_squad_complete) {
		
		$list_rows = array();//
		
		foreach($players_list as $pl_id => $player)
		{
			if (array_key_exists($player->id, $squad))
				$available = "<i class=\"fa fa-check-circle fa-2x text-primary player-bought\"></i>";
			else {
				if ($is_squad_complete) {
					$buy_player_form = drupal_get_form("fantacalcio_buy_player_form", $t_id, $player->id);
					$available = drupal_render($buy_player_form);//"<button class=\"btn btn-sm btn-success buy-player\" id=\"buy-" . $player->id . "\">" . t("Compra") . "</button>";
				}
				else {
					$available = "<button class=\"btn btn-sm btn-success buy-player\" onclick=\"buyPlayer(" . $player->id . ", " . $t_id . ");\" id=\"buy-" . $player->id . "\">" . t("Compra") . "</button><i class=\"hidden fa fa-check-circle fa-2x text-primary player-bought\"></i>";
				}
			}
			$list_rows[$player->id] = array("data" => array(
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
	
	function getQuotation($round = null) {
		$round = (Round::exists($round)) ? $round : Round::getLast();
		
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
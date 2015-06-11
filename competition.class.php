<?php

class Competition {
	
	var $id;
	var $name;
	var $active;
	var $type;
	var $has_matches;
	var $has_standings;
	var $has_newsletters;
	var $groups;

	function __construct($id, $name, $active, $type, $is_default, $has_matches, $has_standings, $has_lineups, $has_newsletters) {
		$this->id = $id;
		$this->name = $name;
		$this->active = $active;
		$this->type = $type;
		$this->is_default = $is_default;
		$this->has_matches = $has_matches;
		$this->has_standings = $has_standings;
		$this->has_lineups = $has_lineups;
		$this->has_newsletters = $has_newsletters;
		$this->sanitized_name = self::sanitize($this->name);
	}
	
	static function get($id) {
		
		$competition = null;
		
		$query = db_select("fanta_competitions", "c");
		$query->condition("c_id", $id);
		$query->fields("c");
		$result = $query->execute();
		foreach($result as $row) {
			$competition = new Competition($row->c_id, $row->name, $row->active, $row->type, $row->is_default, $row->has_matches, $row->has_standings, $row->has_lineups, $row->has_newsletters);
			$competition->groups = Group::allByCompetition($row->c_id);
			$competition->sanitized_name = self::sanitize($competition->name);
		}
		
		return $competition;
	}
	
	static function getDefault() {
	
		$competition = null;
	
		$query = db_select("fanta_competitions", "c");
		$query->condition("is_default", 1);
		$query->fields("c");
		$result = $query->execute();
		foreach($result as $row) {
			$competition = new Competition($row->c_id, $row->name, $row->active, $row->type, $row->is_default, $row->has_matches, $row->has_standings, $row->has_lineups, $row->has_newsletters);
			$competition->groups = Group::allByCompetition($row->c_id);
			$competition->sanitized_name = self::sanitize($competition->name);
		}
	
		return $competition;
	}

	static function exists($c_id) {
		return self::get($c_id) != null;
	}
	
	static function getByName($name) {
		
		$competition = null;
		
		$query = db_select("fanta_competitions", "c");
		$query->condition("name", $name);
		$query->fields("c");
		$result = $query->execute();
		foreach($result as $row) {
			$competition = new Competition($row->c_id, $row->name, $row->active, $row->type, $row->is_default, $row->has_matches, $row->has_standings, $row->has_lineups, $row->has_newsletters);
			$competition->groups = Group::allByCompetition($row->c_id);
			$competition->sanitized_name = self::sanitize($competition->name);
		}
	
		return $competition;
	}
	
	function getTeams() {
		$teams = array();
		
		$query = db_select("fanta_teams", "t");
		$query->join("fanta_teams_groups", "tg", "tg.t_id = t.t_id");
		$query->join("fanta_groups", "g", "tg.g_id = g.g_id");
		$query->condition("g.c_id", $this->id);
		$query->condition("tg.active", 1);
		$query->fields("t");
		$query->distinct();
		
		$result = $query->execute();
		
		foreach($result as $row) {
			$team = new Team($row->t_id, $row->name, $row->uid);
			
			array_push($teams, $team);
		}
		
		return $teams;
	}
	
	static function all($args = array()) {
		$competitions = array();
		$query = db_select("fanta_competitions", "c");
		
		if ($args) {
			foreach($args as $key => $value) {
				$query->condition($key, $value);
			}
		}
		$query->fields("c");
		$result = $query->execute();
		foreach($result as $row) {
			$competitions[$row->c_id] = new Competition($row->c_id, $row->name, $row->active, $row->type, $row->is_default, $row->has_matches, $row->has_standings, $row->has_lineups, $row->has_newsletters);
			$competitions[$row->c_id]->groups = Group::allByCompetition($row->c_id);
			$competitions[$row->c_id]->sanitized_name = self::sanitize($row->name);
		}
	
		return $competitions;
	}
	
	static function allForRound($round, $competition_id = null) {
		$competitions = array();
		
		$query = db_select("fanta_competitions", "c");
		$query->join("fanta_rounds_competitions", "rc", "rc.c_id = c.c_id");		
		$query->condition("rc.round", $round);
		
		if($competition_id != null)
			$query->condition("c.c_id", $competition_id);
		
		$query->fields("c");
		$query->fields("rc");
				
		$result = $query->execute();
		
		foreach($result as $row) {
			$competitions[$row->c_id] = new Competition($row->c_id, $row->name, $row->active, $row->type, $row->is_default, $row->has_matches, $row->has_standings, $row->has_lineups, $row->has_newsletters);
			$competitions[$row->c_id]->competition_round = $row->competition_round;
			$competitions[$row->c_id]->round_label = empty($row->round_label) ? $row->competition_round . t("&ordf; giornata") : $row->round_label;
			$competitions[$row->c_id]->sanitized_name = self::sanitize($row->name);
		}

		return $competitions;
	}
	
	static function choose($args = array(), $page) {
	
		$items = array();
	
		$competitions = self::all($args);
	
		$url_tail = isset($args['url_tail']) ? $args['url_tail'] : "";
		$show_round = isset($args['show_round']) ? $args['show_round'] : "";
	
		if ($competitions) {
			foreach ($competitions as $l_id => $competition) {
				array_push($items, array("data" => l($competition->name, $page . "/" . strtolower($competition->name)), "class" => array("list-group-item")));
			}
		}
	
		return theme_item_list(array("items" => $items, "attributes" => array("class" => array("list-group")), "type" => "ul", "title" => ""));
	}
	
	static function sanitize($name) {
		$sanitized_name = strtolower($name);
		$sanitized_name = preg_replace('@[\s]+@', '-', $sanitized_name);
		$sanitized_name = preg_replace('@[^a-z0-9-_\s]+@', '', $sanitized_name);
		
		return $sanitized_name;
	}
}
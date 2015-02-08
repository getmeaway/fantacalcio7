<?php

class Competition {
	
	var $id;
	var $name;
	var $type;
	var $has_matches;
	var $has_standings;
	var $has_newsletters;
	var $groups;

	function __construct($id, $name, $type, $has_matches, $has_standings, $has_newsletters) {
		$this->id = $id;
		$this->name = $name;
		$this->type = $type;
		$this->has_matches = $has_matches;
		$this->has_standings = $has_standings;
		$this->has_newsletters = $has_newsletters;
	}
	
	static function get($id) {
		$query = db_select("fanta_competitions", "c");
		$query->condition("id", $id);
		$query->fields("c");
		$result = $query->execute();
		foreach($result as $row) {
			$competition = new Competition($row->c_id, $row->name, $row->type, $row->has_matches, $row->has_standings, $row->has_newsletters);
			$competition->groups = Group::allByCompetition($row->c_id);
		}
		
		return $competition;
	}

	static function getByName($name) {
		
		$competition = null;
		
		$query = db_select("fanta_competitions", "c");
		$query->condition("name", $name);
		$query->fields("c");
		$result = $query->execute();
		foreach($result as $row) {
			$competition = new Competition($row->c_id, $row->name, $row->type, $row->has_matches, $row->has_standings, $row->has_newsletters);
			$competition->groups = Group::allByCompetition($row->c_id);
		}
	
		return $competition;
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
			$competitions[$row->c_id] = new Competition($row->c_id, $row->name, $row->type, $row->has_matches, $row->has_standings, $row->has_newsletters);
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
}
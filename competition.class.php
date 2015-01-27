<?php

class Competition {
	
	var $id;
	var $name;
	var $type;
	var $has_matches;
	var $has_standings;
	var $has_newsletters;

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
		}
		
		return $competition;
	}
	
	static function all() {
		$competitions = array();
		$query = db_select("fanta_competitions", "c");
// 		$query->condition("id", $id);
		$query->fields("c");
		$result = $query->execute();
		foreach($result as $row) {
			array_push($competitions, new Competition($row->c_id, $row->name, $row->type, $row->has_matches, $row->has_standings, $row->has_newsletters));
		}
	
		return $competitions;
	}
}
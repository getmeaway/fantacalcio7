<?php

class Group
{

	var $id;
	var $name;
	var $competition_id;
	var $competition_name;
	var $matches_order;
	var $standings_order;
	var $newsletters_order;

	function __construct ($id, $name) {
		$this->id = $id;
		$this->name = $name;
	}

	static function get ($id) {
		$query = db_select("fanta_groups", "g");
		$query->condition("id", $id);
		$query->fields("g");
		$result = $query->execute();
		foreach ( $result as $row ) {
			$group = new Group($row->g_id, $row->name);
			$group->competition_id = $row->competition_id;
			$group->matches_order = $row->matches_order;
			$group->standings_order = $row->standings_order;
			$group->newsletters_order = $row->newsletters_order;
		}
		
		return $group;
	}

	static function all () {
		$groups = array();
		$query = db_select("fanta_groups", "g");
		// $query->condition("id", $id);
		$query->fields("g");
		$result = $query->execute();
		foreach ( $result as $row ) {
			$group = new Group($row->g_id, $row->name);
			$group->competition_id = $row->competition_id;
			$group->matches_order = $row->matches_order;
			$group->standings_order = $row->standings_order;
			$group->newsletters_order = $row->newsletters_order;
			array_push($groups, $group);
		}
		
		return $groups;
	}

}
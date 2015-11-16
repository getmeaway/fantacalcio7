<?php

class Group
{

	var $id;
	var $name;
	var $competition_id;
	var $competition_name;
	var $matches_order;
	var $standings_order;
	var $teams;

	function __construct ($id, $name) {
		$this->id = $id;
		$this->name = $name;
	}

	static function get ($id) {
		$query = db_select("fanta_groups", "g");
		$query->condition("g_id", $id);
		$query->fields("g");
		
		$result = $query->execute();
		
		if($result->rowCount() == 1) {
			foreach ( $result as $row ) {
				$group = new Group($row->g_id, $row->name);
				$group->competition_id = $row->c_id;
				$group->active = $row->active;
				$group->matches_order = $row->matches_order;
				$group->standings_order = $row->standings_order;
				$group->lineups_order = $row->lineups_order;
				$group->teams = Team::allByGroup($row->g_id);
			}

			return $group;
		}
		else
			return null;
		
	}
	
	static function exists($g_id) {
		return self::get($g_id) != null;
	}

	static function all () {
		$groups = array();
		$query = db_select("fanta_groups", "g");
		// $query->condition("id", $id);
		$query->fields("g");
		$result = $query->execute();
		foreach ( $result as $row ) {
			$group = new Group($row->g_id, $row->name);
			$group->active = $row->active;
			$group->competition_id = $row->c_id;
			$group->matches_order = $row->matches_order;
			$group->standings_order = $row->standings_order;
			$group->lineups_order = $row->lineups_order;
			$group->teams = Team::allByGroup($row->g_id);
			$groups[$row->g_id] = $group;
		}
		
		return $groups;
	}

	static function allByCompetition ($c_id) {
		$groups = array();
		$query = db_select("fanta_groups", "g");
		$query->condition("c_id", $c_id);
		$query->condition("active", 1);
		$query->fields("g");
		$result = $query->execute();
		foreach ( $result as $row ) {
			$group = new Group($row->g_id, $row->name);
			$group->competition_id = $row->c_id;
			$group->matches_order = $row->matches_order;
			$group->standings_order = $row->standings_order;
			$group->teams = Team::allByGroup($row->g_id);
			$groups[$row->g_id] = $group;
		}
	
		return $groups;
	}

}

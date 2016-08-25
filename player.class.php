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
    foreach ($result as $row) {
      $player = new Player($row->pl_id, $row->name, $row->role);
    }
    
    return $player;
  }

  static function exists($id) {
    return self::get($id) != null;
  }

  static function all() {
    $players = array();
    $query = db_select("fanta_players", "p");
    $query->fields("p");
    $result = $query->execute();
    foreach ($result as $row) {
      array_push($players, new Player($row->pl_id, $row->name, $row->role));
    }
    
    return $players;
  }

  static function allWithRound($round = null) {
    $players = array();
    $round = ($round != null && is_numeric($round)) ? $round : Round::getNext();
    $query = db_select("fanta_players", "p");
    $query->join("fanta_players_rounds", "r", "r.pl_id = p.pl_id");
    $query->join("fanta_real_teams", "rt", "rt.rt_id = r.rt_id");
    $query->fields("p");
    $query->fields("r");
    $query->condition("r.round", $round);
    $query->addField("rt", "name", "team");
    $result = $query->execute();
    foreach ($result as $row) {
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
    foreach ($result as $row) {
      $player = new Player($row->pl_id, $row->name, $row->role);
      $player->team = $row->team;
      $player->quotation = $row->quotation;
      array_push($players, $player);
    }
    
    return $players;
  }

  static function listForSquad($players_list, $squad, $t_id, $is_squad_complete) {
    $list_rows = array(); //
    
    foreach ($players_list as $pl_id => $player) {
        $hidden_buy = "";
        $hidden_bought = "";
      if (array_key_exists($player->id, $squad))
        $hidden_buy = " hidden";
      else {
        $hidden_bought = " hidden";
      }
      $list_rows[$player->id] = array(
        "data" => array(
          "<span class='fa-stack'>
						<i class='fa fa-square fa-stack-2x squad-player-role-" . $player->role . "'></i>
						<i class='fa fa-stack-1x' style='color: white;'><span class='font-normal'>" . self::convertRole($player->role) . "</span></i>
					</span>", 
          array("data" => $player->name, "class" => array("player-list-text")), 
          array(
            "data" => ucfirst($player->team), 
            "class" => array("player-list-text")), 
          array(
            "data" => $player->quotation, 
            "class" => array("player-list-text")), 
          "<a href=\"#\" data-toggle=\"modal\" data-target=\"#player-stats-modal\" class=\"player-stats\" id=\"player-stat-" . $pl_id . "\"><i class=\"fa fa-bar-chart\"></i></a>",
            "<i class=\"fa fa-check-circle fa-2x text-primary player-bought" . $hidden_bought . "\"></i>" .
          "<button class=\"btn btn-sm btn-success buy-player" . $hidden_buy . "\" onclick=\"buyPlayer(" . $player->id . ", " . $t_id . ")\" id=\"buy-" . $player->id . "\">" . t("Compra") . "</button>"), 
        "class" => array(
          "role-" . $player->role, 
          "show-player-role", 
          "show-player-team", 
          "show-player-name"), 
        "data-name" => $player->name, 
        "data-team" => $player->team, 
        "data-role" => $player->role, 
        "data-quotation" => $player->quotation, 
        "id" => "pl-" . $player->id)

      ;
    }
    
    return $list_rows;
  }

  static function convertRole($role_id) {
    $roles = array(0 => t("P"), 1 => t("D"), 2 => t("C"), 3 => t("A"));
    return $roles[$role_id];
  }

  static function getIdList() {
    $ids = array();
    
    $query = db_select("fanta_players", "p");
    $query->join("fanta_players_rounds", "pr", "pr.pl_id = p.pl_id");
    $query->fields("p");
    $query->distinct();
    $result = $query->execute();
    
    foreach ($result as $row) {
      $ids[strtolower($row->name)] = $row->pl_id;
    }
    
    return $ids;
  }
  
  static function updateList() {

      $players_url = DATA_SOURCE_URL . "/players/" . variable_get("fantacalcio_votes_provider", 1) . ".json?t=" . time();
  
  $players = json_decode(file_get_contents($players_url));
  
  $round = Round::getLast() + 1;
  
  if (count($players) > 0 && $round >= 0) {

        // real teams
        $real_teams = RealTeam::allNames();
        $real_teams = array_flip($real_teams);

        // player ids
        $players_ids = Player::getIdList();

        $tmp_players = array();
        $new_players = array();

        foreach ($players as $player) {

          if (isset($players_ids[$player->name])) {

            $tmp_player = (object) array(
              "pl_id" => $players_ids[$player->name], 
              "name" => $player->name, 
              "quotation" => $player->quotation, 
              "not_rounded_quotation" => $player->not_rounded_quotation, 
              "team" => $real_teams[strtolower($player->team)], 
              "role" => $player->role);

            $tmp_players[$tmp_player->pl_id] = $tmp_player;
          }
          else {
            $new_player = (object) array(
              "name" => $player->name, 
              "quotation" => $player->quotation, 
              "not_rounded_quotation" => $player->not_rounded_quotation, 
              "team" => $real_teams[strtolower($player->team)], 
              "role" => $player->role);

            $new_players[] = $new_player;
          }
        }
      
      echo "tmp_player:" . count($tmp_players) . "<br>";
      echo "new_player:" . count($new_players) . "<br>";

        $subquery = db_select("fanta_players_rounds", "pr")
        ->fields("pr", array("pl_id"));

        $query = db_delete("fanta_players");
        $query->condition("pl_id", $subquery, "NOT IN");
        $query->execute();
      
      // cancello giocatori inseriti
        $query = db_delete("fanta_players_rounds");
        $query->condition("round", $round);
        $query->execute();

        //reinserisco tutti i giocatori presenti la giornata precedente (dati default)
        $query = db_select("fanta_players_rounds", "pr");
        $query->condition("round", $round - 1);
        $query->fields("pr");

        $result = $query->execute();

        $last_players = array();
        foreach($result as $row) {
          db_insert("fanta_players_rounds")->fields(array(
          "pl_id" => $row->pl_id,
          "rt_id" => $row->rt_id,
          "quotation" => 0,
          "not_rounded_quotation" => 0,
          "round" => $round,
          "active" => 0))->execute();

          $last_players[] = $row->pl_id;
        }
      
      echo "last_player:" . count($last_players) . "<br>";

        // aggiorno i giocatori già presenti
        if (count($tmp_players) > 0) {
            if (count($last_players) > 0) {
                $i = 0;
              foreach ($tmp_players as $player) {
                db_update("fanta_players_rounds")->fields(array(
        //          "pl_id" => $player->pl_id, 
                  "rt_id" => $player->team, 
                  "quotation" => $player->quotation, 
                  "not_rounded_quotation" => $player->not_rounded_quotation, 
                  "active" => 1))
                    ->condition("pl_id", $player->pl_id)
                    ->condition("round", $round)
                    ->execute();
                  $i++;
              }
                echo "update: ". $i . "<br>";
            }
            else {
                $i = 0;
                echo "min: ". min(array_keys($tmp_players)) . "<br>";
               foreach ($tmp_players as $player) {
                db_insert("fanta_players_rounds")->fields(array(
                  "pl_id" => $player->pl_id, 
                  "rt_id" => $player->team, 
                  "round" => $round, 
                  "quotation" => $player->quotation, 
                  "not_rounded_quotation" => $player->not_rounded_quotation, 
                  "active" => 1))
        //            ->condition("pl_id", $player->pl_id)
        //            ->condition("round", $round)
                    ->execute();
                   $i++;
              }
                echo "insert: ". $i . "<br>";
            }
        }
//print_r($new_players);die();
        // aggiungo i giocatori nuovi
        if (count($new_players) > 0) {
          foreach ($new_players as $player) {
            // print_r( $player->name)."<br>";
            $new_id = db_insert("fanta_players")->fields(array(
              "name" => strtoupper($player->name), 
              "role" => $player->role))->execute();

            db_insert("fanta_players_rounds")->fields(array(
              "pl_id" => $new_id, 
              "rt_id" => $player->team, 
              "quotation" => $player->quotation, 
              "not_rounded_quotation" => $player->not_rounded_quotation, 
              "round" => $round, 
              "active" => 1))->execute();
          }
        }

        // disattivo i vecchi giocatori
        $old_players = array();
        foreach ($players_ids as $pl_id) {
          if (!in_array($pl_id, array_keys($tmp_players)) && !in_array($pl_id, $last_players)) {
            db_insert("fanta_players_rounds")->fields(array(
              "pl_id" => $pl_id, 
              "rt_id" => NULL, 
              "quotation" => 0, 
              "not_rounded_quotation" => 0, 
              "round" => $round, 
              "active" => 0))->execute();

            $old_players[] = $pl_id;
          }
        }

      echo "old_player:" . count($old_players) . "<br>";
      //die();
      
        drupal_set_message(check_plain("Giocatori aggiornati: " . count($tmp_players)));
        drupal_set_message(check_plain("Giocatori disattivati: " . count($old_players)));
        drupal_set_message(check_plain("Giocatori nuovi: " . count($new_players)));

        watchdog("fantacalcio", t("Lista giocatori aggiornata"), null, WATCHDOG_INFO);

      }
  }

  static function updateStatus() {
    $round = Round::getNext();
    
    $players_status = json_decode(file_get_contents(DATA_SOURCE_URL . "/status/" . $round . ".json"));
    
    $players_ids = self::getIdList();
    $updated_players_ids = array();
    
    foreach ($players_status->data as $player_name => $player_status) {
      if (array_key_exists($player_name, $players_ids)) {
        $pl_id = $players_ids[$player_name];
        
        $position = (!isset($player_status->position)) ? 0 : ($player_status->position == "regular" ? 1 : 2);
        
        db_delete("fanta_players_status")->condition("pl_id", $pl_id)->condition("round", $round)->execute();
        
        db_insert("fanta_players_status")->fields(array(
          "pl_id" => $pl_id, 
          "round" => $round, 
          "status" => $player_status->status, 
          "position" => $position, 
          "percent" => !isset($player_status->percent) ? 0 : $player_status->percent, 
          "updated" => $player_status->updated))->execute();
        
        array_push($updated_players_ids, $pl_id);
      }
    }
    
    foreach ($players_ids as $pl_id) {
      if (!in_array($pl_id, $updated_players_ids)) {
        //$pl_id = $players_ids[$player_name];
                
        db_delete("fanta_players_status")->condition("pl_id", $pl_id)->condition("round", $round)->execute();
        
        db_insert("fanta_players_status")->fields(array(
          "pl_id" => $pl_id, 
          "round" => $round, 
          "status" => "not_found", 
          "position" => 0, 
          "percent" => 0, 
          "updated" => time()))->execute();        
      }
    }
    
    watchdog("fantacalcio", t("Status giocatori aggiornati (Giornata #@round)"), array('@round' => $round), WATCHDOG_INFO);
  }

  function getPlayerRounds() {
//     $sql = "SELECT * FROM {fanta_players_teams} p, {fanta_real_teams} t
//       WHERE p.rt_id = t.rt_id
//       AND p.pl_id = :pl_id
//       ORDER BY p.round";
    
    $query = db_select("fanta_players_rounds", "r");
    $query->join("fanta_real_teams", "rt", "r.rt_id = rt.rt_id");
    $query->fields("r");
    $query->fields("rt");
    $query->condition("pl_id", $this->id);
    
    $result = $query->execute();
    
    $player = array();
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
  
  function get_fantateams($groups, $teams_groups) {
    $fanta_teams = array();
    
    $sql = "SELECT * FROM {fanta_squads}
      WHERE pl_id = '%d'
      AND in_team = 1";
    
    $query = db_select("fanta_squads", "s");
    $query->join("fanta_teams", "t", "t.t_id = s.t_id");
    $query->join("fanta_teams_groups", "tg", "tg.t_id = t.t_id");
    $query->join("fanta_groups", "g", "g.g_id = tg.g_id");
    $query->condition("pl_id", $this->id);
    $query->condition("status", 1);
    $query->fields("s");
    $query->fields("t");
    $query->fields("g");
    
    $result = $query->execute();
    foreach ($result as $row) {
      $fanta_teams[] = $row->t_id;
    }
  
    foreach ($teams_groups as $g_id => $teams_group) {
      $team_name = "-";
  
      foreach ($teams_group as $t_id => $team) {
        if (in_array($t_id, $fanta_teams)) {
          $team_name = $team->name;
          continue;
        }
      }
      $items[$g_id] = $groups[$g_id]->name . ": " . $team_name;
    }
  
    return theme_item_list($items);
  }
  
  function getStatus($round) {
    
    $status = null;
    
    $query = db_select("fanta_players_status", "s")
      ->condition("pl_id", $this->id)
      ->condition("round", $round)
      ->fields("s");
    $result = $query->execute();
    
    foreach($result as $row) {
      $status = $row;
    }
      
    if ($result->rowCount() > 0) {
    
    //partita
    $query = db_select("fanta_players_rounds", "pr");
    $query->condition("pl_id", $this->id);
     $query->condition("round", $round);
       $query->addField("pr", "rt_id", "rt_id");
        
    $result = $query->execute();
    $rt_id = $result->fetchField();
   
    $status->match = "";
 
    $query = db_select("fanta_real_teams_matches", "m");
    	$query->join("fanta_real_teams", "t1", "m.rt1_id = t1.rt_id");
        $query->join("fanta_real_teams", "t2", "m.rt2_id = t2.rt_id");
        $query->condition("round", $round);
        $query->fields("m");
        $query->addField("t1", "name", "home_team");
        $query->addField("t2", "name", "away_team");
        
    $result = $query->execute();
    
    foreach($result as $row) {
    	if ($row->rt1_id == $rt_id || $row->rt2_id == $rt_id) {
        	$status->match = $row->home_team . " - " . $row->away_team;
        }
    } 
    }
    
    return $status;
  }
}

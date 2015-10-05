<?php
/**
* @file fantacalcio.block.inc
* 
*/

function fantacalcio_block_user_summary_title() {
  global $user;
  
  $teams = Team::allByUser($user->uid);  
  $team = array_pop($teams);
  
  return $team->name;
}

#Funzioni per la visualizzazione dei blocchi del modulo fantacalcio
function fantacalcio_block_user_summary() {

  global $user;
  
  $teams = Team::allByUser($user->uid);
  
  //giornata
  $vote_round = Round::getNext();
  $round = Round::getByRound($vote_round);
  
  //orario giornata
  $date = $round->date;

  //per ogni competizione
  $out_competitions = "";
  foreach($round->competitions as $competition_id => $competition_round) {
    
    $competition = Competition::get($competition_id);
    
    //per ogni squadra dell'utente
    $rows = array();
    foreach($teams as $t_id => $team) {

      //partita utente?
      if ($team->hasMatch($competition_id, $competition_round->competition_round)) {
        //partita 
        $match = Match::getByTeamAndRound($t_id, $competition_id, $competition_round->competition_round);
        //formazione inserita o no
//         $rows[$t_id][] = l($match->home_team . " - " . $match->away_team, "scheda/partita/" . $match->id);
        $rows[$t_id][] = $match->home_team . " - " . $match->away_team;
        
        $lineup = Lineup::get($competition_id, $t_id, $competition_round->competition_round);
        
        if ($lineup != null) {
           if (time() > $date)
             $rows[$t_id][] = "<i>" . t("Formazione inserita"). "</i>";
          else
          $rows[$t_id][] = "<i>" . l(t("Modifica formazione"), "formazioni/insert/" . $t_id . "/" . $competition->sanitized_name) . "</i>";
        }
        else {
          if (time() > $date)
             $rows[$t_id][] = "<i>" . t("Formazione non inserita"). "</i>";
          else
            $rows[$t_id][] = "<i>" . l(t("Inserisci formazione"), "formazioni/insert/" . $t_id . "/" . $competition->sanitized_name) . "</i>";
        }
$rows[$t_id][] =  l("<i class='fa fa-bar-chart'></i>", "scheda/partita/" . $match->id, array("html" => true) ) ;
      }
      
    }

    if (count($rows) > 0) {
      $out_competitions .= "<strong>" . $competition->name . " - " . $competition_round->label . "</strong>";
      $out_competitions .= theme("table", array("rows" => $rows, "attributes" => array("class" => array("table", "table-responsive"))));
    }
  }
  
  $out = "<h4 class='pull-right clearfix'><strong>" . date("d-m-Y H:i", $round->date) . "</strong></h4>";
  $out .= "<div class='clearfix'></div>";
  $out .= $out_competitions;
  
  return $out;
}

function fantacalcio_block_competition_summary() {
  return "competition summary";
}


function fantacalcio_block_riepilogo() {

  $out = ''; $out_li = '';

  $out_li .= fantacalcio_avviso_formazioni();
  $out_li .= fantacalcio_avviso_sondaggi();
  if (module_exists('fantasfide')) $out_li .= fantacalcio_avviso_sfide();
  if (module_exists('privatemsg')) $out_li .= fantacalcio_avviso_messaggi();
  if (module_exists('facebook_status')) $out_li .= fantacalcio_avviso_bacheca();
  if (module_exists('bets')) $out_li .= fantacalcio_avviso_scommesse();

  if (!empty($out_li))
    $out .= "\n<div>
        <ul>"
      . $out_li
      . "\n</ul>
        </div>";

  return $out;
}

function check_formazione_inserita() {

  global $user;

  $this_round = get_last_votes() + 1;
  $t_id = get_team_id_by_user($user->uid);
  
  $date = db_result(db_query("SELECT date FROM {fanta_rounds} WHERE round = '%d'", $this_round));

  $rounds = array();
  $sql = "SELECT * FROM {fanta_rounds_competitions} WHERE round = '%d'";
  $result = db_query($sql, $this_round);
  while ($row = db_fetch_array($result)) {
    $rounds[$row['c_id']] = $row['competition_round'];
  }

  $errors = 0;
  foreach ($rounds as $c_id => $round) {
    $sql = "SELECT * FROM {fanta_matches} " .
        "WHERE g_id IN (SELECT g_id FROM {fanta_groups} WHERE c_id = '%d' ) " .
        "AND round = '%d' " .
        "AND (t1_id = '%d' OR t2_id = '%d')";
    $result = db_query($sql, $c_id, $round, $t_id, $t_id);
    while ($row = db_fetch_array($result)) {
      if ( ($date - time() ) < 86400 && ($date - time() ) > 0 ) {
        $sqla = "SELECT * FROM {fanta_lineups_inserts} 
              WHERE t_id = '%d' 
              AND c_id = '%d' 
              AND round = '%d'";
        $resulta = db_query($sqla, $t_id, $c_id, $round);

        if (db_affected_rows($resulta) == 0) $errors++;
      }
    }
  }

  return array("errors" => $errors, "date" => $date);

}


function fantacalcio_block_countdown() {

  $sql = "SELECT date 
          FROM {fanta_rounds} 
          WHERE round = (SELECT MIN(round) FROM {fanta_rounds} WHERE status = 0)";
  $date = db_result(db_query($sql));

  $diff = $date - time();

  if ($diff > 0) {

    drupal_add_js( "sites/all/plugins/jquerycountdown/jquery.countdown.js", 'footer');
    drupal_add_js( "sites/all/plugins/jquerycountdown/jquery.countdown-it.js", 'footer');
    $js = "$(function(){
             var nextRound = new Date();
             var y = " . date("Y", $date) . ";
			 var m = " . date("m", $date) . ";
             var d = " . date("d", $date) . ";
             var h = " . date("H", $date) . ";
             var i = " . date("i", $date) . ";
             var s = " . date("s", $date) . ";
	     nextRound = new Date(y, m-1, d, h, i, s);
	     $('#counter').countdown({until: nextRound});
           });";
    drupal_add_js($js, "inline");
    drupal_add_css("sites/all/plugins/jquerycountdown/jquery.countdown.css");

    $out = "<div id='counter'></div>";
    
    return $out;
  }
}

function fantacalcio_block_last_wall_messages($n = 5) {
  
  $query = db_select("statuses", "s");
  $query->join("users", "u", "u.uid = s.sender");
  $query->fields("s");
  $query->fields("u");
  $query->orderBy("s.created", "DESC");
  $query->range(0, $n);
  
  $result = $query->execute();
  
  $out_messages = "";
  foreach($result as $row) {
    $out_messages .= fantacalcio_block_wall_message_theme($row);
  }
  
  if ($out_messages != "") {
    $out = "<ul class='home-wall-block'>";
    $out .= $out_messages;
    $out .= "\n</ul>";
    $out .= "<p style='width:100%;text-align:right;'>" . l(t("Leggi tutto") ." &raquo;", "bacheca", array("html" => "true")) . "</p>";
    return $out;
  }
  else
    return "";
}

function fantacalcio_block_wall_message_theme($status) {

  if (strlen($status->message) < 80) 
    $status_text = $status->message;
  else {
    $truncated = substr($status->message, 0, 80);
    $last_space = strrpos($truncated, " ");
    $status_text = substr($truncated, 0, $last_space) . "...";
  }
  
  if (isset($status->picture) && $status->picture != 0) {
    $user = user_load($status->sender);
    $status_picture = theme('image_style', array('path' => $user->picture->uri, 'style_name' => 'mini', 'attributes' => array("class" => array("img-circle", "img-responsive", "pull-left"))));
  }
  else
    $status_picture = "<span class='fa fa-user fa-lg' style='padding-top: 10px;'></span>";
  
  $out = "\n<li class='home-wall-message'>"
      . "<div class='home-wall-picture'>"
          . $status_picture
          . "</div>"
              . "<div class='home-wall-text'><strong>"
                  . $status->name                  
                  . "</strong> " . $status_text . "</div>"
                      . "<div class='clearfix'></div>"
                          . "</li>";
  return $out;
}

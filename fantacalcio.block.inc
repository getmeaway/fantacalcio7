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
  
  //prossima giornata
  $next_round_number = Round::getNext();
  $next_round = Round::getByRound($next_round_number);

  //orario giornata
  $date = $next_round->date;

  //per ogni competizione
  $out_next_competitions = "";
  foreach($next_round->competitions as $competition_id => $competition_round) {

    $competition = Competition::get($competition_id);

    //per ogni squadra dell'utente
    $rows = array();
    foreach($teams as $t_id => $team) {

      //partita utente?
      if ($team->hasMatch($competition_id, $competition_round->competition_round)) {
        //partita
        $match = Match::getByTeamAndRound($t_id, $competition_id, $competition_round->competition_round);
        //formazione inserita o no
        $rows[$t_id][] = array("data" => $match->home_team . " - " . $match->away_team, "width" => "50%");

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
      $out_next_competitions .= "<em class='-pull-right'>" . $competition->name . " - " . $competition_round->label . "</em>";
      $out_next_competitions .= theme("table", array("rows" => $rows, "attributes" => array("class" => array("table", "table-responsive"))));
    }
  }

  $out_next = "<h4 class='bold'>" 
	. ((time() > $next_round->date) ? t("Giornata in corso") : t("Prossima giornata") ."<span class='pull-right clearfix'>" . date("d-m-Y H:i", $next_round->date) . "</span>") . "</h4>";
  $out_next .= "<div class='clearfix'></div>";
  $out_next .= $out_next_competitions;
 
  //ultima giornata
  $last_round_number = Round::getLast();
  $last_round = Round::getByRound($last_round_number);

  //orario giornata
  $date = $last_round->date;

  //per ogni competizione
  $out_last_competitions = "";
  foreach($last_round->competitions as $competition_id => $competition_round) {

    $competition = Competition::get($competition_id);

    //per ogni squadra dell'utente
    $rows = array();
    foreach($teams as $t_id => $team) {

      //partita utente?
      if ($team->hasMatch($competition_id, $competition_round->competition_round)) {
        //partita
        $match = Match::getByTeamAndRound($t_id, $competition_id, $competition_round->competition_round);
        $rows[$t_id][] = array("data" => $match->home_team . " - " . $match->away_team, "width" => "50%");
		$rows[$t_id][] = $match->goals_1 . "-" . $match->goals_2;
		$rows[$t_id][] = $match->tot_1 . "-" . $match->tot_2;
		$rows[$t_id][] =  l("<i class='fa fa-bar-chart'></i>", "scheda/partita/" . $match->id, array("html" => true) ) ;
      }

    }

    if (count($rows) > 0) {
      $out_last_competitions .= "<em class='-pull-right'>" . $competition->name . " - " . $competition_round->label . "</em>";
      $out_last_competitions .= theme("table", array("rows" => $rows, "attributes" => array("class" => array("table", "table-responsive"))));
    }
  }

  $out_last = "<h4 class='bold'>" . t("Ultima giornata") . "</h4>";
  $out_last .= "<div class='clearfix'></div>";
  $out_last .= $out_last_competitions;

  $team = Team::get($t_id);

    $default_competition = Competition::getDefault();

    $rows = array();

    $position = $team->statPosition($default_competition);

    $rows[] = array(t("Posizione attuale"), $position . "&deg;");
    $rows[] = array(
      t("Punteggio attuale"),
      $team->statPoints($default_competition) . " pt");

    if ($position == 1)
      $rows[] = array(
        t("Distacco dal secondo"),
        $team->statGap($default_competition) . " pt");
    else
      $rows[] = array(
        t("Distacco dal primo"),
        $team->statGap($default_competition) . " pt");

  if (abs(time() - $next_round->date) > (abs(time() - $last_round->end_date)))
    $out = $out_last . "<div class='clearfix'>&nbsp;</div>" .$out_next;
  else 
    $out = $out_next . "<div class='clearfix'>&nbsp;</div>" .$out_last;

  $out .= "<div class='clearfix'>&nbsp;</div>";
  $out .= "<h4 class='bold'>" . t("Situazione") . "</h4>";
  $out .= "<div class='clearfix'></div>";
  $out .= theme("table", array("rows" => $rows, "attributes" => array("class" => array("table", "table-responsive"))));
 
  return $out;
}

function fantacalcio_block_competition_summary() {
  return "competition summary";
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

<?php

/**
 * @file fantacalcio.payments.inc
 *
 */

/**
 * *****************************
 */
/* quote*/
/**
 * *****************************
 */
function fantacalcio_show_payments() {
  $out = '';
  $payments = array();
  
  $teams = Team::allActive();
  
  $fines_sum = 0;
  $default_competition = Competition::getDefault();
  $groups = Group::allByCompetition($default_competition->id);

  foreach ($groups as $g_id => $group) {

    $rows["g-" . $g_id] = array(
      array("data" => $group->name, "class" => "text-left bold"), 
      array("data" => "", "class" => "text-right"), 
      array("data" => "", "class" => "text-right"), 
      array("data" => "", "class" => "text-right"), 
      array("data" => "", "class" => "text-right"), 
      array("data" => "", "class" => "text-right"), 
      array("data" => "", "class" => "text-right"), 
      array("data" => "", "class" => "text-right"), 
      array("data" => "", "class" => "text-right"), 
      array("data" => "", "class" => "text-right"), 
      array("data" => "", "class" => "text-right"));

  $teams = Team::allByGroup($g_id);
  foreach ($teams as $t_id => $team) {
    
    $query = db_select("fanta_payments", "p");
    $query->condition("t_id", $t_id);
    $query->fields("p");
    $result = $query->execute();
    
    foreach ($result as $row) {
      $quota = $row->is_admin == 1 ? variable_get("fantacalcio_quota_admin") : variable_get("fantacalcio_quota");
      $payed = $row->payed;
      $win_quota = $row->win_quota;
      $lost_quota = $row->lost_quota;
      $received = $row->received;
      $admin_quota = $row->is_admin == 1 ? 0 : variable_get("fantacalcio_quota_admin_players");
    }
    
    $lineups_fine = 0;
    $lineups_rounds = array();
//     $sql = "SELECT * FROM {fanta_lineups_inserts} WHERE status = 0 AND c_id = '%d' AND t_id = '%d'";
//     $result = db_query($sql, variable_get("fantacalcio_main_competition", 1), $t_id);
//     while ($row = db_fetch_object($result)) {
      
    $query = db_select("fanta_lineups_inserts", "l");
    $query->condition("t_id", $t_id);
    $query->condition("status", 0);
    $query->condition("c_id", $default_competition->id);
    $query->fields("l");
    $result = $query->execute();
      
    foreach ($result as $row) {        
      $lineups_fine++;
      array_push($lineups_rounds, $row->round . "&ordf;");
    }
    
    $lineups_fine = $lineups_fine * ($lineups_fine + 1) / 2;
    $lineups_rounds = ($lineups_fine > 0 ? implode(",", $lineups_rounds) : "");
    
    $payoff = -$quota + $payed - $lost_quota - $admin_quota - $lineups_fine + $win_quota;
    
    $payoff_class = ($payoff > 0 ? "ok" : ($payoff < 0 ? "error" : ""));
    $sum_class = ($payoff - $received == 0 ? "text-success" : "");    

    $rows[$t_id] = array(
      array("data" => $team->name, "class" => "text-left"), 
      array("data" => $payed . " &euro;", "class" => "text-right"), 
      array("data" => "-" . $quota . " &euro;", "class" => "text-right"), 
      array("data" => ($admin_quota > 0 ? "-" . $admin_quota  : 0). " &euro;", "class" => "text-right"), 
      array("data" => ($win_quota > 0 ? "+" . $win_quota  : 0) . " &euro;", "class" => "text-right"), 
      array("data" => ($lost_quota > 0 ? "-" . $lost_quota : 0) . " &euro;", "class" => "text-right"), 
      array("data" => ($lineups_fine> 0 ? "-" . $lineups_fine : 0) . " &euro;"  , "class" => "text-right"), 
      array("data" => (empty($lineups_rounds ) ? "" : "<i class='fa fa-info-circle fa-fw' title='(" . $lineups_rounds . ")'></i>"), "class" => "text-right"), 
      array(
        "data" => $payoff . " &euro;", 
        "class" => "text-right bold " . $payoff_class),
      array("data" => $received . " &euro;", "class" => "text-right"), 
      array("data" => ($payoff - $received) . " &euro;", "class" => "text-right bold " . $sum_class));
    
    $fines_sum += $lineups_fine;
  }
$rows["empty-" . $g_id] = array("", "", "", "", "", "", "", "", "", "", "");
  }
  
  $header = array(
    array("data" => "Squadra", "class" => "text-left"), 
    array("data" => "Pagato", "class" => "text-right"), 
    array("data" => "Quota", "class" => "text-right"), 
    array("data" => "Quota Amm.", "class" => "text-right"), 

    array("data" => "Vincita", "class" => "text-right"), 
    array("data" => "Perdita", "class" => "text-right"), 

    array("data" => "Multe", "class" => "text-right"), 
    array("data" => "", "class" => "text-right"), 
    array("data" => "Totale", "class" => "text-right"), 
    array("data" => "Ricevuto", "class" => "text-right"), 
    array("data" => "Saldo", "class" => "text-right"));


$rows["total"] = array(
        array("data" => "Totale multe", "class" => "bold text-left"), "", "", "", "", "", 
        "attributes" => array(
          "data" => $fines_sum . " &euro;", 
          "class" => "bold"),"", "", "", "");
  
  $out = theme("table", array(
    "header" => $header, 
    "rows" => $rows, 
    "attributes" => array("class" => "text-right")));
/*  
  $out .= theme("table", array(
    "header" => array(), 
    "rows" => array(
      array(
        array("data" => "Totale multe", "class" => "bold"), 
        "attributes" => array(
          "data" => $fines_sum . " &euro;", 
          "class" => "bold")))));
  */
  return $out;
}

function fantacalcio_calculate_fine($num) {
  // calcola la multa totale per ciascuna squadra
  return $num * ($num + 1) / 2;
}

/**
 * *****************************
 */
/* END quote*/
/**
 * *****************************
 */

function fantacalcio_payments() {

  global $user;

  $my_teams = Team::allByUser($user->uid);

  foreach($my_teams as $t_id => $team) {
    $groups = $team->getGroups();
    foreach($groups as $g_id => $group) {
      if($group->c_id == Competition::getDefault()->id) {
        $my_g_id = $group->g_id;
      }
    }
  }

  if($my_g_id) {
    $payment_rows = array();
    $teams = Team::allByGroup($my_g_id);
    foreach($teams as $t_id => $team) {
      $query = db_select("fanta_payments", "p");
      $query->condition("t_id", $t_id);
      $query->fields("p");
    
      $result = $query->execute();
      foreach ($result as $row) {
        $payment_rows[$t_id] = array(l($team->name, "pagamenti/" . $team->id), array("data" => $row->payed . " &euro;", "class" => "text-right"));
      }
    }
  }

  $header = array("Squadra", array("data" => "Pagato", "class" => "text-right"));

  $out = theme("table", array("header" => $header, "rows" => $payment_rows, "attributes" => array("class" => array("table", "table-responsive"))));

  return $out;
}

function fantacalcio_payments_all() {

  global $user;

  $payment_rows = array();

  $default_competition = Competition::getDefault();
  $groups = Group::allByCompetition($default_competition->id);

  foreach ($groups as $g_id => $group) {

    $payment_rows["g-" . $g_id] = array(
      array("data" => $group->name, "class" => "text-left bold"), 
      array("data" => "", "class" => "text-right"), 
      array("data" => "", "class" => "text-right"), 
      array("data" => "", "class" => "text-right"), 
      array("data" => "", "class" => "text-right"), 
      array("data" => "", "class" => "text-right"));

  $teams = Team::allByGroup($g_id);
  foreach ($teams as $t_id => $team) {

    foreach($teams as $t_id => $team) {
      $query = db_select("fanta_payments", "p");
      $query->condition("t_id", $t_id);
      $query->fields("p");
    
      $result = $query->execute();
      foreach ($result as $row) {
        $payment_rows[$t_id] = array(l($team->name, "pagamenti/all/" . $team->id), array("data" => $row->is_admin == 1 ? "*" : "", "class" => "text-center"), array("data" => $row->payed . " &euro;", "class" => "text-right"), array("data" => $row->win_quota . " &euro;", "class" => "text-right"), array("data" => $row->lost_quota . " &euro;", "class" => "text-right"), array("data" => $row->received . " &euro;", "class" => "text-right"));
      }
    }
  }
  }

  $header = array("Squadra", array("data" => "Amministratore"), array("data" => "Pagato", "class" => "text-right"), array("data" => "Vincita", "class" => "text-right"), array("data" => "Perdita", "class" => "text-right"), array("data" => "Ricevuto", "class" => "text-right"));

  $out = theme("table", array("header" => $header, "rows" => $payment_rows, "attributes" => array("class" => array("table", "table-responsive"))));

  return $out;
}

function fantacalcio_payments_team($t_id) {

  return drupal_get_form("fantacalcio_payment_form", $t_id);
}

function fantacalcio_payments_all_team($t_id) {

  return drupal_get_form("fantacalcio_payment_all_form", $t_id);
}

function fantacalcio_payment_form($form, $form_state, $t_id) {
  global $user;
    
  if ($t_id == null)
    $team = null;
  else {
    $team = Team::get($t_id);
    $_team = Team::getPayment($t_id);
    $team->payed = $_team->payed;
    $team->is_admin = $_team->is_admin;
  }
  
  $action = "insert";
  $title = t("Pagamenti") . " - " . $team->name;
  drupal_set_title(filter_xss($title));
  
    $form["title"] = array("#title" => check_plain($title));
  
  $form["payed"] = array(
    "#type" => "textfield", 
    "#title" => "Pagato", 
    "#default_value" => $team->payed);
  
  $form["t_id"] = array(
    "#type" => "hidden",
    "#value" => check_plain($team->id));
  
  $form["submit"] = array("#type" => "submit", "#value" => "Ok");
  
  $form["clear"] = array(
    "#type" => "submit", 
    "#value" => "Annulla", 
    "#validate" => array("fantacalcio_team_form_clear"));
  
  return $form;
}

function fantacalcio_payment_form_clear($form, &$form_state) {
  $form_state["rebuild"] = TRUE;
}

function fantacalcio_payment_form_validate($form, &$form_state) {
  $payed = $form_state["values"]["payed"];
  if (!is_numeric($payed))
    form_set_error("payed", t("Inserisci un valore numerico"));    
}

function fantacalcio_payment_form_submit($form, &$form_state) {
  global $user;
  
  $payed = $form_state["values"]["payed"];
  $t_id = $form_state["values"]["t_id"];

    $fields = array(
      'payed' => $payed);
  
    $query = db_update('fanta_payments');
    $query->fields($fields);
    $query->condition('t_id', $t_id);
    $query->execute();
    
    drupal_set_message(t("Pagamento inserito con successo"));  
   
  drupal_goto("pagamenti");
}

function fantacalcio_payment_all_form($form, $form_state, $t_id) {
  global $user;
    
  if ($t_id == null)
    $team = null;
  else {
    $team = Team::get($t_id);
    $_team = Team::getPayment($t_id);
    $team->payed = $_team->payed;
    $team->is_admin = $_team->is_admin;
    $team->win_quota = $_team->win_quota;
    $team->lost_quota = $_team->lost_quota;
    $team->received = $_team->received;
  }
  
  $title = t("Pagamenti") . " - " . $team->name;
  drupal_set_title(filter_xss($title));
  
    $form["title"] = array("#title" => check_plain($title));

	$form["is_admin"] = array(
    "#type" => "checkbox", 
    "#title" => "Amministratore", 
    "#default_value" => $team->is_admin);

  $form["payed"] = array(
    "#type" => "textfield", 
    "#title" => "Pagato", 
    "#default_value" => $team->payed);
  
  $form["win_quota"] = array(
    "#type" => "textfield", 
    "#title" => "Vincita", 
    "#default_value" => $team->win_quota);

  $form["lost_quota"] = array(
    "#type" => "textfield", 
    "#title" => "Perdita", 
    "#default_value" => $team->lost_quota);
  
  $form["received"] = array(
    "#type" => "textfield", 
    "#title" => "Ricevuto", 
    "#default_value" => $team->received);

  $form["t_id"] = array(
    "#type" => "hidden",
    "#value" => check_plain($team->id));
  
  $form["submit"] = array("#type" => "submit", "#value" => "Ok");
  
  $form["clear"] = array(
    "#type" => "submit", 
    "#value" => "Annulla", 
    "#validate" => array("fantacalcio_payment_all_form_clear"));
  
  return $form;
}

function fantacalcio_payment_all_form_clear($form, &$form_state) {
  $form_state["rebuild"] = TRUE;
}

function fantacalcio_payment_all_form_validate($form, &$form_state) {
  $payed = $form_state["values"]["payed"];
  if (!is_numeric($payed))
    form_set_error("payed", t("Inserisci un valore numerico"));

  $win_quota = $form_state["values"]["win_quota"];
  if (!is_numeric($win_quota))
    form_set_error("win_quota", t("Inserisci un valore numerico"));    

  $lost_quota = $form_state["values"]["lost_quota"];
  if (!is_numeric($lost_quota))
    form_set_error("lost_quota", t("Inserisci un valore numerico"));       

  $received = $form_state["values"]["received"];
  if (!is_numeric($received))
    form_set_error("received", t("Inserisci un valore numerico"));        
}

function fantacalcio_payment_all_form_submit($form, &$form_state) {
  global $user;
  
  $payed = $form_state["values"]["payed"];
  $is_admin = $form_state["values"]["is_admin"];
  $win_quota = $form_state["values"]["win_quota"];
  $lost_quota = $form_state["values"]["lost_quota"];
  $received = $form_state["values"]["received"];
  $t_id = $form_state["values"]["t_id"];

    $fields = array(
      'payed' => $payed,
      'win_quota' => $win_quota,  
      'lost_quota' => $lost_quota,  
      'received' => $received,
      'is_admin' => $is_admin);
  
    $query = db_update('fanta_payments');
    $query->fields($fields);
    $query->condition('t_id', $t_id);
    $query->execute();
    
    drupal_set_message(t("Pagamento inserito con successo"));  
   
  drupal_goto("pagamenti/all");
}

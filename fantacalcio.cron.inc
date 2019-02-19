<?php
/**
  * @file fantacalcio.cron.inc
*/

function fantacalcio_create_lineups_reminder($round) {

  $node = new stdClass(); // We create a new node object
  $node->type = "news"; // Or any other content type you want

  node_object_prepare($node);

  $node->title = 'Formazioni ' . $round->round . 'ª giornata';
  $node->language = LANGUAGE_NONE; // Or any language code if Locale module is enabled. More on this below *
//   $node->path = array('alias' => 'news/risultati-' . $vote_round); // Setting a node path
  $node->uid = 1;//$user->uid; // Or any id you wish

  $reminder_date = date("H:i", strtotime($round->date));

  $body = "Inserire le " . l("formazioni", "formazioni/insert"). " entro le " . $reminder_date . " di oggi";

  $node->body[$node->language][0]['value'] = $body;
  $node->body[$node->language][0]['summary'] = $body;
  $node->body[$node->language][0]['format']  = 'full_html';

  $node->status = 1;   // (1 or 0): published or unpublished
  $node->promote = 1;  // (1 or 0): promoted to front page or not
  $node->sticky = 0;  // (1 or 0): sticky at top of lists or not
  $node->comment = 0;  // 2 = comments open, 1 = comments closed, 0 = comments hidden

  $node = node_submit($node); // Prepare node for saving
  node_save($node);

  $nid = $node->nid;
  $node = node_load($nid);

  // Make this change a new revision
  $node->revision = 1;

  node_save($node);

  db_update("fanta_rounds")->fields(array("reminder_sent" => 1))->condition("round", $round->round)->execute();

  watchdog('fantacalcio', t('Promemoria creato per la giornata: %round'), array('%round' => $round->round), WATCHDOG_NOTICE);
}

function fantacalcio_cron_calculate_results($round) {

  // 1. importo formazioni
  Result::importLineups($round);

  // 2. ottengo voti
  Result::insertVotes($round);
  
  // 3. trovo titolari
  Result::findRegulars($round);
  
  // 4. calcolo modificatori
  Result::getModifiers($round);
  
  // 5. calcolo totali
  Result::getTotals($round);
 
  // 6. aggiorno giocatori
  Result::updatePlayers($round); 

  // 7. chiudo giornata
  Result::closeRound($round);

  watchdog('fantacalcio', t('Risultati calcolati per la giornata: %round'), array('%round' => $round->round), WATCHDOG_NOTICE);
}


<?php

/**
  * @file fantarest.common.inc
*/

function choose_competition_rest($args = array()) {
  $items = choose_competition_api($args);
  return drupal_json_output($items);
}

<?php

function fantacalcio_notifications_list() {
	
    global $user;
    
    $last_notifications_check = Notification::getLastCheck($user->uid);
    
	$notifications = Notification::all($user->uid, $last_notifications_check);
    
    Notification::updateLastCheck($user->uid, time());
    
    $items = array();
    
    foreach ($notifications as $notification) {
        $item = array();
        
        $item["class"] = array("list-group-item");
        if (!$notification->unread)
            $item["class"][] = "read";
        
        $text = "<span class='notification-text'>" . $notification->text . "</span>";
        $time = "<span class='notification-time pull-right'><em><time class='timeago' datetime='" . $notification->datetime . "'>" . $notification->datetime . "</time></em></span>";
                
        if (!empty($notification->link))
            $data = l($text, $notification->link, array("html" => true));
        else
            $data = $text;
        
        $item["data"] = $data . $time;
            
        array_push($items, $item);
    }
    
    drupal_add_js('jQuery(document).ready(function() {jQuery("time.timeago").timeago();});', "inline");
        
    return theme("item_list", array("items" => $items, "type" => "ul", "attributes" => array("class" => "list-group")));
}

function fantacalcio_notifications($limit = null) {
    
    global $user;
    
    $last_notifications_check = Notification::getLastCheck($user->uid);
			
	$notifications = Notification::last($user->uid, $last_notifications_check, $limit);
    
    Notification::updateLastCheck($user->uid, time());
    
    drupal_json_output($notifications);
}

function fantacalcio_create_notifications($text, $link, $uid = -1) {
			
	return Notification::create($text, $link, $uid);
}

function fantacalcio_unread_notifications() {
    global $user;    
        
    drupal_json_output(Notification::getUnread($user->uid, Notification::getLastCheck($user->uid)));
}
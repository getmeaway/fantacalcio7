<?php 
	
class Notification {
	
	var $id;
	var $text;
	var $uid;
	var $link;
	var $timestamp;
	var $unread;
	
	function __construct() {
	}
	
	public static function all($uid, $notifications_last_check, $limit = null) {
		$notifications = array();
		
		$user_or = db_or()->condition('uid', $uid)->condition('uid', -1);
	
		$query = db_select("fanta_notifications", "n");
		$query->condition($user_or);
		$query->fields("n");
		$query->orderBy("n.timestamp", "DESC");
	
		if ($limit != null)
			$query->range(0, $limit);
	
		$result = $query->execute();
	
		foreach($result as $row) {
			$notification = new Notification();
			$notification->id = $row->n_id;
            $notification->user = $row->uid;
			$notification->text = $row->text;
			$notification->link = $row->link;
			$notification->timestamp = (int)$row->timestamp;
			$notification->unread = $row->timestamp > $notifications_last_check;
            $notification->datetime = date("c", $row->timestamp);
			array_push($notifications, $notification);
		}
	
		return $notifications;
	
	}

	public static function last($uid, $notifications_last_check, $limit) {
		return self::all($uid, $limit);	
	}

	public static function getUnread($uid, $notifications_last_check) {
		$notifications = self::all($uid, $notifications_last_check);
        
        $count = 0;
        foreach($notifications as $notification) {
            if ($notification->timestamp > (int)$notifications_last_check)
                $count++;
        }
        return $count;
	}
    
    public static function getLastCheck($uid) {
        $query = db_select("fanta_notifications_checks", "c");
        $query->condition("uid", $uid);
        $query->addField("c", "timestamp", "timestamp");
        
        $result = $query->execute();
        
        return $result->fetchField();
    }
    
    public static function updateLastCheck($uid, $timestamp) {
        db_delete("fanta_notifications_checks")->condition("uid", $uid)->execute();
        
        db_insert("fanta_notifications_checks")->fields(array("uid" => $uid, "timestamp" => $timestamp))->execute();
    }
    
	public static function create($text, $link, $uid = -1) {
		db_insert("fanta_notifications")->fields(array("text" => $text, "uid" => $uid, "link" => $link, "timestamp" => time()))->execute();
		watchdog('fantacalcio', "Notifica inserita: @text, user: @user", array("@text" => $text, "@user" => ($uid != null ? $uid : "-")), WATCHDOG_NOTICE );
	}

}
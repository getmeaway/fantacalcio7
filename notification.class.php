<?php 
	
class Notification {
	
	var $id;
	var $text;
	var $uid;
	var $timestamp;
	var $unread;
	
	function __construct() {
	}
	
	public static function all($limit = null) {
		global $user;
		$notifications = array();
	
		$query = db_select("fanta_notifications", "n");
		$query->condition($db_or);
		$query->fields("n");
		$query->orderBy("n.timestamp", "DESC");
	
		if ($limit != null)
			$query->range(0, $limit);
	
		$result = $query->execute();
	
		foreach($result as $row) {
			$notification = new Notification();
			$notification->id = $row->n_id;
			$notification->text = $row->text;
			$notification->timestamp = $row->timestamp;
			$notification->unread = $row->timestamp > $user->last_notifications_check;
			array_push($notifications, $notification);
		}
	
		return $notifications;
	
	}

	public static function last($limit) {
		return self::all($limit);
	
	}

	public static function create($text, $uid = null) {
		db_insert("fanta_notifications")->fields(array("text" => $text, "uid" => $uid, "timestamp" => time()))->execute();
		watchdog('fantacalcio', "Notifica inserita: @text, user: @user", array("@text" => $text, "@user" => ($uid != null ? $uid : "-")), WATCHDOG_NOTICE );
		watchdog('fantacalcio', '@team: formazione importata', array(
    '@team' => $team->name), WATCHDOG_NOTICE);

	}

}
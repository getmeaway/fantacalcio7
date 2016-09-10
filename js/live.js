jQuery(document).ready(function() {

	//icone
	var bonusMalus = {"in": {icon: "http://www.fantagazzetta.com/img/live_ico/entrato_s.png", points: 0},
						"out": {icon: "http://www.fantagazzetta.com/img/live_ico/uscito_s.png", points: 0},
						"yellow_cards": {icon: "http://www.fantagazzetta.com/img/live_ico/amm_s.png", points: -0.5},
						"red_cards": {icon: "http://www.fantagazzetta.com/img/live_ico/esp_s.png", points: -1},
						"assists": {icon: "http://www.fantagazzetta.com/img/live_ico/assist_s.png", points: 1},
						"own_goals": {icon: "http://www.fantagazzetta.com/img/live_ico/autogol_s.png", points: -2},
						"missed_penalties": {icon: "http://www.fantagazzetta.com/img/live_ico/rigoresbagliato_s.png", points: -3},
						"saved_penalties": {icon: "http://www.fantagazzetta.com/img/live_ico/rigoreparato_s.png", points: 3},
						"goals_for": {icon: "http://www.fantagazzetta.com/img/live_ico/golfatto_s.png", points: 3},
						"goals_against": {icon: "http://www.fantagazzetta.com/img/live_ico/golsubito_s.png", points: -1},
						"penalty_goals": {icon: "http://www.fantagazzetta.com/img/live_ico/rigoresegnato_s.png", points: 3},
						"win_goal": {icon: "http://www.fantagazzetta.com/img/live_ico/golvittoria_s.png", points: 0},
						"draw_goal": {icon: "http://www.fantagazzetta.com/img/live_ico/golpareggio_s.png", points: 0}};
	
	setInterval(function () {
		jQuery.ajax({
			url: Drupal.settings.basePath + "live/get",
			dataType: "json",
			type: "GET",
			success: function (data) {
					updateVotes(data.votes);
				},
			error: function(a, b, c) {
				console.log(a);
			}
		});
	
	}, 5000);
	
	function updateVotes(votes) {
		jQuery("td.name").each(function() {
			var name = jQuery(this).html().toLowerCase();
			var vote = votes[name];
			if (vote != undefined) {
				var totalPoints = 0;
				var outEvents = "";
				
				if (vote.events != undefined) {
					//for(eventType in vote.events) {
						//for(event in vote.events[eventType]) {
						for(var i = 0; i < vote.events.length; i++) {
                            var eventType = vote.events[i];
                            outEvents += "<img src='" + bonusMalus[eventType].icon + "'> ";// + vote.events[eventType][event] + "'&nbsp;&nbsp;";
							totalPoints += bonusMalus[eventType].points;
						}
					//}
					
					totalPoints = totalPoints > 0 ? "+" + totalPoints : (totalPoints < 0 ? totalPoints  : "");
			
				}
				
				jQuery(this).parent().find("td.team").remove() //rimuovo squadra per questioni di spazio
				jQuery(this).parent().find("td.live").attr("colspan", 2).html("<strong>" + vote.vote + " " + totalPoints + "</strong>&nbsp;<small>" + outEvents + "</small>");
			}
		});
	}

	window.live = function(data) {
		updateVotes(data.votes);
	}
});
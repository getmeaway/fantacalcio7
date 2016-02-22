jQuery(document).ready(function() {

	//icone
	var bonusMalus = {"in": {icon: "http://www.fantaeuropeo.com/leghe/icons/entrato.png", points: 0},
						"out": {icon: "http://www.fantaeuropeo.com/leghe/icons/uscito.png", points: 0},
						"yellow_cards": {icon: "http://www.fantaeuropeo.com/leghe/icons/amm.png", points: -0.5},
						"red_cards": {icon: "http://www.fantaeuropeo.com/leghe/icons/esp.png", points: -1},
						"assists": {icon: "http://www.fantaeuropeo.com/leghe/icons/assist.png", points: 1},
						"own_goals": {icon: "http://www.fantaeuropeo.com/leghe/icons/autogol.png", points: -2},
						"missed_penalties": {icon: "http://www.fantaeuropeo.com/leghe/icons/rigoresbagliato.png", points: -3},
						"saved_penalties": {icon: "http://www.fantaeuropeo.com/leghe/icons/rigoreparato.png", points: 3},
						"goals_for": {icon: "http://www.fantaeuropeo.com/leghe/icons/golfatto.png", points: 3},
						"goals_against": {icon: "http://www.fantaeuropeo.com/leghe/icons/golsubito.png", points: -1},
						"penalty_goals": {icon: "http://www.fantaeuropeo.com/leghe/icons/rigoresegnato.png", points: 3},
						"win_goal": {icon: "http://www.fantaeuropeo.com/leghe/icons/golvittoria.png", points: 0},
						"draw_goal": {icon: "http://www.fantaeuropeo.com/leghe/icons/golpareggio.png", points: 0}};
	
	setInterval(function () {
		jQuery.ajax({
			url: "http://fantacalciocircus.altervista.org/live/live.json?t=" + Date.now() + "&callback=live",
			dataType: "jsonp",
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
					for(eventType in vote.events) {
						for(event in vote.events[eventType]) {
							outEvents += "<img src='" + bonusMalus[eventType].icon + "'> " + vote.events[eventType][event] + "'&nbsp;&nbsp;";
							totalPoints += bonusMalus[eventType].points;
						}
					}
					
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
//(function (jQuery) {
var roles = ["P", "D", "C", "A"];
	
	//compra
	function buyPlayer(pl_id) {

		if(!checkBuy(pl_id))
			return;
		
		//aggiorno rosa
		jQuery("#my-squad tbody .empty.message").hide();
		
		var tr = jQuery("#players-list tbody tr#pl-" + pl_id);
		var player = {name: jQuery(tr).attr("data-name"), team: jQuery(tr).attr("data-team"), role: jQuery(tr).attr("data-role"), quotation: jQuery(tr).attr("data-quotation")};
		var role_class = "role_" + player.role;
				
		//creo riga da inserire nella tabella #my-squad
		var tr = "<tr id=\"squad-pl-" + pl_id + "\" class=\"player " + role_class + " \" data-name=\"" + player.name + "\" data-team=\"" + player.team + "\" data-role=\"" + player.role + "\" data-quotation=\"" + player.quotation + "\">"
					+ "<td><span class=\"fa-stack\">"
					+ "<i class=\"fa fa-square fa-stack-2x " + role_class + "\"></i>"
					+ "<i class=\"fa fa-stack-1x\" style=\"color: white;\"><span class=\"font-normal\">" + roles[player.role] + "</span></i>"
					+ "</span></td>"
					+ "<td>" + player.name + "</td>" 
					+ "<td>" + player.team + "</td>" 
					+ "<td>" + player.quotation + "</td>"
					+ "<td>" + player.quotation + "</td>"
					+ "<td><a href=\"#\" data-toggle=\"modal\" data-target=\"#player-stats-modal\" class=\"player-stats\" id=\"player-stat-" + pl_id + "\"><i class=\"fa fa-bar-chart\"></i></a></td>"
					+ "<td><button class=\"btn btn-sm btn-warning sell-player\" onclick=\"sellPlayer('" + pl_id + "');\" id=\"sell-" + pl_id + "\">Vendi</button></td>"
					+ "</tr>";
		
		//inserisco la riga nella tabella 
		//TODO trovare punto in cui inserire (ordine per ruolo, nome)
		jQuery("#my-squad tbody").prepend(jQuery(tr));
		
		//nascondo pulsante COMPRA
		jQuery("#players-list tbody tr#pl-" + pl_id).find(".buy-player").hide();
		//mostro spunta per giocatore comprato
		jQuery("#players-list tbody tr#pl-" + pl_id).find(".player-bought").removeClass("hidden");
				
		//update counters
		//incremento progress bar ruolo
		var numberForRole = parseInt(jQuery("#progress-role-" + player.role).attr("aria-valuenow"));
		var maxNumberForRole = parseInt(jQuery("#progress-role-" + player.role).attr("aria-valuemax"));
		jQuery("#progress-role-" + player.role).attr("aria-valuenow", numberForRole + 1);
		jQuery("#progress-role-" + player.role).css("width", ((numberForRole + 1) / maxNumberForRole * 100) + "%" );
		jQuery("#progress-role-" + player.role + " span.value").html((numberForRole + 1) );
		jQuery("#progress-role-" + player.role + " span.sr-only").html((numberForRole + 1));
		
		//aggiorno crediti rimasti
		var credits = parseInt(jQuery("#progress-credits").attr("aria-valuenow"));
		var maxCredits = parseInt(jQuery("#progress-credits").attr("aria-valuemax"));
		jQuery("#progress-credits").attr("aria-valuenow", credits - player.quotation);
		jQuery("#progress-credits").css("width", ((credits - player.quotation) / maxCredits * 100) + "%" );
		jQuery("#progress-credits span.value").html((credits - player.quotation));
		jQuery("#progress-credits span.sr-only").html((credits - player.quotation));
		
		checkComplete();
		
	}
	
	//vendi
	function sellPlayer(pl_id) {
		
		if(!checkSell(pl_id))
			return;
	
		var tr = jQuery("#my-squad tbody tr#squad-pl-" + pl_id);
		var player = {name: jQuery(tr).attr("data-name"), team: jQuery(tr).attr("data-team"), role: jQuery(tr).attr("data-role"), quotation: jQuery(tr).attr("data-quotation")};

		//aggiorno rosa
		jQuery(tr).remove();
		
		//msotro pulsante COMPRA
		jQuery("#players-list tbody tr#pl-" + pl_id).find(".buy-player").show();
		//nascondo spunta per giocatore comprato
		jQuery("#players-list tbody tr#pl-" + pl_id).find(".player-bought").addClass("hidden");
		
		
		if(jQuery("#my-squad tbody tr.player").length == 0) //nessun giocatore in rosa
			jQuery("#my-squad tbody tr.empty.message").show();
					
		//update counters
		//incremento progress bar ruolo
		var numberForRole = parseInt(jQuery("#progress-role-" + player.role).attr("aria-valuenow"));
		var maxNumberForRole = parseInt(jQuery("#progress-role-" + player.role).attr("aria-valuemax"));
		jQuery("#progress-role-" + player.role).attr("aria-valuenow", numberForRole - 1);
		jQuery("#progress-role-" + player.role).css("width", ((numberForRole - 1) / maxNumberForRole * 100) + "%" );
		jQuery("#progress-role-" + player.role + " span.value").html((numberForRole - 1));
		jQuery("#progress-role-" + player.role + " span.sr-only").html((numberForRole - 1));
		
		//aggiorno crediti rimasti
		var credits = parseInt(jQuery("#progress-credits").attr("aria-valuenow"));
		var maxCredits = parseInt(jQuery("#progress-credits").attr("aria-valuemax"));
		jQuery("#progress-credits").attr("aria-valuenow", credits + parseInt(player.quotation));
		jQuery("#progress-credits").css("width", ((credits + parseInt(player.quotation)) / maxCredits * 100) + "%" );
		jQuery("#progress-credits span.value").html((credits + parseInt(player.quotation)));
		jQuery("#progress-credits span.sr-only").html((credits + parseInt(player.quotation)));
				
	}
		
	function checkBuy(pl_id) {
		return true;
	}

	function checkSell(pl_id) {
		return true;
	}
	
	function checkComplete() {
		
		var credits = parseInt(jQuery("#progress-credits span.value").html());
		var numberForRoles_0 = parseInt(jQuery("#progress-role-0 span.value").html());
		var numberForRoles_1 = parseInt(jQuery("#progress-role-1 span.value").html());
		var numberForRoles_2 = parseInt(jQuery("#progress-role-2 span.value").html());
		var numberForRoles_3 = parseInt(jQuery("#progress-role-3 span.value").html());
		
		var maxNumberForRoles_0 = parseInt(jQuery("#progress-role-0 span.max").html());
		var maxNumberForRoles_1 = parseInt(jQuery("#progress-role-1 span.max").html());
		var maxNumberForRoles_2 = parseInt(jQuery("#progress-role-2 span.max").html());
		var maxNumberForRoles_3 = parseInt(jQuery("#progress-role-3 span.max").html());

		if(credits >= 0 
				&& numberForRoles_0 == maxNumberForRoles_0 
				&& numberForRoles_1 == maxNumberForRoles_1 
				&& numberForRoles_2 == maxNumberForRoles_2 
				&& numberForRoles_3 == maxNumberForRoles_3) {
			//squadra completa
			showModalSquadComplete();
		}
	}
	
function showModalSquadComplete() {
	
	//id dei giocatori comprati
	var pl_ids = [];
	
	jQuery("#my-squad tbody tr.player").each(function() {
		console.log(jQuery(this).attr("id"))
		var pl_id = jQuery(this).attr("id").substring(9);
		pl_ids.push(pl_id);
	})
	
	jQuery("#squad-confirm-pl-ids").val(JSON.stringify(pl_ids));
		
	jQuery("#squad-complete-modal").modal("show");
}
//(function (jQuery) {
var roles = ["P", "D", "C", "A"];

//compra
function buyPlayer(pl_id, t_id) {
    console.log(pl_id)

    jQuery.ajax({
        type: "POST"
        , url: Drupal.settings.basePath + "mie/" + t_id + "/movimenti/buy/" + pl_id
        , //data: {id: pl_id},			
        success: function (data) {

            //acquisto non andato a buon fine
            if (!data.success) {
                showModalError(data.error);
            }
            else {
                //aggiorno rosa
                jQuery("#my-squad tbody .empty.message").hide();
                jQuery("#my-squad tbody").empty();

                //render rosa
                for(var i in data.squad_data) {
                    var player = data.squad_data[i];
                    
                    //creo riga da inserire nella tabella #my-squad
                    var tr = "<tr id=\"squad-pl-" + player.pl_id + "\" class=\"player squad-player-role-" + player.role + " \" data-name=\"" + player.name + "\" data-team=\"" + player.team + "\" data-role=\"" + player.role + "\" data-quotation=\"" + player.quotation + "\">"
                        + "<td><span class=\"fa-stack\">"
                        + "<i class=\"fa fa-square fa-stack-2x squad-player-role-" + player.role + "\"></i>"
                        + "<i class=\"fa fa-stack-1x\" style=\"color: white;\"><span class=\"font-normal\">" + roles[player.role] + "</span></i>"
                        + "</span></td>"
                        + "<td>" + player.name + "</td>" 
                        + "<td>" + player.team + "</td>" 
                        + "<td>" + player.quotation + "</td>"
                        + "<td>" + player.quotation + "</td>"
                        + "<td><a href=\"#\" data-toggle=\"modal\" data-target=\"#player-stats-modal\" class=\"player-stats\" id=\"player-stat-" + player.pl_id + "\"><i class=\"fa fa-bar-chart\"></i></a></td>"
                        + "<td><button class=\"btn btn-sm btn-warning sell-player\" onclick=\"sellPlayer(" + player.pl_id + ", " + t_id + ");\" id=\"sell-" + player.pl_id + "\">Vendi</button></td>"
                        + "</tr>";
                    
                    jQuery("#my-squad tbody").append(tr);
                    
                    //nascondo pulsante COMPRA                
                    jQuery("#players-list tbody tr#pl-" + player.pl_id).find(".buy-player").addClass("hidden");
                    //mostro spunta per giocatore comprato
                    jQuery("#players-list tbody tr#pl-" + player.pl_id).find(".player-bought").removeClass("hidden");

                }

                //update counters
                //incremento progress bar ruolo
                for(var i = 0; i < roles.length; i++) {
                    jQuery("#progress-role-" + i).attr("aria-valuenow", data.bought_players[i]);
                    jQuery("#progress-role-" + i).css("width", (data.bought_players[i] / data.expected_players[i] * 100) + "%");
                    jQuery("#progress-role-" + i + " span.value").html(data.bought_players[i]);
                    jQuery("#progress-role-" + i + " span.sr-only").html(data.bought_players[i]);
                }

                //aggiorno crediti rimasti
                jQuery("#progress-credits").attr("aria-valuenow", data.credits);
                jQuery("#progress-credits").css("width", ((data.credits) / data.max_credits * 100) + "%");
                jQuery("#progress-credits span.value").html((data.credits));
                jQuery("#progress-credits span.sr-only").html((data.credits));
                
                if (data.is_team_complete)
                    jQuery("#squad-complete-modal").modal("show");
            }
        }
    });
}

//vendi
function sellPlayer(pl_id, t_id) {

    jQuery.ajax({
        type: "POST"
        , url: Drupal.settings.basePath + "mie/" + t_id + "/movimenti/sell/" + pl_id
        , //data: {id: pl_id},			
        success: function (data) {
            
            if (!data.success) {
                showModalError(data.error);
            }
            else {
                
                if (jQuery("#my-squad tbody tr.player").length == 0) //nessun giocatore in rosa
                    jQuery("#my-squad tbody tr.empty.message").show();
                else {
                    
                    jQuery("#my-squad tbody").empty();

                    //render rosa
                    for(var i in data.squad_data) {
                        var player = data.squad_data[i];

                        //creo riga da inserire nella tabella #my-squad
                        var tr = "<tr id=\"squad-pl-" + player.pl_id + "\" class=\"player squad-player-role-" + player.role + " \" data-name=\"" + player.name + "\" data-team=\"" + player.team + "\" data-role=\"" + player.role + "\" data-quotation=\"" + player.quotation + "\">"
                            + "<td><span class=\"fa-stack\">"
                            + "<i class=\"fa fa-square fa-stack-2x squad-player-role-" + player.role + "\"></i>"
                            + "<i class=\"fa fa-stack-1x\" style=\"color: white;\"><span class=\"font-normal\">" + roles[player.role] + "</span></i>"
                            + "</span></td>"
                            + "<td>" + player.name + "</td>" 
                            + "<td>" + player.team + "</td>" 
                            + "<td>" + player.quotation + "</td>"
                            + "<td>" + player.quotation + "</td>"
                            + "<td><a href=\"#\" data-toggle=\"modal\" data-target=\"#player-stats-modal\" class=\"player-stats\" id=\"player-stat-" + player.pl_id + "\"><i class=\"fa fa-bar-chart\"></i></a></td>"
                            + "<td><button class=\"btn btn-sm btn-warning sell-player\" onclick=\"sellPlayer(" + player.pl_id + ", " + t_id + ");\" id=\"sell-" + player.pl_id + "\">Vendi</button></td>"
                            + "</tr>";

                        jQuery("#my-squad tbody").append(tr);

                        //mostro pulsante COMPRA
                        jQuery("#players-list tbody tr#pl-" + pl_id).find(".buy-player").removeClass("hidden");
                        //nascondo spunta per giocatore comprato
                        jQuery("#players-list tbody tr#pl-" + pl_id).find(".player-bought").addClass("hidden");

                    }

                }

                //update counters
                //incremento progress bar ruolo
                for(var i = 0; i < roles.length; i++) {
                    jQuery("#progress-role-" + i).attr("aria-valuenow", data.bought_players[i]);
                    jQuery("#progress-role-" + i).css("width", (data.bought_players[i] / data.expected_players[i] * 100) + "%");
                    jQuery("#progress-role-" + i + " span.value").html(data.bought_players[i]);
                    jQuery("#progress-role-" + i + " span.sr-only").html(data.bought_players[i]);
                }

                //aggiorno crediti rimasti
                jQuery("#progress-credits").attr("aria-valuenow", data.credits);
                jQuery("#progress-credits").css("width", ((data.credits) / data.max_credits * 100) + "%");
                jQuery("#progress-credits span.value").html((data.credits));
                jQuery("#progress-credits span.sr-only").html((data.credits));
            }
        }
    });
}

function showModalSquadComplete() {

    jQuery("#squad-complete-modal").modal("show");
}

function showModalError(message) {
    jQuery("#error-modal .modal-body").html(message)
    jQuery("#error-modal").modal("show")
}
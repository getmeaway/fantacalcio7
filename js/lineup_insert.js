var _line_up = new Array();
var _positions;
var _old_json_line_up;

jQuery(document).ready(function() {
  var json_line_up = jQuery("input[name='lineup']").val();
  _old_json_line_up = jQuery("input[name='lineup']").val();
  _line_up = eval('(' + json_line_up + ')');
});

function confirm_reset_formazione(){
	jQuery(window).unbind();
	if (confirm("Sei sicuro di voler cancellare la formazione inserita?")) {
			return (true);
	}
	return (false);
}

function confirm_conferma_formazione(){
	if (confirm("Vuoi confermare la formazione?")) {
			return (true);
	}
	return (false);
}

//mobile functions
function changePosition(pl_id, t_id, c_id, role, position) {
	var blockPosition = (position == 2) ? 1 : 0;
	var oldPosition = (position + 3) % 4;
	var positionNames = new Array("Tribuna", "Titolare", "Riserva 1", "Riserva 2");

	jQuery("#" + pl_id + '_' + position).show();
	//jQuery("#" + pl_id + '_' + oldPosition).children().html(positionNames[oldPosition]);
	jQuery("#" + pl_id + '_' + oldPosition).hide();		

	//update arrays
	_line_up[pl_id].position = position;

	jQuery("#edit-line-up").val("" + JSON.stringify(_line_up));

	var checks = checkLineUp(_line_up);

	if (checks[0] && checks[1] && checks[2] && checks[3]) {
		jQuery("#line_up_submit").button('enable');
		jQuery(window).unbind();
	}
	else
		jQuery("#line_up_submit").button('disable');

	if(jQuery("input[name='lineup']").val() == '' || jQuery("input[name='lineup']").val() == _old_json_line_up)
		jQuery(window).unbind();
	else {
		jQuery(window).bind("beforeunload", function() {
			return "Sicuro di voler uscire dalla pagina?";
		});

		jQuery("#line_up_submit").click(function() {
			jQuery(window).unbind();
		});

		jQuery("#edit-clear").click(function() {
			jQuery(window).unbind();
		});
	}
}
//END - mobile functions

//web functions 
jQuery(function() {
    jQuery( "#lineup-squad tbody, #lineup-regulars tbody, #lineup-reserves tbody" ).sortable({
      connectWith: ".lineup-group tbody",
      over: function(event, ui) {
    	  jQuery(event.target).parent().parent().addClass("dropping-group-choosed");
      },
      out: function(event, ui) {
    	  jQuery(event.target).parent().parent().removeClass("dropping-group-choosed");
      },
      receive: function(event, ui) {
    	  jQuery(event.target).parent().parent().addClass("dropping-group").delay(500).removeClass("dropping-group");
      },
      sort: function(event, ui) {
//    	  jQuery(ui.item).addClass("dragging-item");
//    	  jQuery(ui.helper).css("min-height", "30px").addClass("dropping-group");
//    	  jQuery("#lineup-squad, #lineup-regulars, #lineup-reserves" ).css("min-height", "30px").addClass("dropping-group");
    	  jQuery(".lineup-group-container").addClass("dropping-group");
    	  },
      stop: function(event, ui) { 
    	  jQuery(ui.item).removeClass("dragging-item");
    	  jQuery(".lineup-group-container").removeClass("dropping-group");
    	  var pl_id = jQuery(ui.item).attr('data-id');
    	  var role = jQuery(ui.item).attr("data-role");

			var position = jQuery(ui.item).parent().parent().attr('data-position');
			var rows = jQuery("[data-position=" + position + "]").find('tbody tr');console.log(rows);
			var j = 0;
			for (var i = 0; i < rows.length; i++) {
				if (jQuery(rows[i]).attr("data-role") == role) {
					j++;
					if (jQuery(rows[i]).attr("data-id") == pl_id) {
						var blockPosition = j;
					}
				}
			}
			
			
			var blocksNumber = j;
			console.log("block number: " + blocksNumber);
			//position = parseInt(position.substring(4));

			if (position > 1)
				position = parseInt(position) + parseInt(blockPosition) - 1;

			console.log("position: " + position);
			//update arrays
			
			//update position
			var oldPosition = _line_up[pl_id].position;
			_line_up[pl_id].position = position;

			//scalo le riserve
			if (oldPosition > 1) { //diminuisco la posizione delle riserve successive (solo stesso ruolo)
				for(var i = 0; i < Object.keys(_line_up).length; i++) {
					var key = Object.keys(_line_up)[i];
					if (key != pl_id && _line_up[key] != undefined) {
						if (_line_up[key].role == _line_up[pl_id].role) {
							if (_line_up[key].position >= oldPosition){
								_line_up[key].position = parseInt(_line_up[key].position) - 1;
							}
						}
					}
				}
			}

			//nuova posizione < 2 ---> diminuisco i successivi
			if (position > 1) { //aumento la posizione delle riserve successive (solo stesso ruolo)
				for(var j = 0; j < Object.keys(_line_up).length; j++) {
					var key = Object.keys(_line_up)[j];
					if (key != pl_id && _line_up[key] != undefined) {
						if (_line_up[key].role == _line_up[pl_id].role) {
							if (_line_up[key].position >= position){
								_line_up[key].position = parseInt(_line_up[key].position) + 1;
							}
						}
					}
				}
			}

			jQuery("input[name='lineup']").val("" + JSON.stringify(_line_up));

			//jQuery("#show-line-up").val(showObject(_line_up, pl_id));

			var checks = checkLineUp(_line_up);

			if (checks[0] && checks[1] && checks[2] && checks[3]) {
				jQuery("#line_up_submit").removeAttr("disabled").css("opacity", "1");
				jQuery("#lineup-form-modal").modal();
			}
			else
				jQuery("#line_up_submit").attr("disabled", "disabled").css("opacity", "0.5");

			if(jQuery("input[name='lineup']").val() == '' || jQuery("input[name='lineup']").val() == _old_json_line_up)
				jQuery(window).unbind();
			else {
				jQuery(window).bind("beforeunload", function() {
					return "Sicuro di voler uscire dalla pagina?";
				});

				jQuery("#line_up_submit").click(function() {
					jQuery(window).unbind();
				});

				jQuery("#edit-clear").click(function() {
					jQuery(window).unbind();
				});

			}
			
			jQuery(ui.item).removeClass("dragging-item")
	    	  jQuery( "#lineup-squad , #lineup-regulars , #lineup-reserves " ).removeClass("dropping-group");
			
			if (jQuery( "#lineup-squad tbody tr").not(".empty-row").length == 0)
				jQuery( "#lineup-squad tbody tr.empty-row").show();
			else 
				jQuery( "#lineup-squad tbody tr.empty-row").hide();
			
			if (jQuery( "#lineup-regulars tbody tr").not(".empty-row").length == 0)
				jQuery( "#lineup-regulars tbody tr.empty-row").show();
			else 
				jQuery( "#lineup-regulars tbody tr.empty-row").hide();
			
			if (jQuery( "#lineup-reserves tbody tr").not(".empty-row").length == 0)
				jQuery( "#lineup-reserves tbody tr.empty-row").show();
			else 
				jQuery( "#lineup-reserves tbody tr.empty-row").hide();
    	  },
    }).disableSelection();
  });


function inArray(needle, haystack) {
    for(var i = 0; i < haystack.length; i++) {
	    if(haystack[i] === needle) return true;
    }
    return false;
}

function inArrayTwoLevels(needle, haystack) {
    for(var i = 0; i < haystack.length; i++) {
		var check = true;
		if (haystack[i].length == needle.length) {
			for(var j = 0; j < haystack[i].length; j++) {
				if(haystack[i][j] != needle[j]) 
					check = false;
			}
			if (check)
				return true;
		}
    }
    return false;
}

//check lineup
function checkLineUp(line_up){
	//massima posizione
	var maxPosition = 0;
	for(var i = 0; i < Object.keys(line_up).length; i++){
		var key = Object.keys(line_up)[i];
		if (line_up[key].position > maxPosition)
			maxPosition = line_up[key].position;
	}

	//inizializzo gli array
	var positions = new Array();
	for(var j = 0; j <= maxPosition; j++){
		positions[j] = new Array(0, 0, 0, 0);
	}

	//conto gli elementi (per posizione e per ruolo)
	
	for(var k = 0; k < Object.keys(line_up).length; k++){
		var key = Object.keys(line_up)[k];
		var currPosition = parseInt(line_up[key].position);
		var currRole = parseInt(line_up[key].role);
		
		positions[currPosition][currRole]++;
	}

	//moduli consentiti e numero giocatori
	var check_regulars_number = false; 
	var check_regulars_module = false; 
	var check_reserves_number = false; 
	var check_reserves_module = false;
	
	var regulars_modules = new Array(new Array(1, 3, 4, 3), new Array(1, 3, 5, 2), new Array(1, 4, 3, 3), new Array(1, 4, 4, 2), new Array(1, 4, 5, 1), new Array(1, 5, 3, 2), new Array(1, 5, 4, 1), new Array(1, 6, 3, 1));
	var reserves_modules = new Array(new Array(1, 2, 2, 2));
	//var modules_3 = new Array(new Array(0, 1, 1, 1));

	//verifico titolari
	var regulars_module = new Array(0, 0, 0, 0);
	if (positions[1] != undefined ) {
		//numero titolari
		var regulars_number = positions[1][0] + positions[1][1] + positions[1][2] + positions[1][3]; 
		if(regulars_number == 11)
			check_regulars_number = true;
		else 
			check_regulars_number = false;
			
		//modulo titolari
		regulars_module = new Array(positions[1][0], positions[1][1], positions[1][2], positions[1][3]);
		if(inArrayTwoLevels(regulars_module, regulars_modules)) 
			check_regulars_module = true;
		else 
			check_regulars_module = false;

	}

	//verifico riserve
	//if (maxPosition > 1 ) {
	
		//numero riserve
		var number_reserves = 0;
		var reserves_role_0 = 0;
		var reserves_role_1 = 0;
		var reserves_role_2 = 0;
		var reserves_role_3 = 0;
		for(var i = 2; i <= maxPosition; i++ ) {
			number_reserves += positions[i][0] + positions[i][1] + positions[i][2] + positions[i][3];
			reserves_role_0 += positions[i][0];
			reserves_role_1 += positions[i][1];
			reserves_role_2 += positions[i][2];
			reserves_role_3 += positions[i][3];
		}
	
		if(number_reserves == 7)
			check_reserves_number = true;
		else 
			check_reserves_number = false;

		//modulo riserve
		var reserves_module = new Array(reserves_role_0, reserves_role_1, reserves_role_2, reserves_role_3);
		//var module_3 = new Array(positions[3][0], positions[3][1],positions[3][2], positions[3][3]);
		if(inArrayTwoLevels(reserves_module, reserves_modules) ) 
			check_reserves_module = true;
		else 
			check_reserves_module = false;
	//}
	
	for(var i = 2; i <= maxPosition; i++ ) {
	
	}
	
	//update images	
	jQuery("#regulars_number_value").html(regulars_number);
	jQuery("#regulars_module_value").html(regulars_module.join(" - "));
	jQuery("#reserves_number_value").html(number_reserves);
	jQuery("#reserves_module_value").html(reserves_module.join(" - "));
	
	jQuery("#regulars_number i").removeClass("fa-check-circle").removeClass("fa-minus-circle").addClass(check_regulars_number == true ? "fa-check-circle" : "fa-minus-circle");
	jQuery("#regulars_module i").removeClass("fa-check-circle").removeClass("fa-minus-circle").addClass(check_regulars_module == true ? "fa-check-circle" : "fa-minus-circle");
	jQuery("#reserves_number i").removeClass("fa-check-circle").removeClass("fa-minus-circle").addClass(check_reserves_number == true ? "fa-check-circle" : "fa-minus-circle");
	jQuery("#reserves_module i").removeClass("fa-check-circle").removeClass("fa-minus-circle").addClass(check_reserves_module == true ? "fa-check-circle" : "fa-minus-circle");
	jQuery("#regulars_number i").removeClass("text-success").removeClass("text-danger").addClass(check_regulars_number == true ? "text-success" : "text-danger");
	jQuery("#regulars_module i").removeClass("text-success").removeClass("text-danger").addClass(check_regulars_module == true ? "text-success" : "text-danger");
	jQuery("#reserves_number i").removeClass("text-success").removeClass("text-danger").addClass(check_reserves_number == true ? "text-success" : "text-danger");
	jQuery("#reserves_module i").removeClass("text-success").removeClass("text-danger").addClass(check_reserves_module == true ? "text-success" : "text-danger");
	
	//risultato
	console.log(check_regulars_number, check_regulars_module, check_reserves_number, check_reserves_module);
	
	return new Array(check_regulars_number, check_regulars_module, check_reserves_number, check_reserves_module);
}

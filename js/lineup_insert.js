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
	//$("#" + pl_id + '_' + oldPosition).children().html(positionNames[oldPosition]);
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

			//$("#show-line-up").val(showObject(_line_up, pl_id));

			var checks = [true, true, true, true];//checkLineUp(_line_up);

			if (checks[0] && checks[1] && checks[2] && checks[3])
				jQuery("#line_up_submit").removeAttr("disabled").css("opacity", "1");
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
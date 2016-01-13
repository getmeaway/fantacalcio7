var _penalties_order = new Array();
var _old_json_penalties_order;

jQuery(document).ready(function() {
	var json_penalties_order = jQuery("#penalties_order").val();
	_old_json_penalties_order = jQuery("#penalties_order").val();
	_penalties_order = eval('(' + json_penalties_order + ')');
	
	console.log(_penalties_order);
    
    // attach event
    jQuery("#penalties_order_table tbody").sortable({
		over : function(event, ui) {
			jQuery(event.target).parent().parent().addClass(
					"dropping-group-choosed");
		},
		out : function(event, ui) {
			jQuery(event.target).parent().parent().removeClass(
					"dropping-group-choosed");
		},
		sort : function(event, ui) {
			jQuery(".lineup-group-container").addClass(
					"dropping-group");
		},
		stop : function(event, ui) {
			jQuery(ui.item).removeClass("dragging-item");
			jQuery(".lineup-group-container").removeClass(
					"dropping-group");
			var pl_id = jQuery(ui.item).attr('data-id');

			var position = jQuery(ui.item).attr('data-order');
			
			var rows = jQuery(ui.item).parent().parent().find('tbody tr');
			console.log(pl_id + ": " + _penalties_order[pl_id]);
			var j = 0;
			for ( var i = 0; i < rows.length; i++) {
				if (jQuery(rows[i]).attr("data-id") == pl_id) {
					var position = j + 1;
				}
				j++;
			}
		
			// update position
			var oldPosition = _penalties_order[pl_id];
			_penalties_order[pl_id] = position;

			// diminuisco la posizione delle riserve successive
			for ( var i = 0; i < Object.keys(_penalties_order).length; i++) {
				var key = Object.keys(_penalties_order)[i];
				if (key != pl_id && _penalties_order[key] != undefined) {
					if (_penalties_order[key] >= oldPosition) {
						_penalties_order[key] = parseInt(_penalties_order[key]) - 1;
					}					
				}
			}

			// aumento la posizione delle riserve successive 
			for ( var j = 0; j < Object.keys(_penalties_order).length; j++) {
				var key = Object.keys(_penalties_order)[j];
				if (key != pl_id && _penalties_order[key] != undefined) {
					if (_penalties_order[key] >= position) {
						_penalties_order[key] = parseInt(_penalties_order[key]) + 1;
					}
				}
			}
			
			console.log(pl_id + ": " + _penalties_order[pl_id]);
			console.log(_penalties_order);

			jQuery("#penalties_order").val(
					"" + JSON.stringify(_penalties_order));
			
		},
	});

});

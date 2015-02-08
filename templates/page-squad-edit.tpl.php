<div class="row">

	<div class='container'>
		<div class='row'>
			<div class='col-xs-12 col-lg-12'>
				<ul class="nav nav-pills">
					<li role="presentation">
				  		<a href="<?php print base_path();?>mie/<?php print $t_id; ?>/dati"><?php print t("Dati"); ?></a>
				  	</li>
					<li role="presentation">
						<a href="<?php print base_path();?>mie/<?php print $t_id; ?>/rosa"><?php print t("Rosa"); ?></a>
					</li>
					<?php if (variable_get("fantacalcio_league_type", 1) == LEAGUE_TYPE_GP): ?>
				  	<li role="presentation" class="active">
				  		<a href="<?php print base_path();?>mie/<?php print $t_id; ?>/movimenti"><?php print t("Movimenti"); ?></a>
				  	</li>
				  	<?php endif; ?>
				  	<li role="presentation">
				  		<a href="<?php print base_path();?>mie/<?php print $t_id; ?>/lista-movimenti"><?php print t("Lista movimenti"); ?></a>
				  	</li>
				</ul>
			</div>
		</div>
		<div class='row'>
			<div class='col-md-6 col-lg-6'>&nbsp;</div>
		</div>
		<div class='row'>
			<div id='-squad-data' class='col-md-6 col-lg-6'>
				<div class='row'>
					<div class='col-md-12'><?php print render($squad_data); ?></div>
				</div>
			</div>
			<div class='col-md-6 col-lg-6'>
				<div class='well'>
				<div class='row'>
					<div class='col-xs-12'>
						<label><?php print t("Crediti"); ?></label>
						<div class="progress">
						  <div class="progress-bar" role="progressbar" aria-valuenow="<?php print $outflow; ?>" aria-valuemin="0" 
						  	aria-valuemax="<?php print $credits; ?>" style="width: <?php print $outflow / $credits * 100; ?>%">
						  	<?php print $outflow; ?> / <?php print $credits; ?>
						    <span class="sr-only"><?php print $outflow; ?> / <?php print $credits; ?></span>
						  </div>
						</div>
					</div>
				</div>
				<div class='row'>
					<div class='col-xs-6'>
						<label><?php print t("Portieri"); ?></label>
						<div class="progress">
						  <div class="progress-bar progress-bar-success" role="progressbar" aria-valuenow="<?php print $bought_players[0]; ?>" aria-valuemin="0" 
						  	aria-valuemax="<?php print $expected_players[0]; ?>" style="width: <?php print $bought_players[0] / $expected_players[0] * 100; ?>%">
						  	<?php print $bought_players[0]; ?> / <?php print $expected_players[0]; ?>
						    <span class="sr-only"><?php print $bought_players[0]; ?></span>
						  </div>
						</div>
					</div>
					<div class='col-xs-6'>
						<label><?php print t("Difensori"); ?></label>
						<div class="progress">
						  <div class="progress-bar progress-bar-info" role="progressbar" aria-valuenow="<?php print $bought_players[1]; ?>" aria-valuemin="0" 
						  	aria-valuemax="<?php print $expected_players[1]; ?>" style="width: <?php print $bought_players[1] / $expected_players[1] * 100; ?>%">
						  	<?php print $bought_players[1]; ?> / <?php print $expected_players[1]; ?>
						    <span class="sr-only"><?php print $bought_players[1]; ?></span>
						  </div>
						</div>
					</div>
				</div>
				<div class='row'>
					<div class='col-xs-6'>
						<label><?php print t("Centrocampisti"); ?></label>
						<div class="progress">
						  <div class="progress-bar progress-bar-warning" role="progressbar" aria-valuenow="<?php print $bought_players[2]; ?>" aria-valuemin="0" 
						  	aria-valuemax="<?php print $expected_players[2]; ?>" style="width: <?php print $bought_players[2] / $expected_players[2] * 100; ?>%">
						  	<?php print $bought_players[2]; ?> / <?php print $expected_players[2]; ?>
						    <span class="sr-only"><?php print $bought_players[2]; ?></span>
						  </div>
						</div>
					</div>
					<div class='col-xs-6'>
						<label><?php print t("Attaccanti"); ?></label>
						<div class="progress">
						  <div class="progress-bar progress-bar-danger" role="progressbar" aria-valuenow="<?php print $bought_players[3]; ?>" aria-valuemin="0" 
						  	aria-valuemax="<?php print $expected_players[3]; ?>" style="width: <?php print $bought_players[3] / $expected_players[3] * 100; ?>%">
						  	<?php print $bought_players[3]; ?> / <?php print $expected_players[3]; ?>
						    <span class="sr-only"><?php print $bought_players[3]; ?></span>
						  </div>
						</div>
					</div>
					</div>
				</div>
				<div class='row'>
					<div class='col-xs-12'>
						<div class="input-group input-group-sm">
							<span class="input-group-btn btn-group-custom" data-toggle="buttons"> 
							<label class="btn btn-default">
								<input type="checkbox" autocomplete="off" id="filter-role-0" />P
							</label> 
							<label class="btn btn-default" id="filter-role-1-label">
								<input type="checkbox" autocomplete="off" id="filter-role-1" />D
							</label> <label class="btn btn-default" id="filter-role-2-label">
								<input type="checkbox" autocomplete="off" id="filter-role-2" />C
							</label> <label class="btn btn-default" id="filter-role-3-label">
								<input type="checkbox" autocomplete="off" id="filter-role-3" />A
							</label>
							</span> 
							<span class="input-group-btn btn-group" role="group">
								<button type="button" class="btn btn-default dropdown-toggle"
									data-toggle="dropdown" aria-expanded="false">
									<span id="filter-team">Squadre</span> 
									<span class="caret"></span>
								</button>
								<?php print render($real_teams_list); ?>
							</span> 
							<input class="form-control" placeholder="<?php print t("Cerca")?>" type="text" id="filter-name" /> 
							<span class="input-group-btn btn-group-custom">
								<button id="clearSearch" class="btn btn-warning"
									title="<?php print t("Cancella filtri di ricerca") ?>">
									<span class="fa fa-times"></span>
								</button>
							</span>
						</div>
					</div>
				</div>
				<div class="" style="height: 200px; overflow-y: auto">
				<?php print render($players_list); ?>
				</div>
			</div>

		</div>
	</div>
</div>

<script>
(function ($) {
	
	$(function(){

		$("#clearSearch").click(function() {
	    	$("#filter-role-0").prop('checked', false);
		    $("#filter-role-1").prop('checked', false);
		    $("#filter-role-2").prop('checked', false);
		    $("#filter-role-3").prop('checked', false);
	    	$("#filter-name").val("")
	    	$("#filter-team").text("Squadre");
	    	$("#filter-team").val($(this).text());
	    	$("#players-list tbody tr").addClass("show-player").addClass("show-player-name").addClass("show-player-team").addClass("show-player-role").show();
		});

	    $("#real-teams-list.dropdown-menu li a").click(function(){

	        if($(this).text() == " - ") {
		    	$("#filter-team").text("Squadre");
		    	$("#filter-team").val($(this).text());
		    	$("#players-list tbody tr.show-player-name.show-player-role").addClass("show-player-team").show();
	        }
	        else {
		        var team = $(this).text();
		      $("#filter-team").text($(this).text());
		      $("#filter-team").val($(this).text());
		      $("#players-list tbody tr.show-player-name.show-player-role").filter(function() {		        
		       		return $(this).attr("data-team") !== team;
				}).removeClass("show-player-team").hide();
		        $("#players-list tbody tr.show-player-name.show-player-role").filter(function() {
		       		return $(this).attr("data-team") === team;
				}).addClass("show-player-team").show();
	        }
	   });

		$("#filter-name").keyup(function() {
		    var search = $(this).val().toLowerCase();
		    if(search == "") {
		       $("#players-list tbody tr.show-player-team.show-player-role").addClass("show-player-name").show();
		    }
		    else {    
		        $("#players-list tbody tr.show-player-team.show-player-role").filter(function() {		        
		       		return $(this).attr("data-name").toLowerCase().substring(0, search.length) !== search;
				}).removeClass("show-player-name").hide();
				
		        $("#players-list tbody tr.show-player-team.show-player-role").filter(function() {
		       		return $(this).attr("data-name").toLowerCase().substring(0, search.length) === search;
				}).addClass("show-player-name").show();        
		    }
		});

// 		$("#filter-role-0-label, #filter-role-1-label, #filter-role-2-label, #filter-role-3-label").click(function() {
// 			$(this).find("input").trigger("click");
// 		});

		$("#filter-role-0, #filter-role-1, #filter-role-2, #filter-role-3").click(function() {
	
			if( !($("#filter-role-0").is(":checked")) 
	    	    &&  !($("#filter-role-1").is(":checked")) 
	    	    &&  !($("#filter-role-2").is(":checked")) 
	    	    &&  !($("#filter-role-3").is(":checked")) )
	    	{
		    	$("#players-list tr.show-player-team.show-player-name").addClass("show-player-role").show()
			}
		    else {
			    if( $("#filter-role-0").is(":checked")){
			        $("#players-list tr.role-0.show-player-team.show-player-name").addClass("show-player-role").show();}
			    else
			        $("#players-list tr.role-0.show-player-team.show-player-name").removeClass("show-player-role").hide();
			    
			    if( $("#filter-role-1").is(":checked"))
			        $("#players-list tr.role-1.show-player-team.show-player-name").addClass("show-player-role").show();
			    else
			        $("#players-list tr.role-1.show-player-team.show-player-name").removeClass("show-player-role").hide();
			    
			    if( $("#filter-role-2").is(":checked"))
			        $("#players-list tr.role-2.show-player-team.show-player-name").addClass("show-player-role").show();
			    else
			        $("#players-list tr.role-2.show-player-team.show-player-name").removeClass("show-player-role").hide();
			 
			    if( $("#filter-role-3").is(":checked"))
			        $("#players-list tr.role-3.show-player-team.show-player-name").addClass("show-player-role").show();
			    else
			        $("#players-list tr.role-3.show-player-team.show-player-name").removeClass("show-player-role").hide();
	        }
		});

	});

	$(".buy-player").click(function() {
		var id = $(this).attr("id").substring(4);

// 		$.post();
	});
}(jQuery));

</script>

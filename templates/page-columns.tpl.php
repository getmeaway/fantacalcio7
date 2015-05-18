<div class="row">
	<div id='teams_list' class="col-xs-12 col-sm-3">

		<?php if (isset($choose_rounds) ) : ?> 
		<!-- scegli giornata TODO -->
		<div>
			<?php //print ($choose_rounds); ?>
			<div class="dropdown">
			  <button class="btn btn-sm btn-default dropdown-toggle" type="button" id="dropdown-rounds" data-toggle="dropdown" aria-expanded="true">
			    <?php print t("Giornata"); ?>
			    <span class="caret"></span>
			  </button>
			  <ul class="dropdown-menu" role="menu" aria-labelledby="dropdown-rounds">
			  	<?php foreach ($choose_rounds as $round => $link): ?>
			    <li role="presentation"><a role="menuitem" tabindex="-1" href="<?php print $link; ?>"><?php print $round; ?></a></li>
			    <?php endforeach; ?>			    
			  </ul>
			</div>
		</div>
		<?php endif; ?>
		
		<!-- elenco squadre-->
		
		<div class="panel-group" id='accordion_1'>
			<?php if (variable_get("show_teams_filter", 0) > 0 && array_count($teams_list) > variable_get("show_teams_filter", 0)): ?>
			<div class="input-group">
				<span class="input-group-btn btn-group-custom">
					<button id="filterAll" class="btn btn-default"
						title="<?php print t("Tutte le squadre") ?>">
						<span class="fa fa-list"></span>
					</button>
					<button id="filterMine" class="btn btn-default"
						title="<?php print t("Mie squadre") ?>">
						<span class="fa fa-user"></span>
					</button>
				</span> 
				<input class="form-control" id="filterTeamsText" placeholder="Cerca..." type="text" />
			</div>
			<?php endif; ?>
			
			<?php if (count($teams_list) > 1): ?>
			<!-- Accordion -->
			<div class="panel-group" id="teams-accordion" role="tablist" aria-multiselectable="true">
				<?php foreach($teams_list as $t_id => $teams_group): ?>
				<div class="panel panel-default">
					<div class="panel-heading" role="tab" id="heading-<?php print $t_id; ?>">
						<h4 class="panel-title">
							<a <?php if($teams_group["expanded"] == true) print "class=\"collapsed\""; ?> data-toggle="collapse" data-parent="#teams-accordion" href="#collapse-<?php print $t_id; ?>" 
							aria-expanded="<?php print (($teams_group["expanded"]) ? "true" : "false"); ?>" aria-controls="collapse-<?php print $t_id; ?>">
							<?php print $teams_group["group_name"]; ?> 
							</a>
						</h4>
					</div>
					<div id="collapse-<?php print $t_id; ?>" class="list-group panel-collapse collapse <?php print (($teams_group["expanded"]) ? "in" : ""); ?>" role="tabpanel" 
						aria-labelledby="heading-<?php print $t_id; ?>">
						<?php if (count($teams_group["teams"]) > 0): ?>
						<?php print ($teams_group["teams"]) ?>						
						<?php endif; ?>
					</div>
				</div>
				<?php endforeach; ?>
			</div>
			<?php else: ?>
			<div id="teams-list" class="">
				<?php foreach($teams_list as $t_id => $teams_group): ?>
				<?php print ($teams_group["teams"]); ?>
				<?php endforeach; ?>
			</div>
			<?php endif; ?>

		</div>
	</div>

	<!-- output -->
	<div id='team_data' class="col-xs-12 col-sm-9">
		<?php if (isset($main_output) && $main_output != null): ?>
		<?php print render($main_output); ?>
		<?php endif; ?>
	</div>
</div>

<script>
(function ($) {

    $("#filterTeamsText").keyup(function () {
        var search = $(this).val().toLowerCase();
        if (search === "") {
            $("#teams-list li").slideDown();
        } else {
            $("#teams-list li")
                .filter(function () {
                return !($(this).attr("data-name").toLowerCase().substring(0, search.length) === search);
            })
                .slideUp();
            $("#teams-list li").filter(function () {
                return ($(this).attr("data-name").toLowerCase().substring(0, search.length) === search);
            }).slideDown();
        }
    });

    $("#filterAll").click(function () {
        $("#teams-list li").slideDown();
    });

    $("#filterMine").click(function () {
        $("#teams-list li").not(".mine").slideUp();
        $("#teams-list li.mine").slideDown();
    });
      
}(jQuery));

(function($) {
    fakewaffle.responsiveTabs(['xs', 'sm']);
})(jQuery);
</script>
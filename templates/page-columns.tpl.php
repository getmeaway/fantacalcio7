<div class="row">
	<div id='teams_list' class="col-sm-3 col-xs-hidden">

		<?php if (isset($choose_rounds)) : ?> 
		<!-- scegli giornata TODO -->
		<div>		
		</div>
		<?php endif; ?>
		
		<!-- elenco squadre-->
		<div class="panel-group" id='accordion_1'>
			<div class="input-group">
				<span class="input-group-btn btn-group-custom">
					<button id="filterAll" class="btn btn-default" title="<?php print t("Tutte le squadre") ?>">
						<span class="fa fa-list"></span>
					</button>
					<button id="filterMine" class="btn btn-default" title="<?php print t("Mie squadre") ?>">
						<span class="fa fa-user"></span>
					</button> 
				</span>
				<input class="form-control" id="filterTeamsText" placeholder="Cerca..." type="text" />
			</div>
			<div id="teams-list" class="-panel-collapse -collapse">
				<?php print render($teams_list); ?>
			</div>
		</div>
	</div>

	<!-- output -->
	<div id='team_data' class="col-sm-9 col-xs-12">
		<?php if (isset($squad) && $squad != null): ?>
		<div role="tabpanel" class="collapse navbar-collapse">

			<!-- Nav tabs -->
			<ul class="nav nav-pills" role="tablist">
				<li role="presentation" class="active">
					<a href="#squad" aria-controls="squad" role="tab" data-toggle="tab">
					<?php print t("Rosa"); ?>
					</a>
				</li>
				<li role="presentation">
					<a href="#details" aria-controls="details" role="tab" data-toggle="tab">
					<?php print t("Dettagli"); ?>
					</a>
				</li>
				<li role="presentation">
					<a href="#rounds" aria-controls="rounds" role="tab" data-toggle="tab">
					<?php print t("Giornate"); ?>
					</a>
				</li>
				<li role="presentation">
					<a href="#stats" aria-controls="stats" role="tab" data-toggle="tab">
					<?php print t("Statistiche"); ?>
					</a>
				</li>
				<?php if ($is_own_team) : ?>
				<li class="pull-right">
					<a class="btn-info" href="<?php print base_path() . "mie/" . $team->id ?>"><?php print t("Gestione squadra"); ?></a>
				</li>
				<?php endif; ?>
			</ul>

			<!-- Tab panes -->
			<div class="tab-content">
				<div role="tabpanel" class="tab-pane active" id="squad">
				<?php print render($squad); ?>
				</div>
				<div role="tabpanel" class="tab-pane" id="details">
				<?php print render($details); ?>
				</div>
				<div role="tabpanel" class="tab-pane" id="rounds">
				<?php print render($rounds); ?>
				</div>
				<div role="tabpanel" class="tab-pane" id="stats">
				<?php print render($stats); ?>
				</div>
			</div>

		</div>
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

</script>
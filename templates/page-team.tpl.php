 <div role="tabpanel" class="collapse navbar-collapse">

			<!-- Nav tabs -->
	<ul class="nav nav-pills" role="tablist">
		<li role="presentation" class="active"><a href="#squad"
			aria-controls="squad" role="tab" data-toggle="tab">
			<?php print t("Rosa"); ?>
			</a></li>
		<li role="presentation"><a href="#details" aria-controls="details"
			role="tab" data-toggle="tab">
			<?php print t("Dettagli"); ?>
			</a></li>
		<li role="presentation"><a href="#rounds" aria-controls="rounds"
			role="tab" data-toggle="tab">
			<?php print t("Giornate"); ?>
			</a></li>
		<li role="presentation"><a href="#stats" aria-controls="stats"
			role="tab" data-toggle="tab">
			<?php print t("Statistiche"); ?>
			</a></li>
		<?php if ($is_own_team) : ?>
		<li class="pull-right"><a class="btn-info"
			href="<?php print base_path() . "mie/" . $team->id ?>"><?php print t("Gestione squadra"); ?></a>
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
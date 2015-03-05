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
				  	<li role="presentation">
				  		<a href="<?php print base_path();?>mie/<?php print $t_id; ?>/movimenti"><?php print t("Movimenti"); ?></a>
				  	</li>
				  	<?php endif; ?>
				  	<li role="presentation" class="active">
				  		<a href="<?php print base_path();?>mie/<?php print $t_id; ?>/lista-movimenti"><?php print t("Lista movimenti"); ?></a>
				  	</li>
				</ul>
			</div>
		</div>
		<div class='row'>
			<div class='col-md-6 col-lg-6'>&nbsp;</div>
		</div>
		<div class='row'>
			<div class='col-md-12 col-lg-12'>
				<?php print render($movements); ?>
			</div>
		</div>
	</div>
</div>
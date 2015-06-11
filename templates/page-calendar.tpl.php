<div class="container-fluid">
	<?php if (isset($next_round) || isset($last_round)): ?>
	<div class="row">
		<?php print $next_round; ?>
	</div>
	<?php endif; ?>
	
	<?php if (isset($rounds_list)): ?>
	<div class="row">
		<div class="col-xs-12">
			<ul class="list-inline center-block">
			<?php foreach ($rounds_list as $round => $round_label): ?>
				<li><a href="#round_<?php print $round; ?>">
						<?php print $round_label; ?>
					</a></li>
			<?php endforeach; ?>
			</ul>
		</div>
	</div>
	<div class="row">&nbsp;</div>
	<?php endif; ?>
	
	<?php if (isset($matches)): ?>
	<div class="row">
	<?php foreach ($matches as $round => $round_matches): ?>
		<div class="col-xs-12 col-sm-6">
			<h4 id="round_<?php print $round; ?>">
			<?php print $round . t("&ordf; giornata");?></h4>
			<table class="table table-responsive">
			<?php foreach ($round_matches as $match): ?>
				<tr>
					<td><?php print $match->home_team . " - " . $match->away_team; ?></td>
					<td><small><em><?php print date("d-m-Y H:i", $match->date); ?></em></small></td>
					<?php if ($match->played): ?>
					<td><?php print $match->goals_1 . " - " . $match->goals_2; ?></td>
					<td><a href=""><i class="fa fa-bar-chart"></i></a></td>
					<?php else: ?>
					<td></td>
					<td></td>
					<?php endif; ?>
				</tr>
			<?php endforeach; ?>			
			</table>
		</div>
	<?php endforeach; ?>
	</div>
	<?php endif; ?>
</div>
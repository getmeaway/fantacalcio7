<div class="container-fluid">
	<?php if (isset($next_round) || isset($last_round)): ?>
	<div class="row">
		<?php print $next_round; ?>
		<?php print $last_round; ?>
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
			<?php
			 $matches_values = array_values($round_matches);
			 $first_match = array_shift($matches_values); 
			 print (!empty($first_match->round_label) ? $first_match->round_label : $round . t("&ordf; giornata"));?>
			<?php if (!$is_main_competition): ?>
			 <small class="pull-right"><em><?php print date("d-m-Y H:i", $rounds[$round]->date); ?></em></small>
			<?php endif; ?>
			</h4>
			<table class="table table-responsive">
			<?php foreach ($round_matches as $match): ?>
				<tr>
					<td><?php print $match->home_team . " - " . $match->away_team; ?></td>
					<?php if ($is_main_competition): ?>
					<td><small><em><?php print date("d-m-Y H:i", $match->date); ?></em></small></td>
					<?php endif; ?>
					<?php if ($match->played): ?>
					<td><?php print $match->goals_1 . " - " . $match->goals_2; ?></td>
					<?php if (!$is_main_competition): ?>
					<td><small><em><?php print $match->tot_1 . " - " . $match->tot_2; ?></em></small></td>
					<?php endif; ?>
					<td>
					 <?php print l('<i class="fa fa-bar-chart"></i>', "scheda/partita/" . $match->id, array("html" => true)); ?>					 
					</td>
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
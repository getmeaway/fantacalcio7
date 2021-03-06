<div class="container-fluid">
	<?php if ($next_round != null || $last_round != null ): ?>
	<div class="row">
		<?php if ($last_round != null ): ?>
		<div class="col-xs-12 col-sm-6">
			<h4 id="round_<?php print $last_round->round; ?>">
			<?php
			 $matches_values = array_values($last_round->matches);
			 $first_match = array_shift($matches_values); 
			 print t("Ultima giornata") . " - " .(!empty($first_match->round_label) ? $first_match->round_label : $last_round->round . t("&ordf; giornata"));
			 ?>
			<small class="pull-right"><em><?php print date_format(date_create($rounds[$last_round->round]->date), 'd-m-Y H:i'); ?></em></small>
			
			</h4>
			<table class="table table-responsive">
			<?php foreach ($last_round->matches as $match): ?>
				<tr>
					<?php if ($match->match_label != null) : ?>
					<td><?php print $match->match_label ?></td>
					<?php endif; ?>
					<td>
					   <?php 
					     $attributes_1 = array();
					     if (in_array($match->t1_id, $user_teams))
					       $attributes_1 = array("attributes" => array("class" => array("bold")));
					     
					     $attributes_2 = array();
					     if (in_array($match->t2_id, $user_teams))
					       $attributes_2 = array("attributes" => array("class" => array("bold")));
					     
					     print l($match->home_team, "squadre/" . $match->t1_id, $attributes_1) 
					       . " - " 
	                       . l($match->away_team, "squadre/" . $match->t2_id, $attributes_2); 
					   ?>
					</td>
					<?php if ($is_main_competition): ?>
					<td><small><em><?php print date("d-m-Y H:i", $match->date); ?></em></small></td>
					<?php endif; ?>
					<?php if ($match->played): ?>
					<td>
					<?php print ($match->goals_1 + $match->goals_ot_1 + $match->penalties_1) 
					   . " - " . ($match->goals_2 + $match->goals_ot_2 + $match->penalties_2); ?>
					 <?php if ($match->isDraw())
					   print " (" . $match->goals_1 . " - " . $match->goals_2 . ")"; ?>
					</td>
					<?php if (!$is_main_competition): ?>
					<td><small><em><?php print $match->tot_1 . " - " . $match->tot_2; ?></em></small></td>
					<?php endif; ?>
					<td>
					 <?php print l('<i class="fa fa-bar-chart"></i>', "scheda/partita/" . $match->id, array("html" => true, "attributes" => array("data-target" => "#match-details-modal-" . $g_id, "data-toggle" => "modal"))); ?>					 
					</td>
					<?php else: ?>
					<td></td>
					<td></td>
					<?php endif; ?>
				</tr>
			<?php endforeach; ?>			
			</table>
		</div>
	<?php endif; ?>
		<?php if ($next_round != null): ?>
		<div class="col-xs-12 col-sm-6">
			<h4 id="round_<?php print $next_round->round; ?>">
			<?php
			 $matches_values = array_values($next_round->matches);
			 $first_match = array_shift($matches_values); 
			 print t("Prossima giornata") . " - " .(!empty($first_match->round_label) ? $first_match->round_label : $next_round->round . t("&ordf; giornata"));
			 ?>
			<small class="pull-right"><em><?php print date_format(date_create($rounds[$next_round->round]->date), 'd-m-Y H:i'); ?></em></small>
			
			</h4>
			<table class="table table-responsive">
			<?php foreach ($next_round->matches as $match): ?>
				<tr>
<?php if ($match->match_label != null) : ?>
                                        <td><?php print $match->match_label ?></td>
                                        <?php endif; ?>
					<td>
					   <?php 
					     $attributes_1 = array();
					     if (in_array($match->t1_id, $user_teams))
					       $attributes_1 = array("attributes" => array("class" => array("bold")));
					     
					     $attributes_2 = array();
					     if (in_array($match->t2_id, $user_teams))
					       $attributes_2 = array("attributes" => array("class" => array("bold")));
					     
					     print l($match->home_team, "squadre/" . $match->t1_id, $attributes_1) 
					       . " - " 
	                       . l($match->away_team, "squadre/" . $match->t2_id, $attributes_2); 
					   ?>
					</td>
					<?php if ($is_main_competition): ?>
					<td><small><em><?php print date("d-m-Y H:i", $match->date); ?></em></small></td>
					<?php endif; ?>
					<?php if ($match->played): ?>
					<td>
					<?php print ($match->goals_1 + $match->goals_ot_1 + $match->penalties_1) 
					   . " - " . ($match->goals_2 + $match->goals_ot_2 + $match->penalties_2); ?>
					 <?php if ($match->isDraw())  
					   print " (" . $match->goals_1 . " - " . $match->goals_2 . ")"; ?>
					</td>
					<?php if (!$is_main_competition): ?>
					<td><small><em><?php print $match->tot_1 . " - " . $match->tot_2; ?></em></small></td>
					<?php endif; ?>
					<?php else: ?>
					<td></td>
					<?php endif; ?>
					<td>
					 <?php print l('<i class="fa fa-bar-chart"></i>', "scheda/partita/" . $match->id, array("html" => true, "attributes" => array("data-target" => "#match-details-modal-" . $g_id, "data-toggle" => "modal"))); ?>					 
					</td>
				</tr>
			<?php endforeach; ?>			
			</table>
		</div>
	<?php endif; ?>
	</div>
	<div class="row">
		<div class="col-xs-12"><hr></div>
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
	<?php usort($round_matches, function($a, $b) { return $a->match_label > $b->match_label ? 1 : -1;}); ?>
		<div class="col-xs-12 col-sm-6">
			<h4 id="round_<?php print $round; ?>">
			<?php
			 $matches_values = array_values($round_matches);
			 $first_match = array_shift($matches_values); 
			 print (!empty($first_match->round_label) ? $first_match->round_label : $round . t("&ordf; giornata"));?>
			<?php if (!$is_main_competition): ?>
			 <small class="pull-right"><em><?php print date_format(date_create($rounds[$round]->date), 'd-m-Y H:i'); ?></em></small>
			<?php endif; ?>
			</h4>
			<table class="table table-responsive">
			<?php foreach ($round_matches as $match): ?>
				<tr>
<?php if ($match->match_label != null) : ?>
                                        <td><?php print $match->match_label ?></td>
                                        <?php endif; ?>
					<td>
					   <?php 
					     $attributes_1 = array();
					     if (in_array($match->t1_id, $user_teams))
					       $attributes_1 = array("attributes" => array("class" => array("bold")));
					     
					     $attributes_2 = array();
					     if (in_array($match->t2_id, $user_teams))
					       $attributes_2 = array("attributes" => array("class" => array("bold")));
					     
					     print l($match->home_team, "squadre/" . $match->t1_id, $attributes_1) 
					       . " - " 
	                       . l($match->away_team, "squadre/" . $match->t2_id, $attributes_2); 
					   ?>
					</td>
					<?php if ($is_main_competition): ?>
					<td><small><em><?php print date("d-m-Y H:i", $match->date); ?></em></small></td>
					<?php endif; ?>
					<?php if ($match->played): ?>
					<td>
					 <?php print ($match->goals_1 + $match->goals_ot_1 + $match->penalties_1) 
					   . " - " . ($match->goals_2 + $match->goals_ot_2 + $match->penalties_2); ?>
					 <?php if ($match->isDraw()) 
					   print " (" . $match->goals_1 . " - " . $match->goals_2 . ")"; ?>
					</td>
					<?php if (!$is_main_competition): ?>
					<td><small><em><?php print $match->tot_1 . " - " . $match->tot_2; ?></em></small></td>
					<?php endif; ?>
					<?php else: ?>
					<td></td>
					<?php endif; ?>
					<?php if ($match->played || (isset($next_round) && $match->round == $next_round->round)): ?>
					<td>
					 <?php print l('<i class="fa fa-bar-chart"></i>', "scheda/partita/" . $match->id, array("html" => true, "attributes" => array("data-target" => "#match-details-modal-" . $g_id, "data-toggle" => "modal"))); ?>					 
					</td>
					<?php else: ?>
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

<div id='match-details-modal-<?php echo $g_id; ?>' class='modal fade' tabindex='-1' aria-hidden='true' style='z-index: 2000'>
        <div class="modal-dialog modal-lg">
                        <div class="modal-header">
<button type="button" class="close" data-dismiss="modal" aria-label="Close"><span aria-hidden="true">&times;</span></button>
                        </div>
		<div class="modal-content">

                        <div class="modal-body">

                        </div>
                </div>
	</div>
</div>

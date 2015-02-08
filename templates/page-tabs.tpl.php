<div role="tabpanel">

	<?php if (isset($rounds_list)) : ?>
	<div class='navbar navbar-default navbar-static-top'>
		<div class='round_list' id='round-list'>
  			<ul class="nav navbar-nav">
  			<?php if (isset($first_item)): ?>  
	  			<li class='round_list'><?php print l($first_item['value'], $url . "/" . $first_item['key']); ?></li>
  			<?php endif; ?>
  			<?php foreach ($rounds_list as $round => $round_label): ?>
  	  			<li class='round_list'><?php print l($round_label, $url . "/" . $round); ?></li>
  			<?php endforeach; ?>
  			</ul>
  		</div>
	</div>
	<?php endif; ?>

  <?php if (count($groups) > 1) : ?>
  <!-- Nav tabs -->
  <ul class="nav nav-pills" role="tablist">
  	<?php foreach ($groups as $g_id => $group ): ?>
    <li role="presentation" class="<?php print ($group->active ? "active" : "") ?>"><a href="#<?php print $g_id; ?>" aria-controls="<?php print $g_id; ?>" role="tab" data-toggle="tab"><?php print $group->name; ?></a></li>
    <?php endforeach; ?>
  </ul>
  <?php endif; ?>

  <?php if (count($groups) > 1) : ?>
  <!-- Tab panes -->
  <div class="tab-content">
  	<?php foreach ($groups as $g_id => $group ): ?>
    <div role="tabpanel" class="tab-pane <?php print ($group->active ? "active" : "") ?>"" id="<?php print $g_id; ?>">
    	<?php print render ($group->output); ?>
    </div>    
    <?php endforeach; ?>
  </div>
  <?php else: ?>
  	<?php foreach ($groups as $g_id => $group ): ?>
  		<?php print render ($groups[$g_id]->output); ?>
  	<?php endforeach; ?>
  <?php endif; ?>
  
</div>
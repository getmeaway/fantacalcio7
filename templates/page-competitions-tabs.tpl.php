<div role="tabpanel">

  <?php if (count($competitions) > 1) : ?>
  <!-- Nav tabs -->
  <ul class="nav nav-pills" role="tablist">
  	<?php foreach ($competitions as $c_id => $competition ): ?>
    <li role="presentation" class="<?php print ($competition->active ? "active" : "") ?>"><a href="#<?php print $c_id; ?>" aria-controls="<?php print $c_id; ?>" role="tab" data-toggle="tab"><?php print $competition->name; ?></a></li>
    <?php endforeach; ?>
  </ul>
  <?php endif; ?>

  <?php if (count($competitions) > 1) : ?>
  <!-- Tab panes -->
  <div class="tab-content">
  	<?php foreach ($competitions as $c_id => $competition ): ?>
    <div role="tabpanel" class="tab-pane <?php print ($competition->active ? "active" : "") ?>"" id="<?php print $c_id; ?>">
    	<?php print render ($competition->output); ?>
    </div>    
    <?php endforeach; ?>
  </div>
  <?php else: ?>
  	<?php foreach ($competitions as $c_id => $competition ): ?>
  		<?php print render ($competitions[$c_id]->output); ?>
  	<?php endforeach; ?>
  <?php endif; ?>
  
</div>
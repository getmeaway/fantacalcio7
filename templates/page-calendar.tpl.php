<div class="container-fluid">
	<?php if (isset($next_round) || isset($last_round)): ?>
	<div class="row">
		<?php print $next_round; ?>
	</div>
	<?php endif; ?>
	
	<?php if (isset($rounds_list)): ?>
	<div class="row">
	</div>
	<?php endif; ?>
	
	<?php if (isset($matches)): ?>
	<?php foreach ($matches as $round => $round_matches): ?>
		<div class="row">
		
		</div>
	<?php endforeach; ?>
	<?php endif; ?>
</div>
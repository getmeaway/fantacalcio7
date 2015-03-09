<div id='div-formazione'>
	<div id='header-formazione'>
		<div class='clear' style='height:30px;'></div>
		
		<div data-role='popup' id='popup_validate'>
			show_line_up_check(Lineup::get($c_id, $t_id, $round->competition_round)->check())
		</div>
		
		<div id='_formazione_form'>
			<?php drupal_render($formazione_form) ?>
		</div>
	</div>

<div class="row">
	<div class="col-xs-12 col-md-12"></div>
</div>
<div class="row">
	<div class="col-sm-6 col-xs-12"></div>
	<div class="col-sm-6 col-xs-12"></div>
</div>
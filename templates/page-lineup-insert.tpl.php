<div id='div-formazione'>
	<div id='header-formazione'>
		<div class='clear' style='height:30px;'></div>
		
		<div id='_formazione_form' class="well well-sm">
			<?php print render($lineup_check); ?>
		</div>
		
		<div data-role='popup' id='popup_validate'>
			<?php print render($lineup_form); ?>			
		</div>
	</div>

<div class="row">
	<div class="col-xs-12 col-md-12">
	</div>
</div>
<div class="row">
	<div class="col-sm-6 col-xs-12">
		<div class="lineup-group-container">
			<?php print render($squad);?>
		</div>
	</div>
	<div class="col-sm-6 col-xs-12">
		<div class="lineup-group-container">
			<h4><?php print t("Titolari");?></h4>
			<?php print render($regulars);?>
		</div>
		<div class="lineup-group-container">
			<h4><?php print t("Riserve");?></h4>
			<?php print render($reserves);?>
		</div>
	</div>
</div>

<script src="http://code.jquery.com/ui/1.11.4/jquery-ui.js"></script>
<script src="<?php print base_path() . drupal_get_path("module", "fantacalcio"); ?>/js/lineup_insert.js"></script>
<script>
jQuery(function() {
    jQuery( "#lineup-squad tbody, #lineup-regulars tbody, #lineup-reserves tbody" ).sortable({
      connectWith: ".lineup-group tbody"
    }).disableSelection();
  });
</script>
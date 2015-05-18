<div id='div-formazione'>
	<div id='header-formazione'>
		<div class='clear' style='height:30px;'></div>
		
		<div id='_formazione_form'>
			<?php print render($lineup_check); ?>
		</div>
		
<!-- 		<div data-role='popup' id='popup_validate'> 
			<?php //print render($lineup_form); ?>			
 		</div> -->
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

<div id='lineup-form-modal' class='modal' tabindex='-1'style='z-index: 2000' role='dialog' aria-labelledby='squadCompleteModalLabel' aria-hidden='true'>
	<div class='modal-dialog'>
		<div class='modal-content'>
			<div class='modal-header'>
				<button type='button' class='close' data-dismiss='modal' aria-label='Close'>
					<span aria-hidden='true'>&times;</span>
				</button>
				<h4 class='modal-title'>Formazione corretta</h4>
			</div>
			<div class='modal-body'>
				<p><?php print t("La formazione inserita è corretta. Puoi confermarla o chiudere questo popup per continuare le modifiche."); ?></p>				
			</div>
			<div class='modal-footer'>
				<div class="pull-right"><button type='button' class='btn btn-default' data-dismiss='modal'><?php print t("Annulla"); ?></button></div>
				<div class="pull-right"><?php print render($lineup_form); ?></div>
			</div>
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
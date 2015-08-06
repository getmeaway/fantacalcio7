<div id='div-formazione' class="row">

	<div id='_formazione_form' class="col-xs-12 col-sm-10 hidden-xs">
		<?php print render($lineup_check); ?>
	</div>

	<div class="col-xs-12 col-sm-2 hidden-xs">
		<div class="row">
			<div class="col-xs-6 col-sm-12 col-md-12">
				<?php print render($lineup_form_clean); ?>
			</div>
			<div class="col-xs-6 col-sm-12 col-md-12">
				<?php print render($lineup_form_confirm); ?>
			</div>
		</div>
	</div>

</div>

<div class="row">
	<div class="col-xs-12 col-md-12"></div>
</div>

<div class="row">
	<div class="hidden-xs">
		<div class="row">
			<div class="col-sm-6">
				<div class="lineup-group-container">
    			<?php print render($squad);?>
    		</div>
			</div>
			<div class="col-sm-6">
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
	</div>
</div>

<div class="row">
	<div class="visible-xs">
		<div class="row" id="step-1">
      		<div id='_formazione_form' class="col-xs-12 col-sm-10">
      		<?php print render($lineup_check); ?>
      	</div>
			<div class="col-xs-12">
				<div class="lineup-group-container">
    			<?php print render($squad_mobile);?>
    		    </div>

				<div class="row">
					<div class="col-xs-6">
						<div class="center-block">
							<button id="step-1-back " type="button" class="btn btn-default">
				<?php print t("Annulla");?>
				</button>
						</div>
					</div>
					<div class="col-xs-6">
						<div class="center-block">
							<button id="step-1-go" type="button" class="btn btn-success">
				<?php print t("Avanti");?>
				</button>
						</div>
					</div>
				</div>
			</div>
		</div>
		<!-- Ordina riserve -->
		<div class="row hidden" id="step-2">
			<div class="col-xs-12">
				<h4>Ordina Riserve</h4>
				<div class="row">
					<div class="col-xs-6">
						<div class="center-block">
							<button id="step-2-back" type="button" class="btn btn-default">
				<?php print t("Indietro");?>
				</button>
						</div>
					</div>
					<div class="col-xs-6">
						<div class="center-block">
							<button id="step-2-go" type="button" class="btn btn-success">
				<?php print t("Avanti");?>
				</button>
						</div>
					</div>
				</div>
			</div>
		</div>
		<!-- Rigoristi -->
		<div class="row hidden" id="step-3">
			<div class="col-xs-12">
				<h4>Ordina Rigoristi</h4>
				<div class="row">
					<div class="col-xs-6">
						<div class="center-block">
							<button id="step-3-back" type="button" class="btn btn-default">
				<?php print t("Indietro");?>
				</button>
						</div>
					</div>
					<div class="col-xs-6">
						<div class="center-block">
							<button id="step-3-go" type="button" class="btn btn-success">
				<?php print t("Avanti");?>
				</button>
						</div>
					</div>
				</div>
			</div>
		</div>
		<!-- Conferma -->
		<div class="row hidden" id="step-4">
			<div class="col-xs-12">
				<h4>Conferma</h4>
				<div class="row">
					<div class="col-xs-6">
						<div class="center-block">
							<button id="step-4-back" type="button" class="btn btn-default">
				<?php print t("Indietro");?>
				</button>
						</div>
					</div>
					<div class="col-xs-6">
						<div class="center-block">
							<button id="step-4-go" type="button" class="btn btn-success">
				<?php print t("Conferma");?>
				</button>
						</div>
					</div>
				</div>
			</div>
		</div>
	</div>
</div>

<div id='lineup-form-modal' class='modal' tabindex='-1'
	style='z-index: 2000' role='dialog'
	aria-labelledby='squadCompleteModalLabel' aria-hidden='true'>
	<div class='modal-dialog'>
		<div class='modal-content'>
			<div class='modal-header'>
				<button type='button' class='close' data-dismiss='modal'
					aria-label='Close'>
					<span aria-hidden='true'>&times;</span>
				</button>
				<h4 class='modal-title'>Formazione corretta</h4>
			</div>
			<div class='modal-body'>
				<p><?php print t("La formazione inserita è corretta. Puoi confermarla o chiudere questo popup per continuare le modifiche."); ?></p>
			</div>
			<div class='modal-footer'>
				<div class="pull-right">
					<button type='button' class='btn btn-default' data-dismiss='modal'><?php print t("Annulla"); ?></button>
				</div>
				<div class="pull-right"><?php print render($lineup_form); ?></div>
			</div>
		</div>
	</div>
</div>

<script src="http://code.jquery.com/ui/1.11.4/jquery-ui.js"></script>
<script
	src="<?php print base_path() . drupal_get_path("module", "fantacalcio"); ?>/js/lineup_insert.js"></script>
<script>
jQuery(function() {
    jQuery( "#lineup-squad tbody, #lineup-regulars tbody, #lineup-reserves tbody" ).sortable({
      connectWith: ".lineup-group tbody"
    }).disableSelection();
  });

jQuery(function() {
	jQuery("#step-1-go").click(function () {
		jQuery("#step-1").addClass("hidden");
		jQuery("#step-2").removeClass("hidden");
	});
	jQuery("#step-2-go").click(function () {
		jQuery("#step-2").addClass("hidden");
		jQuery("#step-3").removeClass("hidden");
	});
	jQuery("#step-3-go").click(function () {
		jQuery("#step-3").addClass("hidden");
		jQuery("#step-4").removeClass("hidden");
	});
	jQuery("#step-2-back").click(function () {
		jQuery("#step-2").addClass("hidden");
		jQuery("#step-1").removeClass("hidden");
	});
	jQuery("#step-3-back").click(function () {
		jQuery("#step-3").addClass("hidden");
		jQuery("#step-2").removeClass("hidden");
	});
	jQuery("#step-4-back").click(function () {
		jQuery("#step-4").addClass("hidden");
		jQuery("#step-3").removeClass("hidden");
	});
});
</script>
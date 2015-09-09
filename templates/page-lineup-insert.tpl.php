<!-- <script -->
<!-- 	src="/fantacazzismo2015/sites/all/modules/fantacalcio/js/jquery.mobile.custom.min.js"></script> -->
<!-- <script -->
<!-- 	src="/fantacazzismo2015/sites/all/modules/fantacalcio/js/jquery.ui.touch-punch.min.js"></script> -->

<div id='div-formazione' class="">

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
	<div class="col-xs-12 visible-xs">
		<div class="row" id="step-1">
			<div id='_formazione_form' class="col-xs-12 col-sm-10">
      		    <?php print render($lineup_check); ?>
      		    <?php print render($lineup_form_confirm); ?>
      	    </div>
			<div class="col-xs-12">
				<div class="lineup-group-container-mobile">
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
							<button id="step-1-go" type="button" class="btn btn-success" disabled>
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
				  <div class="col-xs-12" id="lineup-reserves-sort"></div>
				</div>
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
		<!-- Conferma -->
		<div class="row hidden" id="step-3">
			<div class="col-xs-12">
				<h4>Anteprima</h4>
				<div class="row">
				  <div class="col-xs-12" id="lineup-preview"></div>
				</div>
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
		prepareReserves();
	});
	jQuery("#step-2-go").click(function () {
		jQuery("#step-2").addClass("hidden");

		jQuery("#step-3").removeClass("hidden");
        show_lineup_preview ();
	});

	jQuery("#step-3-go").click(function () {
		jQuery("#line_up_submit").trigger("click");
	});
	
	jQuery("#step-2-back").click(function () {
		jQuery("#step-2").addClass("hidden");
		jQuery("#step-1").removeClass("hidden");
	});
	jQuery("#step-3-back").click(function () {
		jQuery("#step-3").addClass("hidden");
		jQuery("#step-2").removeClass("hidden");
	});
	// jQuery("#step-4-back").click(function () {
// 		jQuery("#step-4").addClass("hidden");
// 		jQuery("#step-3").removeClass("hidden");
// 	});
});

jQuery(document).ready(function() {

// 	var _line_up = null;
	
	  function swiperight(item) {
		//da titolare a tribuna
		if (jQuery(item).hasClass("regular")) {
	       
			jQuery(item).removeClass("col-xs-offset-1").addClass("col-xs-offset-2").removeClass("regular");
	        
	        jQuery(item).find(".position-message").addClass("position-message-squad").html("Tribuna").fadeIn().delay(500).fadeOut(500, function() {
	            jQuery(this).removeClass("position-message-squad")
	            jQuery(this).parent().find(".player-position span").addClass("hidden")
	            //jQuery(this).parent().find("player-position i.position").html("T")
	            //jQuery(this).parent().find("player-position i.position-class").removeClass("text-warning").addClass("text-success");
	            jQuery(this).parent().removeClass("col-xs-offset-2").addClass("col-xs-offset-1")
	        });

	        changePosition(jQuery(item).attr("data-id"), jQuery(item).attr("data-team"), jQuery(item).attr("data-competition"), jQuery(item).attr("data-role"), 0);
// 	        checkLineUp(_line_up);
	    }
	    //da tribuna a titolare
		else {
			jQuery(item).removeClass("col-xs-offset-1").addClass("col-xs-offset-2").addClass("regular");
	        
	        jQuery(item).find(".position-message").addClass("position-message-regular").html("Titolare").fadeIn().delay(500).fadeOut(500, function() {
	            jQuery(this).removeClass("position-message-regular")
	            jQuery(this).parent().find(".player-position span").removeClass("hidden")
	            jQuery(this).parent().find(".player-position i.position").removeClass("text-warning").addClass("text-success").html("T")
	            jQuery(this).parent().find(".player-position i.position-class").removeClass("text-warning").addClass("text-success");
	            jQuery(this).parent().removeClass("col-xs-offset-2").addClass("col-xs-offset-1")
	        });

	        changePosition(jQuery(item).attr("data-id"), jQuery(item).attr("data-team"), jQuery(item).attr("data-competition"), jQuery(item).attr("data-role"), 1);
// 	        checkLineUp(_line_up);
		}

		jQuery(item).on("swiperight", function() {
	  		var _item = jQuery(this); 
	  		jQuery(_item).off("swiperight");
	  		swiperight(_item);
	  	});    
	}


	function swipeleft(item) {
	
	    //da riserva a tribuna
		if (jQuery(item).hasClass("reserve")) {
		       
	        jQuery(item).removeClass("col-xs-offset-1").addClass("col-xs-offset-0").removeClass("reserve");
	        
	        jQuery(item).find(".position-message").addClass("position-message-squad").html("Tribuna").fadeIn().delay(500).fadeOut(500, function() {
	            jQuery(this).removeClass("position-message-squad")
	            jQuery(this).parent().find(".player-position span").addClass("hidden")
	            //jQuery(this).parent().find("player-position i.position").html("T")
	            //jQuery(this).parent().find("player-position i.position-class").removeClass("text-warning").addClass("text-success");
	            jQuery(this).parent().removeClass("col-xs-offset-0").addClass("col-xs-offset-1")
	        });

	        changePosition(jQuery(item).attr("data-id"), jQuery(item).attr("data-team"), jQuery(item).attr("data-competition"), jQuery(item).attr("data-role"), 0);
// 	        checkLineUp(_line_up);


// 			jQuery(item).one("swipeleft", function() {
// 		  		var _item = jQuery(this); 
// //	 	  		jQuery(_item).off("swipeleft");
// 		  		swipeleft(_item);
// 		  	});
	    }
	    //da tribuna a riserva
		else {
			jQuery(item).removeClass("col-xs-offset-1").addClass("col-xs-offset-0").addClass("reserve");
	        
	        jQuery(item).find(".position-message").addClass("position-message-reserve").html("Riserva").fadeIn().delay(500).fadeOut(500, function() {
	            jQuery(this).removeClass("position-message-reserve")
	            jQuery(this).parent().find(".player-position span").removeClass("hidden")
	            jQuery(this).parent().find(".player-position i.position").removeClass("text-success").addClass("text-warning").html("R")
	            jQuery(this).parent().find(".player-position i.position-class").removeClass("text-success").addClass("text-warning");
	            jQuery(this).parent().removeClass("col-xs-offset-0").addClass("col-xs-offset-1")
	        });

	        changePosition(jQuery(item).attr("data-id"), jQuery(item).attr("data-team"), jQuery(item).attr("data-competition"), jQuery(item).attr("data-role"), 2);
// 	        checkLineUp(_line_up);
	  	
		}

		jQuery(item).on("swipeleft", function() {
	  		var _item = jQuery(this); 
	  		jQuery(_item).off("swipeleft");
	  		swipeleft(_item);
	  	});
	}


	jQuery(".lineup-group-container-mobile .row div").on("swipeleft", function() {
		  var item = jQuery(this); 
		  jQuery(item).off("swipeleft");
		  swipeleft(item);
	});

	jQuery(".lineup-group-container-mobile .row div").on("swiperight", function() {
		  var item = jQuery(this); 
		  jQuery(item).off("swiperight");
		  swiperight(item);
	});
	
	jQuery("#tmp-lineup").val("" + JSON.stringify(_line_up));

	var checks = checkLineUp(_line_up);
	
	if (checks[0] && checks[1] && checks[2] && checks[3]) {
		jQuery("#step-1-go").removeAttr('disabled');
		//jQuery(window).unbind();
	} else
		jQuery("#step-1-go").attr('disabled', 'disabled');
});
</script>
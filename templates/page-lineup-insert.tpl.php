<!-- <script -->
<!-- 	src="/fantacazzismo2015/sites/all/modules/fantacalcio/js/jquery.mobile.custom.min.js"></script> -->
<!-- <script -->
<!-- 	src="/fantacazzismo2015/sites/all/modules/fantacalcio/js/jquery.ui.touch-punch.min.js"></script> -->

<style>
#check-lineup-nav.affix {
	position: fixed;
	top: 55px;
	width: 100%;
	z-index: 10;
}
</style>

<div id='div-formazione' class="">

	<div id='_formazione_form' class="col-xs-12 col-sm-10 hidden-xs">
		<?php print render($lineup_check); ?>
	</div>

	<div class="col-xs-12 col-sm-2 hidden-xs">
		<div class="row">
			<div class="col-xs-6 col-sm-12 col-md-12">
				<div class="hidden">
				<?php print render($lineup_form_clean); ?>
				</div>
				<button type="button" class="center-block btn btn-sm btn-warning"
					data-toggle="modal" data-target="#lineup-delete-modal">
				  <?php print t("Cancella");?>
				</button>
			</div>
			<?php if (count($confirm) > 0 ) : ?>
			<div class="col-xs-6 col-sm-12 col-md-12">
				<div class="hidden">
				<?php print render($lineup_form_confirm); ?>
			     </div>
				<button type="button" class="center-block btn btn-sm btn-primary"
					data-toggle="modal" data-target="#lineup-confirm-modal">
				  <?php print t("Conferma");?>
				</button>
			</div>
			<?php endif;?>
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
			<a data-toggle="modal" data-target="#lineup-info-modal"> <i
				class="fa fa-2x fa-info-circle"></i>
			</a>
			<div id="check-lineup-nav">
				<div class="navbar navbar-static">
					<div class="navbar-collapse">
      		        <?php print render($lineup_check); ?>
      	            </div>
				</div>
			</div>
			<div class="row">
			    <div class="col-xs-6">
 <?php if (count($confirm) > 0 ) : ?>
                        <div class="col-xs-6">
                                <button type="button" class="center-block btn btn-sm btn-primary"
                                        data-toggle="modal" data-target="#lineup-confirm-modal">
                                  <?php print t("Conferma");?>
                                </button>
                        </div>
                        <?php endif;?>
      		    </div>
      		    <div class="col-xs-6">
        		    <button id="step-1-back" type="button"
					class="center-block btn btn-sm btn-warning" data-toggle="modal"
					data-target="#lineup-delete-modal">
  	            <?php print t("Cancella");?>
  	            </button>
  	            </div>
			</div>
			<div class="row">
			    <div class="col-xs-6">&nbsp;</div>
			</div>
			<div class="col-xs-12">
				<div class="lineup-group-container-mobile">
    			<?php print render($squad_mobile);?>
    		    </div>

				<div class="row">
					<div class="col-xs-12">&nbsp;</div>
				</div>

				<div class="row">
					<div class="col-xs-6">
						<div class="center-block">

							<button type="button" class="center-block btn btn-default"
								data-toggle="modal" data-target="#lineup-reset-modal">
            				  <?php print t("Annulla");?>
            				</button>
						</div>
					</div>
					<div class="col-xs-6">
						<div class="center-block">
							<button id="step-1-go" type="button"
								class="center-block btn btn-success" disabled>
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
							<button id="step-2-back" type="button"
								class="center-block btn btn-default">
            				<?php print t("Indietro");?>
            				</button>
						</div>
					</div>
					<div class="col-xs-6">
						<div class="center-block">
							<button id="step-2-go" type="button"
								class="center-block btn btn-success">
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
							<button id="step-3-back" type="button"
								class="center-block btn btn-default">
            				<?php print t("Indietro");?>
            				</button>
						</div>
					</div>
					<div class="col-xs-6">
						<div class="center-block">
							<button id="step-3-go" type="button"
								class="center-block btn btn-success">
            				<?php print t("Conferma");?>
            				</button>
						</div>
					</div>
				</div>
			</div>
		</div>
	</div>
</div>

<?php if (count($confirm) > 0) : ?>
<div id='lineup-confirm-modal' class='modal' tabindex='-1'
	style='z-index: 2000' role='dialog' aria-hidden='true'>
	<div class="modal-dialog">
		<div class="modal-content">
			<div class="modal-header">
				<button type="button" class="close" data-dismiss="modal"
					aria-label="Close">
					<span aria-hidden="true">&times;</span>
				</button>
				<h4 class="modal-title"><?php print t("Conferma formazione"); ?></h4>
			</div>
			<div class="modal-body">
				<p><?php print t("Sei sicuro di voler confermare la formazione?"); ?></p>
				<p><?php print "(" . $confirm['competition'] ." - " . $confirm['round_label'] . ")"; ?></p>
			</div>
			<div class="modal-footer">
				<button type="button" class="btn btn-default" data-dismiss="modal"><?php print t("Annulla"); ?></button>
				<button type="button" class="btn btn-success"
					id="lineup-confirm-button"><?php print t("Ok"); ?></button>
			</div>
		</div>
		<!-- /.modal-content -->
	</div>
	<!-- /.modal-dialog -->
</div>
<!-- /.modal -->
<?php endif; ?>

<div id='lineup-reset-modal' class='modal' tabindex='-1'
	style='z-index: 2000' role='dialog' aria-hidden='true'>
	<div class="modal-dialog">
		<div class="modal-content">
			<div class="modal-header">
				<button type="button" class="close" data-dismiss="modal"
					aria-label="Close">
					<span aria-hidden="true">&times;</span>
				</button>
				<h4 class="modal-title"><?php print t("Annulla modifiche"); ?></h4>
			</div>
			<div class="modal-body">
				<p><?php print t("Sei sicuro di voler annullare le modifiche apportate?"); ?></p>
			</div>
			<div class="modal-footer">
				<button type="button" class="btn btn-default" data-dismiss="modal"><?php print t("Annulla"); ?></button>
				<button type="button" class="btn btn-success"
					id="lineup-reset-button"><?php print t("Ok"); ?></button>
			</div>
		</div>
		<!-- /.modal-content -->
	</div>
	<!-- /.modal-dialog -->
</div>
<!-- /.modal -->

<div id='lineup-delete-modal' class='modal' tabindex='-1'
	style='z-index: 2000' role='dialog' aria-hidden='true'>
	<div class="modal-dialog">
		<div class="modal-content">
			<div class="modal-header">
				<button type="button" class="close" data-dismiss="modal"
					aria-label="Close">
					<span aria-hidden="true">&times;</span>
				</button>
				<h4 class="modal-title"><?php print t("Cancella formazione"); ?></h4>
			</div>
			<div class="modal-body">
				<p><?php print t("Sei sicuro di voler cancellare la formazione?"); ?></p>
			</div>
			<div class="modal-footer">
				<button type="button" class="btn btn-default" data-dismiss="modal"><?php print t("Annulla"); ?></button>
				<button type="button" class="btn btn-success"
					id="lineup-delete-button"><?php print t("Ok"); ?></button>
			</div>
		</div>
		<!-- /.modal-content -->
	</div>
	<!-- /.modal-dialog -->
</div>
<!-- /.modal -->

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

<div id='lineup-info-modal' class='modal' tabindex='-1'
	style='z-index: 2000' role='dialog' aria-hidden='true'>
	<div class='modal-dialog'>
		<div class='modal-content'>
			<div class='modal-header'>
				<button type='button' class='close' data-dismiss='modal'
					aria-label='Close'>
					<span aria-hidden='true'>&times;</span>
				</button>
				<h4 class='modal-title'>Inserimento formazione</h4>
			</div>
			<div class='modal-body'>
				<p>Per inserire la formazione trascina i giocatori a destra o a
					sinistra.</p>
				<p>Sposta un giocatore a destra per inserirlo tra i titolari, a
					sinistra per inserirlo tra le riserve. Spostandolo di nuovo lo
					posizionerai in tribuna.</p>
				<p>Una volta sistemati tutti i giocatori potrai ordinare le riserve.</p>
				<br>
				<h4>Legenda</h4>
				<table class="table">
					<tr>
						<td><span class="fa fa-stack  "><i
								class="fa fa-stack-2x fa-circle-o position-class text-success"></i><i
								class="fa fa-stack-1x position text-success">T</i></span></td>
						<td>Giocatore aggiunto ai titolari</td>
					</tr>
					<tr>
						<td><span class="fa fa-stack  "><i
								class="fa fa-stack-2x fa-circle-o position-class text-warning"></i><i
								class="fa fa-stack-1x position text-warning">R</i></span></td>
						<td>Giocatore aggiunto alle riserve</td>
					</tr>
					<tr>
						<td colspan=2><hr /></td>
					</tr>
					<tr>
						<td><i class="fa fa-lg fa-circle text-success"></i></td>
						<td>Probabile titolare</td>
					</tr>
					<tr>
						<td><i class="fa fa-lg fa-circle text-warning"></i></td>
						<td>Probabile riserva</td>
					</tr>
					<tr>
						<td><i class="fa fa-lg fa-circle text-danger"></i></td>
						<td>Infortunato / Squalificato</td>
					</tr>

				</table>
			</div>
		</div>
	</div>
</div>


<script
	src="<?php print base_path() . drupal_get_path("module", "fantacalcio"); ?>/js/lineup_insert.js"></script>
<script>
/*jQuery(function() {
    jQuery( "#lineup-squad tbody, #lineup-regulars tbody, #lineup-reserves tbody" ).sortable({
      connectWith: ".lineup-group tbody"
    }).disableSelection();
  });*/

jQuery(function() {
	jQuery("#step-1-go").click(function () {
		jQuery("#step-1").addClass("hidden");
		jQuery("#step-2").removeClass("hidden");
        jQuery("body").scrollTop(0);
		prepareReserves();
	});
	jQuery("#step-2-go").click(function () {
		jQuery("#step-2").addClass("hidden");

		jQuery("#step-3").removeClass("hidden");
        jQuery("body").scrollTop(0);
        show_lineup_preview ();
	});

	jQuery("#step-3-go").click(function () {
		jQuery("#line_up_submit").trigger("click");
	});

	jQuery("#lineup-reset-button").click(function () {
		jQuery(window).unbind();
		location.reload();
	});

	jQuery("#lineup-delete-button").click(function () {
		jQuery("#line_up_clean").trigger("click");
	});

	jQuery("#lineup-confirm-button").click(function () {
		jQuery("#line_up_confirm").trigger("click");
	});
	
	jQuery("#step-2-back").click(function () {
		jQuery("#step-2").addClass("hidden");
		jQuery("#step-1").removeClass("hidden");
        jQuery("body").scrollTop(0);
	});
	jQuery("#step-3-back").click(function () {
		jQuery("#step-3").addClass("hidden");
		jQuery("#step-2").removeClass("hidden");
        jQuery("body").scrollTop(0);
	});
	// jQuery("#step-4-back").click(function () {
// 		jQuery("#step-4").addClass("hidden");
// 		jQuery("#step-3").removeClass("hidden");
// 	});
});

jQuery(document).ready(function() {

jQuery('#check-lineup-nav').affix({
      offset: {
        top: jQuery('header').height()
      }
});

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

// 	jQuery(".player-status a[data-toggle='popover']").popover()
	
	jQuery("#tmp-lineup").val("" + JSON.stringify(_line_up));

	var checks = checkLineUp(_line_up);
	
	if (checks[0] && checks[1] && checks[2] && checks[3]) {
		jQuery("#step-1-go").removeAttr('disabled');
		//jQuery(window).unbind();
	} else
		jQuery("#step-1-go").attr('disabled', 'disabled');
});
</script>

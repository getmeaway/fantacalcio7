<!-- Modal -->
<div class="modal fade" id="player-stats-modal" tabindex="-1" role="dialog" aria-labelledby="player-stats-modal-label" aria-hidden="true">
  <div class="modal-dialog">
    <div class="modal-content">
      <div class="modal-header">
        <button type="button" class="close" data-dismiss="modal" aria-label="Close">
        	<span aria-hidden="true"><i class="fa fa-times"></i></span>
        </button>
        <h4 class="modal-title" id="player-stats-modal-label">Modal title</h4>
      </div>
      <div class="modal-body">
        ...e sti cazzi
      </div>
      <div class="modal-footer">
        <p>a</p>
      </div>
    </div>
  </div>
</div>

<div class="row">

	<div class='container'>
		<div class='row'>
			<div class='col-xs-12 col-lg-12'>
				<ul class="nav nav-pills">
					<li role="presentation">
				  		<a href="<?php print base_path();?>mie/<?php print $t_id; ?>/dati"><?php print t("Dati"); ?></a>
				  	</li>
					<li role="presentation" class="active">
						<a href="<?php print base_path();?>mie/<?php print $t_id; ?>/rosa"><?php print t("Rosa"); ?></a>
					</li>
					<?php if (variable_get("fantacalcio_free_movements", 1)): ?>
				  	<li role="presentation">
				  		<a href="<?php print base_path();?>mie/<?php print $t_id; ?>/movimenti"><?php print t("Movimenti"); ?></a>
				  	</li>
				  	<?php endif; ?>
				  	<li role="presentation">
				  		<a href="<?php print base_path();?>mie/<?php print $t_id; ?>/lista-movimenti"><?php print t("Lista movimenti"); ?></a>
				  	</li>
				</ul>
			</div>
		</div>
		<div class='row'>
			<div class='col-md-6 col-lg-6'>&nbsp;</div>
		</div>
		<div class='row'>
			<div class='col-md-12 col-lg-12'>
				<?php print render($squad); ?>
			</div>
		</div>
	</div>
</div>

<script>

(function ($) {
	$('#player-stats-modal').prependTo("body");

// 	$("a.player-stats").click(function() {
// 		$('#player-stats-modal').modal({
// 	//		  keyboard: true,			  
// 			})
// 	});
	
}(jQuery));

</script>
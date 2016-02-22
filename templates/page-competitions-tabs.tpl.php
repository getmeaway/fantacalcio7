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
<script src="http://fantacazzismo.altervista.org/sites/all/modules/fantacalcio/js/jquery.ui.touch-punch.min.js?o19fva"></script>
<script src="http://fantacazzismo.altervista.org/sites/all/modules/fantacalcio/js/jquery.mobile.custom.min.js?o19fva"></script>
<script>/*
jQuery(document).ready(function(){
  // Bind the swipeleftHandler callback function to the swipe event on div.box
  jQuery( "div[role='tabpanel']" ).not(".tab-pane").on( "swipeleft", function() {
 	var panel = jQuery(this); 
	  jQuery(this).off("swipeleft");
	  swipeleft(panel);
  });

  jQuery( "div[role='tabpanel']" ).not(".tab-pane").on( "swiperight", function() {
	  var panel = jQuery(this); 
	  jQuery(this).off("swiperight");
	  swiperight(panel);
	});
});
	
function swiperight(panel) {
        var activeTab = jQuery(panel).find(".nav li.active");
        var prevTab = jQuery(activeTab).prev();
        if (prevTab)
                jQuery(prevTab).find("a").tab("show");
                
        jQuery(panel).on("swiperight", function() {
	  		var _panel = jQuery(this); 
	  		jQuery(_panel).off("swiperight");
	  		swiperight(_panel);
	  	});
  }
  
  function swipeleft(panel) {
        var activeTab = jQuery(panel).find(".nav li.active");
        var nextTab = jQuery(activeTab).next();
        if (nextTab)
                jQuery(nextTab).find("a").tab("show");
                
        jQuery(panel).on("swipeleft", function() {
	  		var _panel = jQuery(this); 
	  		jQuery(_panel).off("swipeleft");
	  		swipeleft(_panel);
	  	});
  }
*/
</script>

<div role="tabpanel">

	<?php if (isset($rounds_list)) : ?>
	<div class='navbar navbar-default'>
		<div class="container-fluid">
		
		<div class="navbar-header">
<!-- 		TODO creare dropdown con elenco giornate -->
      <button type="button" class="navbar-toggle collapsed" data-toggle="collapse" data-target="#bs-example-navbar-collapse-1">
        <span class="sr-only">Toggle navigation</span>
        <span class="icon-bar"></span>
        <span class="icon-bar"></span>
        <span class="icon-bar"></span>
      </button>
      <ul class="nav navbar-nav">
  			<?php if (isset($first_item)): ?>  
	  			<li class='round_list'><?php print l($first_item['value'], $url . "/" . $first_item['key']); ?></li>
  			<?php endif; ?>
  			</ul>
    </div>
		
		<div class='collapse navbar-collapse navbar-right round_list' id='round-list'>
  			<?php if (isset($rounds_list)) : ?>
  			<p class="navbar-text"><?php print t("Giornate"); ?>:</p>
  				<ul class="nav navbar-nav">
  			<?php foreach ($rounds_list as $round => $round_label): ?>
  	  			<li class='round_list'><?php print l($round_label, $url . "/" . $round); ?></li>
  			<?php endforeach; ?>
  				</ul>	
  			<?php endif; ?>  			
  		</div>
  		</div>
	</div>
	<?php endif; ?>

  <?php if (count($groups) > 1) : ?>
  <!-- Nav tabs -->
  <ul class="nav nav-pills" role="tablist">
  	<?php foreach ($groups as $g_id => $group ): ?>
    <li role="presentation" class="<?php print ($group->active ? "active" : "") ?>"><a href="#<?php print $g_id; ?>" aria-controls="<?php print $g_id; ?>" role="tab" data-toggle="tab"><?php print $group->name; ?></a></li>
    <?php endforeach; ?>
  </ul>
  <?php endif; ?>

  <?php if (count($groups) > 1) : ?>
  <!-- Tab panes -->
  <div class="tab-content">
  	<?php foreach ($groups as $g_id => $group ): ?>
    <div role="tabpanel" class="tab-pane <?php print ($group->active ? "active" : "") ?>"" id="<?php print $g_id; ?>">
    	<?php print render ($group->output); ?>
    </div>    
    <?php endforeach; ?>
  </div>
  <?php else: ?>
  	<?php foreach ($groups as $g_id => $group ): ?>
  		<?php print render ($groups[$g_id]->output); ?>
  	<?php endforeach; ?>
  <?php endif; ?>
  
</div>
<script src="http://fantacazzismo.altervista.org/sites/all/modules/fantacalcio/js/jquery.ui.touch-punch.min.js?o19fva"></script>
<script src="http://fantacazzismo.altervista.org/sites/all/modules/fantacalcio/js/jquery.mobile.custom.min.js?o19fva"></script>
<script>
/*jQuery(document).ready(function(){
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


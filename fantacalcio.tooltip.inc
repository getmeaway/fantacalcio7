<?php

function show_tooltip($field) {
			
	if(variable_get("fantacalcio_tooltip_" . $field, "") != "" )		
		return "<span class=\"fa-stack pull-right text-success\" data-toggle=\"tooltip\" data-placement=\"top\" title=\"" . variable_get("fantacalcio_tooltip_" . $field, "") . "\">						
				<i class=\"fa fa-circle-thin fa-stack-2x\"></i>
				<i class=\"fa fa-info fa-stack-1x\"></i>
			</span>";
		
	return "";
}
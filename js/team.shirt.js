jQuery(document).ready(function() {  
    
    jQuery('.swatch-clickable').each(function () 
    {
        jQuery(this).attr('title', jQuery(this).attr('id'));
    });

    jQuery('.swatch-clickable').tooltip();

    jQuery(".swatch-clickable").click(function() 
    {
        var color = jQuery(this).attr('id').replace("color-", "");
        jQuery(this).closest(".group-color").parent().find(".preview").css('color', color);
        jQuery(this).closest(".group-color").parent().parent().find(".color-val").val(color)
        var property = jQuery(this).closest(".group-color").parent().parent().find(".color-val").attr("name")
        myShirt[property] = color
        myShirt.draw();
    });
    
    jQuery(".shirt-type[data-type=" + jQuery("#type").val() + "]").addClass("active");
    jQuery(".preview-bg-color").css('color', jQuery("#bg-color").val());
    jQuery(".preview-main-color").css('color', jQuery("#main-color").val());
    jQuery(".preview-border-color").css('color', jQuery("#border-color").val());
    
    jQuery(".shirt-type").click(function() 
    {
        var type = jQuery(this).attr("data-type");
        jQuery("#type").val(type);
    });
	
  function shirt() {
      this.type = jQuery("#type").val()
      this.bgColor = jQuery("#bg-color").val()
      this.mainColor = jQuery("#main-color").val()
      this.border = jQuery("#border").val()
      this.borderColor = jQuery("#border-color").val()
  };
    
    var myShirt = new shirt();
    
  
  
  jQuery(".shirt-type").click(function() {
      var type = jQuery(this).attr("data-type");
        jQuery("#type").val(type);
    myShirt.type = type;
    myShirt.draw()
  })
  
  jQuery("#bg-color").change(function() {
    myShirt.bgColor = jQuery(this).val();
    myShirt.draw()
  })
  
  jQuery("#main-color").change(function() {
    myShirt.mainColor = jQuery(this).val();
    myShirt.draw()
  })
  
  jQuery("#border").click(function() {
      console.log(myShirt)
    myShirt.border = jQuery(this).is(":checked");
    myShirt.draw()
  })
  
  jQuery("#border-color").change(function() {
    myShirt.borderColor = jQuery(this).val();
    myShirt.draw()
  })

shirt.prototype.draw = function () {
    console.log(this)
    
  var canvas = document.getElementById('myCanvas');
  var context = canvas.getContext('2d');
  
  jQuery("#metadata").val(JSON.stringify(this))
  

  // begin custom shape
  context.beginPath();
  context.moveTo(140, 20);

  //collo
  context.quadraticCurveTo(170, 40, 200, 20);

  //manica sx
  context.lineTo(270, 30);
  context.lineTo(330, 100);
  context.lineTo(290, 140);
  context.lineTo(260, 105);

  //fianco sx
  context.quadraticCurveTo(250, 150, 260, 300);

  //vita
  context.quadraticCurveTo(170, 310, 80, 300);

  //fianco dx
  context.quadraticCurveTo(90, 150, 80, 105);

  //manica dx
  context.lineTo(50, 140);
  context.lineTo(10, 100);
  context.lineTo(70, 30);

  context.lineTo(140, 20);

  context.stroke();

  // complete custom shape
  //      context.closePath();
  context.lineWidth = 1;
  context.fillStyle = this.bgColor;
  context.fill();
  context.strokeStyle = "#666";
  context.stroke();
  
  //type
  var context2 = canvas.getContext('2d');
  context2.beginPath();
  
  if (this.type == 1) { //banda orizzontale centrale
    context2.moveTo(80, 105);
    context2.lineTo(260, 105);
    context2.quadraticCurveTo(256, 140, 255, 162);
    context2.lineTo(84, 162);
    context2.quadraticCurveTo(84, 140, 80, 105);
  }
  
  if (this.type == 2) { //banda verticale centrale
    context2.moveTo(140, 20);
    context.quadraticCurveTo(170, 40, 200, 20);
    context2.lineTo(200, 304);
    context2.quadraticCurveTo(170, 305, 140, 304);
    context2.lineTo(140, 20);    
  }
  
  if (this.type == 3) { //strisce verticali
  	context2.moveTo(80, 300);
    context.quadraticCurveTo(93, 150, 70, 30);
    context2.lineTo(115, 24);
    context2.lineTo(115, 303);
    
    context2.moveTo(150, 25);
    context.quadraticCurveTo(170, 35, 190, 25);
    context2.lineTo(190, 304);
    context2.quadraticCurveTo(170, 305, 150, 304);
    context2.lineTo(150, 25);
    
    context2.moveTo(260, 300);
    context.quadraticCurveTo(247, 150, 270, 30);
    context2.lineTo(225, 24);
    context2.lineTo(225, 303);
  }
    
  if (this.type == 4) { //strisce orizzontali
  	context2.moveTo(80, 300);
    context.quadraticCurveTo(93, 150, 70, 30);
    context2.moveTo(260, 300);
    context.quadraticCurveTo(247, 150, 270, 30); 
    context2.lineWidth = 1;
    context2.strokeStyle = "#666";
    context2.stroke();
  	
    context.beginPath();
    context2.moveTo(80, 105);
    context2.lineTo(260, 105);
    context2.quadraticCurveTo(255, 140, 256, 147);
    context2.lineTo(85, 147);
    context2.quadraticCurveTo(85, 140, 80, 105);
    
    context2.moveTo(85, 185);
    context2.lineTo(255, 185);
    context2.quadraticCurveTo(256, 190, 256, 227);
    context2.lineTo(84, 227);
    context2.quadraticCurveTo(84, 190, 85, 185);
    
    context2.moveTo(82, 265);
    context2.lineTo(258, 265);
    context2.quadraticCurveTo(259, 290, 260, 300);
    context.quadraticCurveTo(170, 310, 80, 300);
    context2.quadraticCurveTo(84, 190, 85, 185);
    context2.lineWidth = 1;
    context2.fillStyle = this.mainColor;
    context2.fill();
    context2.strokeStyle = "#666";
    context2.stroke();
    
    context.beginPath();
    context.lineTo(270, 30);
 		context.quadraticCurveTo(265, 50, 264, 70);
    context.lineTo(76, 70);
    context.lineTo(70, 30);
  	context.lineTo(140, 20);
    context.quadraticCurveTo(170, 40, 200, 20);
    context2.lineWidth = 1;
    context2.fillStyle = this.mainColor;
    context2.fill();
    context2.strokeStyle = "#666";
    context2.stroke();
  }
  
  if (this.type == 5) { //banda diagonale dx top
  	context.moveTo(70, 30);
    context.lineTo(100, 26);
    context.lineTo(260, 300);
    context.quadraticCurveTo(250, 302, 230, 303); 
    context.lineTo(70, 30);
  }
  
  if (this.type == 6) { //banda diagonale sx top
  	context.moveTo(270, 30);
    context.lineTo(240, 26);
    context.lineTo(80, 300);
    context.quadraticCurveTo(90, 302, 110, 303); 
    context.lineTo(270, 30);
  }
  
  if (this.type == 7) { //croce centrale
  	context2.moveTo(140, 20);
    context.quadraticCurveTo(170, 40, 200, 20);
    context2.lineTo(200, 105);
    context2.lineTo(260, 105);
    context2.quadraticCurveTo(256, 140, 255, 162);
    context2.lineTo(200, 162);
    context2.lineTo(200, 304);
    context2.quadraticCurveTo(170, 305, 140, 304);
    context2.lineTo(140, 162);
    context2.lineTo(84, 162);
    context2.quadraticCurveTo(84, 140, 80, 105);
    context2.lineTo(140, 105);
    context2.lineTo(140, 20);
  }
  
  if (this.type == 8) { //quadratoni 1-3
  	context2.moveTo(80, 300);
    context.quadraticCurveTo(93, 150, 70, 30);
    context2.moveTo(260, 300);
    context.quadraticCurveTo(247, 150, 270, 30); 
    context2.lineWidth = 1;
    context2.strokeStyle = "#666";
    context2.stroke();
  	
    context.beginPath();
    context.moveTo(70, 30);
    context.lineTo(140, 20);
    context.quadraticCurveTo(145, 28, 170, 30); 
    context.lineTo(170, 305);
    context.quadraticCurveTo(200, 303, 260, 300);
    context.quadraticCurveTo(257, 240, 255, 170);
    context.lineTo(85, 170);
    context.quadraticCurveTo(85, 100, 70, 30);
  }
  
  if (this.type == 9) { //quadratoni 2-4
  	context2.moveTo(80, 300);
    context.quadraticCurveTo(93, 150, 70, 30);
    context2.moveTo(260, 300);
    context.quadraticCurveTo(247, 150, 270, 30); 
    context2.lineWidth = 1;
    context2.strokeStyle = "#666";
    context2.stroke();
  	
    context.beginPath();
    context.moveTo(270, 30);
    context.lineTo(200, 20);
    context.quadraticCurveTo(195, 28, 170, 30); 
    context.lineTo(170, 305);
    context.quadraticCurveTo(120, 303, 80, 300);
    context.quadraticCurveTo(87, 240, 85, 170);
    context.lineTo(255, 170);
    context.quadraticCurveTo(255, 100, 270, 30);
  }
  
  context2.lineWidth = 1;
  context2.fillStyle = this.mainColor;
  context2.fill();
  context2.strokeStyle = "#666";
  context2.stroke();
  
  if (this.border !== undefined && this.border) {
    var context3 = canvas.getContext('2d');
    context3.beginPath();
    
  	context3.moveTo(321, 90);
    context3.lineTo(330, 100);
  	context3.lineTo(290, 140);
  	context3.lineTo(281, 130);
    context3.lineTo(321, 90);
    
    context3.moveTo(19, 90);
    context3.lineTo(10, 100);
  	context3.lineTo(50, 140);
  	context3.lineTo(59, 130);
    context3.lineTo(19, 90);
    
    context.moveTo(140, 20);
		context.quadraticCurveTo(170, 40, 200, 20);
    context3.lineTo(212, 21);
    context.quadraticCurveTo(170, 55, 128, 21);
    context.lineTo(140, 20);
    
    context3.lineWidth = 1;
  	context3.fillStyle = this.borderColor;
	  context3.fill();
	  context3.strokeStyle = "#666";
  	context3.stroke();
  }
  
	jQuery("#image").val(canvas.toDataURL())  
}

myShirt.draw();
  
})

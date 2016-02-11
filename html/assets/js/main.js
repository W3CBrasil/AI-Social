$(function() {	
	//Tabs
	$('.nav-tabs .dropdown-menu a').click(function(){		
		$('.nav-tabs .dropdown-menu li').removeClass('active');
	});

	// Accordion
	var down = "sprite-accordion-down-arrow";
	var up = "sprite-accordion-up-arrow";
	$('.accordion-arrow').addClass(down);
	$('.panel-title a').click(function(){		
		$(this).find('.accordion-arrow').toggleClass(down);		
		$(this).find('.accordion-arrow').toggleClass(up);
	});
});

$(document).ready(function() {
    $("div.bhoechie-tab-menu>div.list-group>a").click(function(e) {
        e.preventDefault();
        $(this).siblings('a.active').removeClass("active");
        $(this).addClass("active");
        var index = $(this).index();
        $("div.bhoechie-tab>div.bhoechie-tab-content").removeClass("active");
        $("div.bhoechie-tab>div.bhoechie-tab-content").eq(index).addClass("active");
    });
});

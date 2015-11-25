var margin = {top: 30, right: 10, bottom: 10, left: 300},
    width = 1024 - margin.left - margin.right,
    height = 7000 - margin.top - margin.bottom;

var x = d3.scale.linear()
    .range([0, width]);


var y = d3.scale.ordinal()
    .rangeRoundBands([0, height], .1);

var xAxis = d3.svg.axis()
    .scale(x)
    .orient("top")
    .tickSize(-height);

var yAxis = d3.svg.axis()
    .scale(y)
    .orient("left")
    .tickSize(0);
    
var comentariosParticipantes = [];

var svg = d3.select("#container-graph").append("svg")
    .attr("width", width + margin.left + margin.right)
    .attr("height", height + margin.top + margin.bottom)
  .append("g")
    .attr("transform", "translate(" + margin.left + "," + margin.top + ")");
    
d3.tsv('data/comentarios/texto_comentario_participantes.tsv', function(data){
 	comentariosParticipantes.push(data);
 	
return {
  	id: data.id,
  	name: data.comentario,
  };
}, function(error, rows) {
  //console.log(rows);
});


d3.csv("data/comentario_participantes_bar_chart.csv", function(error, data) {

  data.forEach(function(d) { d.value = +d.value; });

  x.domain([0, d3.max(data, function(d) { return d.value; })]);
  y.domain(data.map(function(d) { return d.name; }));
  
  var bar = svg.selectAll("g.bar")
      .data(data)
    .enter().append("g")
      .attr("class", "bar")
      .attr("transform", function(d) { return "translate(0," + y(d.name) + ")"; });

  bar.append("rect")
      .attr("width", function(d) { return x(d.value); })
      .attr("height", y.rangeBand())
      .style("cursor", "pointer")
      .on('click', function(d){
  	  	
  	  	var comentario = "";
  	  		
  	    comentariosParticipantes.forEach(function(item){
  	    	
  	    	if (item.id == d.id)
  	    		comentario += "<p class='comentario'>" + item.comentario + "</p>";

  	    });
  	    
  	    exibeComentarios(comentario);
  	  	$("#comentariosModalDialog").modal("show");
  	  
  	  });

  bar.append("text")
      .attr("class", "value")
      .attr("x", function(d) { return 8; })
      .attr("y", y.rangeBand() / 2)
      .attr("dx", -3)
      .attr("dy", ".35em")
      .attr("text-anchor", function(d){ return (d.value > 1)? "start": "middle"; } )
      .text(function(d) { 
      		
      		return d.value; 
      }).style("cursor", "pointer");

  svg.append("g")
      .attr("class", "x axis")
      .call(xAxis);

  svg.append("g")
      .attr("class", "y axis")
      .call(yAxis);
/*
  svg.append("g")
      .attr("class", "x axis")
      .attr("transform", "translate(0," + height + ")")
      .call(xAxis)
       .selectAll("text")	
            .style("text-anchor", "end")
            .attr("dx", "-.8em")
            .attr("dy", ".15em");

  svg.append("g")
      .attr("class", "y axis")
      .call(yAxis)
    .append("text")
      .attr("transform", "rotate(-90)")
      .attr("y", 6)
      .attr("dy", ".71em")
      .style("text-anchor", "end")
      .text("Frequency");

  svg.selectAll(".bar")
      .data(data)
    .enter().append("rect")
      .attr("class", "bar")
      .attr("x", function(d) { return x(d.name); })
      .attr("width", x.rangeBand())
      .attr("y", function(d) { return y(d.value); })
      .attr("height", function(d) { return height - y(d.value); });

  d3.select("input").on("change", change);

  var sortTimeout = setTimeout(function() {
    d3.select("input").property("checked", true).each(change);
  }, 2000);

  function change() {
    clearTimeout(sortTimeout);

    // Copy-on-write since tweens are evaluated after a delay.
    var x0 = x.domain(data.sort(this.checked
        ? function(a, b) { return b.value - a.value; }
        : function(a, b) { return d3.ascending(a.name, b.name); })
        .map(function(d) { return d.name; }))
        .copy();

    svg.selectAll(".bar")
        .sort(function(a, b) { return x0(a.name) - x0(b.name); });

    var transition = svg.transition().duration(750),
        delay = function(d, i) { return i * 50; };

    transition.selectAll(".bar")
        .delay(delay)
        .attr("x", function(d) { return x0(d.name); });

    transition.select(".x.axis")
        .call(xAxis)
      .selectAll("g")
        .delay(delay);
  }
  
  */
 
 function exibeComentarios(comentario) {
	
  	if ($('.modal-body').has('p').length) {
  			
  		$( ".modal-body" ).empty();	  	
  		$(comentario).appendTo(".modal-body");
  		
  	} else{
  		$(comentario).appendTo(".modal-body");
  	};
  	  	  
} // Fim da função exibeComentarios

  
  
});


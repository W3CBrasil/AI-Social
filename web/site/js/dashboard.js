

$(document).ready(function() {
    console.log("ready!");
    
    CriaComentariosItensBarChart();
    CriaComentariosTemporalLineChart();
    CriaComentariosParticipantesBarChart();
    CriaGrafoComentarioParticipantes();
});
    
    


function CriaGrafoComentarioParticipantes() {
	
	
	var width = $("#comentarios_participantes_graph").parent().width(),
	    height = 280;// 178

	var trans = [0, 0];
	var scale = 1;

	var color = d3.scale.ordinal().domain([0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13])
		.range(["#383838", "#1F77B4", "#FF7F0E", "#2CA02C", "#3C5C87", "#D62728", "#8C564B", "#9467BD", "#8B3E6B", "#E377C2", "#3ee4c4", "#3C7587", "#BCBD22", "#17BECF"]);

	// #345065

	var force = d3.layout.force()
		.charge(-1000)
		.linkDistance(200)
		.size([width, height]);

	var svg = d3.select("#comentarios_participantes_graph")
		.append("svg")
		.attr("width", width)
		.attr("height", height)
		.attr("pointer-events", "all")
		.append('svg:g')
			.call(d3.behavior.zoom()
		.scaleExtent([.1, 3])
			.on("zoom", redraw)).append('svg:g'); 
			
var tip = d3.tip()
	.attr('class', 'd3-tip')
    .offset([-10, 0])
    .html(function(d) {
    		
    var type = d.type;
    var cor = "style=\'color:"+ color(d.group) + "\'";
    var eixo = "";
    var grupo = +d.group;
    
    
    switch(grupo) {
    	case 1:
    		eixo = "Escopo e aplicação";
    		break;
    	case 2:
    		eixo = "Dados pessoais, dados anônimos e dados sensíveis";
    		break;
    	case 3:
    		eixo = "Princípios";
    		break;
    	case 4:
    		eixo = "Consentimento";
    		break;
    	case 5:
    		eixo = "Término do tratamento";
    		break;
    	case 6:
    		eixo = "Direitos do titular";
    		break;
    	case 7:
    		eixo = "Comunicação, interconexão e uso compartilhado de dados";
    		break;
    	case 8:
    		eixo = "Transferência Internacional de dados";
    		break;
    	case 9:
    		eixo = "Responsabilidade dos agentes";
    		break;
    	case 10:
    		eixo = "Segurança e sigilo de dados pessoais";
    		break;
    	case 11:
    		eixo = "Boas Práticas";
    		break;
    	case 12:
    		eixo = "Como assegurar estes direitos, garantias e deveres?";
    		break;
    	case 13:
    		eixo = "Disposições Transitórias";
    		break;
    	default:
    		break;
    		
    }
    
        
    var textoParticipante = "<span id='titulo-tip'> Participante </span><br /> <br /> <div>Nome: <br /> <span>" + d.name + "</span> </div> <div>Comentários: <br /> <span>" + d.value + "</span> </div>";
    var textoLei = "<span id='titulo-tip'>" + eixo + "</span></div><div>Comentários: <br /> <span>" + d.value + "</span> </div>";
    

    if (type == 'user') {
    	return textoParticipante;
    } else {
    	return textoLei;
    }
    
    	
  }); 
  
   svg.call(tip);			
			
		
			
function redraw() {
   trans = d3.event.translate;
   scale = d3.event.scale;
   svg.attr("transform", "translate(" + trans + ")" + " scale(" + scale + ")");
}
			
d3.json("data/dashboard/participantes_eixos_graph.json", function(error, graph) {
  if (error) throw error;

  force.nodes(graph.nodes)
       .links(graph.links)
       .start();

  var link = svg.selectAll(".link")
      .data(graph.links)
    .enter().append("line")
      .attr("class", "link")
      .style("stroke", function(d){
      	
      	return (d.value > 1)? "#BABABA" : "#d7d7d7";
      	
      	
      })
      .style("stroke-width", function(d) { return Math.sqrt(d.value); })
      .style("stroke-opacity", function(d){return (d.value > 1)? 1: 0.7; });
      
      
	//Toggle stores whether the highlighting is on
	var toggle = 0;
	//Create an array logging what is connected to what
	var linkedByIndex = {};
	for ( i = 0; i < graph.nodes.length; i++) {
		linkedByIndex[i + "," + i] = 1;
	};
	graph.links.forEach(function(d) {
		linkedByIndex[d.source.index + "," + d.target.index] = 1;
	});
	//This function looks up whether a pair are neighbours
	function neighboring(a, b) {
		return linkedByIndex[a.index + "," + b.index];
	}

	function connectedNodes() {
		if (toggle == 0) {
			//Reduce the opacity of all but the neighbouring nodes
			d = d3.select(this).node().__data__;
			node.style("opacity", function(o) {
				return neighboring(d, o) | neighboring(o, d) ? 1 : 0.1;
			});
			link.style("opacity", function(o) {
				return d.index == o.source.index | d.index == o.target.index ? 1 : 0.1;
			});

			//Reduce the op
			toggle = 1;
		} else {
			//Put them back to opacity=1
			node.style("opacity", 1);
			link.style("opacity", 1);
			toggle = 0;
		}
	}

  var defaultRadius = 10;
 
  var node = svg.selectAll(".node")
      .data(graph.nodes)
    .enter().append("circle")
      .attr("id", function(d){ return d.id; })
      .attr("class", function(d){ return "node " + d.type; })
      .attr("id", function(d){ return d.id; })
      .attr("r", function(d) {return Math.log(d.value) ? Math.sqrt(d.value) + defaultRadius : defaultRadius;})
      .style("fill", function(d) { 
      	   
      	   if (d.id == "TCE0")
                return "#FFF";
           else 
           		return color(d.group); 
      	})
      	.style("stroke", function (d){
      	
      	   if (d.id == "TCE0") 
      	   		return "#383838";
      	   else
      	   		return "#FFF";
      })
      .call(force.drag)
      .on('click', connectedNodes)
      .on ("mouseover", tip.show)
      .on ("mouseout", tip.hide);
  
  /*
  node.append("title")
      .text(function(d) { return d.name; });
    */  
 
      
 resize();
 d3.select(window).on("resize", resize);

  force.on("tick", function() {
    link.attr("x1", function(d) { return d.source.x; })
        .attr("y1", function(d) { return d.source.y; })
        .attr("x2", function(d) { return d.target.x; })
        .attr("y2", function(d) { return d.target.y; });

    node.attr("cx", function(d) { return d.x; })
        .attr("cy", function(d) { return d.y; });
        
  });
  
  
  function resize() {
    width = window.innerWidth, height = window.innerHeight;
    svg.attr("width", width).attr("height", height);
    force.size([width, height]).resume();
  }
  
  
  
  
  
});



	
} // fim da função CriaGrafoComentarioParticipantes

function CriaComentariosItensBarChart(){
	
var margin = {top: 2, right: 2, bottom: 2, left: 2},
    width = 350 - margin.left - margin.right,
    height = 150 - margin.top - margin.bottom;

var x = d3.scale.ordinal()
    .rangeRoundBands([0, width], .1);

var y = d3.scale.linear()
    .range([height, 0]);

var xAxis = d3.svg.axis()
    .scale(x)
    .orient("bottom");

var yAxis = d3.svg.axis()
    .scale(y)
    .orient("left");

var color = d3.scale.ordinal()
	.domain([0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12])
    .range(["#1F77B4", "#FF7F0E", "#2CA02C", "#3C5C87", "#D62728", "#8C564B", "#9467BD", "#8B3E6B", "#E377C2", "#3ee4c4", "#3C7587", "#BCBD22", "#17BECF"]);



var svg = d3.select("#comentarios_itens_bar_chart").append("svg")
    .attr("width", width + margin.left + margin.right)
    .attr("height", height + margin.top + margin.bottom)
  .append("g")
    .attr("transform", "translate(" + margin.left + "," + margin.top + ")");
    
    
    
    
    
    
 var tip = d3.tip()
	.attr('class', 'd3-tip')
    .offset([-15, 0])
    .html(function(d) {
    		
    var eixo = "";
    var grupo = d.group;
    
    switch(grupo) {
    	case 1:
    		eixo = "Escopo e aplicação";
    		break;
    	case 2:
    		eixo = "Dados Pessoais, Dados Anônimos e Dados Sensíveis";
    		break;
    	case 3:
    		eixo = "Princípios";
    		break;
    	case 4:
    		eixo = "Consentimento";
    		break;
    	case 5:
    		eixo = "Término do Tratamento";
    		break;
    	case 6:
    		eixo = "Direitos do Titular";
    		break;
    	case 7:
    		eixo = "Comunicação, Interconexão e Uso Compartilhado de Dados";
    		break;
    	case 8:
    		eixo = "Transferência Internacional de Dados";
    		break;
    	case 9:
    		eixo = "Responsabilidade dos Agentes";
    		break;
    	case 10:
    		eixo = "Segurança e Sigilo de Dados Pessoais";
    		break;
    	case 11:
    		eixo = "Boas Práticas";
    		break;
    	case 12:
    		eixo = "Como assegurar estes direitos, garantias e deveres?";
    		break;
    	case 13:
    		eixo = "Disposições Transitórias";
    		break;
    	default:
    		break;
    		
    }
    
   
    var textoLei = "<span id='titulo-tip'>" + eixo + "</span></div><div>Comentários: <br /> <span>" + d.value + "</span> </div>";
    

    return textoLei;
    
    	
  }); 
    
    
svg.call(tip); 

var comentariosEixos = [];


 d3.tsv('data/comentarios/texto_comentario_eixos.tsv', function(data){
 	
 	data.forEach(function(d){
 		comentariosEixos.push(data);	
 	});
 	  
 });


d3.csv("data/dashboard/comentarios_eixos_bar_chart_dashboard.csv", function(error, data) {
	
  data.forEach(function(d) { 
  	d.value = +d.value;
  	d.group = +d.group; 
  });

  x.domain(data.map(function(d) { return d.name; }));
  y.domain([0, d3.max(data, function(d) { return d.value; })]);
  
var bar = svg.selectAll("g")
      .data(data)
    .enter().append("g")
      .attr("transform", function(d) { return "translate(" + x(d.name) + ",0)"; });
  
      
bar.append("rect")
	.attr("id", function(d){ return d.id; })
  	.attr("class", "bar")
  	//.attr("x", function(d){ return x(d.name); })
  	.attr("y", function(d){ return y(d.value); })
  	.attr("height", function(d){ return height - y(d.value); })
  	.attr("width", x.rangeBand())
  	.style("fill", function(d) { 
      	   
      	   if (d.name == "ANTEPROJETO DE LEI" || d.name == "Dispõe sobre o trata" || d.name == "A PRESIDENTA DA REPÚBLICA")
                return "#FFF";
           else 
           		return color(d.group); 
     })
     .style("stroke", function (d){
      	
      	   if (d.name == "ANTEPROJETO DE LEI" || d.name == "Dispõe sobre o trata" || d.name == "A PRESIDENTA DA REPÚBLICA") 
      	   		return "#383838";
      	   else
      	   		return "#FFF";
      })
      .on('mouseover', tip.show)
  	  .on('mouseout', tip.hide);
  	  /*
  	  .on('click', function(d){
  	  	
  	  	
  	  	
  	  	var comentario = "<p class='comentario'> </p> <br/>";
  	  	//exibeComentarios(comentario);
  	  	
  	  	
  	  	comentariosEixos.forEach(function(comentarioEixo){
  	  		
  	  		console.log(comentarioEixo.id);
  	  		
  	  		if (comentarioEixo.id == d.id) {
					 	
				comentario += "<p class='comentario'>" + comentarioEixo.comentario + "</p><br/>\n";
				
					 	
			 };
  	  		
  	  	});
  	  	exibeComentarios(comentario);
  	  	$("#comentariosModalDialog").modal("show");
  	  	
  	  	
  	  	/*
  	  	for (var i=0; i < comentariosEixos.length; i++) {
				 
			 if (comentariosEixos[i].id == d.id) {
					 	
				comentario = "<p class='comentario'>" + d.comentario + "</p>";
				exibeComentarios(comentario);
					 	
			 };
		
		} // fim for var i = 0
		
 
 	 });*/
      
      
   bar.append("text")
      .attr("x", function(d){ return (d.value >= 100) ? 1 : 5;})
      .attr("y", function(d) { return (d.value >= 60) ? 132 : 133; })
      .attr("dy", ".75em")
      .attr("text-anchor", function(d){ return (d.value > 1)? "start": "middle"; } )
      .style("fill", function(d){
      	
      	if (d.name == "ANTEPROJETO DE LEI" || d.name == "Dispõe sobre o trata" || d.name == "A PRESIDENTA DA REPÚBLICA")
                return "#000";
        else if(d.value >= 4)
        	return "#FFF";
        else
        	return "#000";
      	
      })
      .style("cursor", "pointer")
      .attr("font-size", 12)
      .text(function(d) { return d.value; })
      .on('mouseover', tip.show)
  	  .on('mouseout', tip.hide);


	
	});
} // Fim da função CriaComentariosItem

function CriaComentariosTemporalLineChart(){
	
var margin = {top: 2, right: 2, bottom: 2, left: 2},
    width = 350 - margin.left - margin.right,
    height = 150 - margin.top - margin.bottom;

var parseDate = d3.time.format("%Y-%m-%d").parse;

var x = d3.time.scale()
    .range([0, width]);

var y = d3.scale.linear()
    .range([height, 0]);

var xAxis = d3.svg.axis()
    .scale(x)
    .orient("bottom");

var yAxis = d3.svg.axis()
    .scale(y)
    .orient("left");

var line = d3.svg.line()
    .x(function(d) { return x(d.date); })
    .y(function(d) { return y(d.freq); });

var svg = d3.select("#comentarios_temporal_line_chart").append("svg")
    .attr("width", width + margin.left + margin.right)
    .attr("height", height + margin.top + margin.bottom)
  .append("g")
    .attr("transform", "translate(" + margin.left + "," + margin.top + ")");
    
d3.tsv("data/dashboard/comentarios_temporal_line_chart.tsv", function(error, data) {
  
  
  if (error) throw error;



  data.forEach(function(d) {
    d.date = parseDate(d.date);
    d.freq = +d.freq;
  });
  
  
	// extract the x labels for the axis and scale domain
	var xLabels = data.map(function (d) { return d['date']; });
	
	x.domain(d3.extent(data, function(d) { return d.date; }));
    y.domain(d3.extent(data, function(d) { return d.freq; }));
	
	
	//x.domain(xLabels);
	//y.domain([0, Math.round(d3.max(data, function(d) { return parseFloat(d['freq']); }))]);
		
	var line = d3.svg.line()
		.x(function(d) { return x(d['date']); })
		.y(function(d) { return y(d['freq']); });

/*
  svg.append("g")
      .attr("class", "x axis")
      .attr("transform", "translate(0," + height + ")")
      .call(xAxis);

  svg.append("g")
      .attr("class", "y axis")
      .call(yAxis)
    .append("text")
      .attr("transform", "rotate(-90)")
      .attr("y", 6)
      .attr("dy", ".71em")
      .style("text-anchor", "end")
      .text("Comentários");*/

  svg.append("path")
      .datum(data)
      .attr("class", "line")
      .attr("d", line);
      
		
		
		// get the x and y values for least squares
		var xSeries = d3.range(1, xLabels.length + 1);
		var ySeries = data.map(function(d) { return parseFloat(d['freq']); });
		
		var leastSquaresCoeff = leastSquares(xSeries, ySeries);
		
		// apply the reults of the least squares regression
		var x1 = xLabels[0];
		var y1 = leastSquaresCoeff[0] + leastSquaresCoeff[1];
		var x2 = xLabels[xLabels.length - 1];
		var y2 = leastSquaresCoeff[0] * xSeries.length + leastSquaresCoeff[1];
		var trendData = [[x1,y1,x2,y2]];
		
		var trendline = svg.selectAll(".trendline")
			.data(trendData);
			
		trendline.enter()
			.append("line")
			.attr("class", "trendline")
			.attr("x1", function(d) { return x(d[0]); })
			.attr("y1", function(d) { return y(d[1]); })
			.attr("x2", function(d) { return x(d[2]); })
			.attr("y2", function(d) { return y(d[3]); })
			.attr("stroke", "#d62728")
			.attr("stroke-width", 2);      
});

} // Fim da Função CriaComentariosTemporalLineChart


// returns slope, intercept and r-square of the line
	function leastSquares(xSeries, ySeries) {
		var reduceSumFunc = function(prev, cur) { return prev + cur; };
		
		var xBar = xSeries.reduce(reduceSumFunc) * 1.0 / xSeries.length;
		var yBar = ySeries.reduce(reduceSumFunc) * 1.0 / ySeries.length;

		var ssXX = xSeries.map(function(d) { return Math.pow(d - xBar, 2); })
			.reduce(reduceSumFunc);
		
		var ssYY = ySeries.map(function(d) { return Math.pow(d - yBar, 2); })
			.reduce(reduceSumFunc);
			
		var ssXY = xSeries.map(function(d, i) { return (d - xBar) * (ySeries[i] - yBar); })
			.reduce(reduceSumFunc);
			
		var slope = ssXY / ssXX;
		var intercept = yBar - (xBar * slope);
		var rSquare = Math.pow(ssXY, 2) / (ssXX * ssYY);
		
		return [slope, intercept, rSquare];
		
} // Fim da função leastSquares

function CriaComentariosParticipantesBarChart(){
	
var margin = {top: 2, right: 2, bottom: 2, left: 2},
    width = 350 - margin.left - margin.right,
    height = 150 - margin.top - margin.bottom;

var x = d3.scale.ordinal()
    .rangeRoundBands([0, width], .1);

var y = d3.scale.linear()
    .range([height, 0]);

var xAxis = d3.svg.axis()
    .scale(x)
    .orient("bottom");

var yAxis = d3.svg.axis()
    .scale(y)
    .orient("left");

var color = d3.scale.ordinal()
	.domain([0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12])
    .range(["#1F77B4", "#FF7F0E", "#2CA02C", "#3C5C87", "#D62728", "#8C564B", "#9467BD", "#8B3E6B", "#E377C2", "#3ee4c4", "#3C7587", "#BCBD22", "#17BECF"]);


var svg = d3.select("#comentarios_participantes_bar_chart").append("svg")
    .attr("width", width + margin.left + margin.right)
    .attr("height", height + margin.top + margin.bottom)
  .append("g")
    .attr("transform", "translate(" + margin.left + "," + margin.top + ")");
    
    
var tip = d3.tip()
	.attr('class', 'd3-tip')
    .offset([-15, 0])
    .html(function(d) {
    	
   
    var textoLei = "<span id='titulo-tip'>" + d.name + "</span></div><div>Comentários: <br /> <span>" + d.value + "</span> </div>";
    

    return textoLei;
    
    	
  }); 
    
tip.direction(function(d){
	return (d.value > 35)? 'e': 'n';
});
   
svg.call(tip); 
    
    
d3.csv("data/dashboard/comentario_participantes_bar_chart.csv", function(error, data) {
	
  data.forEach(function(d) { 
  	d.value = +d.value;
  	d.group = +d.group; 
  });

  x.domain(data.map(function(d) { return d.name; }));
  y.domain([0, d3.max(data, function(d) { return d.value; })]);
  
var bar = svg.selectAll("g")
      .data(data)
    .enter().append("g")
      .attr("transform", function(d) { return "translate(" + x(d.name) + ",0)"; });
  
      
bar.append("rect")
    .attr("id", function(d){ return d.id; })
  	.attr("class", "bar")
  	//.attr("x", function(d){ return x(d.name); })
  	.attr("y", function(d){ return y(d.value); })
  	.attr("height", function(d){ return height - y(d.value); })
  	.attr("width", x.rangeBand())
  	.style("fill", "#383838")
  	.on('mouseover', tip.show)
  	.on('mouseout', tip.hide);
  	
  	/*
  	.on('click', function(d){
  		
  		
  		var comentario = "<p class='comentario'>" + d.id + "</p>";
  		
  		exibeComentarios(comentario);
 
 	 });*/
      
      
   bar.append("text")
      .attr("x", function(d){ return (d.value > 100) ? 3 : 8; })
      .attr("y", 128)
      .attr("dy", ".75em")
      .attr("text-anchor", function(d){ return (d.value > 1)? "start": "middle"; } )
      .style("fill", "#FFF")
      .style("cursor", "pointer")
      .text(function(d) { return d.value; })
      .on('mouseover', tip.show)
  	  .on('mouseout', tip.hide);
      
      /*.on('click', function(d){
  		
  		var comentario = "<p class='comentario'>" + d.value + "</p>";
  		
  		exibeComentarios(comentario);
 
 	 });*/
      


	
	});
} // Fim da função CriaComentariosItem



function exibeComentarios(comentario) {
	
	 
  		
  	if ($('.modal-body').has('p').length) {
  			
  		$( ".modal-body" ).empty();
  		$(comentario).appendTo(".modal-body");
  			
  	} else{
  		$(comentario).appendTo(".modal-body");
  	};
  	
  	
  	  
} // Fim da função exibeComentarios



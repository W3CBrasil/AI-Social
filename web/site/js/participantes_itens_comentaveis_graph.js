var width = 960,
    height = 758;

width = window.innerWidth, height = window.innerHeight;

var trans = [0, 0];
var scale = 1;

var color = d3.scale.ordinal().domain([0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13])
    .range(["#383838", "#1F77B4", "#FF7F0E", "#2CA02C", "#3C5C87", "#D62728", "#8C564B", "#9467BD", "#8B3E6B", "#E377C2", "#3ee4c4", "#3C7587", "#BCBD22", "#17BECF"]);

// #345065

var comentariosItensLei = [];
var comentariosParticipantes = [];
    
var force = d3.layout.force()
    .charge(-3000)
    .linkDistance(550)
    .size([width, height]);

var svg = d3.select("#container-graph").append("svg")
    .attr("width", width)
    .attr("height", height)
    .attr("pointer-events", "all")
    .append('svg:g')
    	.call(d3.behavior.zoom().scaleExtent([.1, 3]).on("zoom", redraw))
    .append('svg:g');


var svgLegenda = d3.select("#container-legenda").append("svg")
    .attr("width", 220)
    .attr("height", 545)
    .append('svg:g')
    	.attr("transform", "translate(0, 100)");


var legenda = svgLegenda.append("rect")
	.attr("width", 220)
	.attr("height", 545)
	.attr("x", 20)
	.attr("y", -37)
	.attr("class", "legenda")
	.style("fill", "#CDCDCD");
		   
var tip = d3.tip()
	.attr('class', 'd3-tip')
    .offset([-10, 0])
    .html(function(d) {
    		
    var type = d.type;
    var cor = "style=\'color:"+ color(d.group) + "\'";
    var eixo = "";
    var grupo = d.group;
    
    
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
    var textoLei = "<span id='titulo-tip'> Item de Lei </span><br /> <br /> <div>Nome: <br /> <span>" + d.name + "</span></div>" + "</span> </div>" + "<div>Eixo: <br /> <span>" + eixo + "</span></div><div>Comentários: <br /> <span>" + d.value + "</span> </div>";
    

    if (type == 'user') {
    	return textoParticipante;
    } else {
    	return textoLei;
    }
    
    	
  }); 
    
var legendRectSize = 18;
var legendSpacing = 4;


var legend = svgLegenda.selectAll('.legend')
	.data(["Participantes", "Arts. 1º ao 4º", "Arts. 5º, 12º e 13º", "Art. 6º", "Arts. 7º ao 11º", "Arts. 14º e 15º", "Arts. 16º ao 21º", "Arts. 22º ao 27º", "Arts. 28º ao 33º", "Arts. 34º ao 41º", "Arts. 42º ao 47º", "Arts. 48º ao 49º", "Art. 50º", "Arts. 51º ao 52º"])
  	.enter().append('g')
  	.attr("class", 'legend')
  	.attr("transform", function(d, i) { return "translate(50," + (i * 30) + ")"; });

legend.append('rect')
  .attr('width', legendRectSize)
  .attr('height', legendRectSize)
  .style('fill', color)
  .style('stroke', color);
  
  
legend.append("text")
  .attr('x', legendRectSize + legendSpacing)
  .attr('y', legendRectSize - legendSpacing)
  .attr('fill', "#383838")
  .text(function(d) { return d; });




    svg.call(tip);


    
function redraw() {
   trans = d3.event.translate;
   scale = d3.event.scale;
   svg.attr("transform", "translate(" + trans + ")" + " scale(" + scale + ")");
}


d3.tsv('data/comentarios/texto_comentario_participantes.tsv', function(data){
 	comentariosParticipantes.push(data);
 	
return {
  	id: data.id,
  	name: data.comentario,
  };
}, function(error, rows) {
  //console.log(rows);
});


d3.tsv('data/comentarios/texto_comentario_itens.tsv', function(data){
 	comentariosItensLei.push(data);
 	
return {
  	id: data.id,
  	name: data.comentario,
  };
}, function(error, rows) {
  //console.log(rows);
});












d3.json("data/participantes_itens_comentaveis_graph.json", function(error, graph) {
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
      .style("stroke-width", function(d) { return d.value; })
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

  var defaultRadius = 5;
 
  var node = svg.selectAll(".node")
      .data(graph.nodes)
    .enter().append("circle")
      .attr("class", function(d){ return "node " + d.type; })
      .attr("id", function(d){ return d.id; })
      .attr("r", function(d) {return d.value ? d.value + defaultRadius : defaultRadius;})
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
      .call(force.drag)
      .on('click', connectedNodes)
      .on ("mouseover", tip.show)
      .on ("mouseout", tip.hide)  	  
      .on('dblclick', function(d){
  	  	
  	  	var comentario = "";
  	  	
		if (d.type == 'user') {
			
			comentariosParticipantes.forEach(function(item){
  	    	
  	    		if (item.id == d.id)
  	    			comentario += "<p class='comentario'>" + item.comentario + "</p>";

  	    	});
  	    
  	    	exibeComentarios(comentario);
  	  		$("#comentariosModalDialog").modal("show");

		} else {
			
			comentariosItensLei.forEach(function(item){
  	    		
  	    		if (item.id == d.id)
  	    			comentario += "<p class='comentario'>" + item.comentario + "</p>";

  	    	});
  	    
  	    	exibeComentarios(comentario);
  	  		$("#comentariosModalDialog").modal("show");
		
		} // Fim if/else

  	  		

  	  
  	  });;
  
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


function searchNode() {
	
    //find the node
    var selectedVal = document.getElementById('search').value;
    
    var node = svg.selectAll(".node");
    if (selectedVal == "none") {
        node.style("stroke", "white").style("stroke-width", "1");
    } else {
        var selected = node.filter(function (d, i) {
        	return d.name.toLowerCase() != selectedVal.toLowerCase();
        });
        selected.style("opacity", "0");
        var link = svg.selectAll(".link");
        link.style("opacity", "0");
        d3.selectAll(".node, .link").transition()
            .duration(5000)
            .style("opacity", 1);
    }
  }
  
  
  $(document).keypress(function(event) {
    var keycode = (event.keyCode ? event.keyCode : event.which);
    if (keycode == '13') {
        $('#botaoPesquisar').click();
    }
});




function exibeComentarios(comentario) {
	
  	if ($('.modal-body').has('p').length) {
  			
  		$( ".modal-body" ).empty();	  	
  		$(comentario).appendTo(".modal-body");
  		
  	} else{
  		$(comentario).appendTo(".modal-body");
  	};
  	  	  
} // Fim da função exibeComentarios






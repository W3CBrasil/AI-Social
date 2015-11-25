$(document).ready(function(){
	
 



d3.json("data/tabela_topicos.json", function(error, data) {
	
	if (error) throw error;
	
	var topicos = data.topicos;
	
	topicos.forEach(function(d){
		d.contagem_comentarios = +d.contagem_comentarios;
	});
	
	
	
	
	topicos.sort(function(a, b) { return d3.descending(a.contagem_comentarios, b.contagem_comentarios); });
	
	
	topicos.forEach(function(d, i){
		
		
		var palavras = "";
				

		$("#tabela_topicos").find('tbody')
			.append($('<tr>')
				
				
				.append($('<td>').append($('<input>').attr('type', "text").attr('class', 'form-control').attr('placeholder', "Nome do Tópico")))
				.append($('<td>').append($('<div>').attr('class', 'container-celula').attr('class', 'text-right frequencia').text(d.contagem_comentarios)))
				
				
				
				
				// ADICIONA TERMOS
				.append($('<td>').append($('<div>').attr('class', 'container-celula').attr('id', "termos-" + i))) // Fim de adciona termos
				
				
				
				.append($('<td>').append($('<div>').attr('class', 'container-grafico').attr('id', "topico-" + i)))
				
				
		);
		
		
		d.top_palavras.forEach(function(p, j) {
			
			$('#termos-' + i)
				.append($('<span>').attr('class', 'termo').text(
				
				
				p[0]
				
			).css('font-size', function(d){
				
					var valor = Math.ceil((p[1] * 100)) + 12;
					
					console.log(valor);
					
					return valor;
				
				
				
				})
			
			
			);
			
			//palavras += " " + p[0] + ", ";	
		});
		
		
		//CriaTermos("termos-" + i, d.top_palavras);
		CriaComentariosItensBarChart("topico-" + i, d.frequencia_artigos, i);
		
			
		
		});
		
	});
	
	CriaLegenda();

});


function CriaLegenda() {
	
var color = d3.scale.ordinal()
	.domain([0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12])
    .range(["#1F77B4", "#FF7F0E", "#2CA02C", "#3C5C87", "#D62728", "#8C564B", "#9467BD", "#8B3E6B", "#E377C2", "#3ee4c4", "#3C7587", "#BCBD22", "#17BECF"]);
	
var svgLegenda = d3.select("#container-legenda").append("svg")
    .attr("width", 220)
    .attr("height", 545)
    .append('svg:g')
    	.attr("transform", "translate(0, 100)");


var legenda = svgLegenda.append("rect")
	.attr("width", 200)
	.attr("height", 450)
	.attr("x", 20)
	.attr("y", -37)
	.attr("fill-opacity", 0.7)
	.attr("class", "legenda")
	.style("fill", "#ECECEB");
	
var legendRectSize = 18;
var legendSpacing = 4;


var legend = svgLegenda.selectAll('.legend')
	.data(["Arts. 1º ao 4º", "Arts. 5º, 12º e 13º", "Art. 6º", "Arts. 7º ao 11º", "Arts. 14º e 15º", "Arts. 16º ao 21º", "Arts. 22º ao 27º", "Arts. 28º ao 33º", "Arts. 34º ao 41º", "Arts. 42º ao 47º", "Arts. 48º ao 49º", "Art. 50º", "Arts. 51º ao 52º"])
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


	
} // Fim do método Cria Legenda


function CriaTermos(idContainer, dados){
	
	var margin = {top: 0, right: 0, bottom: 0, left: 0},
    	width = 300 - margin.left - margin.right,
    	height = 12 - margin.top - margin.bottom;
    	
    var svg = d3.select("#" + idContainer).append("svg")
    			.attr("preserveAspectRatio", "xMinYMin meet") 
    			.attr("viewBox","0 0 " + (width + margin.left + margin.right) + " " + (height + margin.top + margin.bottom)) 
				  	.append("g")
    			.attr("transform", "translate(" + margin.left + "," + margin.top + ")");
    			
    dados.forEach(function(d){
    	
    	
    	svg.append("text").attr('font-size', 10).text(d[0]);
    	
    	
    	
    });
	
}

function CriaComentariosItensBarChart(idContainer, dados){
	
	
	
var margin = {top: 2, right: 2, bottom: 2, left: 2},
    width = 90 - margin.left - margin.right,
    height = 35 - margin.top - margin.bottom;

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



var svg = d3.select("#" + idContainer).append("svg")
    .attr("preserveAspectRatio", "xMinYMin meet")	// Way to Go!! 
    	.attr("viewBox","0 0 " + (width + margin.left + margin.right) + " " + (height + margin.top + margin.bottom)) 
  	.append("g")
    	.attr("transform", "translate(" + margin.left + "," + margin.top + ")");

dados.forEach(function(d) {
	
	

  x.domain(dados.map(function(d) { return d[0]; }));
  y.domain([0, d3.max(dados, function(d) { return d[2]; })]);
  
var bar = svg.selectAll("g")
      .data(dados)
    .enter().append("g")
      .attr("transform", function(d) { return "translate(" + x(d[0]) + ",0)"; });
  
      
bar.append("rect")
  	//.attr("x", function(d){ return x(d[0]); })
  	
  	.style("fill", function(d) {
  		
  		
  		
  		var idGrupo = d[1];
  		var grupo = +idGrupo.substring(3);
     
      	   if (d[0] == "ANTEPROJETO DE LEI" || d[0] == "Dispõe sobre o trata" || d[0] == "A PRESIDENTA DA REPÚBLICA")
                return "#FFF";
           else 
           		return color(d[1]); 
   	})
  	.attr("y", function(d){ return y(d[2]); })
  	.attr("height", function(d){ return height - y(d[2]); })
  	.transition().ease('in')
        .delay(function (d, i) { return i * 100; })
  	.attr("width", x.rangeBand());
        
	
	});
} // Fim da função CriaComentariosItem












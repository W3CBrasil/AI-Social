var cor = d3.scale.category20();

var corTermos = d3.scale.linear()
            .domain([2, 4, 6, 8, 10])
    		.range(['#252525', '#636363', '#969696', '#cccccc', '#f7f7f7']);



const DIRETORIO_BUSCA = "../calaca/";
const TERMO = "comment_text.acento:";
const ID_TOPICO = "topico_id:";
const ID_EIXO = "commentable_axis:";

const TAMANHO_FONTE = 12;


////////////////////////////////////////////
// Região de Pré-processamento dos Dados // 
//////////////////////////////////////////

// Carrega os Dados
//d3.json("data/dados_topicos.json", function(error, data) {
d3.json("dados.json", function(error, data) {
	
	if (error) throw error;
	
	var topicos = data;
	
	topicos.forEach(function(d){ d.contagem_comentarios = +d.contagem_comentarios; });
	topicos.sort(function(a, b) { return d3.descending(a.contagem_comentarios, b.contagem_comentarios); });
	
	main(topicos);

});


function main(dados) {
	
	CriaLegenda("#legenda-dashboard");
	CriaTabelaTopicos(dados);
	
	dados.forEach(function(d, i){
		
		CriaBarras(d.frequencia_eixos, "#topico-" + (i + 1));	
		
	});
	
	DesenhaBarChart();
	
} // Fim do método Main



//////////////////////////////////////////
// Região de Criação das Visualizações // 
////////////////////////////////////////

function CriaLegenda(container) {
	
	var legendRectSize = 18;
	var legendSpacing = 4;
	
	var tamanho = TamanhoSVG(container);
	
	var svg = d3.select(container).append("svg")
		.attr("viewBox", "0 0 "+ tamanho[0] + " " + 380)
		.attr("preserveAspectRatio", "xMidYMid meet");
		
		
	svg.selectAll('.legend')
		.data(["Arts. 1º ao 4º", "Arts. 5º, 12º e 13º", "Art. 6º", "Arts. 7º ao 11º", "Arts. 14º e 15º", "Arts. 16º ao 21º", "Arts. 22º ao 27º", "Arts. 28º ao 33º", "Arts. 34º ao 41º", "Arts. 42º ao 47º", "Arts. 48º ao 49º", "Art. 50º", "Arts. 51º ao 52º"])
  		.enter().append('g')
  		.attr("class", 'legend')
  		.attr("transform", function(d, i) { return "translate(10," + (i * 30) + ")"; })
  		.append('rect')
  		.attr('width', legendRectSize)
  		.attr('height', legendRectSize)
  		.style('fill', function(d, i) { return cor(i); })
  		.style('stroke', function(d, i) { return (i == 0) ? "#383838" : cor(i); } );
  		
  		
  svg.selectAll('.legend').append("text")
  		.attr('x', legendRectSize + legendSpacing)
  		.attr('y', legendRectSize - legendSpacing)
  		.attr('fill', "#FFFFFF")
  		.text(function(d) { return d; });;	

}




function CriaTabelaTopicos(dados) {

	var tr = d3.select("tbody")
		.selectAll("tr")
			.data(dados)
		.enter()
		.append("tr");


	tr.append("td")
		.append("a")
			.attr("class", "termo")
			.attr("href", function(d){  return DIRETORIO_BUSCA + ID_TOPICO + d.topico_id; })
			.attr("target", "_blank")
			.text(function(d){ return d.nome;});
	
	
	tr.append("td")
		.append("div")
			.attr("id", function(d, i){ return "topico-" + (i + 1); })
			.attr("class", "container-grafico");
	

	tr.append("td")
		.append("div")
			.attr('class', 'container-celula text-right frequencia')
			.text(function(d) { return d.contagem_comentarios; });

	
	tr.append("td")
		.append("div")
			.attr("id", function(d, i){ return "termo-" + (i + 1); })
			.attr("class", "container-celula celula-termo")
		.selectAll(".celula-termo")
		.data(function(d) {
			
			
			var topPalavras = d.top_palavras;
			var termosDescritivos = [];
			var n = topPalavras.length;
			
			topPalavras.forEach(function(e, i){
				
				termosDescritivos[i] = d3.range(1).map(function(){
					return {termo:e, topico:d.topico_id };
				});
				
				
			});	
			
			return termosDescritivos; 
			
			
		}).enter()
		.append("a")
			.attr("class", "termo")
			.attr("href", function(d, i){  return DIRETORIO_BUSCA + TERMO + d[0].termo[0] + " AND " + ID_TOPICO + d[0].topico; })
			.attr("target", "_blank")
			.style("font-size", function(d){ return (d[0].termo[1] * 100) + TAMANHO_FONTE + "px"; })
			.transition().duration(3000)
         	.ease("in")
			.text(function(d){ return d[0].termo[0];});
			
	

} // Fim do método CriaPlanilha


function CriaBarras(dadosSelecionados, container) {
	
	var tamanho = TamanhoSVG(container);
	var svg = d3.select(container).append("svg")
		.attr("viewBox", "0 0 "+ tamanho[0] + " " + tamanho[1])
		.attr("preserveAspectRatio", "xMidYMid meet");
	
	
	svg.selectAll(".bar")
		.data(dadosSelecionados)
		.enter()
		.append("rect")
		.on("click", Click);
	
} // Fim do método CriaBarras




//////////////////////////////////////////
// Região de Desenho das Visualizações // 
////////////////////////////////////////



function AtualizaVisualizacao() {
	
	DesenhaBarChart();
	OrdenaDecrescente();
	
} // fim do método atualizaVisualizacao


/* Obtém o tamanho da imagem SVG */
function TamanhoSVG(container) {
	
	var width = parseFloat(d3.select(container).node().clientWidth);
	var height = parseFloat(d3.select(container).node().clientHeight);
	
	return [width, height];
	
} // Fim do método TamanhoSVG


function DesenhaBarChart() {
	
	
	d3.select("tbody")
		.selectAll("tr svg").data()
		.forEach(function(d, i){
			
			var container = "#topico-" + (i + 1);
			var numeroDeBarras = d3.select(container).selectAll("rect").size();
			var dados = d3.select(container).selectAll("rect").data();
			
			var valorMax = d3.max(dados, function(d){ return d[1]; });
			var tamanho = TamanhoSVG(container);

			
			var x = d3.scale.ordinal()
				.domain(dados.map(function(d){ return d[0]; }))
				.rangeRoundBands([0, tamanho[0]], .15);

			var y = d3.scale.linear()
				.domain([0, valorMax])
				.range([tamanho[1], 0]);
	
			d3.select(container)
				.selectAll("rect")
				.style("fill", function(d) { 
					
					 return (d.key == "0")? "#FFFFFF" : cor(d[0]); 
				
				
				})
				.style("stroke", function(d) { return (d.key == "0") ? "#333333" : cor(d[0]); })
				.style("opacity", 1)
				.style("shape-rendering", "crispEdges")
				.attr("width", function(){ return x.rangeBand(); })
				.attr("x", function(d){ return x(d[0]); })
				.attr("y", function(d){ return tamanho[1]; })
				.attr("height", 0)
				.transition()
				.delay(function(d, i) { return i * 100; })
				.attr("y", function(d){ return y(d[1]); })
				.attr("height", function(d) { return tamanho[1] - y(d[1]); });
		 
		});
	 
} // Fim do método desenha barras



//////////////////////////////////////////
///////// Região dos Eventos ////////////  
////////////////////////////////////////

//window.onresize = function(event) { DesenhaVisualizacao(); };


$(document).ready(function(){
    $('[data-toggle="tooltip"]').tooltip();   
});



function Click(elemento) {

	window.open(DIRETORIO_BUSCA + ID_EIXO + elemento[0], "_blank");

} // Fim do método Click



function AbreAjuda() {
	$("#comentariosModalDialog").show();
}

//////////////////////////////////////////
///////// Região das Ordenações /////////  
////////////////////////////////////////


function OrdenaCrescente() {
	
	
	d3.select("tbody")
		.selectAll("tr")
		.sort(function(a, b){ return a.contagem_comentarios - b.contagem_comentarios;});

	
} // Fim do método OrdenaCrescente


function OrdenaDecrescente() {
	
	d3.select("tbody")
		.selectAll("tr")
		.sort(function(a, b){ return b.contagem_comentarios - a.contagem_comentarios;});
		
} // Fim do método OrdenaDecrescente



function OrdenaAlfCrescente() {
		
	d3.select("tbody")
		.selectAll("tr")
		.sort(function(a, b){ return a.topico_id - b.topico_id; });
	
} // Fim do método OrdenaCrescente



function OrdenaAlfDecrescente() {
	
	d3.select("tbody")
		.selectAll("tr")
		.sort(function(a, b){ return b.topico_id - a.topico_id;});
		
} // Fim do método OrdenaDecrescente










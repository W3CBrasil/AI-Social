





var margin = {top: 80, right: 180, bottom: 120, left: 500},
    width = 1280,
    height = 2048;   

var x = d3.scale.ordinal(),
	y = d3.scale.ordinal(),
    z = d3.scale.linear().domain([0, 4]).clamp(true),
    cor = d3.scale.ordinal().domain(d3.range(0, 14)),
    rotuloLinha = d3.scale.linear(),
    rotuloColuna = d3.scale.linear();


var paleta = ["#FFFFFF", "#1F77B4", "#FF7F0E", "#2CA02C", "#3C5C87", "#D62728", "#8C564B", "#9467BD", "#8B3E6B", "#E377C2", "#3ee4c4", "#3C7587", "#BCBD22", "#17BECF"];
var paletaFonte = ['#252525',  '#cccccc']; 

var matriz = [];
var linhas = [];
var colunas = [];

var m = 0;
var n = 0;
var dimensao = 0;
var tamanhoGrid;

var tipCelula;

var ordenacaoLinha = { };
var ordenacaoColuna = { };



var cor = d3.scale.category20();

var corTermos = d3.scale.linear()
            .domain([2, 4, 6, 8, 10])
    		.range(['#252525', '#636363', '#969696', '#cccccc', '#f7f7f7']);


var escalaCor = d3.scale.linear()
	.domain([0, 25, 50, 75, 100])
    .range(['#d7191c','#fdae61','#ffffbf','#abd9e9','#2c7bb6']);


const DIRETORIO_BUSCA = "../calaca/";
const TERMO = "comment_text:";
const ID_PARTICIPANTE = "author_id:";
const ID_COMENTAVEL = "commentable_id:";

const TAMANHO_FONTE = 12;


////////////////////////////////////////////
// Região de Pré-processamento dos Dados // 
//////////////////////////////////////////

// Carrega os Dados
//d3.json("data/dados_sentimento.json", function(error, data) {
d3.json("dados.json", function(error, data) {
	if (error) throw error;
	main(data);
});


function main(dados) {
	
	CriaLegenda("#legenda-dashboard");
		
	var participantes = dados.media_participante;
	var itens = dados.media_item;
	var dadosMatriz = dados.media_matriz; 
	

	ProcessaDados(dadosMatriz);
	CriaMatriz();
	DesenhaMatriz();
	
	
	CriaTabelaParticipantes("#sentimento-participantes", participantes);
	CriaTabelaComentavel("#sentimento-itens", itens);
	
	
	
	
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
		
		
		
	svg.append("text").text("oi");	
		
	svg.selectAll('.legend')
		.data([100, 75, 50, 25, 0])
  		.enter().append('g')
  		.attr("class", 'legend')
  		.attr("transform", function(d, i) { return "translate(10," + (i * 30) + ")"; })
  		.append('rect')
  		.attr('width', legendRectSize)
  		.attr('height', legendRectSize)
  		.style('fill', function(d, i) { return escalaCor(d); });
  		
  		
  		
  svg.selectAll('.legend').append("text")
  		.attr('x', legendRectSize + legendSpacing)
  		.attr('y', legendRectSize - legendSpacing)
  		.attr('fill', "#FFFFFF")
  		.text(function(d) { return d + "%"; });	


}


function CriaTabelaParticipantes(container, dados) {
	
	var tr = d3.select(container)
		.select("tbody")
		.selectAll("tr")
			.data(dados)
		.enter()
		.append("tr");
		
		
	tr.append("td")
		.append("a")
			.attr("class", "termo")
			.attr("href", function(d){  return DIRETORIO_BUSCA + ID_PARTICIPANTE + d.author_id; })
			.attr("target", "_blank")
			.text(function(d){ return d.author_name;});

			
	tr.append("td")
		.append("div")
			.attr('class', 'container-celula text-right frequencia')
			.text(function(d) { return (d.sentiment_avg * 100).toFixed(2).replace(".", ","); });	

		
	tr.append("td")
		.append("div")
			.attr("id", function(d, i){ return "participante-" + (i + 1); })
			.attr("class", "container-barra-positividade")
			.attr("class", "bar-participante");		
			
	var svg = d3.selectAll(".bar-participante").append("svg")
			.attr("viewBox", "0 0 "+ 100 + " " + 30)
			.attr("preserveAspectRatio", "xMidYMid meet");
			
					
    	svg.append("rect")
			.attr("x", 0)
			.attr("y", 10)
			.attr("width", 100)
			.attr("height", 30)
			.style("fill", function(d) {return "#D6D6D6"; })
			.style("shape-rendering", "crispEdges");	
    		
		svg.append("rect")
			.attr("x", 0)
			.attr("y", 10)
			.attr("width", 0)
			.transition().duration(800)
			.attr("width", function(d) { return d.sentiment_avg * 100; })
			.attr("height", 30)
			.style("fill", function(d) {return escalaCor(d.sentiment_avg*100); })
			.style("shape-rendering", "crispEdges");		

}


function CriaTabelaComentavel(container, dados) {


	var tr = d3.select(container)
		.select("tbody")
		.selectAll("tr")
			.data(dados)
		.enter()
		.append("tr");
		
		
	tr.append("td")
		.append("a")
			.attr("class", "termo")
			.attr("href", function(d){  return DIRETORIO_BUSCA + ID_COMENTAVEL + d.commentable_id; })
			.attr("target", "_blank")
			.text(function(d){ return d.commentable_name;});
	
		
	tr.append("td")
		.append("div")
			.attr('class', 'container-celula text-right frequencia')
			.text(function(d) { return (d.sentiment_avg * 100).toFixed(2).replace(".", ","); });
			

	tr.append("td")
		.append("div")
			.attr("id", function(d, i){ return "item-comentavel-" + (i + 1); })
			.attr("class", "container-barra-positividade")
			.attr("class", "bar-item-comentavel");		


	var svg = d3.selectAll(".bar-item-comentavel").append("svg")
			.attr("viewBox", "0 0 "+ 100 + " " + 30)
			.attr("preserveAspectRatio", "xMidYMid meet");
			
					
    	svg.append("rect")
			.attr("x", 0)
			.attr("y", 10)
			.attr("width", 100)
			.attr("height", 30)
			.style("fill", function(d) {return "#D6D6D6"; })
			.style("shape-rendering", "crispEdges");	
    		
		svg.append("rect")
			.attr("x", 0)
			.attr("y", 10)
			.attr("width", 0)
			.transition().duration(800)
			.attr("width", function(d) { return d.sentiment_avg * 100; })
			.attr("height", 30)
			.style("fill", function(d) {return escalaCor(d.sentiment_avg*100); })
			.style("shape-rendering", "crispEdges");
	
}










function ProcessaDados(dados) {
	
	var links = dados.links;
	linhas = dados.row;
	colunas = dados.column;
	matriz = [];


	m = linhas.length,
	n = colunas.length,
	dimensao = m * n;
	tamanhoGrid = Math.floor(width / n);


	// Cria matriz
	for (var i=0; i < m ; i++)
		matriz[i] = d3.range(n).map(function(j){ return {x: j, y: i, z: -1}; });
	   	
	
	// Indexa linhas e colunas
	linhas.forEach(function(d, i){	
		d.index = i;
		d.count = 0;
	});
	
	colunas.forEach(function(d, i){
		d.index = i;
		d.count = 0;	
	});
	

	// Cria relacionamentos na matriz e conta as ocorrências de relcionamentos
	links.forEach(function(d){	
		matriz[d.source][d.target].z = (d.value * 100);			
		linhas[d.source].count += (d.value * 100);	
		colunas[d.target].count += (d.value * 100);
	});


	// Precomputa ordenações d3.ascending(colunas[b].name, colunas[a].name)
	ordenacaoColuna = {
		name: d3.range(n).sort(function(a, b){  return d3.ascending(colunas[b].name, colunas[a].name); }),
		count: d3.range(n).sort(function(a, b){ return colunas[a].count - colunas[b].count; }),
		group: d3.range(n).sort(function(a, b){ return colunas[a].group - colunas[b].group; })
	};
	
	ordenacaoLinha = {
		name: d3.range(m).sort(function(a, b){ return d3.ascending(linhas[b].name, linhas[a].name); }),
		count: d3.range(m).sort(function(a, b){ return linhas[a].count - linhas[b].count; })
	};
	
	
	// Ordenação Padrão
	x.domain(ordenacaoColuna.count);
	y.domain(ordenacaoLinha.count);
	

	
	paletaFonte.sort(function(a, b) { return d3.hsl(b).l - d3.hsl(a).l; });	
	paleta.sort(function(a, b) { return d3.hsl(b).h - d3.hsl(a).h; });
	cor.range(paleta);
	
}



function CriaMatriz() {
	
	var tamanho = TamanhoSVG("#container-matriz");
	
	width = tamanho[0]  - margin.left - margin.right;
	height = tamanho[1]  - margin.top - margin.bottom;
	
	width = ((670 * n) / 34);
	height = ((1888 * m) / 110);
	
	
	
	d3.select("#container-matriz").style("height", (height+ margin.top + margin.bottom) + "px");
	
	
	tamanhoGrid =  Math.floor(width / n);
	
	x.rangeRoundBands([0, (width)], .1);
	y.rangeRoundBands([height, 0], .1);

	svg = d3.select("#container-matriz").append("svg")
			.attr("viewBox", "0 0 "+  (width + margin.left + margin.right) + " " + (height + margin.top + margin.bottom))
			.attr("preserveAspectRatio", "xMidYMid meet")
		.append("g")
    		.attr("transform", "translate(" + (margin.left) + "," + (margin.top) + ")");
	
} // Fim do método CriaMatriz



function DesenhaMatriz() {
	
	
	
	tipCelula = d3.tip().attr('class', 'd3-tip')
		.offset([-10, 0])
		.html(function(d) {
			
			var nomeItemLei = colunas[d.x].name; 
			var nomeParticipante = linhas[d.y].name;
			var valor = d.z;
			
			if (d.z > -1){
				return "<div>Item de Lei: <br /> <span>" + nomeItemLei + "</span></div>" + 
				"<div>Participante: <br /> <span>" + nomeParticipante + "</span></div>" +
				"<div>Positividade: <br /> <span>" + valor.toFixed(2).replace(".", ",") + "%" + "</span></div>";	
			} else {
			
				return "<div>Item de Lei: <br /> <span>" + nomeItemLei + "</span></div>" + 
				"<div>Participante: <br /> <span>" + nomeParticipante + "</span></div>" +
				"<div>Positividade: <br /> <span>" + "Nenhuma"+ "</span></div>";
			
			}

			
		});
    
    
    svg.call(tipCelula);
    

    var linha = svg.selectAll(".row")
    		.data(linhas);
    		
    
    // Atualiza elementos antigos se necessário		
    linha.transition().duration(750)
      		.attr("id", function(d){ return "row-" + d.index; })
      		.attr("transform", function(d, i) { return "translate(0," + y(i) + ")"; })
      		.each(PreencheLinha); 		
    
    linha.selectAll("text")
    	.attr("y", y.rangeBand() / 2);
    
    
    // Cria novos elementos caso precise		
    linha.enter().append("g")
      		.attr("class", "row")
      		.attr("id", function(d){ return "row-" + d.index; })
      		.attr("transform", function(d, i) { return "translate(0," + y(i) + ")"; })
      		.each(PreencheLinha)
      	.append("text")
    		.attr("x", -6)
    		.attr("y", y.rangeBand() / 2)
    		.attr("dy", ".32em")
    		.attr("text-anchor", "end")
    		.attr("fill", function(d){ return rotuloLinha(d.count); })
    		.text(function(d, i){ return d.name; });
    	
    	
    	
    // Remove elementos antigos  		
    linha.exit().transition().duration(300)
		.attr("x", 80)
		.style("fill-opacity", 1e-6)
    	.remove();  		
      		

   var coluna = svg.selectAll(".column")
   		.data(colunas);
   	
   	
   	
   	
   	// Atualiza elementos antigos se necessário			
    coluna.transition().duration(750)
    	.attr("id", function(d){ return d.index; })
    	.attr("transform", function(d, i) { return "translate(" + x(i) + ")rotate(-90)"; });
 
    coluna.selectAll("text")
    	.attr("y", x.rangeBand() / 2);	

    	
    
   	// Cria novos elementos caso precise		
   	coluna.enter().append("g")
   		.attr("id", function(d){ return d.index; })   
   		.attr("class", "column")
   		.attr("transform", function(d, i) { return "translate(" + x(i) + ")rotate(-90)"; })
	.append("text")
    	.attr("x", 6)
    	.attr("y", x.rangeBand() / 2)
    	.attr("dy", ".32em")
    	.attr("text-anchor", "start")
    	.text(function(d, i){ return d.name; });
    	
    	
    coluna.exit().transition().duration(300)
    	.attr("x", 80)
		.style("fill-opacity", 1e-6)
    	.remove();  		

	

} // Fim do método DesenhaMatriz


function PreencheLinha(linha, index) {


	//console.log(matriz[index]);

	var celula = d3.select(this)
		.selectAll(".cell")
		.data(matriz[index]);



  // Atualiza elementos antigos se necessário		
  /*celula.transition().duration(750)
    	.attr("x", function(d){ return x(d.x); })
    	.attr("width", x.rangeBand() )
		.attr("height", y.rangeBand() )
    	.attr("fill-opacity", function(d){ return (d.z > 0) ? z(d.z) : 1; })
		.attr("fill", function(d) { 	return (d.z > 0) ? cor(colunas[d.x].group) : "#E6E6E6"; })
		.style("stroke", function(d) { 
			
			if ((d.z > -1) && colunas[d.x].group == "0")
				return "#E6E6E6";	
			
		});
*/

  celula.enter()
		.append("rect")
		.attr("class", "cell")
		.attr("x", function(d){ return x(d.x); })
		.attr("width", x.rangeBand() )
		.attr("height", y.rangeBand())
		//.attr("fill-opacity", function(d){ return (d.z > 0) ? z(d.z) : 1; })
		.style("fill", function(d) { 	return (d.z != -1) ? escalaCor(d.z): "#d6d6d6"; })
		.style("stroke", function(d) { 
			
			if ((d.z > -1) && colunas[d.x].group == "0")
				return "#E6E6E6";	
			
		})
		.on('mouseover', tipCelula.show)
  		.on('mouseout', tipCelula.hide);
  		
  		
  celula.exit().transition()
    	.attr("opacity", 1e-6)
    	.remove();  		
      			
  		
} // Fim do método PreencheLinha
















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





//////////////////////////////////////////
///////// Região dos Eventos ////////////  
////////////////////////////////////////

//window.onresize = function(event) { DesenhaVisualizacao(); };


//////////////////////////////////////////
///////// Região das Ordenações /////////  
////////////////////////////////////////

d3.select("#order").on("change", function(){
	OrdenaColuna(this.value);
});



function OrdenaColuna(valor) {
	
	
	x.domain(ordenacaoColuna[valor]);
	
	
	if (valor != "group")
		y.domain(ordenacaoLinha[valor]);
	else
		y.domain(ordenacaoLinha["count"]);
	
	
	var svg = d3.select("#container-matriz svg");
	var t = svg.transition().duration(2000);


	t.selectAll(".row")
			.delay(function(d, i) { return y(i) * 2; })
			.attr("transform", function(d, i) { return "translate(0," + y(i)  + ")"; })
		.selectAll(".cell")
			.delay(function(d) { return x(d.x) * 2; })
			.attr("x", function(d) { return x(d.x); });	
	
	
	t.selectAll(".column")
			.delay(function(d, i) { return x(i) * 2; })
			.attr("transform", function(d, i) { return "translate(" + x(i) + ")rotate(-90)"; });
	
	

	
} // Fim do método OrdenaCrescente







function OrdenaCrescente() {
	
	
	d3.select("tbody")
		.selectAll("tr")
		.sort(function(a, b){ return a.sentiment_avg - b.sentiment_avg;});

	
} // Fim do método OrdenaCrescente


function OrdenaDecrescente() {
	
	d3.select("tbody")
		.selectAll("tr")
		.sort(function(a, b){ return b.sentiment_avg - a.sentiment_avg;});
		
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










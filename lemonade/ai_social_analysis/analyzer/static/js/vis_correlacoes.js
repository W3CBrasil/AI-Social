
var svg;
var dados;

var margin = {top: 80, right: 0, bottom: 80, left: 500},
    width = 1280,
    height = 2048;   

var x = d3.scale.ordinal(),
	y = d3.scale.ordinal(),
    z = d3.scale.linear().domain([0, 4]).clamp(true),
    cor = d3.scale.ordinal().domain(d3.range(0, 14)),
    rotuloLinha = d3.scale.linear(),
    rotuloColuna = d3.scale.linear();


var paleta = ["#FFFFFF", "#1F77B4", "#FF7F0E", "#2CA02C", "#3C5C87", "#D62728", "#8C564B", "#9467BD", "#8B3E6B", "#E377C2", "#3ee4c4", "#3C7587", "#BCBD22", "#17BECF"];
var paletaFonte = ['#000000', '#252525', '#636363', '#969696', '#cccccc']; 

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

var totalPadrao = 0;
var pagina = 1;
var padroes = [];


const DIRETORIO_BUSCA = "../calaca/";
const TERMO = "comment_text:";
const ID_TOPICO = "topico_id:";
const ID_EIXO = "axis_id:";

const TAMANHO_FONTE = 12;







////////////////////////////////////////////
// Região de Pré-processamento dos Dados // 
//////////////////////////////////////////

// Carrega os Dados matriz.json
//d3.json("data/dados_padroes_correlacao.json", function(error, data) {
d3.json("dados.json", function(error, data) {
	
	if (error) throw error;
	
	dados = data;
	totalPadrao = d3.keys(data).length;
	padroes = d3.keys(data);
	

	
	
	$('#page-selection').bootpag({
		total: totalPadrao,
		page: pagina,
		maxVisible: 10
	}).on('page', function(event, i){
		pagina = i;
		AtualizaVisualizacao();
	});
	
	ProcessaDados();
	main();

});



function ProcessaDados() {
	
	var links = dados[pagina].links;
	linhas = dados[pagina].row;
	colunas = dados[pagina].column;
	matriz = [];
	
	
	
	
	m = linhas.length,
	n = colunas.length,
	dimensao = m * n;
	tamanhoGrid = Math.floor(width / n);


	// Cria matriz
	for (var i=0; i < m ; i++)
		matriz[i] = d3.range(n).map(function(j){ return {x: j, y: i, z:0}; });
	   	
	
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
		matriz[d.source][d.target].z = d.value;			
		linhas[d.source].count += d.value;	
		colunas[d.target].count += d.value;
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
	x.domain(ordenacaoColuna.group);
	y.domain(ordenacaoLinha.count);
	

	
	paletaFonte.sort(function(a, b) { return d3.hsl(b).l - d3.hsl(a).l; });	
	paleta.sort(function(a, b) { return d3.hsl(b).h - d3.hsl(a).h; });
	cor.range(paleta);
	
		
		rotuloColuna.domain([0, d3.max(colunas, function(d){ return d.count; })])
			.range(paletaFonte);
			
			
		//console.log(paletaFonte);	
			
			
			
		rotuloLinha.domain([0, d3.max(linhas, function(d){ return d.count; })])
			.range(paletaFonte);
	
	//console.log(rotuloLinha(d3.max(linhas, function(d){ return d.count; })));
	
}

function main() {
	
	
	
	

	CriaMatriz();	
	CriaLegenda("#legenda-dashboard");
	DesenhaMatriz();	
	
} // Fim do método Main



//////////////////////////////////////////
// Região de Criação das Visualizações // 
////////////////////////////////////////

/* Obtém o tamanho da imagem SVG */
function TamanhoSVG(container) {
	
	var width = parseFloat(d3.select(container).node().clientWidth);
	var height = parseFloat(d3.select(container).node().clientHeight);
	
	return [width, height];
	
} // Fim do método TamanhoSVG



function CriaLegenda(container) {

	var legendRectSize = 18;
	var legendSpacing = 4;
	
	var tamanho = TamanhoSVG(container);
	
	var svg = d3.select(container).append("svg")
		.attr("viewBox", "0 0 "+ tamanho[0] + " " + 420)
		.attr("preserveAspectRatio", "xMidYMid meet");
		
		
	svg.selectAll('.legend')
		.data(["Indefinido", "Arts. 1º ao 4º", "Arts. 5º, 12º e 13º", "Art. 6º", "Arts. 7º ao 11º", "Arts. 14º e 15º", "Arts. 16º ao 21º", "Arts. 22º ao 27º", "Arts. 28º ao 33º", "Arts. 34º ao 41º", "Arts. 42º ao 47º", "Arts. 48º ao 49º", "Art. 50º", "Arts. 51º ao 52º"])
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
  		.text(function(d) { return d; });

}


function CriaMatriz() {
	
	var tamanho = TamanhoSVG("#container-matriz");
	
	width = tamanho[0]  - margin.left - margin.right;
	height = tamanho[1]  - margin.top - margin.bottom;
	
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

			return "<div>Item de Lei: <br /> <span>" + nomeItemLei + "</span></div>" + 
				"<div>Participante: <br /> <span>" + nomeParticipante + "</span></div>" +
				"<div>Comentários: <br /> <span>" + valor + "</span></div>";
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

	var celula = d3.select(this)
		.selectAll(".cell")
		.data(matriz[index]);



  // Atualiza elementos antigos se necessário		
  celula.transition().duration(750)
    	.attr("x", function(d){ return x(d.x); })
    	.attr("width", x.rangeBand() )
		.attr("height", y.rangeBand() )
    	.attr("fill-opacity", function(d){ return (d.z > 0) ? z(d.z) : 1; })
		.attr("fill", function(d) {	return (d.z > 0) ? cor(colunas[d.x].group) : "#E6E6E6"; })
		.style("stroke", function(d) { 
			
			if ((d.z > -1) && colunas[d.x].group == "0")
				return "#E6E6E6";	
			
		});


  celula.enter()
		.append("rect")
		.attr("class", "cell")
		.attr("x", function(d){ return x(d.x); })
		.attr("width", x.rangeBand() )
		.attr("height", y.rangeBand())
		.attr("fill-opacity", function(d){ return (d.z > 0) ? z(d.z) : 1; })
		.attr("fill", function(d) {	return (d.z > 0) ? cor(colunas[d.x].group) : "#E6E6E6"; })
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
	

	ProcessaDados();
	
	console.log(n);
	console.log(m);
	
	
	
	width = ((670 * n) / 34);
	height = ((1888 * m) / 110);
	
	tamanhoGrid =  Math.floor(width / n);
	
	x.rangeRoundBands([0, (width)], .1);
	y.rangeRoundBands([height, 0], .1);

	svg.attr("viewBox", "0 0 "+  (width + margin.left + margin.right) + " " + (height + margin.top + margin.bottom));
	
	//svg.attr("viewBox", "0 0 "+  (width ) + " " + (height));
	
	
	DesenhaMatriz();

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

function selectRow(d) {
	

	
	var linhaSelecionada = d3.select(this).node().parentNode;
	
	
	//d3.select(linhaSelecionada)
	
	var celulas = d3.select("#" + linhaSelecionada.id)
		.selectAll(".cell");

		celulas.classed("checked", !celulas.classed("checked"));

	var celulasNaoSelecionadas = d3.selectAll("g.row:not(#" + linhaSelecionada.id + ")")
		.selectAll(".cell");
		
		
		celulasNaoSelecionadas.classed("unchecked", !celulasNaoSelecionadas.classed("unchecked"));
	
	
	
	
}



d3.select("#order").on("change", function(){
	OrdenaColuna(this.value);
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


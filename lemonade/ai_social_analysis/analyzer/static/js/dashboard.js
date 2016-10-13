const MAX_RANKING_PARTICIPANTES = 20;

var cor = d3.scale.category20();
var corTermos = d3.scale.linear()
            .domain([10, 20, 40, 80, 100])
    		.range(['#252525', '#636363', '#969696', '#cccccc', '#f7f7f7']);



////////////////////////////////////////////
// Região de Pré-processamento dos Dados // 
//////////////////////////////////////////

// Carrega os Dados
d3.json("dados.json", function(error, data) {
	
	if (error) throw error;
	
	var dados = data;
	main(dados);

});


function main(dados) {
	
	var dadosSelecionados = dados.comentarios;
	var termosFrequentes = dados.termos_comentarios;
	
	CriaTabelaDados(dadosSelecionados, "#container-tabela-dados");
	CriaWordCloud(termosFrequentes, "#container-nuvem-termos");
	
	// Aninha hierarquicamente os dados selecionados por eixo
	var grupoLeisNested = d3.nest()
		.key(function(d){ return d.axis_id; })
		.entries(dadosSelecionados);
		
	//console.log(grupoLeisNested.values.filter(function(d) { return d.author_id == 321; }));
		
	// Ordena valores pela frequência
	grupoLeisNested.sort(function(a, b) { return b.values.length - a.values.length;  });
	
	
	
	// Aninha hierarquicamente os dados selecionados por participantes
	var participantesNested = d3.nest()
		.key(function(d){ return d.author_id; })
		.entries(dadosSelecionados);
	
	console.log(participantesNested);
	
	
	// Ordena valores pela frequência
	participantesNested.sort(function(a, b) { return b.values.length - a.values.length;  });
	
	
	// Pega os top primeiros participantes 
	var rankingParticipantesNested = participantesNested.slice(0, MAX_RANKING_PARTICIPANTES);
	
	var participantes = participantesNested.length;
	var comentarios = dadosSelecionados.length;
	
	grupoLeisHierarquico = {id: "root", values: grupoLeisNested};

	CriaContadorDadosGerais(participantes, comentarios);
	CriaBarras(grupoLeisNested, "#container-eixos");
	CriaBarras(rankingParticipantesNested, "#container-participantes");
	CriaTreeMap(grupoLeisHierarquico, "#container-item-lei");
	
	CriaLegenda("#legenda-dashboard");
	
	DesenhaVisualizacao();
	
} // Fim do método Main



//////////////////////////////////////////
// Região de Criação das Visualizações // 
////////////////////////////////////////

function CriaContadorDadosGerais(participantes, comentarios) {
	
	var options = {
  		useEasing : true, 
  		useGrouping : true, 
  		separator : '.', 
  		decimal : ',', 
  		prefix : '', 
  		suffix : '' 
	};
	
	var contadorParticipantes = new CountUp("contador-participantes", 0, participantes, 0, 2, options);
	var contadorComentarios = new CountUp("contador-comentarios", 0, comentarios, 0, 2, options);
	
	contadorComentarios.start();
	contadorParticipantes.start();
	
} // Fim do método


function CriaLegenda(container) {
	
	var legendRectSize = 18;
	var legendSpacing = 4;
	
	var tamanho = TamanhoSVG(container);
	
	var svg = d3.select(container).append("svg")
		.attr("viewBox", "0 0 "+ tamanho[0] + " " + tamanho[1])
		.attr("preserveAspectRatio", "xMidYMid meet");
		
		
	svg.selectAll('.legend')
		.data(["Participantes", "Arts. 1º ao 4º", "Arts. 5º, 12º e 13º", "Art. 6º", "Arts. 7º ao 11º", "Arts. 14º e 15º", "Arts. 16º ao 21º", "Arts. 22º ao 27º", "Arts. 28º ao 33º", "Arts. 34º ao 41º", "Arts. 42º ao 47º", "Arts. 48º ao 49º", "Art. 50º", "Arts. 51º ao 52º"])
  		.enter().append('g')
  		.attr("class", 'legend')
  		.attr("transform", function(d, i) { return "translate(10," + (i * 30) + ")"; })
  		.append('rect')
  		.attr('width', legendRectSize)
  		.attr('height', legendRectSize)
  		.style('fill', function(d, i) { return (i == 0) ? "#383838" : cor(i); })
  		
  		.style('stroke', function(d, i) { return (i == 0) ? "#383838" : cor(i); } );
  		
  		
  svg.selectAll('.legend').append("text")
  		.attr('x', legendRectSize + legendSpacing)
  		.attr('y', legendRectSize - legendSpacing)
  		.attr('fill', "#FFFFFF")
  		.text(function(d) { return d; });;	
		
		
		
	
}



function CriaWordCloud(termosFrequentes, container) {

	var tamanho = TamanhoSVG(container);
	
	var svg = d3.select(container).append("svg")
		.attr("viewBox", "0 0 "+ tamanho[0] + " " + tamanho[1])
		.attr("preserveAspectRatio", "xMidYMid meet")
		.append("g")
    	.attr("transform", "translate(" + (tamanho[0] / 2) + "," + (tamanho[1] / 2) + ")");
  
	d3.layout.cloud()
		.size([tamanho[0], tamanho[1]])
		.words(termosFrequentes)
		.rotate(0)
		.fontSize(function(d) { return Math.floor(Math.sqrt(d.doc_count)); })
		.on("end", DesenhaWordCloud)
		.start();

} // Fim do método CriaWordCloud



function CriaTabelaDados(dadosSelecionados, container) {
	
	// Obtém cabeçalho das colunas
	
	var cabecalho = d3.keys(dadosSelecionados[0]);
	var ordemCabecalho = ["id", "author_id", "author", "axis_id", "axis", "article_id", "article", "commentable_id", "commentable", "date"];
	
	cabecalho.sort(function(a, b) { return ordemCabecalho.indexOf(a) - ordemCabecalho.indexOf(b); })
		
	// Cria Tabela
	d3.select(container)
		.append("table")
			.attr("class", "table table-hover table-bordered")
		.append("thead");
	
	// Cria Cabeçalho		
	d3.select("thead")
		.append("tr")
		.selectAll("th")
			.data(cabecalho)
			.enter()
		.append("th")
		.attr("class", function(d){
			
			if (d == "id" || d == "article_id" || d == "author_id" || d == "commentable_id" || d == "axis_id") {
				return "hidden";
			}
				
		})
		.text(function(d){ 

			if (d == "author")
				return "Participante";
			else if (d == "commentable")
				return "Item de Lei";
			else if (d == "date")
				return "Data";
			else if (d == "axis")
				return "Eixo";
			else if (d == "article")
				return "Artigo";
			else
				return d
		});
		
		
	d3.select("table")
		.append("tbody")
		.selectAll("tr")
			.data(dadosSelecionados)
			.enter()
		.append("tr");


	d3.selectAll("tbody tr")
		.selectAll("td")
		.data(function(d){ 
			
			return d3.entries(d).sort(function(a, b) { return ordemCabecalho.indexOf(a.key) - ordemCabecalho.indexOf(b.key); }); 	
		
		})
		.enter()
		.append("td")
		.attr("class", function(d){
			
			if (d.key == "id" || d.key == "article_id" || d.key == "author_id" || d.key == "commentable_id" || d.key == "axis_id") {
				return "hidden";
			}
				
		}).text(function(d){ return d.value; });
		
	$(container + " table").DataTable({
		responsive: true,
		fixedHeader: true,
		 dom: 'Bfrtip',
		buttons: [ 'pageLength', 'copy', 'csvHtml5', 'print']
	});		

} // Fim do método CriaPlanilha


function CriaBarras(dadosSelecionados, container) {
	
	
	var tamanho = TamanhoSVG(container);
	var svg = d3.select(container).append("svg")
		.attr("viewBox", "0 0 "+ tamanho[0] + " " + tamanho[1])
		.attr("preserveAspectRatio", "xMidYMid meet");
	
	
	var tip = d3.tip().attr('class', 'd3-tip')
		.offset([-10, 0])
		.html(function(d) {
			
			var totalCaracteres = 20;
			
			var nome = "";
			var nomeTruncado = "";
			
			d.values.forEach(function(e) {
		
				if (d.key == e.author_id) {	
					nome = e.author;
				} else if (d.key == e.axis_id) {
					nome = e.axis;
				}
				
			})
			
			
			if (nome.length > totalCaracteres)
				nomeTruncado = nome.substring(0, totalCaracteres) + " . . . ";
			else
				nomeTruncado = nome.substring(0, totalCaracteres);
			
			
			return "<span>" + nomeTruncado + "</span>" + "<br /><br /><strong>Comentários:</strong> <span>" + d.values.length + "</span>";
		});
	
	
	svg.call(tip);

	svg.selectAll("bar")
		.data(dadosSelecionados)
		.enter()
		.append("rect")
		.on("click", Click)
		.on('mouseover', tip.show)
  		.on('mouseout', tip.hide)
  		.on("mouseleave", MouseOut);
	
} // Fim do método CriaBarras




function CriaTreeMap(dadosSelecionados, container) {
	
	
	var tamanho = TamanhoSVG(container);
	var svg = d3.select(container).append("svg")
		.attr("viewBox", "0 0 "+ tamanho[0] + " " + tamanho[1])
		.attr("preserveAspectRatio", "xMidYMid meet");
		
		
		var tip = d3.tip().attr('class', 'd3-tip')
		.offset([-10, 0])
		.html(function(d) {
			
			
			
			return "<span>" + d.axis + "</span>" + "<br /><br /><strong>Item:</strong> <span>" + d.commentable + "</span>";
		});
	
	
	svg.call(tip);
	
	

		
	var treemap = d3.layout.treemap()
		.size([tamanho[0], tamanho[1]])
		.sticky(true)
		.children(function(d) { return d.values; })
		.value(function(d) {  return 1; });	
		
			
	var node = svg.datum(dadosSelecionados).selectAll(".node")
		.data(treemap.nodes).enter()
		.append("rect")
		.call(position)
		.on('mouseover', tip.show)
  		.on('mouseout', tip.hide)
		.on("mouseleave", MouseOut);
		//.on("click", ClickTreeMap);
			
} // Fim do método CriaTreeMap

function position() {
	
 this.attr("x", function(d) { return d.x; })
      .attr("y", function(d) { return d.y; })
      .attr("width", function(d) { return Math.max(0, d.dx - 1); })
      .attr("height", function(d) { return Math.max(0, d.dy - 1); });
	
}


//////////////////////////////////////////
// Região de Desenho das Visualizações // 
////////////////////////////////////////



function DesenhaVisualizacao() {
	
	DesenhaBarChart();
	DesenhaRanking();
	DesenhaTreeMap();
	
} // fim do método atualizaVisualizacao


/* Obtém o tamanho da imagem SVG */
function TamanhoSVG(container) {
	
	var width = parseFloat(d3.select(container).node().clientWidth);
	var height = parseFloat(d3.select(container).node().clientHeight);
	
	return [width, height];
	
} // Fim do método TamanhoSVG


function DesenhaBarChart() {
	
	var container = "#container-eixos";
	
	
	var numeroDeBarras = d3.select(container).selectAll("rect").size();
	var dados = d3.select(container).selectAll("rect").data();
	var valorMax = d3.max(dados, function(d){ return d.values.length; });
	var tamanho = TamanhoSVG(container);

	var x = d3.scale.ordinal()
		.domain(dados.map(function(d){ return d.key; }))
		.rangeRoundBands([0, tamanho[0]], .15);

	var y = d3.scale.linear()
		.domain([0, valorMax])
		.range([tamanho[1], 0]);
		
	d3.select(container)
		.selectAll("rect")
		.style("fill", function(d) { return (d.key == "0")? "#FFFFFF" : cor(d.key); })
		.style("stroke", function(d) { return (d.key == "0") ? "#333333" : cor(d.key); })
		.style("opacity", 1)
		.style("shape-rendering", "crispEdges")
		.attr("width", function(){ return x.rangeBand(); })
		.attr("x", function(d){ return x(d.key); })
		.attr("y", function(d){ return tamanho[1]; })
		.attr("height", 0)
		.transition()
		.delay(function(d, i) { return i * 100; })
		.attr("y", function(d){ return y(d.values.length); })
		.attr("height", function(d) { return tamanho[1] - y(d.values.length); });
	
	 
} // Fim do método desenha barras


function DesenhaRanking() {
	
	var container = "#container-participantes";
	
	
	var numeroDeBarras = d3.select(container).selectAll("rect").size();
	var dados = d3.select(container).selectAll("rect").data();
	var valorMax = d3.max(dados, function(d){ return d.values.length; });
	var tamanho = TamanhoSVG(container);

	var x = d3.scale.ordinal()
		.domain(dados.map(function(d){ return d.key; }))
		.rangeRoundBands([0, tamanho[0]], .15);

	var y = d3.scale.linear()
		.domain([0, valorMax])
		.range([tamanho[1], 0]);
		
	d3.select(container)
		.selectAll("rect")
		.style("shape-rendering", "crispEdges")
		.style("opacity", 1)
		.attr("width", function(){ return x.rangeBand(); })
		.attr("x", function(d){ return x(d.key); })
		.attr("y", function(d){ return tamanho[1]; })
		.attr("height", 0)
		.transition()
		.delay(function(d, i) { return i * 60; })
		.attr("y", function(d){ return y(d.values.length); })
		.attr("height", function(d) { return tamanho[1] - y(d.values.length); });
	
	 
} // Fim do método desenha barras




function DesenhaWordCloud(termos) {
	
	
	var tip = d3.tip().attr('class', 'd3-tip')
		.offset([-10, 0])
		.html(function(d) {
			
			return "<span><strong>" + d.key + "</strong> aparece em </span>" + "<br /><br />  <span>" + d.doc_count + " comentários</span>";
		});
		
d3.select("#container-nuvem-termos svg")
			.call(tip);	
	
d3.select("#container-nuvem-termos svg")
			.select("g")
			.selectAll("text")
			.data(termos).enter()
			.append("text")
			  	.on('mouseover', tip.show)
  		 	.on('mouseout', tip.hide)
			.style("font-size", function(d) { return Math.floor(Math.sqrt(d.doc_count)) + "px"; })
			.style("fill", function(d, i) { return corTermos(i); })
			.attr("transform", function(d) {
         		return "translate(" + [d.x, d.y] + ")rotate(" + d.rotate + ")";
         	})
         	.transition()
         	.ease("in")
         	.delay(function(d, i) { return i * 20; })
         	.text(function(d) { return d.key; });
}




function DesenhaTreeMap() {
	
	d3.select("#container-item-lei").selectAll("rect")
		.attr("class", "node")
		.style("opacity", 1)
		.style("fill", function(d){ 
			
			var corRetangulo = cor(d.axis_id);
			
			
			if (d.axis_id == 0)
				corRetangulo = "#FFFFFF"
			
			return d.children ? "transparent" : corRetangulo; 
			

			})	 
		.style("stroke", function(d) { return (d.axis_id == 0) ? "#333333"  :  "#FFFFFF"; });
		
	
}


//////////////////////////////////////////
///////// Região dos Eventos ////////////  
////////////////////////////////////////

//window.onresize = function(event) { DesenhaVisualizacao(); };

function MouseOver(elemento) {

} // Fim do método MouseOver

function MouseOut() {
	
	// Gráfico de Barras Eixos
	var numeroDeBarras = d3.select("#container-eixos").selectAll("rect").size();
	var barrasNaoFiltradas = d3.select("#container-eixos").selectAll("rect:not(.filtrado)").size();
		
	if (numeroDeBarras == barrasNaoFiltradas) {
			
		d3.selectAll("#container-eixos rect")
		.transition().duration(600)
  		.ease('in').style("opacity", 1);
	}
	
	
	// Gráfico de Barras Participantes
	var numeroDeBarras = d3.select("#container-participantes").selectAll("rect").size();
	var barrasNaoFiltradas = d3.select("#container-participantes").selectAll("rect:not(.filtrado)").size();
		
	if (numeroDeBarras == barrasNaoFiltradas) {
			
		d3.selectAll("#container-participantes rect")
		.transition().duration(600)
  		.ease('in').style("opacity", 1);
	}
	
	
	// TreeMap Itens de Lei
	var numeroDeBarras = d3.select("#container-item-lei").selectAll("rect").size();
	var barrasNaoFiltradas = d3.select("#container-item-lei").selectAll("rect:not(.filtrado)").size();
		
	if (numeroDeBarras == barrasNaoFiltradas) {
			
		d3.selectAll("#container-item-lei rect")
		.transition().duration(600)
  		.ease('in').style("opacity", 1);
	}
	
} // Fim do método MouseOver


function ClickTreeMap(elemento) {
	
	
	// Gráfico de Barras Eixos	.select("#container-item-lei")
	// d == elemento  || d.key == elemento.key || elemento.values.indexOf(d) > -1 || d.author_id == elemento.key || d.axis_id == elemento.key ;
	var elementoSelecionado = d3
			.selectAll("rect")
			.filter(function(d) {
	
				return  d == elemento; 

			});
			
	if (!elementoSelecionado.classed("filtrado")) {

		elementoSelecionado.style("opacity", 1)
			.classed("filtrado", true);
			
		d3.selectAll("rect:not(.filtrado)")
			.transition().duration(500)
  			.ease('out')
  			.style("opacity", .25);
	
	} else {
		
		elementoSelecionado
			.classed("filtrado", false)
			.transition()
			.duration(500)
  			.ease('out')
  			.style("opacity", .25);
	}
			
			
	
	
}


function Click(elemento) {


	

	// Gráfico de Barras Eixos	
	var elementoSelecionado = d3.selectAll("rect")
			.filter(function(d) { return  d == elemento || elemento.values.indexOf(d) > -1 ;  });


	if (!elementoSelecionado.classed("filtrado")) {

		elementoSelecionado.style("opacity", 1)
			.classed("filtrado", true);
			
		d3.selectAll("rect:not(.filtrado)")
			.transition().duration(500)
  			.ease('out')
  			.style("opacity", .25);
	
	} else {
		
		elementoSelecionado
			.classed("filtrado", false)
			.transition()
			.duration(500)
  			.ease('out')
  			.style("opacity", .25);
	}


} // Fim do método Click



//////////////////////////////////////////
///////// Região das Ordenações /////////  
////////////////////////////////////////


function OrdenaCrescente() {
	
	OrdenaParticipantesAsc();
	OrdenaEixosAsc();
	
	
} // Fim do método OrdenaCrescente


function OrdenaDecrescente() {
	
	
	OrdenaParticipantesDesc();
	OrdenaEixosDesc();
	
	
} // Fim do método OrdenaDecrescente

function OrdenaParticipantesAsc() {
	
	var dados = d3.select("#container-participantes").selectAll("rect").data();
	var tamanho = TamanhoSVG("#container-participantes");
	
	var x = d3.scale.ordinal()
		.domain(dados.map(function(d){ return d.key; }))
		.rangeRoundBands([0, tamanho[0]], .15);
	
	
	var x0 = x.domain(dados.sort(function(a, b){ return a.values.length - b.values.length; })
		.map(function(d){ return d.key; }))	
		.copy();
	
	
	d3.select("#container-participantes").selectAll("rect")
		.sort(function(a, b){ return x0(a.values.length) - x0(b.values.length); });
		
		
	var transicao = d3.select("#container-participantes").select("svg")	
			.transition()
			.duration(750),
		 	delay = function(d, i){ return i * 50; };
		
	transicao.selectAll("rect")	
		.delay(delay)
		.attr("x", function(d){ return x0(d.key); });
	
}

function OrdenaParticipantesAlf() {
	

	
	var dados = d3.select("#container-participantes").selectAll("rect").data();
	var tamanho = TamanhoSVG("#container-participantes");
	
	var x = d3.scale.ordinal()
		.domain(dados.map(function(d){ return d.key; }))
		.rangeRoundBands([0, tamanho[0]], .15);
	
	
	
	var x0 = x.domain(dados.sort(function(a, b){  return a.values.length - b.values.length; })
		.map(function(d){ return d.key; }))	
		.copy();
	
	
	d3.select("#container-participantes").selectAll("rect")
		.sort(function(a, b){ return x0(a.values.length) - x0(b.values.length); });
		
		
	var transicao = d3.select("#container-participantes").select("svg")	
			.transition()
			.duration(750),
		 	delay = function(d, i){ return i * 50; };
		
	transicao.selectAll("rect")	
		.delay(delay)
		.attr("x", function(d){ return x0(d.key); });
	
}


function OrdenaEixosAsc() {
	
	var dados = d3.select("#container-eixos").selectAll("rect").data();
	var tamanho = TamanhoSVG("#container-eixos");
	
	var x = d3.scale.ordinal()
		.domain(dados.map(function(d){ return d.key; }))
		.rangeRoundBands([0, tamanho[0]], .15);
	
	
	var x0 = x.domain(dados.sort(function(a, b){ return a.values.length - b.values.length; })
		.map(function(d){ return d.key; }))	
		.copy();
	
	
	d3.select("#container-eixos").selectAll("rect")
		.sort(function(a, b){ return x0(a.values.length) - x0(b.values.length); });
		
		
	var transicao = d3.select("#container-eixos").select("svg")	
			.transition()
			.duration(750),
		 	delay = function(d, i){ return i * 50; };
		
	transicao.selectAll("rect")	
		.delay(delay)
		.attr("x", function(d){ return x0(d.key); });
	
}



function OrdenaParticipantesDesc() {
	
	var dados = d3.select("#container-participantes").selectAll("rect").data();
	var tamanho = TamanhoSVG("#container-participantes");
	
	var x = d3.scale.ordinal()
		.domain(dados.map(function(d){ return d.key; }))
		.rangeRoundBands([0, tamanho[0]], .15);
	
	
	var x0 = x.domain(dados.sort(function(a, b){ return b.values.length - a.values.length; })
		.map(function(d){ return d.key; }))	
		.copy();
	
	
	d3.select("#container-participantes").selectAll("rect")
		.sort(function(a, b){ return x0(a.values.length) - x0(b.values.length); });
		
		
	var transicao = d3.select("#container-participantes").select("svg")	
			.transition()
			.duration(750),
		 	delay = function(d, i){ return i * 50; };
		
	transicao.selectAll("rect")	
		.delay(delay)
		.attr("x", function(d){ return x0(d.key); });
	
}

function OrdenaEixosDesc() {
	
	var dados = d3.select("#container-eixos").selectAll("rect").data();
	var tamanho = TamanhoSVG("#container-eixos");
	
	var x = d3.scale.ordinal()
		.domain(dados.map(function(d){ return d.key; }))
		.rangeRoundBands([0, tamanho[0]], .15);
	
	
	var x0 = x.domain(dados.sort(function(a, b){ return b.values.length - a.values.length; })
		.map(function(d){ return d.key; }))	
		.copy();
	
	
	d3.select("#container-eixos").selectAll("rect")
		.sort(function(a, b){ return x0(b.values.length) - x0(a.values.length); });
		
		
	var transicao = d3.select("#container-eixos").select("svg")	
			.transition()
			.duration(750),
		 	delay = function(d, i){ return i * 50; };
		
	transicao.selectAll("rect")	
		.delay(delay)
		.attr("x", function(d){ return x0(d.key); });
	
}

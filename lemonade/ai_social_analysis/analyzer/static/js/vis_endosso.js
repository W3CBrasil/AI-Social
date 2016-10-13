
var svg;
var dados;

var width = 1280,
    height = 550;   


var cores = ["#FFFFFF", "#1F77B4", "#FF7F0E", "#2CA02C", "#3C5C87", "#D62728", "#8C564B", "#9467BD", "#8B3E6B", "#E377C2", "#3ee4c4", "#3C7587", "#BCBD22", "#17BECF"];
 

//var zoom = d3.behavior.zoom()
//    .translate([0, 0])
//    .scale(1)
//    .scaleExtent([1, 8])
//    .on("zoom", Zoomed);


const DIRETORIO_BUSCA = "../calaca/";
const TERMO = "comment_text:";
const ID_TOPICO = "topico_id:";
const ID_EIXO = "axis_id:";

const TAMANHO_FONTE = 12;

const TAMANHO_NO = 15;



var force = d3.layout.force()
    .charge(-250)
    .linkDistance(70)
    .size([width, height]);
    
var container = "#container-grafo";    
    
var tamanho = TamanhoSVG(container);

var svg = d3.select(container).append("svg")
	.attr("viewBox", "0 0 "+ tamanho[0] + " " + height)
	.attr("preserveAspectRatio", "xMidYMid meet");    


// build the arrow.
svg.append("svg:defs").selectAll("marker")
    .data(["end"])      // Different link/path types can be defined here
  .enter().append("svg:marker")    // This section adds in the arrows
    .attr("id", String)
    .attr("viewBox", "0 -5 10 10")
    .attr("refX", 15)
    .attr("refY", -1.5)
    .attr("markerWidth", 6)
    .attr("markerHeight", 6)
    .attr("orient", "auto")
  .append("svg:path")
    .attr("d", "M0,-5L10,0L0,5");

////////////////////////////////////////////
// Região de Pré-processamento dos Dados // 
//////////////////////////////////////////

// Carrega os Dados matriz.json
//d3.json("data/dados_endosso.json", function(error, data) {
d3.json("dados.json", function(error, data) {
	
	if (error) 
		throw error;

	force
      .nodes(data.nodes)
      .links(data.links)
      .start();

  	var link = svg.selectAll(".link")
      .data(data.links)
    .enter().append("line")
      .attr("class", "link")
      .style("stroke-width", function(d) { return d.value; });

  	var node = svg.selectAll(".node")
      .data(data.nodes)
    .enter().append("circle")
      .attr("class", "node")
      .attr("r", function(d){ return (d.value > 0) ? Math.log(d.value) + TAMANHO_NO: TAMANHO_NO; })
      .style("fill", "#0BB3D9")
      .call(force.drag);

  	node.append("title")
      .text(function(d) { return d.author_name; });

  force.on("tick", function() {
    link.attr("x1", function(d) { return d.source.x; })
        .attr("y1", function(d) { return d.source.y; })
        .attr("x2", function(d) { return d.target.x; })
        .attr("y2", function(d) { return d.target.y; });

    node.attr("cx", function(d) { return d.x; })
        .attr("cy", function(d) { return d.y; });
     });


});


/* Obtém o tamanho da imagem SVG */
function TamanhoSVG(container) {
	
	var width = parseFloat(d3.select(container).node().clientWidth);
	var height = parseFloat(d3.select(container).node().clientHeight);
	
	return [width, height];
	
} // Fim do método TamanhoSVG































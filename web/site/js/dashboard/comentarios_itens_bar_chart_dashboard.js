var margin = {top: 80, right: 30, bottom: 150, left: 20},
    width = 7000 - margin.left - margin.right,
    height = 800 - margin.top - margin.bottom;

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
	.domain([0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13])
    .range(["#383838", "#1F77B4", "#FF7F0E", "#2CA02C", "#3C5C87", "#D62728", "#8C564B", "#9467BD", "#8B3E6B", "#E377C2", "#3ee4c4", "#3C7587", "#BCBD22", "#17BECF"]);


var svg = d3.select("#comentarios_itens_bar_chart").append("svg")
    .attr("width", width + margin.left + margin.right)
    .attr("height", height + margin.top + margin.bottom)
  .append("g")
    .attr("transform", "translate(" + margin.left + "," + margin.top + ")");
    
/*    
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
    
    console.log(eixo);
   
    var textoLei = "<span id='titulo-tip'>" + d.name + "</span><br /><div>Eixo: <br /> <span>" + eixo + "</span></div><div>Comentários: <br /> <span>" + d.value + "</span> </div>";
    

    return textoLei;
    
    	
  }); 


tip.direction(function(d){
	return (d.value >= 53)? 'e': 'n';
});

svg.call(tip);    
*/    
 
d3.csv("data/comentarios_itens_bar_chart.csv", function(error, data) {
	
  console.log(data);

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
      });
      
      //.on('mouseover', tip.show)
  	  //.on('mouseout', tip.hide);
      
      
   svg.append("g")
      .attr("class", "x axis")
      .attr("transform", "translate(0," + height + ")")
      .call(xAxis)
   .selectAll("text")	
            .style("text-anchor", "end")
            .attr("dx", "-.8em")
            .attr("dy", ".15em")
            .attr("transform", function(d) { return "rotate(-65)"; });

  svg.append("g")
      .attr("class", "y axis")
      .attr("transform", "translate(70, -1)")
      .call(yAxis)
    .append("text")
      .attr("transform", "rotate(-90)")
      .attr("y", 6)
      .attr("dy", ".71em")
      .style("text-anchor", "middle")
      .text("Comentários");
      
 bar.append("text")
      .attr("x", function(d){ return (d.value >= 10) ? 8 : 12;})
      .attr("y", function(d) { return (d.value >= 4) ? 290 : 280; })
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
      .text(function(d) { return d.value; });
  	
  	
  	
  //	.style("fill", function(d){ return color(d.group);  });
  
  
  /*
  var bar = svg.selectAll("g.bar")
      .data(data)
    .enter().append("g")
      .attr("class", "bar")
      .attr("transform", function(d) { return "translate(0," + y(d.name) + ")"; });

  bar.append("rect")
      .attr("width", function(d) { return x(d.value); })
      .attr("height", y.rangeBand());

  bar.append("text")
      .attr("class", "value")
      .attr("x", function(d) { return 8; })
      .attr("y", y.rangeBand() / 2)
      .attr("dx", -3)
      .attr("dy", ".35em")
      .attr("text-anchor", function(d){ return (d.value > 1)? "start": "middle"; } )
      .text(function(d) { 
      		
      		return d.value; 
      });
*/
  
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
  
  
});
/* var brasileiro = d3.locale({
	months: ["Janeiro", "Fevereiro", "Março", "Abril", "Maio", "Junho", "Julho", "Agosto", "Setembro", "Outubro", "Novembro", "Dezembro"]
});
*/

var margin = {top: 20, right: 20, bottom: 30, left: 50},
    width = 350 - margin.left - margin.right,
    height = 200 - margin.top - margin.bottom;


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
		


console.log(line);


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
      .text("Comentários");

  svg.append("path")
      .datum(data)
      .attr("class", "line")
      .attr("d", line);
      
		
		
		// get the x and y values for least squares
		var xSeries = d3.range(1, xLabels.length + 1);
		var ySeries = data.map(function(d) { return parseFloat(d['freq']); });
		
		console.log(ySeries);
		
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
	}
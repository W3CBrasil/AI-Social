$(document).ready(function() {
  window.ids_dataflow = [];
  window.algorithms = {};
  window.current_dataflow = null;
  window.graph = new joint.dia.Graph();
  window.paper = new joint.dia.Paper({
    el: $('#paper'),
    width: 2000,
    height: 1000,
    gridSize: 50,
    model: graph,
    gridSize: 1,
    linkConnectionPoint: joint.util.shapePerimeterConnectionPoint,
    snapLinks: {
      radius: 50
    },
    validateConnection: function(cellViewS, magnetS, cellViewT, magnetT, end, linkView) {
      if (magnetS && magnetS.getAttribute('type') === 'input') {
        return false;
      }
      if (cellViewS === cellViewT) {
        return false;
      }
      return magnetT && magnetT.getAttribute('type') === 'input';
    },
    validateMagnet: function(cellView, magnet) {
      return magnet.getAttribute('magnet') !== 'passive';
    },
    linkView: joint.dia.LinkView.extend({
      pointerdblclick: function(evt, x, y) {
        var source_id, visu_url;
        source_id = this.model.attributes.source.id;
        visu_url = graph.getCell(source_id).attributes.urlVisu;
        setModalUrl(visu_url);
        $('#tracking_modal').modal('show');
      }
    }),
    interactive: function(cellView) {
      if (cellView.model instanceof joint.dia.Link) {
        return {
          vertexAdd: false
        };
      }
      return true;
    },
    multiLinks: false
  });
  window.gridX = $('.app-container').width();
  return window.gridY = $('.app-container').height();
});

window.putUrlVisuAttr = function(id, url) {
  var cell;
  cell = graph.getCell(id);
  if (cell) {
    cell.prop('urlVisu', url);
  }
};

window.color_box = function(id, color_stroke, color_port) {
  var cell;
  cell = graph.getCell(id);
  if (cell) {
    return cell.attr({
      '.body': {
        stroke: color_stroke
      },
      '.port-body': {
        fill: color_port
      },
      '.inPorts circle': {
        fill: color_port
      },
      '.outPorts circle': {
        fill: color_port
      }
    });
  }
};

window.recolor_all = function(ids, msg) {
  if (msg !== '') {
    $('.container-fluid').notify({
      text: msg
    }, {
      className: 'success',
      elementPosition: 'center',
      position: 'botton center right',
      autoHideDelay: 5000
    });
  }
  $.each(ids, function(index, id) {
    color_box(id, '#5a300c', '#9d5515');
  });
};

window.block_interface = function(msg, link) {
  $('input').prop('disabled', true);
  $('a').each(function() {
    $(this).data('href', $(this).attr('href')).removeAttr('href');
    $(this).data('data-target', $(this).attr('data-target')).removeAttr('data-target');
    $(this).data('data-confirm', $(this).attr('data-confirm')).removeAttr('data-confirm');
    $(this).data('data-method', $(this).attr('data-method')).removeAttr('data-method');
  });
  $('#execute-play').hide();
  $('.delete').hide();
  if (msg !== '') {
    $('.container-fluid').notify({
      text: msg,
      link: link
    }, {
      className: 'spinner',
      elementPosition: 'center',
      position: 'botton center right',
      style: 'spinner',
      arrowShow: false,
      autoHide: false
    });
  }
};

window.unblock_interface = function(msg, stats, link, time_to_hide) {
  if (time_to_hide == null) {
    time_to_hide = 5000;
  }
  $('input').prop('disabled', false);
  $('a').each(function() {
    $(this).attr('href', $(this).data('href'));
    $(this).attr('data-target', $(this).data('data-target'));
    $(this).attr('data-confirm', $(this).data('data-confirm'));
    $(this).attr('data-method', $(this).data('data-method'));
  });
  $('#execute-play').show();
  $('.delete').show();
  $('.container-fluid').notify().click();
  if (msg !== '') {
    $('.container-fluid').notify({
      text: msg,
      link: link
    }, {
      className: stats,
      elementPosition: 'center',
      position: 'botton center right',
      autoHideDelay: time_to_hide,
      style: 'spinner',
      arrowShow: false
    });
  }
};

$(document).ready(function() {
  window.current_dataflow = $('#current_dataflow').attr('value');
  $(document).ajaxError(function(event, request, settings) {
    if (settings.url === gon.elastic_search + "/tarefa/criar_fluxo/") {
      alert(gon.nao_logado);
    }
  });
  if ($('body').attr('data-controller') === 'dataflows' && $('body').attr('data-action') === 'show') {
    $(window).resize(function() {
      paper.setDimensions(2000, 1000);
    });
    $.ajax({
      url: '/dataflows/get_interface/' + current_dataflow,
    });
    $.get({
      url: gon.elastic_search + "/tarefa/criar_fluxo/",
      xhrFields: {
        withCredentials: true
      },
      crossDomain: true
    }, function(data) {
      window.algorithms = JSON.parse(data).algorithm;
    });

    $('#save-btn').click(function() {
      $('#form_save_interface').val(JSON.stringify(graph));
    });
    $('#saveAs-btn').click(function() {
      $('#form_saveAs_interface').val(JSON.stringify(graph));
    });
    $('#execute').click(function() {
      var dataflow, has_input, item, name;
      block_interface(gon.executando_fluxo, gon.acompanhar_tarefa);
      name = $('#dataflow-name').text().trim();
      dataflow = {
        name: name,
        "interface": JSON.stringify(graph),
        algorithms: window.algorithms

      };
      has_input = ((function() {
        var i, len, ref, results;
        ref = JSON.parse(dataflow['interface'])['cells'];
        results = [];
        for (i = 0, len = ref.length; i < len; i++) {
          item = ref[i];
          if (item['name'] === "in_db_elastic") {
            results.push(item['name']);
          }
        }
        return results;
      })()).length;
      if (has_input < 1) {
        alert(gon.necessario_input);
      }
      $.ajax({
        url: '/dataflows/execute/' + current_dataflow + ".js",
        type: 'POST',
        data: dataflow,
        success: function() {}
      });
    });
    window.save_dataflow = function() {
      $.ajax({
        url: '/dataflows/' + current_dataflow,
        method: 'PATCH',
        data: {
          dataflow: {
            "interface": JSON.stringify(graph)
          }
        },
        success: function(response) {}
      });
    };
    window.create_box_params = function(id, name) {
      return $.ajax({
        url: '/interfaces',
        type: 'POST',
        data: {
          id: id,
          dataflow: current_dataflow,
          box_name: name
        },
        success: function(response) {
          graph.addCell(graph_element);
          save_dataflow();
        }
      });
    };
    window.elastic_api_input = function(id, name, dataset_input, algorithms ) {
      return $.ajax({
        url: '/interfaces',
        type: 'POST',
        data: {
          id: id,
          dataflow: current_dataflow,
          box_name: name,
          dataset_input: dataset_input,
          algorithms: algorithms,
          type: "elastic_input"
        },
        success: function(response) {
          graph.addCell(graph_element);
          save_dataflow();
        }
      });
    };
    window.get_input_api = function(id, name) {
      $.get({
        url: gon.elastic_search + "/tarefa/criar_fluxo/",
        xhrFields: {
          withCredentials: true
        },
        crossDomain: true
      }, function(data) {
        var dataset_input;
        var algorithms;
        dataset_input = [
        {
          name: " ",
          id: 0
        }
        ].concat(JSON.parse(data).dataset_input);
        window.algorithms = JSON.parse(data).algorithm;
        elastic_api_input(id, name, dataset_input, algorithms);
      });
    };
    return;
  }
});

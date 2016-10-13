joint.shapes.devs.Box = joint.shapes.devs.Model.extend({
  defaults: joint.util.deepSupplement({
    type: 'devs.Box',
    size: { width: 80, height: 80 },
    category: 'default',
    attrs: {
      '.label': { text: 'Box', 'ref-y': 90}
    }
  }, joint.shapes.devs.Model.prototype.defaults)
});

joint.shapes.devs.BoxView = joint.shapes.devs.ModelView.extend({
  template: [
    '<div class="html-element">',
    '<button class="delete" id= >x</button>',
    '</div>'].join(""),
  initialize: function() {
    _.bindAll(this, 'updateBox');
    joint.dia.ElementView.prototype.initialize.apply(this, arguments);

    this.$box = $(_.template(this.template)());
    this.$box.find('.delete').on('click', _.bind(this.model.remove, this.model));
    // Update the box position whenever the underlying model changes.
    this.model.on('change', this.updateBox, this);
    // Remove the box when the model gets removed from the graph.
    this.model.on('remove', this.removeBox, this);
    this.model.on('change:position', function() {lastPosition = this.get('position')});
    this.updateBox();
  },
  render: function() {
    joint.dia.ElementView.prototype.render.apply(this, arguments);
    this.paper.$el.prepend(this.$box);
    this.updateBox();
    return this;
  },
  updateBox: function() {
    // Set the position and dimension of the box so that it covers the JointJS element.
    var bbox = this.model.getBBox();
    // Example of updating the HTML with a data stored in the cell model.
    this.$box.css({ width: bbox.width, height: bbox.height, left: bbox.x, top: bbox.y, transform: 'rotate(' + (this.model.get('angle') || 0) + 'deg)' });
  },
  removeBox: function(evt) {
    id = this.model.id
      $.ajax({
        url: "/interfaces/" + id,
        method: "DELETE",
        //data: {dataflow: current_dataflow},
        success:  function(response) {
        }
      });
    this.$box.remove();
  }
});

$(document).ready(function(){
  paper.on('cell:pointerdblclick', function(cell, evt, x, y){
    id = cell.model.id;
    name = cell.model.attributes.name;
    form = cell.model.attributes.parameters;
    $.ajax({
      url: "/interfaces/" + id + "/edit",
      data: {dataflow: current_dataflow},
      success:  function(response) {
      }
    });
  });
  paper.on('cell:mouseover', function(cellView,evt){
  })
});

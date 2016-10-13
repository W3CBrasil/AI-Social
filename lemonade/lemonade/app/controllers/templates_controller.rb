class TemplatesController < ApplicationController

  def edit
    @dataflow = Dataflow.find( params[:dataflow])
    @name = @dataflow.box[params[:id]]['display_name']
    @parameters = @dataflow.box[params[:id]]['parameters']
  end

  def show
    @template = Template.find(params[:id])
  end

  def update
    @dataflow = Dataflow.find( params[:dataflow])
    @parameters = @dataflow.box[params[:id]]["parameters"]
    @parameters.each do |type, opt|
      @parameters[type].each do |k,v|
        if params[k] and type != 'select'
          @parameters[type][k] = params[k]
        elsif type == 'select'
          @parameters[type][k]['selected'] = params[k]
        end
      end
    end
    @response = @dataflow.save
  end

  def destroy
    @dataflow = Dataflow.find( params[:dataflow])
    if @dataflow.box.delete(params[:id])
      @response = @dataflow.save
    end
  end

  private

  def delete_box_interface dataflow, id
    interface = JSON.parse(dataflow.interface)["cells"]
    interface.each_with_index do |box, i|
      interface.delete_at(i) if box["id"] == id
    end
    dataflow.save
  end
end

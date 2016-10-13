class InterfacesController < ApplicationController

  def index
  end

  def create
    @dataflow = Dataflow.find( params[:dataflow])
    @template = Template.where(name:params[:box_name]).first
    @template['id'] = params[:id]
    @box = @dataflow.box.create(JSON.parse(@template.to_json))
    if params[:type] == "elastic_input"
      @box.elastic_input(params[:dataset_input], params[:algorithms])
    end
    @box.save
  end

  def new
    @box = Interface.where(name: params[:format]).first
  end

  def edit
    @box = Box.find(params[:id])
    @name = @box['name']
    @parameters = @box['parameters']
  end

  def show
  end

  def update
    @box = Box.find(params[:id])
    @box.parameters.each do |type, opt|
      @box.parameters[type].each do |k,v|
        if params[k] and type != 'select'
          @box.parameters[type][k] = params[k]
        elsif type == 'select'
          @box.parameters[type][k]['selected'] = params[k]
        end
      end
    end
    @box.save

  end

  def destroy
    @box = Box.find(params[:id])
    @box.destroy
  end

end

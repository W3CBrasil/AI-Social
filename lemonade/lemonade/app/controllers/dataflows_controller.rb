class DataflowsController < ApplicationController
  require 'tsort'
  before_action :default
  before_action :authenticate_user!

  def index
    @dataflows = current_user.dataflow.order_by(updated_at: 'desc')
    @box = {}
    @value = {}
    @dataflows.each do |dataflow|
      id = dataflow[:id]
      @box[id] = dataflow.box.detect{|el| el.is_input?}
      next if @box[id].nil?
      @value[id] = @box[id]['parameters']['select']['Datasets']
    end
  end

  def create
    @dataflow = current_user.dataflow.new
    @dataflow.name = params[:name]
    @dataflow.interface = params[:interface]
    @dataflow.log = {}
    @response = @dataflow.save
    redirect_to dataflow_path(@dataflow.id)
  end

  def new
    return head(:forbidden)
  end

  def edit
    return head(:forbidden) unless @dataflow = current_user.dataflow.where(name: params[:name]).first
    redirect_to dataflow_path @dataflow.id
  end

  def show
    return head(:forbidden) unless @dataflow = current_user.dataflow.where(id: params[:id]).first
    @dataflow_list = current_user.dataflow.map{|dataflow| dataflow.name}
    @inputs = Interface.where(category: 'input')
    @filters = Template.where(category: 'filter')
    @elasticsearch = Template.where(category: 'elastic')
    @outputs = Template.where(category: 'output')
    render 'show'
  end

  def update
    return head(:forbidden) unless @dataflow = current_user.dataflow.where(id: params[:id]).first
    interface = JSON.parse(params[:dataflow][:interface])
    interface['cells'].each do |cell|
      puts cell['id']
      if cell['type'] == 'devs.Box'
        cell['attrs']['.body']['stroke'] = '#5a300c'
      end
    end
    @response = @dataflow.update( interface: interface.to_json, last_result: nil)
  end

  def destroy
    return head(:forbidden) unless @dataflow = current_user.dataflow.where(id: params[:id]).first
    @dataflow.box.all.delete
    if @dataflow and @dataflow.destroy
      @response = 'destroy_success'
    else
      @response = 'name_not_found'
    end

    redirect_to root_path
  end

  def get_interface
    return head(:forbidden) unless @dataflow = current_user.dataflow.where(id: params[:id]).first
  end

  def execute_dataflow
    return head(:forbidden) unless @dataflow = current_user.dataflow.where(id: params[:id]).first
    @id = params[:id]
    @response = @dataflow.update( interface: params[:interface] ) if params[:interface]
    interface = JSON.parse(@dataflow.interface)
    sequence, tree_hash = @dataflow.draw_flow
    @feed = sequence.map{|el| el.id}
    @input_id  = sequence.detect{|el| el.is_input? }.get_dataset_id
    @input_name  = sequence.detect{|el| el.is_input? }.get_dataset_name

    if @input_id == nil
      @input_id = 0
    end

    @output = sequence.detect{|el| el.is_output? }
    @filters = sequence.select{|el| el.is_algorithm? }.map do |el|
      algorithm_params = el[:parameters][:text]
      algorithm_params.delete("Step name")
      {
        id: el.get_algorithm_id(params[:algorithms]),
        algorithm_params: algorithm_params
      }
    end.to_json
  end

  def stop_dataflow
    return head(:forbidden) unless @dataflow = current_user.dataflow.where(id: params[:id]).first
  end

  def update_run
    return head(:forbidden) unless @dataflow = current_user.dataflow.where(id: params[:id]).first

    @dataflow.last_result = params[:url]
    @dataflow.save

    respond_to do |format|
      format.json { head :ok }
    end
  end

  private
  def default
    @default_dataflow = Dataflow.where( name: 'default_dataflow').first
  end
end

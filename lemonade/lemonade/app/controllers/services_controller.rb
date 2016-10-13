class ServicesController < ApplicationController

  def show
    render template: "services/#{params[:service]}"
  end

end

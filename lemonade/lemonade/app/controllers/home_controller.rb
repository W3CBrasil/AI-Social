class HomeController < ApplicationController
  def index
    redirect_to dataflows_path and return unless params[:username]

    user = User.where(name: params[:username]).first

    unless user
      user = User.new()
      user.name = params[:username]
      user.email = params[:email]
      user.save
    end

    sign_in(:user, user)
    redirect_to root_path
  end
end

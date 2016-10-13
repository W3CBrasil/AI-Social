class LocalesController < ApplicationController

  def set_locale
    current_user.update_attribute(:locale, params[:id])
    redirect_to :back
  end
end

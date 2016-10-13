class ApplicationController < ActionController::Base
  protect_from_forgery with: :exception
  before_filter :set_locale
  before_filter :configure_devise_permitted_parameters, if: :devise_controller?
  before_action :set_gon
  after_filter :store_location

  def store_location
    session[:previous_url] = request.fullpath unless request.fullpath =~ /\/users/ || request.fullpath =~ /\/services/
  end

  def after_sign_in_path_for(resource)
    session[:previous_url] || root_path
  end

  protected

  def configure_devise_permitted_parameters
    permitted_params = [:name, :email, :password, :password_confirmation, :unit_prefs]

    if params[:action] == 'update'
      devise_parameter_sanitizer.for(:account_update) {
        |u| u.permit(permitted_params << :current_password)
      }
    elsif params[:action] == 'create'
      devise_parameter_sanitizer.for(:sign_up) {
        |u| u.permit(permitted_params)
      }
    end
  end

  def set_locale
    I18n.locale = current_user.try(:locale) || params[:locale] || I18n.default_locale
  end

  def default_url_options(options={})
    { locale: I18n.locale }
  end

  def set_gon
    names = {}

    %i{ acompanhar_tarefa erro erro_executando erro_servidor executando_fluxo
        fluxo_executado_sucesso minha_tarefa necessarion_input
        nome_nao_encontrado parametros_alterados projeto_carregado
        projeto_removido projeto_salvo voce_redirecionado nao_logado
        necessario_input acessar_resultado}.each{|n| names[n] = t(n)}

    names[:elastic_search] = $elastic_search

    gon.push(names)
  end
end

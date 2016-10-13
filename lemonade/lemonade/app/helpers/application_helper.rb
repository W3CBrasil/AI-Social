module ApplicationHelper
  def contexto_controller(controller)
    return { namespace: controller.class.to_s.split("::").first.downcase, controller: controller.controller_name, action: controller.action_name }
  end

  def resource_name
    :user
  end

  def resource
    @resource ||= User.new
  end

  def devise_mapping
    @devise_mapping ||= Devise.mappings[:user]
  end
end
